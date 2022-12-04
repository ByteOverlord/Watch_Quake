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
// sound.h -- client sound i/o functions

#define PAINTBUFFER_SIZE 2048 //512

#define SND_THREADSAFE

#ifdef SND_THREADSAFE
#define SND_LOCK pthread_mutex_lock(&snd_lock);
#define SND_UNLOCK pthread_mutex_unlock(&snd_lock);
#define SND_TRYLOCK pthread_mutex_trylock(&snd_lock)
#else
#define SND_LOCK
#define SND_UNLOCK
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __SOUND__
#define __SOUND__

#define DEFAULT_SOUND_PACKET_VOLUME 255
#define DEFAULT_SOUND_PACKET_ATTENUATION 1.0

// !!! if this is changed, it much be changed in asm_i386.h too !!!
typedef struct
{
	int left;
	int right;
} portable_samplepair_t;

typedef struct
{
    float intensity;
    float alpha;
    float accum[2];
} underwater_t;

extern underwater_t underwater;

// !!! if this is changed, it much be changed in asm_i386.h too !!!
typedef struct
{
    int     length;
    int     loopstart;
    int     speed;
    int     width;
    int     stereo;
    byte    data[1];        // variable sized
} sfxcache_t;

typedef uint32_t u32;

typedef struct sfx_s
{
	//char name[MAX_QPATH];
	//cache_user_t cache;
    char* namePtr;
    u32 cacheHandle;
} sfx_t;

#define SNDCALL_TYPE_NULL 0
#define SNDCALL_TYPE_PLAY 1
#define SNDCALL_TYPE_STOP 2

typedef struct sndcall_s
{
    int type;
    // play, stop
    int entnum;
    int entchannel;
    // play
    sfx_t *sfx;
    vec3_t origin;
    float fvol;
    float attenuation;
} sndcall_t;

typedef struct
{
	//qboolean		gamealive;
	//qboolean		soundalive;
	//qboolean		splitbuffer;
    qboolean        isInterleaved;
    qboolean        isFloat;
	int				channels;
	int				samples;				// mono samples in buffer
	int				submission_chunk;		// don't mix less than this #
	int				samplepos;				// in mono samples
	int				samplebits;
	int				speed;
	unsigned char	*buffer;
} dma_t;

// !!! if this is changed, it much be changed in asm_i386.h too !!!
typedef struct
{
	sfx_t	*sfx;			// sfx number
	int		leftvol;		// 0-255 volume
	int		rightvol;		// 0-255 volume
	int		end;			// end time in global paintsamples
	int 	pos;			// sample position in sfx
	int		looping;		// where to loop, -1 = no looping
	int		entnum;			// to allow overriding a specific sound
	int		entchannel;		//
	vec3_t	origin;			// origin of sound effect
	vec_t	dist_mult;		// distance multiplier (attenuation/clipK)
	int		master_vol;		// 0-255 master volume
} channel_t;

typedef struct
{
	int		rate;
	int		width;
	int		channels;
	int		loopstart;
	int		samples;
	int		dataofs;		// chunk starts this many bytes from file start
} wavinfo_t;

#include "DefragAllocator.h"
extern DefragCache_t soundfx_cache;
void S_InitSoundFXMemory(u32 maxSfx, u32 sampleRate);

void S_Init (void);
void S_Startup (void);
void S_Shutdown (void);
void S_StartSound (int entnum, int entchannel, sfx_t *sfx, vec3_t origin, float fvol,  float attenuation);
void S_StaticSound (sfx_t *sfx, vec3_t origin, float vol, float attenuation);
void S_StopSound (int entnum, int entchannel);
void S_BatchProcessingCheck(void);
void S_StopAllSounds(qboolean clear);
void S_ClearBuffer (void);
void GetSoundtime(void);
void S_Update (vec3_t origin, vec3_t v_forward, vec3_t v_right, vec3_t v_up);
void S_ExtraUpdate (void);
void S_QWAudioUpdate (void);

sfx_t *S_PrecacheSound (char *sample);
void S_TouchSound (char *sample);
void S_ClearPrecache (void);
void S_BeginPrecaching (void);
void S_EndPrecaching (void);
void S_PaintChannels(int endtime);
void S_InitPaintChannels (void);

// picks a channel based on priorities, empty slots, number of channels
channel_t *SND_PickChannel(int entnum, int entchannel);

// spatializes a channel
void SND_Spatialize(channel_t *ch);

// initializes cycling through a DMA buffer and returns information on it
qboolean SNDDMA_Init(void);

// gets the current DMA position
int SNDDMA_GetDMAPos(void);

// shutdown the DMA xfer.
void SNDDMA_Shutdown(void);

// ====================================================================
// User-setable variables
// ====================================================================

#define SFX_VOLUME_MULTIPLIER 0.5
#define	MAX_CHANNELS			128 // was 128
#define	MAX_DYNAMIC_CHANNELS	8 // was 8


extern	channel_t   channels[MAX_CHANNELS];
// 0 to MAX_DYNAMIC_CHANNELS-1	= normal entity sounds
// MAX_DYNAMIC_CHANNELS to MAX_DYNAMIC_CHANNELS + NUM_AMBIENTS -1 = water, etc
// MAX_DYNAMIC_CHANNELS + NUM_AMBIENTS to total_channels = static sounds

extern	int			total_channels;

//
// Fake dma is a synchronous faking of the DMA progress used for
// isolating performance in the renderer.  The fakedma_updates is
// number of times S_Update() is called per second.
//

extern qboolean 		fakedma;
extern int 			fakedma_updates;
extern int		paintedtime;
extern vec3_t listener_origin;
extern vec3_t listener_forward;
extern vec3_t listener_right;
extern vec3_t listener_up;
extern volatile dma_t *shm;
extern volatile dma_t sn;
extern vec_t sound_nominal_clip_dist;


extern cvar_t sndspeed;
//extern cvar_t snd_mixspeed;
extern cvar_t snd_filterquality;

extern	cvar_t loadas8bit;
extern	cvar_t bgmvolume;
extern	cvar_t sfxvolume;

extern qboolean	snd_initialized;

extern int		snd_blocked;

void S_LocalSound (char *s);
sfxcache_t *S_LoadSound (sfx_t *s);

wavinfo_t GetWavinfo (char *name, byte *wav, int wavlength);

void SND_InitScaletable (void);
void SNDDMA_Submit(void);

void SND_OnChangeSfxVolume(cvar_t* var);

void S_TransferPaintBuffer(int endtime);

void S_AmbientOff (void);
void S_AmbientOn (void);
void S_SetUnderwaterIntensity(float);

typedef struct
{
    float *memory;     // kernelsize floats
    float *kernel;     // kernelsize floats
    int    kernelsize; // M+1, rounded up to be a multiple of 16
    int    M;          // M value used to make kernel, even
    int    parity;     // 0-3
    float  f_c;        // cutoff frequency, [0..1], fraction of sample rate
} filter_t;

void S_LowpassFilter (int *data, int stride, int count, filter_t *memory);
void S_UnderwaterFilter (int endtime);

#endif

#ifdef __cplusplus
}
#endif
