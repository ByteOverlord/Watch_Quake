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

#ifdef __cplusplus
extern "C" {
#endif

//
// the net drivers should just set the apropriate bits in m_activenet,
// instead of having the menu code look through their internal tables
//
#define	MNET_IPX		1
#define	MNET_TCP		2

extern	int	m_activenet;

//
// menus
//
void M_Init (void);
void M_Keydown (int key);
void M_Draw (void);
void M_ToggleMenu_f (void);

void M_SaveSettings(void);

int M_IsCheatActive(int i);
qboolean M_IsInMapSelect(void);
const char* M_GetSelectedMapName(void);

typedef void (*OnMenuNavigate)(void);
typedef void (*OnMenuBenchmark)(const char*,int,int);

typedef void (*HostSpawnCallback)(void);

extern OnMenuBenchmark M_BenchmarkBegin;
extern OnMenuNavigate M_BenchmarkClear;
extern OnMenuNavigate M_EnterMapsFunc;
extern OnMenuNavigate M_ExitMapsFunc;
extern OnMenuNavigate M_SelectMapsFunc;
extern HostSpawnCallback M_OnHostSpawn;

#ifdef __cplusplus
}
#endif
