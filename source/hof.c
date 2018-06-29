/******************************************************************************
BINIAX HALL OF FAME IMPLEMENTATIONS
COPYRIGHT JORDAN TUZSUZOV, (C) 2006-2009

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

LICENSE ORIGIN : http://www.gzip.org/zlib/zlib_license.html

For complete product license refer to LICENSE.TXT file

******************************************************************************/

/******************************************************************************
INCLUDES
******************************************************************************/
#include <string.h>

#include "inc.h"

#define chCursor		'_'			/* Cursor ON */
#define chSpace			' '			/* Cursor OFF*/

#define csHOFName		"/apps/biniax/data/hof.bnx2"	/* File name */
#define cHOFFileSize	504			/* File size */

BNX_HALL	Hof;

int indexLetter=0;
BNX_INT16		curPos = 0;

/******************************************************************************
FUNCTIONS
******************************************************************************/
void hofAddLetter( BNX_INT16 pos, char a, char *name );
void hofBlinkCursor( BNX_INT16 pos, char *name );
void hofResetCursor( BNX_INT16 pos, char *name );


void hofAddLetter( BNX_INT16 pos, char a, char *name )
{
	if ( pos < cHofNameLen-1 )
	{
		name[ pos ] = a;
	}
}

void hofBlinkCursor( BNX_INT16 pos, char *name )
{
	static BNX_INT16 delayer = 0;

	if ( pos < cHofNameLen-1 )
	{
		if ( (delayer & 1) == 0 )
		{
			if ( name[ pos ] == chSpace )
			{
				name[ pos ] = chCursor;
			}
			else
			{
				name[ pos ] = chSpace;
			}
			delayer = 0;
		}
		delayer ++;
	}
}

void hofResetCursor( BNX_INT16 pos, char *name )
{
	if ( pos < cHofNameLen-1 )
	{
		name[ pos ] = chSpace;
	}
}


BNX_BOOL hofInit()
{
	FILE		*file;
	BNX_INT16	i, j;
	

	for ( i = 0; i < cHofEntries; ++i )
	{
		strcpy( Hof.arcade[ i ].name, "wiimpathy                    " );
		Hof.arcade[ i ].score = (cHofEntries - i) * cHofInitScore;

		strcpy( Hof.tactic[ i ].name, "JORDAN                       " );
		Hof.tactic[ i ].score = (cHofEntries - i) * cHofInitScore;
	}

	if ( sysGetFileLen( sysGetFullFileName( csHOFName ) ) != cHOFFileSize )
		return BNX_FALSE;
	file = fopen( sysGetFullFileName( csHOFName ), "rb" );
	if ( file == (FILE *) NULL )
		return BNX_FALSE;

	for ( i = 0; i < cHofEntries; ++i )
	{
		for ( j = 0; j < cHofNameLen; ++j )
		{
			Hof.arcade[ i ].name[ j ] = sysFGet8( file );
		}
		sysFGet2byte( file );
		Hof.arcade[ i ].score = sysFGet32( file );
	}

	for ( i = 0; i < cHofEntries; ++i )
	{
		for ( j = 0; j < cHofNameLen; ++j )
		{
			Hof.tactic[ i ].name[ j ] = sysFGet8( file );
		}
		sysFGet2byte( file );
		Hof.tactic[ i ].score = sysFGet32( file );
	}

	fclose( file );

	return BNX_TRUE;
}

BNX_BOOL hofSave()
{
	FILE		*file;
	int			i, j;

	file = fopen( sysGetFullFileName( csHOFName ), "wb" );

	if ( file == (FILE *) NULL )
		return BNX_FALSE;

	for ( i = 0; i < cHofEntries; ++i )
	{
		for ( j = 0; j < cHofNameLen; ++j )
		{
			sysFPut8( Hof.arcade[ i ].name[ j ], file );
		}
		sysFPut2byte( file );
		sysFPut32( Hof.arcade[ i ].score, file );
	}

	for ( i = 0; i < cHofEntries; ++i )
	{
		for ( j = 0; j < cHofNameLen; ++j )
		{
			sysFPut8( Hof.tactic[ i ].name[ j ], file );
		}
		sysFPut2byte( file );
		sysFPut32( Hof.tactic[ i ].score, file );
	}

	fclose( file );

	return BNX_TRUE;
}

static void pushKeyboardEvent( SDLKey key, BNX_BOOL pressed )
{
  SDL_Event event;
  event.type = pressed ? SDL_KEYDOWN : SDL_KEYUP;
  event.key.state = pressed ? SDL_PRESSED : SDL_RELEASED;
  event.key.keysym.scancode = 0;
  event.key.keysym.sym = key;
  event.key.keysym.unicode = 0;
  SDL_PushEvent( &event );
}

/* Assign keys to pads and button in order to write name */
void joyTokey()
{
    SDL_Event	event;
	int			x, y;
	SDLKey letter[] = {SDLK_a, SDLK_b, SDLK_c, SDLK_d, SDLK_e, SDLK_f, SDLK_g, SDLK_h, SDLK_i, SDLK_k, SDLK_l,
  					   SDLK_m, SDLK_n, SDLK_o, SDLK_p, SDLK_q, SDLK_r, SDLK_s, SDLK_t, SDLK_u, SDLK_v, SDLK_w,
				       SDLK_x, SDLK_y, SDLK_z }; 

    while( SDL_PollEvent( &event ) ) 
	{
        switch( event.type ) 
		{
			case SDL_JOYHATMOTION:

				/* Wiimote 1 Classic Controller 1 Pads */
				if (event.jhat.which == 0)
				{
					if ( event.jhat.value == SDL_HAT_UP )
					{
						if (indexLetter > 24)
							indexLetter=0;
						pushKeyboardEvent( letter[indexLetter], 1 );
						if ( curPos > 0 )
						{
							curPos--;
						}
						indexLetter++;					
					}						
					else if ( event.jhat.value == SDL_HAT_DOWN )
					{	
						indexLetter--;
						if (indexLetter < 0)
							indexLetter=24;
						pushKeyboardEvent( letter[indexLetter], 1 );
						if ( curPos > 0 )
						{
							curPos--;
						}								
					}
					else if ( event.jhat.value == SDL_HAT_LEFT )
					{	
						indexLetter=0;
						pushKeyboardEvent( SDLK_BACKSPACE, 1 );
					}
					else if ( event.jhat.value == SDL_HAT_RIGHT )
					{
						indexLetter=0;
						curPos++;	
						pushKeyboardEvent( SDLK_a, 1 );		
						curPos--;
					}
				}
				/* Gamecube 1 Pads */
				if (event.jhat.which == 4)
				{
					if ( event.jhat.value == SDL_HAT_UP )
					{
						if (indexLetter > 24)
							indexLetter=0;
						pushKeyboardEvent( letter[indexLetter], 1 );
						if ( curPos > 0 )
						{
							curPos--;
						}
						indexLetter++;					
					}						
						else if ( event.jhat.value == SDL_HAT_DOWN )
					{	
						indexLetter--;
						if (indexLetter < 0)
							indexLetter=24;
						pushKeyboardEvent( letter[indexLetter], 1 );
						if ( curPos > 0 )
						{
							curPos--;
						}								
					}
						else if ( event.jhat.value == SDL_HAT_LEFT )
					{	
						indexLetter=0;
						pushKeyboardEvent( SDLK_BACKSPACE, 1 );
					}
						else if ( event.jhat.value == SDL_HAT_RIGHT )
					{
						indexLetter=0;
						curPos++;	
						pushKeyboardEvent( SDLK_a, 1 );		
						curPos--;
					}
				}
			break;

			case SDL_JOYBUTTONDOWN:
					if ( event.jbutton.button==0 || event.jbutton.button==9 )
						_Inp.keyA = BNX_TRUE;
			break;

			case SDL_KEYDOWN:
				switch( event.key.keysym.sym ) 
				{
					case SDLK_BACKSPACE :
					case SDLK_DELETE :
						_Inp.keyDel = BNX_TRUE;
						break;

				}
				if ( event.key.keysym.sym >= SDLK_a && event.key.keysym.sym <= SDLK_z )
				{
					_Inp.letter = (event.key.keysym.sym - SDLK_a) + 'A';
				}
				else if ( event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9 )
				{
					_Inp.letter = (event.key.keysym.sym - SDLK_0) + '0';
				}
				else if ( event.key.keysym.sym == SDLK_SPACE )
				{
					_Inp.letter = ' ';
				}
			break;


			case SDL_QUIT:
				SDL_Quit();
				exit( 2 );
			break;

		}
	}	

}

BNX_BOOL hofEnter( BNX_GAME *game )
{
	BNX_INT32		startTime;
	BNX_INT16		viewoption = cGfxHofNoKeyboard;
	//BNX_INT16		curPos = 0;
	BNX_INT16		i, j;
	char			cChar = 0;
	BNX_HALL_ENTRY	*recEntry = 0;
	curPos = 0;

	switch ( game->mode )
	{
		case cModeRealtime:
			for ( i = 0; i < cHofEntries; ++i )
			{
				if ( Hof.arcade[ i ].score < game->score[ cPlayer1 ] )
				{
					recEntry = &Hof.arcade[ i ];
					viewoption = cGfxHofKeyboardDn;
					break;
				}
			}
			for ( j = cHofEntries-1; j > i; --j )
			{
				strcpy( Hof.arcade[ j ].name, Hof.arcade[ j-1 ].name );
				Hof.arcade[ j ].score = Hof.arcade[ j-1 ].score;
			}
			break;
		case cModeTurn:
			for ( i = 0; i < cHofEntries; ++i )
			{
				if ( Hof.tactic[ i ].score < game->score[ cPlayer1 ] )
				{
					recEntry = &Hof.tactic[ i ];
					viewoption = cGfxHofKeyboardUp;
					break;
				}
			}
			for ( j = cHofEntries-1; j > i; --j )
			{
				strcpy( Hof.tactic[ j ].name, Hof.tactic[ j-1 ].name );
				Hof.arcade[ j ].score = Hof.tactic[ j-1 ].score;
			}
			break;
		default:
			return BNX_TRUE;
	}

	if ( recEntry == 0 )
	{
		return BNX_FALSE;
	}

	strcpy( recEntry->name, "                             " );
	recEntry->score = game->score[ cPlayer1 ];
	inpInit();
	do
	{
		startTime = sysGetTime();

		gfxGetVirtualChar( game, inpDirect() );
		cChar = inpGetChar();
		if ( cChar > 0 )
		{
			hofAddLetter( curPos, cChar, recEntry->name );
			if ( curPos < cHofNameLen - 1 )
			{
				curPos++;
			}
		}
		if ( inpKeyDel() == BNX_TRUE )
		{
			hofResetCursor( curPos, recEntry->name );
			if ( curPos > 0 )
			{
				curPos--;
			}
		}
		hofBlinkCursor( curPos, recEntry->name );

		joyTokey();
		//inpUpdate();
		gfxRenderHof( &Hof, viewoption );
		gfxUpdate();
		sndUpdate();
		sysUpdate();

		// Synchronize with the clock
		while ( sysGetTime() - startTime < cDeltaTime )
		{
			sysUpdate();
		}
	}
	while ( inpKeyA() == BNX_FALSE && inpKeyB() == BNX_FALSE );	
	hofResetCursor( curPos, recEntry->name );

	return BNX_TRUE;
}

void hofView()
{
	BNX_INT32	startTime;

	inpInit();
	do
	{
		startTime = sysGetTime();

		inpUpdate();
		gfxGetHelpPen( inpDirect() );
		gfxRenderHof( &Hof, cGfxHofNoKeyboard );
		gfxUpdate();
		sndUpdate();
		sysUpdate();

		// Synchronize with the clock
		while ( sysGetTime() - startTime < cDeltaTime )
		{
			sysUpdate();
		}
	}
	while ( inpKeyA() == BNX_FALSE );
}

BNX_HALL *hofGet()
{
	return (BNX_HALL *) &Hof;
}
