
// Metro-Cross.
// Code: Clement '17o2!!' CORDE
// Contact: Clement CORDE, c1702@yahoo.com

// Using :
// ST-Sound by Leonard/OXG.
// Atari ST Musics and 8x8 font.
// Arcade graphics.


//#define DEBUG_KEYS	(1)		// A commenter.
#define	EMERGENCY_EXIT	(1)	// Sortie du jeu avec Esc.

#include "includes.h"
#include "sprites_inc.h"


// Variables générales.
struct SGene gVar;

//=============================================================================

/*
#if defined(__LINUX__) || defined(__APPLE__)
#include <strings.h>
// stricmp n'existe pas en Linux : C'est strcasecmp à la place, dans strings.h.
int stricmp(char *pStr1, char *pStr2)
{
	return (strcasecmp(pStr1, pStr2));
}
#endif
*/

//=============================================================================

struct SCheatCodes	gCCodes;

// Cheat codes - Reset de la saisie.
void CheatCodes_Reset(void)
{
	gCCodes.nIdx = 0;
	gCCodes.nCnt = 0;
	gCCodes.nEnter = 0;
}

// Cheat codes - Compteur.
void CheatCodes_Counter(void)
{
	if (gCCodes.nCnt == 0) return;
	if (--gCCodes.nCnt == 0) CheatCodes_Reset();
}

// Les codes en version non lisible avec un éditeur hexa. (Chr majuscules : 0x40 > 0x5F).
u8 gpCodes[CHEATCODES_NB][CHEATCODES_TBSZMAX + 1] =
{
	{ 'L' ^ 0x1F, 'E' ^ 0x2E, 'V' ^ 0x3D, 'E' ^ 0x0C, 'L' ^ 0x1B, 'S' ^ 0x2A, 0, 0, 0, 0, 0 },				// Level select.
	{ 'N' ^ 0x1F, 'O' ^ 0x2E, 'T' ^ 0x3D, 'I' ^ 0x0C, 'M' ^ 0x1B, 'E' ^ 0x2A, 0, 0, 0, 0, 0 },				// Unlimited time.
};
u8 gpCodesOR[CHEATCODES_NB] =
	{ e_Cheat_LevelSelect, e_Cheat_UnlimitedTime };

// Cheat codes - Touche pressée.
void CheatCodes_KeyPressed(u32 nSdlKey)
{
	// On est en mode entrée de cheat ?
	if (gCCodes.nEnter == 0)
	{	// Non. On y passe ?
		if (nSdlKey == SDLK_F5) gCCodes.nEnter = 1;
		return;
	}
	gVar.pKeys[nSdlKey] = 0;	// RAZ touche.

	// Appui.
	if (gCCodes.nIdx >= CHEATCODES_TBSZMAX - 1) return;
	if (nSdlKey < SDLK_a || nSdlKey > SDLK_z) return;
	nSdlKey -= SDLK_a;
	nSdlKey += 'A';
	nSdlKey ^= (((gCCodes.nIdx + 1) * 0x10) & 0x30) | (0x0F - gCCodes.nIdx);	// On code ici comme le cheat la lettre entrée.
	gCCodes.pTb[gCCodes.nIdx++] = (char)nSdlKey;
	gCCodes.pTb[gCCodes.nIdx] = 0;		// Arrête la chaîne au suivant.
	gCCodes.nCnt = CHEATCODES_TIMER;	// Timer.

	// Test du code.
	u32	i;
	for (i = 0; i < CHEATCODES_NB; i++)
	{
		if (memcmp(gCCodes.pTb, &gpCodes[i][0], strlen((char *)&gpCodes[i][0])) == 0)
		{
			gCCodes.nCheat ^= gpCodesOR[i];
			CheatCodes_Reset();
			Sfx_PlaySfx(e_Sfx_Cracker, e_SfxPrio_0);	// Sfx.
			return;
		}
	}

}

// Affichage des cheat codes (sur la page principale).
void CheatCodes_Display(void)
{
	char	*ppTxt[CHEATCODES_NB] = { "LVL", "TIM" };
	char	pStr[((CHEATCODES_NB + 1) * 4) + 1];	// (CHEATCODES_NB + 1) pour l'affichage du numéro du level.
	u32	nIdx = 0;
	u32	i;

	if (gCCodes.nCheat == 0) return;

	pStr[nIdx] = 0;
	for (i = 0; i < CHEATCODES_NB; i++)
	{
		if (gCCodes.nCheat & (1 << i))
		{
			// On copie le code du cheat sur 3 lettres.
			if (nIdx) pStr[nIdx++] = '-';
			pStr[nIdx] = *(ppTxt[i]);
			pStr[nIdx+1] = *(ppTxt[i] + 1);
			pStr[nIdx+2] = *(ppTxt[i] + 2);
			nIdx += 3;
			pStr[nIdx] = 0;
			// Cas spécial du choix du level qu'on ajoute au cul de LVL : "LVL(xx)".
			if (i == 0)
			{
				pStr[nIdx] = '(';
				snprintf(&pStr[nIdx+1], 3, "%02d", gCCodes.nLevel + 1);
				pStr[nIdx+3] = ')';
				nIdx += 4;
				pStr[nIdx] = 0;
			}
		}
	}
	if (nIdx)
		Font8_Print(0, SCR_Height-8, pStr, gVar.pnColors[e_Color_Yellow]);

}

// Level selector si cheat code.
void CheatCodes_LevelSelector(void)
{
	if (gCCodes.nCheat & e_Cheat_LevelSelect)
	{
		if (gVar.pKeys[gMetroCfg.pKeys[e_CfgKey_Right]])
		{
			if (gCCodes.nLevel < LVL_NB-1) gCCodes.nLevel++;
			gVar.pKeys[gMetroCfg.pKeys[e_CfgKey_Right]] = 0;
		}
		if (gVar.pKeys[gMetroCfg.pKeys[e_CfgKey_Left]])
		{
			if (gCCodes.nLevel > 0) gCCodes.nLevel--;
			gVar.pKeys[gMetroCfg.pKeys[e_CfgKey_Left]] = 0;
		}
	}
}

//=============================================================================

void MenuTimer_Reset(void);

// Gestionnaire d'évènements.
int EventHandler(u32 nInGame)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_KEYDOWN:
			gVar.pKeys[event.key.keysym.sym] = 1;

			// Pas en cours de jeu ?
			if (nInGame == 0)
			{
				CheatCodes_KeyPressed(event.key.keysym.sym);	// Test des cheat codes.
				MenuTimer_Reset();		// RAZ du timer de time out pour les menus.
#ifndef EMERGENCY_EXIT
				if (gVar.pKeys[SDLK_ESCAPE]) return (1);	// Emergency exit.
#endif
			}

// Sortir après la boucle ? => pas sûr.
			// Toggle fullscreen/windowed.
			if (gVar.pKeys[SDLK_F10])
			{
				gRender.nFullscreenMode ^= 1;
				Render_SetVideoMode();
			}
			// Toggle render mode.
			if (gVar.pKeys[SDLK_F9])
			{
				if (++gRender.nRenderMode >= e_RenderMode_MAX) gRender.nRenderMode = 0;
				Render_SetVideoMode();
			}

#ifdef EMERGENCY_EXIT
			if (gVar.pKeys[SDLK_ESCAPE]) return (1);	// Emergency exit.
#endif
			break;

		case SDL_KEYUP:
			gVar.pKeys[event.key.keysym.sym] = 0;
			break;

		case SDL_QUIT:		// Fermeture de la fenêtre.
			exit(0);
			break;
		}
	}

	// Gestion du joystick.
	if (gVar.pJoystick != NULL)
	{
		static u16	pHatMsk[e_CfgKey_LAST] = { SDL_HAT_UP, SDL_HAT_DOWN, SDL_HAT_LEFT, SDL_HAT_RIGHT, 0x100 };
		static u8	pKeyVal[e_CfgKey_LAST] = { e_CfgKey_Up, e_CfgKey_Down, e_CfgKey_Left, e_CfgKey_Right, e_CfgKey_ButtonA };

		u32	i;
		s16	nAxis;
		u16	nHat, nVal;
		nHat = 0;
		// Croix.
		if (gVar.nJoystickNoHat == 0)
			nHat = SDL_JoystickGetHat(gVar.pJoystick, 0);
		// Stick.
		if (gVar.nJoystickNoAxes == 0)
		{
			nAxis = SDL_JoystickGetAxis(gVar.pJoystick, 0);	// X
			if (ABS(nAxis) > 3200) nHat |= (nAxis > 0 ? SDL_HAT_RIGHT : SDL_HAT_LEFT);
			nAxis = SDL_JoystickGetAxis(gVar.pJoystick, 1);	// Y
			if (ABS(nAxis) > 3200) nHat |= (nAxis > 0 ? SDL_HAT_DOWN : SDL_HAT_UP);
		}
		// Boutons.
		if (SDL_JoystickGetButton(gVar.pJoystick, gMetroCfg.pKeys[e_CfgKey_JoyButtonA])) nHat |= 0x100;
//		if (SDL_JoystickGetButton(gVar.pJoystick, gMetroCfg.pKeys[e_CfgKey_JoyButtonB])) nHat |= 0x200;
//		if (SDL_JoystickGetButton(gVar.pJoystick, gMetroCfg.pKeys[e_CfgKey_JoyButtonC])) nHat |= 0x400;

		nVal = gVar.nJoystickState ^ nHat;
		gVar.nJoystickState = nHat;		// Sauvegarde pour le prochain tour.

		for (i = 0; i < e_CfgKey_LAST; i++)
		if (nVal & pHatMsk[i])
		{
			gVar.pKeys[gMetroCfg.pKeys[pKeyVal[i]]] = (nHat & pHatMsk[i] ? 1 : 0);
			gVar.pKeysSDL[gMetroCfg.pKeys[pKeyVal[i]]] = (nHat & pHatMsk[i] ? 1 : 0);
		}

		// Pas en cours de jeu ?
		if (nInGame == 0 && nVal) MenuTimer_Reset();		// RAZ du timer de time out pour les menus.

	}

	return (0);
}

//=============================================================================

// Le Menu (générique).
u32 Menu(void (*pFctInit)(void), u32 (*pFctMain)(void))
{
	u32	nMenuVal = MENU_Null;





/*
//  ***************************************
#define PI 3.14159265
#define	RAYON (200)	//(128)	//(64)
#define RAYCALC (RAYON - 1)

s32	pnOffs[SCR_Width * SCR_Height];	// Table d'offsets.

{
	s32	ix, iy;
	double	dist;
	double	ht;
	double	angleh;
	s32	nOffsX, nOffsY;

	// Précalcul de la loupe (quart supérieur gauche).
	for (iy = 0; iy < SCR_Height; iy++)
	{
// Petite correction pour éviter les lignes et colonnes du centre, qui font chier.
//s32	iyy = iy;
//if (iyy < (SCR_Height / 2)) iyy--;
//else
//iyy++;
//if (iyy > (SCR_Height / 2)) iyy++;
//s32	iyy = (iy < (SCR_Height / 2) ? iy - 1 : iy + 1);
s32	iyy = (iy < (SCR_Height / 2) ? iy - 2 : iy + 2);

		for (ix = 0; ix < SCR_Width; ix++)
		{
			nOffsX = nOffsY = 0;

// Petite correction pour éviter les lignes et colonnes du centre, qui font chier.
//s32	ixx = ix;
//if (ixx < (SCR_Width / 2)) ixx--;
//else
//ixx++;
//if (ixx > (SCR_Width / 2)) ixx++;
//s32	ixx = (ix < (SCR_Width / 2) ? ix - 1 : ix + 1);		// Avec le /2 pour sphère, ok. Avec le /4, petit glitch au centre => on fait +-2.
s32	ixx = (ix < (SCR_Width / 2) ? ix - 2 : ix + 2);


			// Est-on dans le cercle ?
//			dist = ((RAYCALC - ix) * (RAYCALC - ix)) + ((RAYCALC - iy) * (RAYCALC - iy));
			dist = (((SCR_Width / 2) - ixx) * ((SCR_Width / 2) - ixx)) + (((SCR_Height / 2) - iyy) * ((SCR_Height / 2) - iyy));
			if (dist < (RAYCALC * RAYCALC))
			{
//b
//double angc = atan2((SCR_Height / 2) - iy, (SCR_Width / 2) - ix) * 180 / PI;
//double rayon = fabs(RAYCALC * cos(angc * 2 * PI / 360));

				// Dans la loupe.
				ht = sqrt((RAYCALC * RAYCALC) - dist);		// Hauteur à ce point. (Pythagore ! Voir de côté : R^2 = dist^2 + h^2).

				// Calcul ?
				angleh = acos(ht / RAYCALC);
//				pOffsX[(iy * RAYON) + ix] = (s32) ( (((RAYCALC - ix)/2) * cos(angleh * 2 * PI / 360)));
//				nOffsX = (s32) ( (((RAYCALC - ix)/2) * cos(angleh * 2 * PI / 360)));
				nOffsX = (s32)( (((SCR_Width / 2) - ixx) * cos(angleh))/2 );	// 2 pour sphere, 4 pour aplatir un peu.

				angleh = asin(ht / RAYCALC);// * 180.0 / PI;
//				pOffsY[(iy * RAYON) + ix] = (s32) ( (((RAYCALC - iy)/2) * sin(angleh * 2 * PI / 360)));
//				nOffsY = (s32) ( (((RAYCALC - iy)/2) * sin(angleh * 2 * PI / 360)));
				nOffsY = (s32)( (((SCR_Height / 2) - iyy) * sin(angleh))/2 );	// 2 pour sphere, 4 pour aplatir un peu.
			}
//			pnOffs[(iy * SCR_Width) + ix] = ((0 + nOffsY) * SCR_Width) + (0 + nOffsX);
			pnOffs[(iy * SCR_Width) + ix] = ((iy + nOffsY) * (gGfxBlk.pBackground->pitch/2)) + (ix + nOffsX);
		}
	}
}

s32 i, j;
Frame_Init();
while (nMenuVal == MENU_Null)
{
	// Gestion des évenements.
	if (EventHandler(0) != 0) { nMenuVal = MENU_Quit; break; }


static u8 nOffs = 0;

if (gVar.pKeys[SDLK_RIGHT]) nOffs -= 1;
if (gVar.pKeys[SDLK_LEFT]) nOffs += 1;

u16	*pSrc, *pDst;
pSrc = (u16 *)gGfxBlk.pBackground->pixels;
pSrc += nOffs;
//nOffs += 1;	//nOffs &= 63;
pDst = (u16 *)gVar.pScreen->pixels;
for (j = 0; j < SCR_Height; j++)
{
	for (i = 0; i < SCR_Width; i++)
	{
//		*(pDst + i + ((j * gVar.pScreen->pitch) / 2)) = *(pSrc + i + ((j * gGfxBlk.pBackground->pitch) / 2));
//		*(pDst + i + ((j * gVar.pScreen->pitch) / 2)) = *(pSrc + i + pnOffsX[i] + ((j * gGfxBlk.pBackground->pitch) / 2));
		*(pDst + i + ((j * gVar.pScreen->pitch) / 2)) = *(pSrc + pnOffs[i + (j * SCR_Width)]);
//	*(pDst + i + ((j * gVar.pScreen->pitch) / 2)) = SDL_MapRGB(gVar.pScreen->format, pnOffs[i + (j * SCR_Width)] / gGfxBlk.pBackground->pitch, 0, (pnOffs[i + (j * SCR_Width)] % gGfxBlk.pBackground->pitch)<<3 );
//		*(pDst + i + ((j * gVar.pScreen->pitch) / 2)) = *(pSrc + i + (j * gGfxBlk.pBackground->pitch) + pnOffs[i + (j * SCR_Width)]);
	}
}


if (gVar.pKeys[SDLK_y]) { Frame_Wait(); Frame_Wait(); Frame_Wait(); Frame_Wait(); Frame_Wait(); Frame_Wait(); }
	Render_Flip(1);
}
return (nMenuVal);
//  ***************************************
*/




	Transit_Init(e_Transit_Off);

	CheatCodes_Reset();

	// Fonction d'init.
	(*pFctInit)();

	gGame.nHiScore = Scr_HighScore_Get();		// Reinit du high-score affiché en cas de cheat. !!! APRES l'init !!!

	// Main loop.
	Frame_Init();
	#if CACHE_ON == 1
	CacheClear();		// RAZ cache.
	#endif
	while (nMenuVal == MENU_Null)
	{
		// Gestion des évenements.
		if (EventHandler(0) != 0) { nMenuVal = MENU_Quit; break; }

		CheatCodes_Counter();
		// Menu, Main fct.
		nMenuVal = (*pFctMain)();
		// Les cheats.
		CheatCodes_LevelSelector();		// Level selector si cheat code.
		CheatCodes_Display();
		// Gestion de la transition.
		Transit_Manage();

#ifdef	DEBUG_KEYS
if (gVar.pKeys[SDLK_y]) { Frame_Wait(); Frame_Wait(); Frame_Wait(); Frame_Wait(); Frame_Wait(); Frame_Wait(); }
#endif
		Render_Flip(1);
	}

	return (nMenuVal);
}

//=============================================================================

/*
#define	PAUSE_MENU	(1)		// Comment to disable.

#if defined(PAUSE_MENU)

#define	PAUSE_TXT_NB	(3)
#define	PAUSE_KEY	(SDLK_F12)
#define	PAUSE_TXT_LNHT	(12)
#define	PAUSE_TXT_LNPOSY(nLn)	( (SCR_Height / 2) - (((PAUSE_TXT_NB + 1) * PAUSE_TXT_LNHT) / 2) + 10 + ((nLn) * PAUSE_TXT_LNHT) )
#define	PAUSE_CLOSEFX_LG	(8)		// !!! Divisible par 4 !!!

// Affichage du texte de la pause.
void Pause_sub_TxtDisplay(char **pTxt, s32 *pnTxtSz, u32 nChoice)
{
	u32	i;

	// Text.
	CursorMove();
	Font_Print((SCR_Width / 2) - (pnTxtSz[0] / 2), PAUSE_TXT_LNPOSY(0), pTxt[0], 0);
	u32 nFontFlag;
	for (i = 1; i < PAUSE_TXT_NB; i++)
	{
		nFontFlag = 0;

		// Selecteur.
		if (i - 1 == nChoice)
		{
			nFontFlag = FONT_Highlight;
			Font_Print((SCR_Width / 2) - (pnTxtSz[i] / 2) - 18+4 - (CursorGetPos() >> 8), PAUSE_TXT_LNPOSY(i + 1), "\x81", nFontFlag);
			Font_Print((SCR_Width / 2) + (pnTxtSz[i] / 2) + 10-4 + (CursorGetPos() >> 8), PAUSE_TXT_LNPOSY(i + 1), "\x80", nFontFlag);
		}
		Font_Print((SCR_Width / 2) - (pnTxtSz[i] / 2), PAUSE_TXT_LNPOSY(i + 1), pTxt[i], nFontFlag);
	}
}

// Pause (ingame).
// Out: 0=Resume / 1=Quit.
u32 Pause(void)
{
	SDL_Surface	*pBkg;

	// Surface to store currently displayed picture.
	pBkg = SDL_CreateRGBSurface(SDL_SWSURFACE, gVar.pScreen->w, gVar.pScreen->h, gVar.pScreen->format->BitsPerPixel,
		gVar.pScreen->format->Rmask, gVar.pScreen->format->Gmask, gVar.pScreen->format->Bmask, gVar.pScreen->format->Amask);
	if (pBkg == NULL) return (0);	// Failed.

	// Saves current gnFrame.
	u32	nFrame_sav = gnFrame;

	// Current screen => background.
	SDL_BlitSurface(gVar.pScreen, NULL, pBkg, NULL);

	Sfx_ChannelsSave();		// Save data + clear currently playing sounds.

	// Shade.
	{	// (Block added to reduce stack size).
	SDL_LockSurface(pBkg);

	u32	y, x;
	u8	*pSrc = pBkg->pixels;
	u32	nClr;
	u16	*pSrc2;

	for (y = 0; y < SCR_Height; y++)
	{
		pSrc2 = (u16 *)pSrc;
		for (x = 0; x < SCR_Width; x++)
		{
/ *
			// v1 : Shade only.
			nClr = gpTV2xCLUT[*pSrc2];
			nClr = gpTV2xCLUT[nClr];
			nClr = gpTV2xCLUT[nClr];
			*pSrc2++ = nClr;
* /

			// v2 : Grayscale + shade.
			// RGB to grayscale : x = 0.299r + 0.587g + 0.114b.
			u8	r, g, b;

			SDL_GetRGB(*pSrc2, gVar.pScreen->format, &r, &g, &b);
			nClr = (r * 0.299) + (g * 0.587) + (b * 0.114);
			nClr = SDL_MapRGB(gVar.pScreen->format, nClr, nClr, nClr);
			nClr = gpTV2xCLUT[nClr];
			nClr = gpTV2xCLUT[nClr];
			*pSrc2++ = nClr;

		}
		pSrc += pBkg->pitch;
	}

	SDL_UnlockSurface(pBkg);
	}

	// Calculate text size.
	char *pTxt[PAUSE_TXT_NB] = { "- PAUSED -", "RESUME", "QUIT" };
	s32	pnTxtSz[PAUSE_TXT_NB];

	u32	i;
	for (i = 0; i < PAUSE_TXT_NB; i++)
		pnTxtSz[i] = Font_Print(0, 0, pTxt[i], FONT_NoDisp);

	// Loop.
	u32	nChoice = 0;			// Cur line.
	gVar.pKeys[PAUSE_KEY] = 0;
	gVar.pKeys[gMSCfg.pKeys[e_CfgKey_Up]] = gVar.pKeys[gMSCfg.pKeys[e_CfgKey_Down]] = 0;
	CursorInit();
	RenderFlip(1);
	while (1)
	{
#ifdef	DEBUG_KEYS
		if (EventHandler(1) != 0) { nChoice = 0; break; }
#else
		EventHandler(1);
#endif

		// Up/Down.
		u32	nLastChoice = nChoice;
		if (gVar.pKeys[gMSCfg.pKeys[e_CfgKey_Up]])
		{
			if (--nChoice > PAUSE_TXT_NB-2) nChoice = PAUSE_TXT_NB-2;
			gVar.pKeys[gMSCfg.pKeys[e_CfgKey_Up]] = 0;
		}
		if (gVar.pKeys[gMSCfg.pKeys[e_CfgKey_Down]])
		{
			if (++nChoice > PAUSE_TXT_NB-2) nChoice = 0;
			gVar.pKeys[gMSCfg.pKeys[e_CfgKey_Down]] = 0;
		}
		nLastChoice ^= nChoice;
		if (nLastChoice)
		{
			CursorInit();								// Slt parce que c'est plus joli.
			Sfx_PlaySfx(FX_Menu_Move, e_SfxPrio_10);	// Sfx.
		}

		// Button A.
		if (gVar.pKeys[SDLK_RETURN] || gVar.pKeys[SDLK_SPACE] || gVar.pKeys[gMSCfg.pKeys[e_CfgKey_ButtonA]])
		{
//			Sfx_PlaySfx(FX_Menu_Clic, e_SfxPrio_10);    // Sfx.
			gVar.pKeys[gMSCfg.pKeys[e_CfgKey_ButtonA]] = 0;
			break;
		}
		// Pause (resume).
		if (gVar.pKeys[PAUSE_KEY])
		{
//			Sfx_PlaySfx(FX_Menu_Clic, e_SfxPrio_10);    // Sfx.
			nChoice = 0;
			gVar.pKeys[PAUSE_KEY] = 0;
			break;
		}

		// Bkg.
		SDL_BlitSurface(pBkg, NULL, gVar.pScreen, NULL);
		// Txt.
		Pause_sub_TxtDisplay(pTxt, pnTxtSz, nChoice);

		SprDisplayAll();
		// Wait for frame, Flip.
		RenderFlip(1);
	}

	// End.
	if (nChoice == 0)
		Sfx_ChannelsRestore();		// Resume => Restore audio data.
	else
	{
		// Quit. Closing transition, the lame way.
//		SDL_BlitSurface(gVar.pScreen, NULL, pBkg, NULL);	// Frame buffer to bkg (so we have the pause text's sprites too).
		Transit2D_Reset();
		Transit2D_InitClosing(e_Transit_Menu);
		while (Transit2D_CheckEnd() == 0)
		{
			SDL_BlitSurface(pBkg, NULL, gVar.pScreen, NULL);	// Bkg.
			Pause_sub_TxtDisplay(pTxt, pnTxtSz, nChoice);		// Txt.
			SprDisplayAll();
//			SDL_BlitSurface(pBkg, NULL, gVar.pScreen, NULL);
			// Affichage de la transition.
			Transit2D_Manage();
			// Wait for frame, Flip.
			RenderFlip(1);
		}
	}

	SDL_FreeSurface(pBkg);

	// Restores gnFrame.
	gnFrame = nFrame_sav;

	return (nChoice);
}
#endif

*/


// Game.
void GameLoop(void)
{
	Transit_Init(e_Transit_Off);

	Game_Init();

	// Main loop.
	Frame_Init();

	#if CACHE_ON == 1
	CacheClear();		// RAZ cache.
	#endif
	while (gGame.nExitCode == e_GameEC_Playing)
	{
		// Gestion des évenements.
#ifdef EMERGENCY_EXIT
		if (EventHandler(1)) { gGame.nExitCode = e_GameEC_Aborted; break; }
#else
		EventHandler(1);
#endif

		// Gestion du jeu.
		Game_Manage();
		// Gestion de la transition.
		Transit_Manage();

		// Wait for frame, Flip.
#ifdef DEBUG_KEYS
if (gVar.pKeys[SDLK_y]) { Frame_Wait(); Frame_Wait(); Frame_Wait(); Frame_Wait(); Frame_Wait(); Frame_Wait(); }
//if (gVar.pKeys[SDLK_y]) { Frame_Wait(); Frame_Wait(); Frame_Wait(); Frame_Wait(); Frame_Wait(); Frame_Wait(); }
//if (gVar.pKeys[SDLK_y]) { Frame_Wait(); Frame_Wait(); Frame_Wait(); Frame_Wait(); Frame_Wait(); Frame_Wait(); }
#endif
		Render_Flip(1);

	}
	Music_Start(e_YmMusic_NoMusic, 1);

	// Si jeu abandonné, pas de game over, pas de high score, pas de crédits.
	if (gGame.nExitCode == e_GameEC_Aborted) return;

	// All clear ?
	if (gGame.nExitCode == e_GameEC_AllClear)
	{
		Music_Start(e_YmMusic_HighScore, 0);
		Menu(MenuCongratulations_Init, MenuCongratulations_Main);
	}

	// High score ?
	if (gCCodes.nCheat == 0)	// Seulement si pas de cheat code !
	if (gGame.nExitCode == e_GameEC_GameOver || gGame.nExitCode == e_GameEC_AllClear)
	{
		if (Scr_CheckHighSc(gGame.nScore) >= 0)
		{
			Music_Start(e_YmMusic_HighScore, 0);
			Menu(MenuHighScores_Init2, MenuHighScores_Main);
		}
	}
	Music_Start(e_YmMusic_NoMusic, 1);

}


/*

// La boucle de jeu.
void Game(void)
{
	// Jeu.
	GameLoop(MISSIONOFFS_LEVELS + ((gCCodes.nCheat & e_Cheat_LevelSelect) ? gCCodes.nLevel : 0));

	// Si jeu abandonné, pas de game over, pas de high score, pas de crédits.
	if (gGameVar.nExitCode == e_Game_Aborted) return;

	// Credits si jeu terminé.
	if (gGameVar.nExitCode == e_Game_AllClear)
	{
		u32	nPlayerScore_sav = gGameVar.nBestScore;
		GameLoop(MISSIONOFFS_CREDITS);
		gGameVar.nBestScore = nPlayerScore_sav;
	}

	// Game Over.
	Music_Start(e_YmMusic_GameOver, 1);
	Menu(MenuGameOver_Init, MenuGameOver_Main);

	// High score ?
	if (gCCodes.nCheat == 0)	// Seulement si pas de cheat code !
	if (gGameVar.nExitCode == e_Game_GameOver || gGameVar.nExitCode == e_Game_AllClear)
	{
		if (Scr_CheckHighSc(gGameVar.nBestScore) >= 0)
		{
			Music_Start(e_YmMusic_HighScore, 1);
			// Saisie du nom.
			Menu(MenuGetName_Init, MenuGetName_Main);
			// Affichage de la table des high scores.
			Menu(MenuHighScores_Init, MenuHighScores_Main);
		}
	}
	Music_Start(e_YmMusic_NoMusic, 1);
}
*/

//=============================================================================

// Lecture de tous les sprites.
#define	SLBAR_X	(10)
#define	SLBAR_Y	(206)
#define	SLBAR_W	(300)
#define	SLBAR_H	(12)
void SpritesLoad(void)
{
#if SPR_SAVE == 1

//#include <direct.h>		// _chdir()	// Windows
//#include <unistd.h>		// chdir()	// Linux

	char *pSprFiles[] =
	{
		"gfx/_font_small1.psd",
		"gfx/_spr_hero0.psd",
		"gfx/_spr_objects0.psd",
		"gfx/_testrect.psd",
	};
	u32	i, nNbFiles;
	SDL_Rect	rRct;
	SDL_Surface	*pBkg;

	// Blitte l'image de la disquette à l'écran.
	if ((pBkg = SDL_LoadBMP("gfx/_bkg_disk.bmp")) == NULL)
	{
		fprintf(stderr, "Couldn't load picture 'bkg_disk.bmp': %s\n", SDL_GetError());
		exit(1);
	}
	SDL_BlitSurface(pBkg, NULL, gVar.pScreen, NULL);
	SDL_FreeSurface(pBkg);
	// Préparation des prm fixes de l'indicateur.
	rRct.x = SLBAR_X;
	rRct.y = SLBAR_Y;
	rRct.h = SLBAR_H;

	// Capture.
	nNbFiles = sizeof(pSprFiles)/sizeof(pSprFiles[0]);
	SprInitEngine();
	// Loop.
	for (i = 0; i < nNbFiles; i++)
	{
		// Indicateur de chargement.
		rRct.w = (SLBAR_W * (i + 1)) / nNbFiles;
		SDL_FillRect(gVar.pScreen, &rRct, SDL_MapRGB(gVar.pScreen->format, 255, 0, 0));
		//SDL_Flip(gVar.pScreen);
		Render_Flip(0);

		// Lecture planche de sprites.
		SprLoadGfx(pSprFiles[i]);
	}
	SprBinariesSave();
#else
	SprInitEngine();		// Pour initialiser gnSprSto.
	SprBinariesLoad();
#endif
	SprEndCapture();

}

//=============================================================================

/*
//NDEBUG	// Définir pour supprimer les asserts !
//gcc -DNDEBUG
*/

/*
#ifndef NDEBUG
void Mst00CheckStructSizes(void);
#endif
*/

u32 ChecksumVerify(u32 nLevNo);
u32 ChecksumCalc(u8 *pBuf, u32 nSz);

//#define	EXE_CHECKSUM	(1)		// Commenter la ligne pour virer le test.

#ifdef EXE_CHECKSUM
// Recherche d'une chaîne dans un buffer binaire.
u8 * StrFind(u8 *pToFind, u32 nToFindSz, u8 *pToSearch, u32 nToSearchSz)
{
	if (nToFindSz > nToSearchSz) return (NULL);

	u32	i;
	for (i = 0; i <= nToSearchSz - nToFindSz; i++)
	{
		// On ne compare que le premier char pour commencer.
		if (*pToSearch == *pToFind)
			if (strncmp((char *)pToSearch, (char *)pToFind, nToFindSz) == 0) return (pToSearch);
		pToSearch++;
	}
	return (NULL);
}

// Teste le checksum de l'exe.
void ExeChecksumTst(char *pFilename)
{
	FILE	*fPt = NULL;
	u8	*pBuf = NULL;

	// Ouverture du fichier.
	if ((fPt = fopen(pFilename, "rb")) == NULL)
	{
		fprintf(stderr, "Error opening file '%s'.\n", pFilename);
		goto _err_exit1;
	}
	// Récupération de la taille du fichier.
	s32	nFileSzToLoad, nSz;
	fseek(fPt, 0L, SEEK_END);
	nFileSzToLoad = ftell(fPt);
	fseek(fPt, 0L, SEEK_SET);
//printf("fsize = %d\n", nFileSzToLoad);
	// Malloc du buffer de lecture.
	if ((pBuf = (u8 *)malloc(nFileSzToLoad)) == NULL)
	{
		fprintf(stderr, "malloc failed (pBuf).\n");
		goto _err_exit1;
	}
	// Lecture.
	nSz = fread(pBuf, 1, nFileSzToLoad, fPt);
	if (nSz != nFileSzToLoad)
	{
		fprintf(stderr, "fread error.\n");
		goto _err_exit1;
	}
	fclose(fPt); fPt = NULL;

	// Recherche de la chaîne qui va bien.
	static char	*pStrToFind = "CC\x17\x02****";		// !!! Attention à bien laisser les 4 étoiles pour poke externe du checksum !!!
	u8	*pPtr;

	pPtr = StrFind((u8 *)pStrToFind, 4, pBuf, nFileSzToLoad);
	if (pPtr == NULL)
	{
		fprintf(stderr, "File '%s' has been altered. Aborted.\n", pFilename);
		goto _err_exit1;
	}
//printf("Substring found at offset %X\n", (int)(pPtr - pBuf));
	// Checksum du fichier.
	u32	nSum0 = *(u32 *)(pPtr + 4);
	// On efface les 4 cases du checksum.
	memset(pPtr + 4, 0, 4);
	// Recalcul du Checksum.
	u32	nSum1 = ChecksumCalc(pBuf, nFileSzToLoad);
//printf("Exe checksum = %X\n", nSum1);
	free(pBuf); pBuf = NULL;

	// Ok ?
	if (nSum0 != nSum1)
	{
		fprintf(stderr, "File '%s' has been altered. Aborted.\n", pFilename);
		exit(1);
	}
	return;

_err_exit1:
	if (fPt != NULL) fclose(fPt);
	if (pBuf != NULL) free(pBuf);
	exit (1);

}
#endif

//=============================================================================

// Recherche d'un éventuel flag 'nogl' sur la ligne de commande.
void CmdLine_Search(int argc, char **argv)
{
	u32	i = 1;			// On saute le nom de l'exe.
	while (i < argc)
	{
		char	*pPrm = argv[i++];
//		printf("%s\n", pPrm);
		if (*pPrm == '-' || *pPrm == '/') pPrm++;
		if (stricmp(pPrm, "nogl") == 0)
		{
//			printf("nogl found!\n");
			gRender.nUseGL = 0;
			return;
		}
	}
}

//=============================================================================

// Point d'entrée.
int main(int argc, char **argv)
{
	u32	nLoop;
	u32	nMenuVal;

	gRender.nUseGL = 1;
	CmdLine_Search(argc, argv);		// Cherche pour 'nogl' dans les arguments.

/*
#ifndef NDEBUG
	// Debug : Vérifie la taille des structures spécifiques des monstres.
	Mst00CheckStructSizes();
#endif
*/

#ifdef EXE_CHECKSUM
	// Test d'intégrité sur l'exe.
	ExeChecksumTst(argv[0]);
#endif

	// Test d'intégrité sur les fichiers EDT.
	u32	i;
	for (i = 1; i <= LVL_NB; i++)
		ChecksumVerify(i);

	// SDL Init.
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
	{
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		exit(1);
	}
	// atexit : Quand on quittera (exit, return...), SDL_Quit() sera appelée.
	atexit(SDL_Quit);

	const SDL_VideoInfo *pVideoInfo = SDL_GetVideoInfo();
#if defined(RENDER_BPP)
	gRender.nRenderBPP = (pVideoInfo->vfmt->BitsPerPixel > 16 ? 1 : 0);
//printf("bits pp = %d / v=%d\n", pVideoInfo->vfmt->BitsPerPixel, gRender.nRenderBPP);
//printf("bytes pp = %d\n", pVideoInfo->vfmt->BytesPerPixel);
#endif
	int	nOrgW, nOrgH, nOrgBitsPerPixel;
	nOrgW = pVideoInfo->current_w;
	nOrgH = pVideoInfo->current_h;
	nOrgBitsPerPixel = pVideoInfo->vfmt->BitsPerPixel;
//fprintf(stderr, "Original resolution: %d x %d - %d BPP\n", nOrgW, nOrgH, nOrgBitsPerPixel);



	// Video mode init.
	Render_InitVideo();
	SDL_WM_SetCaption("Metro-Cross by 17o2!!", NULL);	// Nom de la fenêtre.



//gRender.nRenderMode = e_RenderMode_TV2x;
//Render_SetVideoMode();




/*
{
// Capture de la fonte.
	// Lecture de la fonte.
	SDL_Surface	*pFnt = SDL_LoadBMP("font8_mc.bmp");
	if (pFnt == NULL) { fprintf(stderr, "Couldn't load picture: %s\n", SDL_GetError()); exit(1); }
	u8 *pSrc = (u8 *)pFnt->pixels;
	u32 ix, xx, yy;
	printf("\n#define FONT8_CHR_NB\t(%d)\t// Nb de chars dans la fonte.\n", (int)pFnt->w/8);
/ *
	printf("u8	gpFont8[] = {\n");
	for (ix = 0; ix < (u32)pFnt->w; ix += 8)
	{
		for (yy = 0; yy < (u32)pFnt->h; yy++)
		{
			for (xx = 0; xx < 8; xx++)
			{
				printf("%d,", (*(pSrc + ix + xx + (yy * pFnt->w)) ? 1 : 0));
			}
			printf("\n");
		}
		printf("\n");
	}
	printf("};\n\n");
* /

	u16	pChrIdx[256];
	u32	nChrIdx = 0;
	u32	nChrCur = 0;

	u32	k, idx;
	u8	pLine[8];
	printf("u8	gpFontData[] = {\n");
	for (ix = 0; ix < (u32)pFnt->w; ix += 8)
	{
		pChrIdx[nChrCur++] = nChrIdx;		// Position de la lettre dans pData.

		for (yy = 0; yy < (u32)pFnt->h; yy++)
		{
			for (k = 0; k < 8; k++) pLine[k] = 0;
			idx = 0;
			for (xx = 0; xx < 8; xx++)
			{
//				printf("%d,", (*(pSrc + ix + xx + (yy * pFnt->w)) ? 1 : 0));
				if (*(pSrc + ix + xx + (yy * pFnt->w)))
					pLine[idx++] = xx * 2;	// Parce que écran en u16.
			}
			printf("%d,", idx);
			for (k = 0; k < idx; k++) printf("%d,", pLine[k]);
			nChrIdx += idx + 1;
			printf("\n");
		}
		printf("\n");
	}
	printf("};\n\n");

	printf("u16	gpFontIdx[] = {\n");
	for (k = 0; k < nChrCur; k++)
	{
		printf("%d,",pChrIdx[k]);
		if (k && (k & 15) == 0) printf("\n");
	}
	printf("};\n\n");

	SDL_FreeSurface(pFnt);

exit(1);
}
*/




	// Lecture du fichier de conf.
	Cfg_Load();
	// Passe dans le mode video de la conf.
	if (gMetroCfg.nVideoMode != gRender.nRenderMode)
	{
		gRender.nRenderMode = gMetroCfg.nVideoMode;
		Render_SetVideoMode();
	}

	// Init joystick.
	gVar.pJoystick = NULL;
	gVar.nJoystickState = 0;
	if (SDL_NumJoysticks() > 0)
	{
		SDL_JoystickEventState(SDL_ENABLE);
		gVar.pJoystick = SDL_JoystickOpen(0);		// Si pas NULL, joystick !
		if (gVar.pJoystick != NULL)
		{
			u32	nJoystickNumButtons = SDL_JoystickNumButtons(gVar.pJoystick);
			gVar.nJoystickNoHat = (SDL_JoystickNumHats(gVar.pJoystick) < 1 ? 1 : 0);
			gVar.nJoystickNoAxes = (SDL_JoystickNumAxes(gVar.pJoystick) < 2 ? 1 : 0);

			// Config boutons ok ?
			if (gMetroCfg.pKeys[e_CfgKey_JoyButtonA] >= nJoystickNumButtons)// ||
				//gMetroCfg.pKeys[e_CfgKey_JoyButtonB] >= nJoystickNumButtons ||
				//gMetroCfg.pKeys[e_CfgKey_JoyButtonC] >= nJoystickNumButtons)
			{
				fprintf(stderr, "Joystick configuration can't be used with this joystick.\nPlease run the configuration program.\n");
				goto _JoyOff;
			}
			// Hardware ok ?
			if (nJoystickNumButtons < 3 || (gVar.nJoystickNoHat && gVar.nJoystickNoAxes))
			{
				// Pas assez de boutons ? => Disable.
				fprintf(stderr, "Unable to handle joystick.\n");
_JoyOff:
				fprintf(stderr, "Joystick disabled.\n");
				SDL_JoystickClose(gVar.pJoystick);
				gVar.pJoystick = NULL;
			}
		}
	}

	gnFrame = 0;	// RAZ compteur de frames.
	// Init du clavier.
	gVar.pKeysSDL = SDL_GetKeyState(NULL);
	memset(gVar.pKeys, 0, SDLK_LAST);
	// Allocation des buffers de scroll.
	Scroll_Allocate();
	// Lecture des graphs de décor (communs à tous les niveaux).
	Loader_BlocksLoad("gfx/gnd0.psd", &gGfxBlk.pBlocks);
	Loader_BlocksLoad("gfx/bkg0.psd", &gGfxBlk.pBackground);
	// Logo.
	Loader_BlocksLoad("gfx/mclogo.psd", &gVar.pImgLogo);
	SDL_SetColorKey(gVar.pImgLogo, SDL_SRCCOLORKEY, 0);
	// Image pour rotozoom. (Load à part car il ne faut pas faire le convert surface !).
	char	*pImgRotoFilename = "gfx/img_roto.psd";
	gVar.pImgRoto = PSDLoadToSDLSurf(pImgRotoFilename);
	if (gVar.pImgRoto == NULL)
	{
		fprintf(stderr, "Couldn't load picture '%s': %s\n", pImgRotoFilename, SDL_GetError());
		exit(1);
	}

	// Preca Sinus et Cosinus.
	PrecaSinCos();

	SDL_ShowCursor(SDL_DISABLE);	// Cache le pointeur de la souris.

	// Load sprites.
	SpritesLoad();

	// Load levels.
	Loader_LevelsLoad();

	// Init sound.
	Sfx_SoundInit();
	Sfx_LoadWavFiles();
	Sfx_LoadYMFiles();
	Sfx_SoundOn();	// Starts playback.

	// Misc inits.
	Transit_Init(e_Transit_Off);
	gCCodes.nCheat = 0;		// RAZ cheat codes.
	gCCodes.nLevel = 0;

	Scr_Load();				// Lecture de la table des high-scores.
	gGame.nScore = 0;
	gGame.nHiScore = Scr_HighScore_Get();

	RotoZoom_Init();

	// Boucle infinie.
	nMenuVal = MENU_Main;
	nLoop = 1;
	while (nLoop)
	{
		switch (nMenuVal)
		{
		case MENU_Main:		// Main menu.
			nMenuVal = Menu(MenuMain_Init, MenuMain_Main);
			break;

		case MENU_Game:		// Jeu.
			GameLoop();
			nMenuVal = MENU_Main;
			break;

		case MENU_HallOfFame:	// High scores.
//Menu(MenuCongratulations_Init, MenuCongratulations_Main);
//nMenuVal = MENU_Main;
//break;

/*
{
//static u32 ndefg=0;
//gGame.nScore = 255000 + ndefg; ndefg+=50000;
//Menu(MenuCongratulations_Init, MenuCongratulations_Main);
////nMenuVal = MENU_Main;
////break;

////static u32 ndefg=0;
////gGame.nScore = 55000 + ndefg; ndefg+=10000;
//gGame.nRound = 3;
//if (Scr_CheckHighSc(gGame.nScore) >= 0)


gGame.nRound = 13;
gGame.nScore = 310000;
if (Scr_CheckHighSc(gGame.nScore) >= 0)
	Menu(MenuHighScores_Init2, MenuHighScores_Main);
nMenuVal = MENU_Main;
}
break;
*/


			Menu(MenuHighScores_Init, MenuHighScores_Main);
			nMenuVal = MENU_Main;
			break;

/*
		case MENU_HowToPlay:	// How to play.
//			GameLoop(MISSIONOFFS_HOWTOPLAY);
//			for (i = 0; i < e_CfgKey_LAST; i++) gVar.pKeys[gMSCfg.pKeys[i]] = 0;	// RAZ Kb (utile slt avec Esc).
//			nMenuVal = MENU_Main;
nMenuVal = MENU_Quit;
			break;
*/

		case MENU_Quit:		// Sortie.
			nLoop = 0;
			break;
		}

	}

	SDL_ShowCursor(SDL_ENABLE);		// Réautorise l'affichage du curseur de la souris.

	if (gVar.pJoystick != NULL) SDL_JoystickClose(gVar.pJoystick);

	Sfx_SoundOff();	// Stops playback.
	Sfx_FreeWavFiles();	// Libère les ressources des Fx.
	Sfx_FreeYMFiles();	// Libère les ressources des YMs.

	// Libère les niveaux.
	Loader_LevelsRelease();

	// Libère les ressources des sprites.
	SprRelease();
	// Libère les buffers de scroll.
	Scroll_Release();
	// Libère les graphs de décor.
	Loader_BlocksRelease(&gGfxBlk.pBlocks);
	Loader_BlocksRelease(&gGfxBlk.pBackground);

	Loader_BlocksRelease(&gVar.pImgLogo);
	SDL_FreeSurface(gVar.pImgRoto);

	// Libère les ressources de rendu.
	Render_Release();


#if defined(__LINUX__)
if (gRender.nUseGL)
{
fprintf(stderr, "***\nTo Linux users:\n\
Please enter the following command if the resolution is messed up:\n\
xrandr -s %dx%d\n", nOrgW, nOrgH);
}
#endif


	SDL_Quit();
	return (0);
}



