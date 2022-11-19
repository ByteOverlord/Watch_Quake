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

#include "quakedef.h"

#ifdef _WIN32
#include "winquake.h"
#else
#define DWORD	unsigned long
#endif

#include <pthread/pthread.h>

extern pthread_mutex_t snd_lock;

portable_samplepair_t paintbuffer[PAINTBUFFER_SIZE];

int		snd_scaletable[32][256];
int 	*snd_p, snd_linear_count, snd_vol;
short	*snd_out;

void Snd_WriteLinearBlastStereo16 (void);

#if	!id386
void Snd_WriteLinearBlastStereo16 (void)
{
	int		i;
	int		val;

	for (i=0 ; i<snd_linear_count ; i+=2)
	{
		//val = (snd_p[i]*snd_vol)>>8;
        val = snd_p[i] / 256;
		if (val > 0x7fff)
			snd_out[i] = 0x7fff;
		else if (val < (short)0x8000)
			snd_out[i] = (short)0x8000;
		else
			snd_out[i] = val;

		//val = (snd_p[i+1]*snd_vol)>>8;
        val = snd_p[i+1] / 256;
		if (val > 0x7fff)
			snd_out[i+1] = 0x7fff;
		else if (val < (short)0x8000)
			snd_out[i+1] = (short)0x8000;
		else
			snd_out[i+1] = val;
	}
}
#endif

void S_TransferStereo16 (int endtime)
{
	int		lpos;
	int		lpaintedtime;
	DWORD	*pbuf;
#ifdef _WIN32
	int		reps;
	DWORD	dwSize,dwSize2;
	DWORD	*pbuf2;
	HRESULT	hresult;
#endif
	
	//snd_vol = volume.value*256;

	snd_p = (int *) paintbuffer;
	lpaintedtime = paintedtime;

#ifdef _WIN32
	if (pDSBuf)
	{
		reps = 0;

		while ((hresult = pDSBuf->lpVtbl->Lock(pDSBuf, 0, gSndBufSize, &pbuf, &dwSize, 
									   &pbuf2, &dwSize2, 0)) != DS_OK)
		{
			if (hresult != DSERR_BUFFERLOST)
			{
				Con_Printf ("S_TransferStereo16: DS::Lock Sound Buffer Failed\n");
				S_Shutdown ();
				S_Startup ();
				return;
			}

			if (++reps > 10000)
			{
				Con_Printf ("S_TransferStereo16: DS: couldn't restore buffer\n");
				S_Shutdown ();
				S_Startup ();
				return;
			}
		}
	}
	else
#endif
	{
		pbuf = (DWORD *)shm->buffer;
	}

	while (lpaintedtime < endtime)
	{
	// handle recirculating buffer issues
		lpos = lpaintedtime & ((shm->samples>>1)-1);

		snd_out = (short *) pbuf + (lpos<<1);

		snd_linear_count = (shm->samples>>1) - lpos;
		if (lpaintedtime + snd_linear_count > endtime)
			snd_linear_count = endtime - lpaintedtime;

		snd_linear_count <<= 1;

	// write a linear blast of samples
		Snd_WriteLinearBlastStereo16 ();

		snd_p += snd_linear_count;
		lpaintedtime += (snd_linear_count>>1);
	}

#ifdef _WIN32
	if (pDSBuf)
		pDSBuf->lpVtbl->Unlock(pDSBuf, pbuf, dwSize, NULL, 0);
#endif
}

#define SHRT_MIN -32768
#define SHRT_MAX 32767

void S_TransferPaintBuffer(int endtime)
{
	int 	out_idx;
	int 	count;
	int 	out_mask;
	int 	*p;
	int 	step;
	int		val;
	//int		snd_vol;
	DWORD	*pbuf;
#ifdef _WIN32
	int		reps;
	DWORD	dwSize,dwSize2;
	DWORD	*pbuf2;
	HRESULT	hresult;
#endif

    /*if (shm->samplebits == 32 && shm->channels == 2)
    {
        S_TransferStereo32(endtime);
        return;
    }*/
	if (shm->samplebits == 16 && shm->channels == 2)
	{
		S_TransferStereo16 (endtime);
		return;
	}
	
	p = (int *) paintbuffer;
	count = (endtime - paintedtime) * shm->channels;
	out_mask = shm->samples - 1; 
	out_idx = paintedtime * shm->channels & out_mask;
	step = 3 - shm->channels;
	//snd_vol = volume.value*256;

#ifdef _WIN32
	if (pDSBuf)
	{
		reps = 0;

		while ((hresult = pDSBuf->lpVtbl->Lock(pDSBuf, 0, gSndBufSize, &pbuf, &dwSize, 
									   &pbuf2,&dwSize2, 0)) != DS_OK)
		{
			if (hresult != DSERR_BUFFERLOST)
			{
				Con_Printf ("S_TransferPaintBuffer: DS::Lock Sound Buffer Failed\n");
				S_Shutdown ();
				S_Startup ();
				return;
			}

			if (++reps > 10000)
			{
				Con_Printf ("S_TransferPaintBuffer: DS: couldn't restore buffer\n");
				S_Shutdown ();
				S_Startup ();
				return;
			}
		}
	}
	else
#endif
	{
		pbuf = (DWORD *)shm->buffer;
	}

    if (shm->samplebits == 32)
    {
        float* out = (float*) pbuf;
        while (count--)
        {
            val = (*p) / 256;
            p+= step;
            if (val > SHRT_MAX)
                val = SHRT_MAX;
            else if (val < SHRT_MIN)
                val = SHRT_MIN;
            out[out_idx] = val * (1.0f / 32768.0);
            out_idx = (out_idx + 1) & out_mask;
        }
    }
	else if (shm->samplebits == 16)
	{
		short *out = (short *) pbuf;
		while (count--)
		{
			val = (*p) / 256;
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
			val = (*p) / 256;
			p+= step;
            if (val > SHRT_MAX)
                val = SHRT_MAX;
            else if (val < SHRT_MIN)
                val = SHRT_MIN;
			out[out_idx] = (val / 256) + 128;
			out_idx = (out_idx + 1) & out_mask;
		}
	}
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

#ifdef _WIN32
	if (pDSBuf) {
		DWORD dwNewpos, dwWrite;
		int il = paintedtime;
		int ir = endtime - paintedtime;
		
		ir += il;

		pDSBuf->lpVtbl->Unlock(pDSBuf, pbuf, dwSize, NULL, 0);

		pDSBuf->lpVtbl->GetCurrentPosition(pDSBuf, &dwNewpos, &dwWrite);

//		if ((dwNewpos >= il) && (dwNewpos <= ir))
//			Con_Printf("%d-%d p %d c\n", il, ir, dwNewpos);
	}
#endif
}

/*
===============================================================================
UNDERWATER EFFECT
===============================================================================
*/

/*
 UnderwaterFilter from vkQuake
 https://github.com/Novum/vkQuake
*/

#undef min
#undef max

// clang-format off
#define GENERIC_TYPES(x, separator) \
    x(int, i) separator \
    x(unsigned int, u) separator \
    x(long, l) separator \
    x(unsigned long, ul) separator \
    x(long long, ll) separator \
    x(unsigned long long, ull) separator \
    x(float, f) separator \
    x(double, d)

#define COMMA ,
#define NO_COMMA

#define IMPL_GENERIC_FUNCS(type, suffix) \
static inline type q_min_##suffix (type a, type b) \
{ \
    return (a < b) ? a : b; \
} \
static inline type q_max_##suffix (type a, type b) \
{ \
    return (a > b) ? a : b; \
} \
static inline type clamp_##suffix (type minval, type val, type maxval) \
{ \
    return (val < minval) ? minval : ((val > maxval) ? maxval : val); \
}

GENERIC_TYPES (IMPL_GENERIC_FUNCS, NO_COMMA)

#define SELECT_Q_MIN(type, suffix) type: q_min_##suffix
#define q_min(a, b) _Generic((a) + (b), GENERIC_TYPES (SELECT_Q_MIN, COMMA))(a, b)

#define SELECT_Q_MAX(type, suffix) type: q_max_##suffix
#define q_max(a, b) _Generic((a) + (b), GENERIC_TYPES (SELECT_Q_MAX, COMMA))(a, b)

#define SELECT_CLAMP(type, suffix) type: clamp_##suffix
#define CLAMP(minval, val, maxval) _Generic((minval) + (val) + (maxval), \
    GENERIC_TYPES (SELECT_CLAMP, COMMA))(minval, val, maxval)
// clang-format on

underwater_t underwater = {0.f, 1.f, {0.f, 0.f}};

extern cvar_t snd_waterfx;

void S_SetUnderwaterIntensity (float target)
{
    target *= CLAMP (0.f, snd_waterfx.value, 2.f);
    if (underwater.intensity < target)
    {
        underwater.intensity += host_frametime * 4.f;
        underwater.intensity = q_min (underwater.intensity, target);
    }
    else if (underwater.intensity > target)
    {
        underwater.intensity -= host_frametime * 4.f;
        underwater.intensity = q_max (underwater.intensity, target);
    }
    underwater.alpha = expf(-underwater.intensity * logf(12.f));
}

void S_UnderwaterFilter (int endtime)
{
    int i;
    if (!underwater.intensity)
    {
        if (endtime > 0)
        {
            underwater.accum[0] = paintbuffer[endtime - 1].left;
            underwater.accum[1] = paintbuffer[endtime - 1].right;
        }
        return;
    }
    for (i = 0; i < endtime; i++)
    {
        underwater.accum[0] += underwater.alpha * (paintbuffer[i].left - underwater.accum[0]);
        underwater.accum[1] += underwater.alpha * (paintbuffer[i].right - underwater.accum[1]);
        paintbuffer[i].left = (int)underwater.accum[0];
        paintbuffer[i].right = (int)underwater.accum[1];
    }
}

/*
===============================================================================

CHANNEL MIXING

===============================================================================
*/

void SND_PaintChannelFrom8 (channel_t *ch, sfxcache_t *sc, int endtime, int paintbufferstart);
void SND_PaintChannelFrom16 (channel_t *ch, sfxcache_t *sc, int endtime, int paintbufferstart);

void S_PaintChannels(int endtime)
{
	int 	i;
	int 	end;
	channel_t *ch;
	sfxcache_t	*sc;
	int		ltime, count;

    snd_vol = sfxvolume.value * 256;

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
            sc = ch->sfx->cacheForMixer;//S_LoadSound (ch->sfx);
			if (!sc)
				continue;

			ltime = paintedtime;

			while (ltime < end)
			{	// paint up to end
				if (ch->end < end)
					count = ch->end - ltime;
				else
					count = end - ltime;

				if (count > 0)
				{	
					if (sc->width == 1)
						SND_PaintChannelFrom8(ch, sc, count,ltime - paintedtime);
					else
						SND_PaintChannelFrom16(ch, sc, count,ltime - paintedtime);
	
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
					{	// channel just stopped
						ch->sfx = NULL;
						break;
					}
				}
			}
		}
        // clip each sample to 0dB, then reduce by 6dB (to leave some headroom for
        // the lowpass filter and the music). the lowpass will smooth out the
        // clipping
        /*for (i = 0; i < end - paintedtime; i++)
        {
            paintbuffer[i].left = CLAMP (-32768 * 256, paintbuffer[i].left, 32767 * 256) / 2;
            paintbuffer[i].right = CLAMP (-32768 * 256, paintbuffer[i].right, 32767 * 256) / 2;
        }*/

        // apply a lowpass filter
        //if (sndspeed.value == 11025 && shm->speed == 44100)
        /*if (sndspeed.value == 11025 && shm->speed == 44100)
        {
            static filter_t memory_l, memory_r;
            S_LowpassFilter ((int *)paintbuffer, 2, end - paintedtime, &memory_l);
            S_LowpassFilter (((int *)paintbuffer) + 1, 2, end - paintedtime, &memory_r);
        }*/
        S_UnderwaterFilter (end - paintedtime);
	// transfer out according to DMA format
		S_TransferPaintBuffer(end);
		paintedtime = end;
	}
}

void SND_InitScaletable (void)
{
	int	i, j;
    int scale;
	for (i=0 ; i<32 ; i++)
    {
        scale = i * 8 * 256 * sfxvolume.value * SFX_VOLUME_MULTIPLIER;
        for (j=0 ; j<256 ; j++)
        {
            snd_scaletable[i][j] = ((j < 128) ? j : j - 256) * scale;
        }
    }
}

void SND_OnChangeSfxVolume(cvar_t* var)
{
    SND_LOCK
    SND_InitScaletable();
    SND_UNLOCK
}

#if	!id386

void SND_PaintChannelFrom8 (channel_t *ch, sfxcache_t *sc, int count, int paintbufferstart)
{
	int 	data;
	int		*lscale, *rscale;
	unsigned char *sfx;
	int		i;

	if (ch->leftvol > 255)
		ch->leftvol = 255;
	if (ch->rightvol > 255)
		ch->rightvol = 255;
		
	lscale = snd_scaletable[ch->leftvol >> 3];
	rscale = snd_scaletable[ch->rightvol >> 3];
    sfx = (unsigned char *)sc->data + ch->pos;
	//sfx = (signed char *)sc->data + ch->pos;

    portable_samplepair_t* pb = paintbuffer + paintbufferstart;
	for (i=0 ; i<count ; i++)
	{
		data = sfx[i];
		pb[i].left += lscale[data];
		pb[i].right += rscale[data];
	}
	
	ch->pos += count;
}

#endif	// !id386


void SND_PaintChannelFrom16 (channel_t *ch, sfxcache_t *sc, int count, int paintbufferstart)
{
	int data;
	int left, right;
	int leftvol, rightvol;
	signed short *sfx;
	int	i;

	leftvol = ch->leftvol * snd_vol;
	rightvol = ch->rightvol * snd_vol;
    // new
    leftvol /= 256;
    rightvol /= 256;
    //
	sfx = (signed short *)sc->data + ch->pos;

    portable_samplepair_t* pb = paintbuffer + paintbufferstart;
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

