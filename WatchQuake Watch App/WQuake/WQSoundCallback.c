//
//  WQSoundCallback.c
//  WatchQuake Watch App
//
//  Created by ByteOverlord on 29.10.2022.
//

#include "WQSoundCallback.h"

#include "quakedef.h"
#include "sound.h"

#include <pthread/pthread.h>

#include "Common.h"

extern pthread_mutex_t snd_lock;
extern int soundtime;        // sample PAIRS
extern int paintedtime;
extern cvar_t _snd_mixahead;
extern cvar_t snd_filterquality;
extern int sound_started;

extern portable_samplepair_t paintbuffer[PAINTBUFFER_SIZE];
extern int snd_vol;

extern int snd_scaletable[32][256];

volatile int g_runMixer = 1;

extern int g_WQState;

#define SHRT_MIN -32768
#define SHRT_MAX 32767

/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// snd_mix.c -- portable code to mix sounds for snd_dma.c
#include "Threading.hpp"
#include "Threading.h"

#include "DefragAllocator.h"
extern DefragCache_t soundfx_cache;

int g_WQMixerState = 1;

void* WQAudioMixerLoop(void* p)
{
    while (g_runMixer)
    {
        if (g_WQState != 1)
        {
            if (g_WQMixerState)
            {
                SetAudioMixerQOS(0);
                g_WQMixerState = 0;
            }
            SleepFor(1);
        }
        else
        {
            if (!g_WQMixerState)
            {
                SetAudioMixerQOS(1);
                g_WQMixerState = 1;
            }
        }
        SND_LOCK
        unsigned endtime;
        int samps;
        
        if (!sound_started || (snd_blocked > 0))
        {
            goto unlock;
        }

        // Updates DMA time
        //GetSoundtime();
        {
            int        samplepos;
            static    int        buffers;
            static    int        oldsamplepos;
            int        fullsamples;
            fullsamples = shm->samples / shm->channels;
            // it is possible to miscount buffers if it has wrapped twice between
            // calls to S_Update.  Oh well.
            //samplepos = SNDDMA_GetDMAPos();
            samplepos = shm->samplepos;
            if (samplepos < oldsamplepos)
            {
                buffers++;                    // buffer wrapped
                if (paintedtime > 0x40000000)
                {   // time to chop things off to avoid 32 bit limits
                    buffers = 0;
                    paintedtime = fullsamples;
                    SND_UNLOCK
                    S_StopAllSounds (qTrue);// uses SND_LOCK
                    SND_LOCK
                }
            }
            oldsamplepos = samplepos;
            soundtime = buffers*fullsamples + samplepos/shm->channels;
        }
        //end GetSoundtime

        // check to make sure that we haven't overshot
        if (paintedtime < soundtime)
        {
            //Con_Printf ("S_Update_ : overflow\n");
            paintedtime = soundtime;
        }

        // mix ahead of current position
        endtime = soundtime + _snd_mixahead.value * shm->speed;
        samps = shm->samples >> (shm->channels-1);
        if (endtime - soundtime > samps)
        {
            endtime = soundtime + samps;
        }

        //DefragAllocator_DefragRoutine(&soundfx_cache.mem);
        //S_PaintChannels (endtime);
        {
            int     i;
            int     end;
            channel_t *ch;
            sfxcache_t    *sc;
            int        ltime, count;

            snd_vol = sfxvolume.value * 256 * SFX_VOLUME_MULTIPLIER;
            int bgm_vol = bgmvolume.value * 256;

            while (paintedtime < endtime)
            {
            // if paintbuffer is smaller than DMA buffer
                end = endtime;
                if (endtime - paintedtime > PAINTBUFFER_SIZE)
                    end = paintedtime + PAINTBUFFER_SIZE;

            // clear the paint buffer
                Q_memset(paintbuffer, 0, (end - paintedtime) * sizeof(portable_samplepair_t));

            // paint in the channels.
                ch = channels;
                for (i=0; i<total_channels ; i++, ch++)
                {
                    if (!ch->sfx)
                        continue;
                    if (!ch->leftvol && !ch->rightvol)
                        continue;
                    sc = DefragCache_GetPointer(&soundfx_cache,ch->sfx->cacheHandle,DEFRAG_TYPE_SFXCACHE);
                    if (!sc)
                        continue;

                    ltime = paintedtime;

                    while (ltime < end)
                    {    // paint up to end
                        if (ch->end < end)
                            count = ch->end - ltime;
                        else
                            count = end - ltime;

                        if (count > 0)
                        {
                            int paintbufferstart = ltime - paintedtime;
                            portable_samplepair_t* pb = paintbuffer + paintbufferstart;
                            if (sc->width == 1)
                            {
                                //SND_PaintChannelFrom8(ch, sc, count,ltime - paintedtime);
                                {
                                    int data;
                                    int *lscale, *rscale;
                                    unsigned char *sfx;
                                    int i;

                                    if (ch->leftvol > 255)
                                        ch->leftvol = 255;
                                    if (ch->rightvol > 255)
                                        ch->rightvol = 255;
                                        
                                    lscale = snd_scaletable[ch->leftvol >> 3];
                                    rscale = snd_scaletable[ch->rightvol >> 3];
                                    sfx = (unsigned char *)sc->data + ch->pos;
                                    //sfx = (signed char *)sc->data + ch->pos;
                                    for (i=0 ; i<count ; i++)
                                    {
                                        data = sfx[i];
                                        pb[i].left += lscale[data];
                                        pb[i].right += rscale[data];
                                    }
                                    
                                    ch->pos += count;
                                }
                            }
                            else
                            {
                                //SND_PaintChannelFrom16(ch, sc, count,ltime - paintedtime);
                                {
                                    int data;
                                    int left, right;
                                    int leftvol, rightvol;
                                    signed short *sfx;
                                    int i;

                                    leftvol = ch->leftvol * snd_vol;
                                    rightvol = ch->rightvol * snd_vol;
                                    // new
                                    leftvol /= 256;
                                    rightvol /= 256;
                                    //
                                    sfx = (signed short *)sc->data + ch->pos;
                                    for (i=0 ; i<count ; i++)
                                    {
                                        data = sfx[i];
                                        // new
                                        left = data * leftvol;
                                        right = data * rightvol;
                                        // old
                                        //left = (data * leftvol) >> 8;
                                        //right = (data * rightvol) >> 8;
                                        pb[i].left += left;
                                        pb[i].right += right;
                                    }

                                    ch->pos += count;
                                }
                            }
            
                            ltime += count;
                        }

                    // if at end of loop, restart
                        if (ltime >= ch->end)
                        {
                            if (sc->loopstart >= 0)
                            {
                                ch->pos = sc->loopstart;
                                ch->end = ltime + sc->length - ch->pos;
                            }
                            else
                            {    // channel just stopped
                                ch->sfx = NULL;
                                break;
                            }
                        }
                    }
                }
#ifdef WQ_AUDIO_UNDERWATER_FILTER
                /*
                    UnderwaterFilter from vkQuake
                    https://github.com/Novum/vkQuake
                */
                //S_UnderwaterFilter (end - paintedtime);
                {
                    int paintEndtime = end - paintedtime;
                    int i;
                    if (!underwater.intensity)
                    {
                        if (paintEndtime > 0)
                        {
                            underwater.accum[0] = paintbuffer[paintEndtime - 1].left;
                            underwater.accum[1] = paintbuffer[paintEndtime - 1].right;
                        }
                    }
                    else
                    {
                        for (i = 0; i < paintEndtime; i++)
                        {
                            underwater.accum[0] += underwater.alpha * (paintbuffer[i].left - underwater.accum[0]);
                            underwater.accum[1] += underwater.alpha * (paintbuffer[i].right - underwater.accum[1]);
                            paintbuffer[i].left = (int)underwater.accum[0];
                            paintbuffer[i].right = (int)underwater.accum[1];
                        }
                    }
                }
#endif
                if (bgm_vol != 0)
                {
                    ltime = paintedtime;
                    count = end - ltime;
                    i = 0;
                    int paintbufferstart = ltime - paintedtime;
                    portable_samplepair_t* pb = paintbuffer + paintbufferstart;
                    CDAudio_MixSamples(pb,count,bgm_vol);
                }
                // transfer out according to DMA format
                //S_TransferPaintBuffer(end);
                {
                    int     out_idx;
                    int     count;
                    int     out_mask;
                    int     *p;
                    int     step;
                    int        val;
                    //int        snd_vol;
                    //DWORD    *pbuf;

                    // todo
                    // stereo interleaved, non interleaved
                    if (shm->channels == 2)
                    {
                        /*if (shm->samplebits == 32)
                         {
                         S_TransferStereo32(endtime);
                         }
                        if (shm->samplebits == 16)
                        {
                            S_TransferStereo16(endtime);
                        }*/
                    }
                    else
                    {
                        p = (int *) paintbuffer;
                        count = (endtime - paintedtime) * shm->channels;
                        out_mask = shm->samples - 1;
                        out_idx = paintedtime * shm->channels & out_mask;
                        step = 3 - shm->channels;
                        unsigned char* pbuf = shm->buffer;

                        if (shm->samplebits == 32)
                        {
                            if (shm->isFloat)
                            {
                                float* out = (float*) pbuf;
                                while (count--)
                                {
                                    val = *p;
                                    p+= step;
                                    if (val > (SHRT_MAX * 256))
                                        val = (SHRT_MAX * 256);
                                    else if (val < (SHRT_MIN * 256))
                                        val = (SHRT_MIN * 256);
                                    out[out_idx] = (val * ((1.0f / 32768.0) / 256.0f));
                                    out_idx = (out_idx + 1) & out_mask;
                                }
                            }
                            else
                            {
                                int* out = (int*) pbuf;
                                while (count--)
                                {
                                    val = *p;
                                    p+= step;
                                    out[out_idx] = val;
                                    out_idx = (out_idx + 1) & out_mask;
                                }
                            }
                        }
                        else if (shm->samplebits == 16)
                        {
                            short *out = (short *) pbuf;
                            while (count--)
                            {
                                //val = (*p) / 256;
                                val = (*p) >> 8;
                                p+= step;
                                if (val > SHRT_MAX)
                                    val = SHRT_MAX;
                                else if (val < SHRT_MIN)
                                    val = SHRT_MIN;
                                out[out_idx] = val;
                                out_idx = (out_idx + 1) & out_mask;
                            }
                        }
                        else if (shm->samplebits == 8)// && !shm->signed8
                        {
                            unsigned char *out = (unsigned char *) pbuf;
                            while (count--)
                            {
                                //val = (*p) / 256;
                                val = (*p) >> 8;
                                p+= step;
                                if (val > SHRT_MAX)
                                    val = SHRT_MAX;
                                else if (val < SHRT_MIN)
                                    val = SHRT_MIN;
                                //out[out_idx] = (val / 256) + 128;
                                out[out_idx] = (val >> 8) + 128;
                                out_idx = (out_idx + 1) & out_mask;
                            }
                        }
                        /*
                         https://github.com/Novum/vkQuake
                        */
                        /*else if (shm->samplebits == 8) // S8 format, e.g. with Amiga AHI
                        {
                            unsigned char *out = (unsigned char *) pbuf;
                            while (count--)
                            {
                                val = (*p) / 256;
                                p+= step;
                                if (val > 0x7fff)
                                    val = 0x7fff;
                                else if (val < (short)0x8000)
                                    val = (short)0x8000;
                                out[out_idx] = (val / 256);
                                out_idx = (out_idx + 1) & out_mask;
                            }
                        }*/
                    }
                }
                paintedtime = end;
            }
        }
        // end S_PaintChannels
unlock:
        SND_UNLOCK
    }
    return NULL;
}

extern int sndBufferSize;
extern int sndDesiredSamples;

#include <assert.h>

void CheckAudioValue(float f)
{
    if (isnan(f) || isinf(f))
    {
        assert(0);
    }
    if (f < -1.1f)
    {
        assert(0);
    }
    if (f > 1.1f)
    {
        assert(0);
    }
}

/*
 https://github.com/Novum/vkQuake
*/
void WQAudioCallback(void *const *channelData, uint frameCount, uint channelCount, uint bits)
{
    unsigned char* ch0 = (unsigned char*)channelData[0];
    unsigned char* ch1 = (unsigned char*)channelData[1];

    int cached_samplepos = shm->samplepos;
    int cached_samplebits = shm->samplebits;
    int cached_bufferSize = sndBufferSize;
    unsigned char* srcBuffer = shm->buffer;

    int framebufferLength = frameCount;// sndDesiredSamples;

    int pos = cached_samplepos * (cached_samplebits / 8);
    if (pos >= cached_bufferSize)
    {
        shm->samplepos = cached_samplepos = pos = 0;
    }
    int toBuffend = cached_bufferSize - pos;
    int len = framebufferLength * (cached_samplebits / 8);
    int len1 = len;
    int len2 = 0;
    if (len1 > toBuffend)
    {
        len1 = toBuffend;
        len2 = len - len1;
    }
    float* src = __builtin_assume_aligned((float*)(srcBuffer + pos),16);
#ifdef DEBUG
    assert(AlignedCheck(src,16));
    if (shm->isFloat)
    {
        int len1Samples = len1 / (bits / 8);
        for (int i=0; i<len1Samples; i++)
        {
            CheckAudioValue(src[i]);
        }
    }
#else
#endif
    if (ch0)
    {
        memcpy(ch0,src,len1);
    }
    if (ch1)
    {
        memcpy(ch1,src,len1);
    }
    ch0 += len1;
    ch1 += len1;
    if (len2 <= 0)
    {
        shm->samplepos += len1 / (shm->samplebits / 8);
    }
    else
    {
        src = __builtin_assume_aligned((float*)(srcBuffer),16);
#ifdef DEBUG
        assert(AlignedCheck(src,16));
        if (shm->isFloat)
        {
            int len2Samples = len2 / (bits / 8);
            for (int i=0; i<len2Samples; i++)
            {
                CheckAudioValue(src[i]);
            }
        }
#else
#endif
        if (ch0)
        {
            memcpy(ch0,src,len2);
        }
        if (ch1)
        {
            memcpy(ch1,src,len2);
        }
        shm->samplepos = len2 / (shm->samplebits / 8);
    }
    if (shm->samplepos * (shm->samplebits / 8) >= cached_bufferSize)
    {
        shm->samplepos = 0;
    }
}
