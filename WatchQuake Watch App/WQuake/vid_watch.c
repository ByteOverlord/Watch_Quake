//
//  vid_watch.c
//  WatchQuake Watch App
//
//  Created by ByteOverlord on 29.10.2022.
//

//
//  vid_osx.c
//  Quake_OSX
//
//  Created by Heriberto Delgado on 1/30/16.
//
//
// based on https://github.com/Izhido/Quake_For_OSX

#include "quakedef.h"
#include "d_local.h"

#include "Common.h"

unsigned char* g_DataImages[3] = {NULL,NULL,NULL};
u32* d_8to24tables[3] = {NULL,NULL,NULL};

byte* vid_buffer = NULL;
short* zbuffer = NULL;
byte* surfcache = NULL;

unsigned short d_8to16table[256];
u32* d_8to24table;

uint g_WQVidScreenWidth;
uint g_WQVidScreenHeight;

int g_QWPaletteCopied;

void VID_SetSize(int width, int height)
{
    D_FlushCaches();
    
    if (surfcache != NULL)
    {
        AlignedFree(surfcache);
    }
    
    g_WQVidScreenWidth = width;
    
    if (g_WQVidScreenWidth < 320)
    {
        g_WQVidScreenWidth = 320;
    }
    
    if (g_WQVidScreenWidth > 1280)
    {
        g_WQVidScreenWidth = 1280;
    }

    g_WQVidScreenHeight = height;
    
    if (g_WQVidScreenHeight < 240)
    {
        g_WQVidScreenHeight = 240;
    }
    
    if (g_WQVidScreenHeight > 960)
    {
        g_WQVidScreenHeight = 960;
    }

    vid.width = vid.conwidth = g_WQVidScreenWidth;
    vid.height = vid.conheight = g_WQVidScreenHeight;
    if (vid.height < vid.width)
    {
        vid.aspect = ((float)vid.height / (float)vid.width) * (320.0 / 240.0);
    }
    else
    {
        vid.aspect = ((float)vid.width / (float)vid.height) * (320.0 / 240.0);
    }

    vid.buffer = vid.conbuffer = vid_buffer;
    vid.rowbytes = vid.conrowbytes = g_WQVidScreenWidth;
    
    d_pzbuffer = zbuffer;
    
    int surfcachesize = D_SurfaceCacheForRes(g_WQVidScreenWidth, g_WQVidScreenHeight);
    
    surfcache = AlignedMalloc(surfcachesize,16);
    
    D_InitCaches (surfcache, surfcachesize);

    vid.recalc_refdef = 1;
}

void VID_SetPalette (unsigned char *palette)
{
    byte* pal = palette;
    u32* table = d_8to24table;
    for (int i=0 ; i<256 ; i++)
    {
        // 8 8 8 encoding
        *table++ = (255 << 24) | (((u32)pal[2]) << 16) | (((u32)pal[1]) << 8) | ((u32)pal[0]);
        pal += 3;
    }
    g_QWPaletteCopied = 1;
}

void VID_ShiftPalette (unsigned char *palette)
{
    VID_SetPalette(palette);
}

cvar_t vid_width = {"vid_width","368",CVAR_NONE};
cvar_t vid_height = {"vid_height","448",CVAR_NONE};
cvar_t vid_refreshrate = {"vid_refreshrate","60",CVAR_NONE};

void VID_Create(void)
{
    vid.maxwarpwidth = WARP_WIDTH;
    vid.maxwarpheight = WARP_HEIGHT;
    vid_width.value = g_WQVidScreenWidth;
    vid_height.value = g_WQVidScreenHeight;

    if (g_WQVidScreenWidth < 320)
    {
        g_WQVidScreenWidth = 320;
    }
    
    if (g_WQVidScreenWidth > 1280)
    {
        g_WQVidScreenWidth = 1280;
    }
    
    if (g_WQVidScreenHeight < 240)
    {
        g_WQVidScreenHeight = 240;
    }
    
    if (g_WQVidScreenHeight > 960)
    {
        g_WQVidScreenHeight = 960;
    }

    vid.width = vid.conwidth = g_WQVidScreenWidth;
    vid.height = vid.conheight = g_WQVidScreenHeight;

    if (vid.height < vid.width)
    {
        vid.aspect = ((float)vid.height / (float)vid.width) * (320.0 / 240.0);
    }
    else
    {
        vid.aspect = ((float)vid.width / (float)vid.height) * (320.0 / 240.0);
    }
    vid.rowbytes = vid.conrowbytes = g_WQVidScreenWidth;
    
    d_pzbuffer = zbuffer;
    
    int surfcachesize = D_SurfaceCacheForRes(g_WQVidScreenWidth, g_WQVidScreenHeight);
    
    surfcache = AlignedMalloc(surfcachesize,16);
    
    D_InitCaches (surfcache, surfcachesize);
}

void VID_SwapBuffer(int i)
{
    vid.buffer = vid.conbuffer = vid_buffer = g_DataImages[i];
    d_8to24table = d_8to24tables[i];
}

void VID_Init (unsigned char *palette)
{
    Cvar_RegisterVariable(&vid_width);
    Cvar_RegisterVariable(&vid_height);
    Cvar_RegisterVariable(&vid_refreshrate);

    uint maxImgSize = 1280 * 960;
    // triple buffering
    g_DataImages[0] = AlignedMalloc(maxImgSize,16);
    g_DataImages[1] = AlignedMalloc(maxImgSize,16);
    g_DataImages[2] = AlignedMalloc(maxImgSize,16);
    //
    zbuffer = AlignedMalloc(maxImgSize * sizeof(short),16);
    d_pzbuffer = zbuffer;
    for (int i=0; i<3; i++)
    {
        memset(g_DataImages[i],0,maxImgSize);
    }
    vid_buffer = (byte*)g_DataImages[0];
    vid.buffer = vid.conbuffer = vid_buffer;
    
    vid.numpages = 1;
    vid.colormap = host_colormap;
    vid.fullbright = 256 - LittleLong (*((int *)vid.colormap + 2048));

    d_8to24tables[0] = AlignedMalloc(256 * 3 * sizeof(u32),16);
    d_8to24tables[1] = d_8to24tables[0] + 256;
    d_8to24tables[2] = d_8to24tables[1] + 256;

    d_8to24table = d_8to24tables[0];

    VID_SetPalette(palette);
}

void VID_Shutdown (void)
{
    AlignedFree(surfcache);
    AlignedFree(zbuffer);
    AlignedFree(g_DataImages[2]);
    AlignedFree(g_DataImages[1]);
    AlignedFree(g_DataImages[0]);
    AlignedFree(d_8to24tables[2]);
    AlignedFree(d_8to24tables[1]);
    AlignedFree(d_8to24tables[0]);
    g_DataImages[0] = NULL;
    g_DataImages[1] = NULL;
    g_DataImages[2] = NULL;
}

void VID_Update (vrect_t *rects)
{
}

/*
 ================
 D_BeginDirectRect
 ================
 */
void D_BeginDirectRect (int x, int y, byte *pbitmap, int width, int height)
{
}


/*
 ================
 D_EndDirectRect
 ================
 */
void D_EndDirectRect (int x, int y, int width, int height)
{
}

