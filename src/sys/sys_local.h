/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Tremulous.

Tremulous is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Tremulous is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Tremulous; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

#include "../qcommon/q_shared.h"
#include "../qcommon/qcommon.h"

// Input subsystem
void IN_Init (void);
void IN_Frame (void);
void IN_Shutdown (void);

// Console
void CON_Hide( void );
void CON_Show( void );
void CON_Shutdown( void );
void CON_Init( void );
char *CON_ConsoleInput(void);

#ifdef MACOS_X
char *Sys_StripAppBundle( char *pwd );
#endif

void Sys_SigHandler( int signal );
