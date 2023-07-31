
// Includes.
#include <stdlib.h>
#include <unistd.h>
#include "SDL.h"
#include "SDL_opengl.h"
#include <math.h>
#include <time.h>
#include <assert.h>

#include "ctypes.h"

#include "psd.h"
#include "preca.h"
#include "render.h"
#include "frame.h"
#include "sfx.h"
#include "scroll.h"
#include "loader.h"
#include "mst.h"
#include "monsters.h"
#include "menu.h"
#include "roto.h"
#include "font.h"
#include "sprcache.h"
#include "sprites.h"
#include "sprrz.h"
#include "spritesdef.h"
#include "animspr.h"
#include "anims.h"
#include "game.h"

//=============================================================================

#define	ABS(x)	((x) < 0 ? -(x) : (x))
#define	SGN(x)	((x) >= 0 ? 1 : -1)
#define	MIN(x,y) ((x) < (y) ? (x) : (y))
#define NBELEM(tab) (sizeof(tab) / sizeof(tab[0]))

// Define.
#define SCR_Width	(320)
#define SCR_Height	(224)

enum
{
	e_Color_HUDBkg = 0,
	e_Color_AreaBlue,
	e_Color_AreaGray,
	e_Color_Black,
	e_Color_White,
	e_Color_Gray,
	e_Color_Cyan,
	e_Color_Yellow,
	e_Color_CyanDark,
	e_Color_Red,
	e_Color_Green,

	e_Color_MAX
};


// Types de variables.
struct SGene
{
	SDL_Surface *pScreen;	// Ptr sur le buffer écran.

	SDL_Surface	*pImgRoto;	// Image pour le rotozoom.
	SDL_Surface	*pImgLogo;	// Logo.

	u8	*pKeysSDL;		// Pointeur sur le buffer clavier de la SDL.
	u8	pKeys[SDLK_LAST];	// Buffer clavier.

	SDL_Joystick	*pJoystick;
	u16	nJoystickState;		// 8 bits pour la croix (la fct de Hat renvoie un u8) et les boutons sur les 8 bits de poids fort.
	u8	nJoystickNoHat;		// Pas de croix ?
	u8	nJoystickNoAxes;	// Pas de stick analogique ?

	s16	*pSin;			// Ptrs sur les tables.
	s16	*pCos;

	u16	pnColors[e_Color_MAX];		// Couleurs en 16 bits.

};

//=============================================================================

#define	CHEATCODES_NB		(2)
#define	CHEATCODES_TBSZMAX	(10)
#define	CHEATCODES_TIMER	(24)
struct SCheatCodes
{
	u8	pTb[CHEATCODES_TBSZMAX + 1];
	u8	nIdx;		// Index case en cours.
	u8	nCnt;		// Compteur pour reset idx.
	u8	nEnter;		// Mode entrée.
	u8	nLevel;		// Level en cours pour selecteur de niveau.

	u8	nCheat;		// Les cheats à tester.
};

enum
{
	e_Cheat_LevelSelect		= 1 << 0,
	e_Cheat_UnlimitedTime	= 1 << 1,
};

//=============================================================================

// Variables générales.
extern struct SGene gVar;
extern struct SCheatCodes	gCCodes;


