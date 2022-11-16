//
//  cd_watch.c
//  WatchQuake Watch App
//
//  Created by ByteOverlord on 4.11.2022.
//

#include "cd_watch.h"

#include "quakedef.h"

#include "Common.h"

#include <pthread/pthread.h>
extern pthread_mutex_t snd_lock;

char musicPath[1024];

void CDAudio_SetPath(const char* path)
{
    memset(musicPath,0,1024);
    size_t len = strlen(path);
    if (path != 0 && len != 0)
    {
        memcpy(musicPath,path,len);
    }
}

#define WQ_CD_AUDIO_FORMAT_MS_ADPCM  0x0002
#define WQ_CD_AUDIO_FORMAT_IMA_ADPCM 0x0011

// IMA ADPCM
//const i8 ima_adpcm_index_table[16] = {-1,-1,-1,-1,2,4,6,8,-1,-1,-1,-1,2,4,6,8};
// (nibble & 7) reduces table size from 16 to 8 bytes
const i8 ima_adpcm_index_table[8] = {-1,-1,-1,-1,2,4,6,8};
const i16 ima_adpcm_step_table[89] = {
    7,8,9,10,11,12,13,14,16,17,19,21,23,25,28,31,34,37,41,45,50,55,60,66,73,80,88,97,107,118,
    130,143,157,173,190,209,230,253,279,307,337,371,408,449,494,544,598,658,724,796,
    876,963,1060,1166,1282,1411,1552,1707,1878,2066,2272,2499,2749,3024,3327,3660,4026,4428,4871,5358,
    5894,6484,7132,7845,8630,9493,10442,11487,12635,13899,15289,16818,18500,20350,22385,24623,27086,29794,32767
};
// MS ADPCM
i16 ms_AdaptationTable [] = {
  230, 230, 230, 230, 307, 409, 512, 614,
  768, 614, 512, 409, 307, 230, 230, 230
} ;

i16 ms_AdaptCoeffs[] = {256,0, 512,-256, 0,0, 192,64, 240,0, 460,-208, 392,-232};

#define WQ_CD_BLOCK_STATE_NULL     0
#define WQ_CD_BLOCK_STATE_NEWBLOCK 1
#define WQ_CD_BLOCK_STATE_READ     2

#define WQ_CD_STATE_STOP  0
#define WQ_CD_STATE_PLAY  1
#define WQ_CD_STATE_PAUSE 2

u8* cd_buffer = NULL;
u32 cd_buffer_index = 0;
u32 cd_buffer_maxSize = 0;

int cd_block_state = WQ_CD_BLOCK_STATE_NULL;
u32 cd_readPos = 0;
u32 cd_readBlock = 0;
int cd_track = -1;
int cd_isLooping = 0;
int cd_state = WQ_CD_STATE_STOP;

//double cd_acc = 0.0;
//double cd_step = 1.0;
i32 cd_acc = 0;
i32 cd_step = 65536;

u32 cd_mixer_sampleRate = 22050;

#define WQ_ADPCM_BUFFER_CAPACITY 256
i16* adpcm_buffer = NULL;
i32 adpcm_buffer_size = 0;
i32 adpcm_buffer_index = 0;

i16* ms_coeffPtr = ms_AdaptCoeffs;

typedef struct
{
    i32 blocksize;
    union
    {
        i32 stepIndex;// ima
        i32 delta;// ms
    };
    union
    {
        i32 predictor;// ima
        i32 sample1;// ms
    };
    // ms
    i32 sample2;
    i32 coeff1;
    i32 coeff2;
} adpcmstate_t;

void adpcm_init(adpcmstate_t* state)
{
    state->blocksize = 0;
    state->delta = 0;
    state->sample1 = 0;
    state->sample2 = 0;
    state->coeff1 = 0;
    state->coeff2 = 0;
    ms_coeffPtr = ms_AdaptCoeffs;
}

adpcmstate_t adpcm;

typedef struct
{
    u16 format;
    u16 channels;
    u32 samplesPerSec;
    u32 bytesPerSec;
    u16 blockAlign;
    u16 bitsPerSample;
    // ^ 16 bytes
    u8 extra[48];
} fmtData_t;

typedef struct
{
    u32 riffSize;
    u32 fmtSize;
    fmtData_t fmt;
    u32 dataOffset;
    u32 dataSize;
} waveHeader_t;

waveHeader_t wavHeader;
FILE *f = NULL;

i32 decodeIMA4nibble(u8 nibble)
{
    i32 step = ima_adpcm_step_table[adpcm.stepIndex];
    u32 delta = nibble & 7;
    i32 diff = step >> 3;
    if (delta & 1) { diff += step >> 2;}
    if (delta & 2) { diff += step >> 1;}
    if (delta & 4) { diff += step;}
    if (nibble & 8) { diff = -diff;}// sign

    adpcm.predictor += diff;
    adpcm.predictor = (adpcm.predictor < -32768) ? -32768 : (adpcm.predictor > 32767) ? 32767 : adpcm.predictor;

    adpcm.stepIndex += ima_adpcm_index_table[delta];
    adpcm.stepIndex = adpcm.stepIndex < 0 ? 0 : adpcm.stepIndex > 88 ? 88 : adpcm.stepIndex;
    return adpcm.predictor;
}

i32 decodeMSnibble(u8 nibble)
{
    i32 predictor = ((adpcm.sample1 * adpcm.coeff1) + (adpcm.sample2 * adpcm.coeff2)) >> 8;
    predictor += (signed)((nibble & 0x08) ? (nibble - 16) : nibble) * adpcm.delta;
    predictor = predictor < -32768 ? -32768 : predictor > 32767 ? 32767 : predictor;
    adpcm.sample2 = adpcm.sample1;
    adpcm.sample1 = predictor;
    adpcm.delta = (ms_AdaptationTable[nibble] * adpcm.delta) >> 8;
    adpcm.delta = adpcm.delta < 16 ? 16 : adpcm.delta;
    return predictor;
}

#include <assert.h>

u16 read16(u8* src)
{
    return (((u16)(src[1])) << 8) | (u16)(src[0]);
}

void decodeADPCM(void)
{
    u8* src = cd_buffer;
    if (cd_block_state != WQ_CD_BLOCK_STATE_NULL)
    {
        while (adpcm_buffer_size < (WQ_ADPCM_BUFFER_CAPACITY-1))// no need to special case two decodeIMA4nibble reads
        {
            if (cd_block_state == WQ_CD_BLOCK_STATE_READ)
            {
                i32 bytesLeft = wavHeader.dataSize - (cd_readBlock * adpcm.blocksize + cd_readPos);
                i32 blockLeft = adpcm.blocksize - cd_readPos;
                bytesLeft = blockLeft < bytesLeft ? blockLeft : bytesLeft;
                i32 maxRead = (WQ_ADPCM_BUFFER_CAPACITY - adpcm_buffer_size) / 2;
                maxRead = bytesLeft < maxRead ? bytesLeft : maxRead;
                if (wavHeader.fmt.format == WQ_CD_AUDIO_FORMAT_IMA_ADPCM)
                {
                    while (maxRead)
                    {
                        u32 nibble = src[cd_readPos];
                        cd_readPos++;
                        
                        adpcm_buffer[adpcm_buffer_size] = decodeIMA4nibble(nibble & 0xF);
                        adpcm_buffer_size++;
                        
                        adpcm_buffer[adpcm_buffer_size] = decodeIMA4nibble((nibble >> 4) & 0xF);
                        adpcm_buffer_size++;
                        
                        --maxRead;
                    }
                }
                else // WQ_CD_AUDIO_FORMAT_MS_ADPCM
                {
                    while (maxRead)
                    {
                        u32 nibble = src[cd_readPos];
                        cd_readPos++;
                        
                        adpcm_buffer[adpcm_buffer_size] = decodeMSnibble((nibble >> 4) & 0xF);
                        adpcm_buffer_size++;

                        adpcm_buffer[adpcm_buffer_size] = decodeMSnibble(nibble & 0xF);
                        adpcm_buffer_size++;

                        --maxRead;
                    }
                }
                
                if ((cd_readBlock * adpcm.blocksize + cd_readPos) < wavHeader.dataSize)
                {
                    if (cd_readPos >= adpcm.blocksize)
                    {
                        cd_readBlock++;
                        cd_block_state = WQ_CD_BLOCK_STATE_NEWBLOCK;
                    }
                }
                else
                {
                    if (cd_isLooping)
                    {
                        cd_readPos = 0;
                        cd_readBlock = 0;
                        cd_block_state = WQ_CD_BLOCK_STATE_NEWBLOCK;
                    }
                    else
                    {
                        cd_block_state = WQ_CD_BLOCK_STATE_NULL;
                        break;
                    }
                }
            }
            else if (cd_block_state == WQ_CD_BLOCK_STATE_NEWBLOCK)
            {
                fseek(f,wavHeader.dataOffset + cd_readBlock * adpcm.blocksize,SEEK_SET);
                u32 readEnd = cd_readBlock * adpcm.blocksize + adpcm.blocksize;
                if (readEnd <= wavHeader.dataSize)
                {
                    fread(cd_buffer,1,adpcm.blocksize,f);
                }
                else
                {
                    fread(cd_buffer,1,wavHeader.dataSize - cd_readBlock * adpcm.blocksize,f);
                }
                // who needs a standard format anyway...
                if (wavHeader.fmt.format == WQ_CD_AUDIO_FORMAT_IMA_ADPCM)
                {
                    adpcm.predictor = *((i16*)(src));
                    adpcm.stepIndex = src[2];
                    adpcm.predictor = (adpcm.predictor < -32768) ? -32768 : (adpcm.predictor > 32767) ? 32767 : adpcm.predictor;
                    adpcm_buffer[adpcm_buffer_size] = adpcm.predictor;
                    adpcm_buffer_size++;
                    adpcm.stepIndex = (adpcm.stepIndex > 88) ? 88 : adpcm.stepIndex;
                    cd_readPos = 4;
                }
                else //WQ_CD_AUDIO_FORMAT_MS_ADPCM
                {
                    u8 coeffIdx = src[0];
                    coeffIdx = coeffIdx > 6 ? 6 : coeffIdx;
                    adpcm.coeff1 = ms_coeffPtr[coeffIdx * 2 + 0];
                    adpcm.coeff2 = ms_coeffPtr[coeffIdx * 2 + 1];
                    adpcm.delta = (i16)read16(src + 1);
                    adpcm.sample1 = (i16)read16(src + 3);
                    adpcm.sample2 = (i16)read16(src + 5);
                    adpcm_buffer[adpcm_buffer_size] = adpcm.sample2;
                    adpcm_buffer_size++;
                    adpcm_buffer[adpcm_buffer_size] = adpcm.sample1;
                    adpcm_buffer_size++;
                    cd_readPos = 7;
                }
                
                cd_block_state = WQ_CD_BLOCK_STATE_READ;
            }
        }
    }
    if (cd_block_state == WQ_CD_BLOCK_STATE_NULL)
    {
        while (adpcm_buffer_size < WQ_ADPCM_BUFFER_CAPACITY)
        {
            adpcm_buffer[adpcm_buffer_size] = adpcm.predictor;
            adpcm_buffer_size++;
            // fade out?
            if (adpcm.predictor > 0)
            {
                adpcm.predictor--;
            }
            else if (adpcm.predictor < 0)
            {
                adpcm.predictor++;
            }
        }
    }
}

void loadWaveHeader(const char* path)
{
    cd_block_state = 0;
    if (f != NULL)
    {
        fclose(f);
    }
    f = fopen(path,"rb");
    if (!f)
    {
        return;
    }

    u32 flags = 0;

    const u32 riffFlag = 0x1;
    const u32 waveFlag = 0x2;
    const u32 fmtFlag = 0x4;
    const u32 dataFlag = 0x8;
    const u32 complete = riffFlag | waveFlag | fmtFlag | dataFlag;
    int isADPCM = 0;
    u32 unknownTags = 0;
    size_t unknownFirstPosition = 0;

    {
        i8 tag[4];
        u32 tagSize;
        long fsize;
        {
            long pos;
            long end;
            
            pos = ftell(f);
            fseek(f,0,SEEK_END);
            end = ftell(f);
            fseek(f,pos,SEEK_SET);
            fsize = end;
        }
        fread(tag,1,4,f);
        if (!memcmp("RIFF",tag,4))
        {
            flags |= riffFlag;
            fread(&wavHeader.riffSize,4,1,f);
            printf("RIFF %i\n",wavHeader.riffSize);
        }
        fread(tag,1,4,f);
        if (!memcmp("WAVE",tag,4))
        {
            flags |= waveFlag;
            printf("WAVE\n");
        }

        while (ftell(f) < fsize)
        {
            fread(tag,1,4,f);
            fread(&tagSize,4,1,f);
            if (!memcmp("fmt ",tag,4))
            {
                flags |= fmtFlag;
                wavHeader.fmtSize = tagSize;
                printf("fmt %i\n",wavHeader.fmtSize);
                assert(wavHeader.fmtSize <= 64);
                fread(&wavHeader.fmt,wavHeader.fmtSize,1,f);
                if (wavHeader.fmt.bitsPerSample == 4)
                {
                    if (wavHeader.fmt.format == 0x0001)
                    {
                        printf("format pcm\n");
                    }
                    else if (wavHeader.fmt.format == WQ_CD_AUDIO_FORMAT_MS_ADPCM)
                    {
                        printf("format ms adpcm\n");
                        isADPCM = 1;
                        u16 samplesPerBlock = (u16)(*(wavHeader.fmt.extra + 2));
                        u16 coeffs = (u16)(*(wavHeader.fmt.extra + 4));
                        printf("ms samples per block %u\n",samplesPerBlock);
                        printf("ms coeffs %u\n",coeffs);
                        if (coeffs >= 7)
                        {
                            ms_coeffPtr = (i16*)(wavHeader.fmt.extra + 6);
                        }
                        else
                        {
                            ms_coeffPtr = ms_AdaptCoeffs;
                        }
                        for (int i=0; i<coeffs; i++)
                        {
                            printf("%i ",ms_coeffPtr[i*2+0]);
                        }
                        printf("\n");
                        for (int i=0; i<coeffs; i++)
                        {
                            printf("%i ",ms_coeffPtr[i*2+1]);
                        }
                        printf("\n");
                    }
                    else if (wavHeader.fmt.format == WQ_CD_AUDIO_FORMAT_IMA_ADPCM)
                    {
                        printf("format ima adpcm\n");
                        isADPCM = 1;
                    }
                    adpcm.blocksize = wavHeader.fmt.blockAlign;
                    if (isADPCM)
                    {
                        printf("adpcm block size %u\n",adpcm.blocksize);
                    }
                }
            }
            else if (!memcmp("data",tag,4))
            {
                printf("data %i\n",tagSize);
                flags |= dataFlag;
                wavHeader.dataSize = tagSize;
                long pos = ftell(f);
                wavHeader.dataOffset = (u32)pos;
                printf("data offset %i\n",wavHeader.dataOffset);
                pos += wavHeader.dataSize;
                fseek(f,pos,SEEK_SET);
            }
            else if (!memcmp("fact",tag,4))
            {
                printf("fact %i\n",tagSize);
                fseek(f,tagSize,SEEK_CUR);
            }
            else
            {
                if (!unknownTags)
                {
                    unknownFirstPosition = ftell(f) - 8;
                }
                unknownTags++;
                fseek(f,tagSize,SEEK_CUR);
            }
        }
        if (unknownTags)
        {
            printf("unknown tags %i\n",unknownTags);
            printf("first at position: %zu\n",unknownFirstPosition);
        }
        if (flags == complete)
        {
            printf("samplerate %u\n",wavHeader.fmt.samplesPerSec);
            if (isADPCM)
            {
                if (wavHeader.fmt.channels == 1)
                {
                    cd_block_state = WQ_CD_BLOCK_STATE_NEWBLOCK;
                    cd_readPos = 0;
                    cd_readBlock = 0;
                }
                else
                {
                    printf("cd track supports only mono channel audio\n");
                }
            }
            else
            {
                printf("cd track must be in IMA ADPCM format\n");
            }
        }
    }
}

void CDAudio_Play(byte track, qboolean looping)
{
SND_LOCK
    cd_state = WQ_CD_STATE_STOP;
    cd_isLooping = looping;
    adpcm_buffer_size = 0;
    if (cd_track != track)
    {
        char buffer[1024];
        snprintf(buffer,1024,"%s/track%02i.wav",musicPath,track);
        printf("cd loading track %s\n",buffer);
        loadWaveHeader(buffer);
        if (cd_block_state)
        {
            cd_track = track;
            //cd_step = (double)(wavHeader.fmt.samplesPerSec) / (double)(cd_mixer_sampleRate);
            cd_step = (double)(wavHeader.fmt.samplesPerSec) / (double)(cd_mixer_sampleRate) * 65536;
            cd_acc = 0;
            cd_state = WQ_CD_STATE_PLAY;
            if (adpcm.blocksize > cd_buffer_maxSize)
            {
                AlignedFree(cd_buffer);
                cd_buffer = AlignedMalloc(adpcm.blocksize,16);
                cd_buffer_maxSize = adpcm.blocksize;
            }
            printf("cd track %u loaded\n",track);
        }
    }
    else
    {
        cd_step = (double)(wavHeader.fmt.samplesPerSec) / (double)(cd_mixer_sampleRate) * 65536;
        cd_acc = 0;
        cd_readPos = 0;
        cd_readBlock = 0;
        cd_block_state = WQ_CD_BLOCK_STATE_NEWBLOCK;
        cd_state = WQ_CD_STATE_PLAY;
    }

SND_UNLOCK
}


void CDAudio_Stop(void)
{
SND_LOCK
    cd_state = WQ_CD_STATE_STOP;
SND_UNLOCK
}


void CDAudio_Pause(void)
{
SND_LOCK
    cd_state = WQ_CD_STATE_PAUSE;
SND_UNLOCK
}


void CDAudio_Resume(void)
{
SND_LOCK
    if (cd_state == WQ_CD_STATE_PAUSE)
    {
        cd_state = WQ_CD_STATE_PLAY;
    }
SND_UNLOCK
}


void CDAudio_Update(void)
{
    
}

#include <assert.h>

// barebones music mixer
// downsampling not supported
// mono sound only
void CDAudio_MixSamples(portable_samplepair_t* pb, int count, int volume)
{
    if (cd_block_state == WQ_CD_BLOCK_STATE_NULL)
    {
        return;
    }
    if (cd_state == WQ_CD_STATE_STOP)
    {
        return;
    }
    int sample;
    if (adpcm_buffer_size == 0)// predictor is the current sample value
    {
        sample = adpcm.predictor * volume;
    }
    else // buffer contains the current sample value
    {
#ifdef DEBUG
        assert(adpcm_buffer_index < WQ_ADPCM_BUFFER_CAPACITY);
#endif
        sample = adpcm_buffer[adpcm_buffer_index] * volume;
    }
    while (count)
    {
        cd_acc += cd_step;
        // if (cd_acc >= 1.0)
        if (cd_acc > 0xFFFF)
        {
            //cd_acc -= 1.0;
            cd_acc -= 0x10000;
            if (adpcm_buffer_size == 0)
            {
                decodeADPCM();
                adpcm_buffer_index = 0;
            }
            sample = adpcm_buffer[adpcm_buffer_index] * volume;
            adpcm_buffer_index++;
            adpcm_buffer_size--;
        }
        pb->left += sample;
        pb->right += sample;
        pb++;
        count--;
    }
}

void CDAudio_SetMixerSamplerate(u32 sampleRate)
{
    cd_mixer_sampleRate = sampleRate;
}

int CDAudio_Init(void)
{
    cd_buffer_maxSize = 0;
    cd_buffer = NULL;
    adpcm_buffer = AlignedMalloc(WQ_ADPCM_BUFFER_CAPACITY*sizeof(i16),16);
    //cd_step = 22050.0 / 48000.0;// 0.459375
    cd_step = (22050.0 / 48000.0) * 65536;
    // cd_step * 65536 = 30105,6 ~= 30105
    // 1.0 / 65536.0 = 0.00001525
    cd_acc = 0;
    adpcm_init(&adpcm);
    return 0;
}

void CDAudio_Shutdown(void)
{
    AlignedFree(cd_buffer);
    AlignedFree(adpcm_buffer);
}

