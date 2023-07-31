
#include "includes.h"

//=============================================================================

struct STransit
{
	u32	nState;
	u32	nCnt;
};
struct STransit	gTransit;

// Transit in.
void Transit_In(void)
{
	SDL_LockSurface(gVar.pScreen);

	u32	i, y, x;
	s32	nOrg;
	u16	*pDst;
	u32	nClr;

	for (i = 2; i < SCR_Height / 8; i++)
	{
		nClr = gVar.pnColors[(i < 23 ? e_Color_Black : e_Color_HUDBkg)];
		nOrg = (s32)gTransit.nCnt - (i-2);
		if (nOrg < 0) nOrg = 0;
		//
		for (y = nOrg; y < 8; y++)
		{
			pDst = (u16 *)( gVar.pScreen->pixels + (((i * 8) + y) * gVar.pScreen->pitch) );
			for (x = 0; x < SCR_Width; x++)
			{
				*pDst++ = nClr;//0xFFFF;
			}
		}
	}
	SDL_UnlockSurface(gVar.pScreen);
}

// Transit out.
void Transit_Out(void)
{
	SDL_LockSurface(gVar.pScreen);

	u32	i, x;
	s32	nOrg, y;
	u16	*pDst;
	u32	nClr;

	for (i = 2; i < SCR_Height / 8; i++)
	{
		nClr = gVar.pnColors[(i < 23 ? e_Color_Black : e_Color_HUDBkg)];
		nOrg = (s32)gTransit.nCnt - (i-2);
		if (nOrg > 8) nOrg = 8;
		//
		for (y = 0; y < nOrg; y++)
		{
			pDst = (u16 *)( gVar.pScreen->pixels + (((i * 8) + y) * gVar.pScreen->pitch) );
			for (x = 0; x < SCR_Width; x++)
			{
				*pDst++ = nClr;//0xFFFF;
			}
		}
	}
	SDL_UnlockSurface(gVar.pScreen);
}

#define	TRANSIT_CNT_MAX	(33)

// Gestion de la transition on/off.
void Transit_Manage(void)
{
	if (gTransit.nState == e_Transit_Off) return;

	if (gTransit.nState == e_Transit_FadeIn)
	{
		if (++gTransit.nCnt > TRANSIT_CNT_MAX)
			gTransit.nState = e_Transit_Off;
		else
			Transit_In();
	}
	else
	{
/*
		if (++gTransit.nCnt > TRANSIT_CNT_MAX)
			gTransit.nState = e_Transit_Off;
		else
			Transit_Out();
*/
		if (gTransit.nCnt <= TRANSIT_CNT_MAX)
			gTransit.nCnt++;
		Transit_Out();

	}

}

// Renvoie l'état.
u32 Transit_StateGet(void)
{
	return (gTransit.nState);
}

// Transition finie ?
// Out : 0 = Pas finie / 1 = Finie.
u32 Transit_IsDone(void)
{
//	return (gTransit.nState == e_Transit_Off);
	return (gTransit.nCnt > TRANSIT_CNT_MAX);
}

// Initialise la transition.
void Transit_Init(u32 nOnOff)
{
	gTransit.nState = nOnOff;
	gTransit.nCnt = 0;
}

// Coupe la transition.
void Transit_Off(void)
{
	gTransit.nState = e_Transit_Off;
	gTransit.nCnt = TRANSIT_CNT_MAX;
}

//=============================================================================

enum
{
	MENU_State_FadeIn = 0,
	MENU_State_Input,
	MENU_State_FadeOut,
};

struct SMenuGen
{
	u32	nState;
	u32	nTimerGen;
};
struct SMenuGen	gMenu;

//=============================================================================

#define	MENU_TIMER_GEN	(64*5)

// RAZ du compteur.
void MenuTimer_Reset(void)
{
	if (gMenu.nState == MENU_State_FadeOut) return;		// Pas de reset pendant les fermetures (appel depuis EventHandler qui pourrait fausser le Check).
	gMenu.nTimerGen = 0;		// Timer pour changer d'écran quand inactivité.
}

// Le compteur avance.
void MenuTimer_Tick(void)
{
	gMenu.nTimerGen++;
}

// Test.
// Out: 1 = Max atteint. 0 sinon.
u32 MenuTimer_Check(u32 nMaxCnt)
{
	return (gMenu.nTimerGen >= nMaxCnt ? 1 : 0);
}

//=============================================================================

// Macro pour afficher un texte et son ombre, centré.
#define Fnt8PrintY(str, len, posy, color) \
Font8_Print((SCR_Width / 2) - (((len) * 8) / 2)+1, (posy)+1, (str), gVar.pnColors[e_Color_Black]); \
Font8_Print((SCR_Width / 2) - (((len) * 8) / 2)  , (posy)  , (str), gVar.pnColors[color]);

// Macro pour afficher un texte et son ombre, en (x,y).
#define Fnt8PrintXY(posx, posy, str, color) \
Font8_Print((posx)+1, (posy)+1, (str), gVar.pnColors[e_Color_Black]); \
Font8_Print((posx)  , (posy)  , (str), gVar.pnColors[color]);

//=============================================================================

// Menu main : Init.
void MenuMain_Init(void)
{
/*
	u32	i;

	MenuInitFade();
	// Décor.
	gVar.pBackground = gVar.pBkg[0];

	Transit2D_InitOpening(e_Transit_Menu);

	// Récupère les longueurs des phrases.
	for (i = 0; i < NBELEM(gpMenuItems_Main); i++)
	{
		gpMenuItems_Main[i].nLg = Font_Print(0, 0, gpMenuItems_Main[i].pTxt, FONT_NoDisp);
	}

//	gMenu.nChoix = 0;
	gMenu.nChoix = gMenu.nMain_Pos;

	gMenu.nFrmCnt0 = 0;		// !!! Pour lecture du GIF image par image.

	CursorInit();
	gMenu.nMain_SinIdx = MENUMAIN_SIN_MIN;
*/

	gMenu.nState = MENU_State_FadeIn;
	MenuTimer_Reset();

	Transit_Init(e_Transit_FadeIn);

}

/*
#define	MENUMAIN_BkgClr_R	64
#define	MENUMAIN_BkgClr_G	56
#define	MENUMAIN_BkgClr_B	48

void CheatCodes_Display(void);
char * Level_NameGet(u32 nLevelNo);
s32 Level_RealNumber(u32 nLevelNo);
*/

// Struct pour sauver la position des texte, leur taille, etc. (Originellement dans le menu de congratulations).
struct SCong
{
	char	*pStr;
	u32	nSz;
	u32	nPosY;
	u32	nColor;
};
#define	STRSTO(str)		str, sizeof(str)

// Menu main : Main.
u32 MenuMain_Main(void)
{
	u32	nRetVal = MENU_Null;
	u32	i;

	// Selon l'état.
	switch (gMenu.nState)
	{
	case MENU_State_FadeIn:
/*
		if (Transit2D_CheckEnd()) gMenu.nState = MENU_State_Input;
*/
		if (Transit_IsDone())
			gMenu.nState = MENU_State_Input;
		break;

	case MENU_State_FadeOut:
/*
//		if (Transit2D_CheckEnd()) nRetVal = gpMenuItems_Main[gMenu.nChoix].nMenuVal;
		if (Transit2D_CheckEnd())
		{
			if (MenuTimer_Cnt(MENU_TIMER_GEN))
			{
				// Time out.
				static	u8	nTimerChoice = 0;
				static	u8	pnTimerChoices[] = { MENU_HallOfFame, MENU_HowToPlay };
				if (++nTimerChoice >= sizeof(pnTimerChoices)/sizeof(pnTimerChoices[0])) nTimerChoice = 0;
				nRetVal = pnTimerChoices[nTimerChoice];
			}
			else
				nRetVal = gpMenuItems_Main[gMenu.nChoix].nMenuVal;		// Sortie normale, choix en cours.

			gMenu.nMain_Pos = gMenu.nChoix;		// Sauvegarde de la pos en cours du curseur.
		}
*/
		if (Transit_IsDone())
			nRetVal = (MenuTimer_Check(MENU_TIMER_GEN) ? MENU_HallOfFame : MENU_Game);
		break;

	case MENU_State_Input:
/*
		{
			u32	nLastChoix = gMenu.nChoix | (gCCodes.nLevel << 8);

			// Selecteur de level ?
			if (gCCodes.nCheat & e_Cheat_LevelSelect)
			{
				if (gVar.pKeys[gMSCfg.pKeys[e_CfgKey_Right]])
				{
					if (Level_RealNumber(MISSIONOFFS_LEVELS + gCCodes.nLevel + 1) > 0) gCCodes.nLevel++;
					gVar.pKeys[gMSCfg.pKeys[e_CfgKey_Right]] = 0;
				}
				if (gVar.pKeys[gMSCfg.pKeys[e_CfgKey_Left]])
				{
					if (gCCodes.nLevel > 0) gCCodes.nLevel--;
					gVar.pKeys[gMSCfg.pKeys[e_CfgKey_Left]] = 0;
				}
			}

			// Déplacement du curseur.
//			u32	nLastChoix = gMenu.nChoix;
			if (gVar.pKeys[gMSCfg.pKeys[e_CfgKey_Up]])
			{
				if (gMenu.nChoix > 0) gMenu.nChoix--;
				gVar.pKeys[gMSCfg.pKeys[e_CfgKey_Up]] = 0;
			}
			if (gVar.pKeys[gMSCfg.pKeys[e_CfgKey_Down]])
			{
				if (gMenu.nChoix < NBELEM(gpMenuItems_Main) - 1) gMenu.nChoix++;
				gVar.pKeys[gMSCfg.pKeys[e_CfgKey_Down]] = 0;
			}

//			if (nLastChoix != gMenu.nChoix)
			nLastChoix ^= (gMenu.nChoix | (gCCodes.nLevel << 8));
			if (nLastChoix)
			{
				if (nLastChoix & 0xFF) CursorInit();	// Slt parce que c'est plus joli. Et uniquement sur haut/bas.
				Sfx_PlaySfx(FX_Menu_Move, e_SfxPrio_10);	// Sfx.
			}

			// Validation ?
			if (gVar.pKeys[SDLK_RETURN] || gVar.pKeys[SDLK_SPACE] ||
				gVar.pKeys[gMSCfg.pKeys[e_CfgKey_ButtonA]] || gVar.pKeys[gMSCfg.pKeys[e_CfgKey_ButtonB]] || gVar.pKeys[gMSCfg.pKeys[e_CfgKey_ButtonC]] ||
				MenuTimer_Cnt(MENU_TIMER_GEN))
			{
				gMenu.nState = MENU_State_FadeOut;
				Transit2D_InitClosing(e_Transit_Menu);
				Sfx_PlaySfx(FX_Menu_Clic, e_SfxPrio_10);	// Sfx.
			}

			// Changement du nombre de credits ?
			if (gVar.pKeys[SDLK_F1])
			{
				Credits_NextSel();
				gVar.pKeys[SDLK_F1] = 0;
				Sfx_PlaySfx(FX_Menu_Move, e_SfxPrio_10);		// Sfx.
			}
		}
*/



		// Validation ?
		if (gVar.pKeys[SDLK_RETURN] || gVar.pKeys[SDLK_SPACE] || gVar.pKeys[gMetroCfg.pKeys[e_CfgKey_ButtonA]] ||
			MenuTimer_Check(MENU_TIMER_GEN))
		{
			Transit_Init(e_Transit_FadeOut);
			gMenu.nState = MENU_State_FadeOut;
//			Sfx_PlaySfx(FX_Menu_Clic, e_SfxPrio_10);	// Sfx.
		}
		else
			MenuTimer_Tick();

		break;
	}



	//>>> Affichage.

	// Clear screen.
////	SDL_FillRect(gVar.pScreen, NULL, SDL_MapRGB(gVar.pScreen->format, MENUMAIN_BkgClr_R, MENUMAIN_BkgClr_G, MENUMAIN_BkgClr_B));
//	SDL_FillRect(gVar.pScreen, NULL, 0);
	RotoZoom_Draw();

	HUD_Part1Disp();

	SDL_Rect	sRct;
	sRct.x = (SCR_Width / 2) - (gVar.pImgLogo->w / 2);
//	sRct.y = (SCR_Height / 2) - (gVar.pImgLogo->h / 2);
	sRct.y = 40;
	SDL_BlitSurface(gVar.pImgLogo, NULL, gVar.pScreen, &sRct);

//	Font8_Print((SCR_Width / 2) - ((14 * 8) / 2)+1, 112+1, "(C) 1985 NAMCO", gVar.pnColors[e_Color_Black]);
//	Font8_Print((SCR_Width / 2) - ((14 * 8) / 2), 112, "(C) 1985 NAMCO", gVar.pnColors[e_Color_Red]);
//	Font8_Print((SCR_Width / 2) - ((29 * 8) / 2)+1, 128+1, "2013 VERSION BY CLEMENT CORDE", gVar.pnColors[e_Color_Black]);
//	Font8_Print((SCR_Width / 2) - ((29 * 8) / 2), 128, "2013 VERSION BY CLEMENT CORDE", gVar.pnColors[e_Color_Gray]);
//	Font8_Print((SCR_Width / 2) - ((29 * 8) / 2)+1, 136+1, "USING ST-SOUND BY LEONARD/OXG", gVar.pnColors[e_Color_Black]);
//	Font8_Print((SCR_Width / 2) - ((29 * 8) / 2), 136, "USING ST-SOUND BY LEONARD/OXG", gVar.pnColors[e_Color_Gray]);

//	Fnt8PrintY("(C) 1985 NAMCO", 14, 112, e_Color_Red)
//	Fnt8PrintY("2013 VERSION BY CLEMENT CORDE", 29, 128, e_Color_Gray)
//	Fnt8PrintY("USING ST-SOUND BY LEONARD/OXG", 29, 136, e_Color_Gray)

	static struct SCong	pCong[] =
	{
		{ STRSTO("(C) 1985 NAMCO"), 112, e_Color_Red },
		{ STRSTO("2013 VERSION BY CLEMENT CORDE"), 128, e_Color_Gray },
		{ STRSTO("USING ST-SOUND BY LEONARD/OXG"), 136, e_Color_Gray },
		{ STRSTO("PRESS START"), 160, e_Color_Yellow },
	};

	for (i = 0; i < NBELEM(pCong) - 1; i++)
	{
		Fnt8PrintY(pCong[i].pStr, pCong[i].nSz, pCong[i].nPosY, pCong[i].nColor)
	}


//	u32	nColor;
//	nColor = (gnFrame & 16 ? gVar.pnColors[e_Color_Yellow] : gVar.pnColors[e_Color_Red]);
//	Font8_Print((SCR_Width / 2) - ((11 * 8) / 2)+1, 160+1, "PRESS START", gVar.pnColors[e_Color_Black]);
//	Font8_Print((SCR_Width / 2) - ((11 * 8) / 2), 160, "PRESS START", nColor);

//	Fnt8PrintY("PRESS START", 11, 160, (gnFrame & 16 ? e_Color_Yellow : e_Color_Red))

	i = NBELEM(pCong) - 1;
	Fnt8PrintY(pCong[i].pStr, pCong[i].nSz, pCong[i].nPosY, (gnFrame & 16 ? e_Color_Yellow : e_Color_Red))




/*
	CursorMove();
	if (gMenu.nMain_SinIdx < MENUMAIN_SIN_MAX) gMenu.nMain_SinIdx += MENUMAIN_SIN_SPD;

	//>>> Affichage.

	// Clear screen.
//	SDL_FillRect(gVar.pScreen, 0, SDL_MapRGB(gVar.pScreen->format, MENUMAIN_BkgClr_R, MENUMAIN_BkgClr_G, MENUMAIN_BkgClr_B));

	// Bkg qui scrolle, image 128 x 128.
	Bkg1Scroll(-gnFrame >> 1, -gnFrame >> 1);

	// Image suivante dans le GIF animé.
	if ((gMenu.nFrmCnt0++ & 3) == 0) GIF_GetNextImage(gVar.pGif);
	// Affichage.
//	GIF_Display(gVar.pGif, SDL_MapRGB(gVar.pScreen->format, MENUMAIN_BkgClr_R, MENUMAIN_BkgClr_G, MENUMAIN_BkgClr_B), 26, -33);
	GIF_Display(gVar.pGif, SDL_MapRGB(gVar.pScreen->format, MENUMAIN_BkgClr_R, MENUMAIN_BkgClr_G, MENUMAIN_BkgClr_B), 26 + (255 + gVar.pSin[gMenu.nMain_SinIdx]), -33);

	// Logo MS en haut à gauche.
//	SprDisplayAbsolute(e_Spr_MS_Logo, 10, 10, 200);
	SprDisplayAbsolute(e_Spr_MS_Logo, 10 - (255 + gVar.pSin[gMenu.nMain_SinIdx]), 10, 200);

	// Menu.
	u32	nFontFlag;
	for (i = 0; i < NBELEM(gpMenuItems_Main); i++)
	{
		nFontFlag = 0;
		// Selecteur.
		if (i == gMenu.nChoix)
		{
			nFontFlag = FONT_Highlight;
//			Font_Print((SCR_Width / 2) - (gpMenuItems_Main[i].nLg / 2) - 18+4 - (gMenu.nCursPos >> 8), MENU_Main_StartLn + (i * 12), ">", nFontFlag);
//			Font_Print((SCR_Width / 2) + (gpMenuItems_Main[i].nLg / 2) + 10-4 + (gMenu.nCursPos >> 8), MENU_Main_StartLn + (i * 12), "<", nFontFlag);
			static char *pCursL = "\x81";	// ' ' + 94 + 3
			static char *pCursR = "\x80";	// ' ' + 94 + 2
			Font_Print((SCR_Width / 2) - (gpMenuItems_Main[i].nLg / 2) - 18+4 - (gMenu.nCursPos >> 8), MENU_Main_StartLn + (i * 12), pCursL, nFontFlag);
			Font_Print((SCR_Width / 2) + (gpMenuItems_Main[i].nLg / 2) + 10-4 + (gMenu.nCursPos >> 8), MENU_Main_StartLn + (i * 12), pCursR, nFontFlag);
		}
		Font_Print((SCR_Width / 2) - (gpMenuItems_Main[i].nLg / 2), MENU_Main_StartLn + (i * 12), gpMenuItems_Main[i].pTxt, nFontFlag);
	}

	// Selecteur de level ?
	if (gCCodes.nCheat & e_Cheat_LevelSelect)
	{
		char	*pLvlName = Level_NameGet(MISSIONOFFS_LEVELS + gCCodes.nLevel);
		i = Font_Print(0, 0, pLvlName, FONT_NoDisp);
		Font_Print((SCR_Width / 2) - (i / 2), MENU_Main_StartLn - (1 * 12), pLvlName, 0);
	}

	// Les credits.
	Credits_Display(gVar.nCreditsToUse);
	// Les cheats.
	CheatCodes_Display();
*/

	return (nRetVal);

}

//=============================================================================

#define	MENU_TIMER_CONGRATULATIONS	(FPS * 30)

// Congratulations : Init.
void MenuCongratulations_Init(void)
{
	Transit_Init(e_Transit_FadeIn);
	MenuTimer_Reset();

	gMenu.nState = MENU_State_FadeIn;

}

void CheatCodes_Reset(void);

// Congratulations : Main.
u32 MenuCongratulations_Main(void)
{
	u32	nRetVal = MENU_Null;
	u32	i;

	CheatCodes_Reset();		// On ne coupe pas les cheats pendant les congratulations ! (Pour éviter la triche : Pas de high score si cheats !).

	// Selon l'état.
	switch (gMenu.nState)
	{
	case MENU_State_FadeIn:
		if (gGame.nScore > gGame.nHiScore) gGame.nHiScore = gGame.nScore;	// Patch ici, pas mettable dans l'init car high score repris dans la table APRES l'init (en cas de cheat).

		if (Transit_IsDone())
			gMenu.nState = MENU_State_Input;
		break;

	case MENU_State_FadeOut:
		if (Transit_IsDone())
			nRetVal = MENU_Game;
		break;

	case MENU_State_Input:
//		CheatCodes_LevelSelector();		// Level selector si cheat code.

		// Validation ?
		if (gVar.pKeys[SDLK_RETURN] || gVar.pKeys[SDLK_SPACE] || gVar.pKeys[gMetroCfg.pKeys[e_CfgKey_ButtonA]] ||
			MenuTimer_Check(MENU_TIMER_CONGRATULATIONS))
		{
			Transit_Init(e_Transit_FadeOut);
			gMenu.nState = MENU_State_FadeOut;
//			Sfx_PlaySfx(FX_Menu_Clic, e_SfxPrio_10);	// Sfx.
		}
		MenuTimer_Tick();
		break;
	}


	//>>> Affichage.

	// Clear screen.
//	SDL_FillRect(gVar.pScreen, NULL, SDL_MapRGB(gVar.pScreen->format, MENUMAIN_BkgClr_R, MENUMAIN_BkgClr_G, MENUMAIN_BkgClr_B));
//	SDL_FillRect(gVar.pScreen, NULL, 0);
	RotoZoom_Draw();

	HUD_Part1Disp();

	SDL_Rect	sRct;
	sRct.x = (SCR_Width / 2) - (gVar.pImgLogo->w / 2);
	sRct.y = 48+32;
	SDL_BlitSurface(gVar.pImgLogo, NULL, gVar.pScreen, &sRct);

/*
	struct SCong
	{
		char	*pStr;
		u32	nSz;
		u32	nPosY;
		u32	nColor;
	};
	#define	STRSTO(str)		str, sizeof(str)
*/
	static struct SCong	pCong[] =
	{
		{ STRSTO("CONGRATULATIONS!"), 32, e_Color_Red },
		{ STRSTO("THE WHOLE DISTANCE OF"), 32+8+8+8, e_Color_Gray },
		{ STRSTO("THE"), 32+32+8, e_Color_Cyan },
		{ STRSTO("HAS RUN"), 32+64+32+8, e_Color_Gray },
		{ STRSTO("THANK YOU FOR YOUR RUNNING!"), 32+64+32+8+8+8+8, e_Color_Yellow },
	};

	for (i = 0; i < NBELEM(pCong); i++)
	{
//		Font8_Print((SCR_Width / 2) - (pCong[i].nSz * 4), pCong[i].nPosY, pCong[i].pStr, gVar.pnColors[pCong[i].nColor]);
		Fnt8PrintY(pCong[i].pStr, pCong[i].nSz, pCong[i].nPosY, pCong[i].nColor)
	}

	return (nRetVal);
}

//=============================================================================
//	Font8_Print(50, 170, "CLEMENT*CORDE***  ALL  0000000", nColor);
//#define	LVL_NB	(8)				// Nombre de niveaux.

#define HISC_Nb	(5)

#define HISC_NameLg (16+1)
#define HISC_Filename "high.scr"

#define	HISC_DefaultName	"UNKNOWN RUNNER"

#pragma pack(1)
struct SScore
{
	char	pName[HISC_NameLg];
	u32	nScore;
	u8	nRound;
};
#pragma pack()
struct SScore	gpHighScores[HISC_Nb];

struct SMenuHS
{
	u8	nEntry;		// 0, pas de saisie du nom / 1, saisie d'un high score.
	s32	nRank;		// Hi-score en cours.
	u8	nPos;		// Position dans le texte.
	u8	nBlink;		// Pour clignotement du curseur.
};
struct SMenuHS	gMenuHS;

// Menu high-scores : Init.
void MenuHighScores_Init(void)
{
	gMenuHS.nEntry = 0;		// 0, pas de saisie du nom.

	gMenu.nState = MENU_State_FadeIn;
	MenuTimer_Reset();

	Transit_Init(e_Transit_FadeIn);
}

#define	MENU_HISC_ENTRY_TIMEOUT	(FPS * 10)		// Time out sur l'entrée du nom.
// Menu high-scores : Init2, Initialise la saisie du nom.
void MenuHighScores_Init2(void)
{
	MenuHighScores_Init();

	gMenuHS.nRank = Scr_PutNameInTable("", gGame.nScore, gGame.nRound);
	if (gMenuHS.nRank < 0) return;		// nEntry initialisé à 0 dans l'init normale !

	gMenuHS.nEntry = 1;		// 1, saisie d'un high score.
	gMenuHS.nPos = 0;		// Position dans le texte.
	gMenuHS.nBlink = 0;		// Pour clignotement du curseur.

}

// Menu high-scores : Main.
u32 MenuHighScores_Main(void)
{
	u32	nRetVal = MENU_Null;

	// Selon l'état.
	switch (gMenu.nState)
	{
	case MENU_State_FadeIn:
		if (Transit_IsDone())
			gMenu.nState = MENU_State_Input;
		break;

	case MENU_State_FadeOut:
		if (Transit_IsDone())
			nRetVal = MENU_Main;
		break;

	case MENU_State_Input:
//		CheatCodes_LevelSelector();		// Level selector si cheat code.

		// Saisie en cours ?
		if (gMenuHS.nEntry)
		{	// Oui.
			u32	i;
//			u32	nLastPos = gMenuHS.nPos;
			// Touches ?
			if (gMenuHS.nPos < HISC_NameLg - 1)
			{
				for (i = SDLK_a; i <= SDLK_z; i++)
					if (gVar.pKeys[i])
					{
						gpHighScores[gMenuHS.nRank].pName[gMenuHS.nPos++] = i - SDLK_a + 'A';
						gpHighScores[gMenuHS.nRank].pName[gMenuHS.nPos] = 0;
						gVar.pKeys[i] = 0;
						goto _OneLetterDone;
					}
				for (i = SDLK_0; i <= SDLK_9; i++)
					if (gVar.pKeys[i])
					{
						gpHighScores[gMenuHS.nRank].pName[gMenuHS.nPos++] = i - SDLK_0 + '0';
						gpHighScores[gMenuHS.nRank].pName[gMenuHS.nPos] = 0;
						gVar.pKeys[i] = 0;
						goto _OneLetterDone;
					}
				for (i = SDLK_KP0; i <= SDLK_KP9; i++)
					if (gVar.pKeys[i])
					{
						gpHighScores[gMenuHS.nRank].pName[gMenuHS.nPos++] = i - SDLK_KP0 + '0';
						gpHighScores[gMenuHS.nRank].pName[gMenuHS.nPos] = 0;
						gVar.pKeys[i] = 0;
						goto _OneLetterDone;
					}
				if (gVar.pKeys[SDLK_SPACE])
				{
					gpHighScores[gMenuHS.nRank].pName[gMenuHS.nPos++] = ' ';
					gpHighScores[gMenuHS.nRank].pName[gMenuHS.nPos] = 0;
					gVar.pKeys[SDLK_SPACE] = 0;
					goto _OneLetterDone;
				}
			}
_OneLetterDone:
//			if (nLastPos != gMenuHS.nPos)	// fx.

			// Backspace ?
			if (gVar.pKeys[SDLK_BACKSPACE])
			{
				if (gMenuHS.nPos) gpHighScores[gMenuHS.nRank].pName[--gMenuHS.nPos] = 0;
				gVar.pKeys[SDLK_BACKSPACE] = 0;
			}

			// Validation ? (Ou time out ?)
			if (gVar.pKeys[SDLK_RETURN] ||
				MenuTimer_Check(MENU_HISC_ENTRY_TIMEOUT))		// Note : Reset du timer géré avec les events.
			{

				// Pas de nom ? => Nom par défaut.
				u32	nNoName = 0;
				if (gMenuHS.nPos == 0)
					nNoName = 1;
				else
				{	// Que des espaces ? => Nom par défaut.
					i = 0;
					while (gpHighScores[gMenuHS.nRank].pName[i] == ' ') i++;
					if (gpHighScores[gMenuHS.nRank].pName[i] == 0) nNoName = 1;
				}
				if (nNoName) strcpy(gpHighScores[gMenuHS.nRank].pName, HISC_DefaultName);

				// Sauvegarde + repassage en mode normal.
				Scr_Save();
				gMenuHS.nEntry = 0;

				gVar.pKeys[SDLK_RETURN] = 0;
				// Music. On repasse sur la musique du menu.
				Music_Start(e_YmMusic_NoMusic, 1);

//				// Music. On repasse sur la musique du menu.
//				Music_Start(e_YmMusic_Menu, 1);

//	Transit_Init(e_Transit_FadeOut);
//	gMenu.nState = MENU_State_FadeOut;

//				Sfx_PlaySfx(FX_Menu_Clic, e_SfxPrio_10);	// Sfx.
			}
			else
				MenuTimer_Tick();

		}
		else
		{	// Non.

/*
			// Time out ?
			if (gMenuGen.nTimer > MENU_TIMEOUT)
			{
				gMenuGen.nRetVal = MENU_Main;
				gMenuGen.nPhase = e_MenuGen_FadeOut;
			}

			// Appui sur bouton A ou bouton B ?
			if (gVar.pKeys[gAirballCfg.pKeys[e_CfgKey_ButtonA]] ||
				gVar.pKeys[gAirballCfg.pKeys[e_CfgKey_ButtonB]])
			{
				gVar.pKeys[gAirballCfg.pKeys[e_CfgKey_ButtonA]] = gVar.pKeys[gAirballCfg.pKeys[e_CfgKey_ButtonB]] = 0;
				gMenuGen.nRetVal = MENU_Game;			// Choix à la fin du fade out.
				gMenuGen.nPhase = e_MenuGen_FadeOut;
			}
*/

			// Validation ?
			if (gVar.pKeys[SDLK_RETURN] || gVar.pKeys[SDLK_SPACE] || gVar.pKeys[gMetroCfg.pKeys[e_CfgKey_ButtonA]] ||
				MenuTimer_Check(MENU_TIMER_GEN))
			{
				Transit_Init(e_Transit_FadeOut);
				gMenu.nState = MENU_State_FadeOut;
//				Sfx_PlaySfx(FX_Menu_Clic, e_SfxPrio_10);	// Sfx.
			}
			else
				MenuTimer_Tick();

		}
		break;
	}


	//>>> Affichage.

	// Clear screen.
//	SDL_FillRect(gVar.pScreen, NULL, SDL_MapRGB(gVar.pScreen->format, MENUMAIN_BkgClr_R, MENUMAIN_BkgClr_G, MENUMAIN_BkgClr_B));
//	SDL_FillRect(gVar.pScreen, NULL, 0);
	RotoZoom_Draw();

	// Texte d'intro.
	if (gMenuHS.nEntry)
	{	// Avec entrée du nom.
		char	pRnk1[] = "YOUR RANKING IS NO.1 IN";
		static char	pRnk2[] = "THE ORTHEMA CITY.";
		pRnk1[19] += gMenuHS.nRank;
//		Font8_Print((SCR_Width / 2) - ((sizeof(pRnk1) - 1) * 4), 32, pRnk1, gVar.pnColors[e_Color_Gray]);
		Fnt8PrintXY((SCR_Width / 2) - ((sizeof(pRnk1) - 1) * 4), 32, pRnk1, e_Color_Gray)
//		Font8_Print((SCR_Width / 2) - ((sizeof(pRnk2) - 1) * 4), 40, pRnk2, gVar.pnColors[e_Color_Gray]);
		Fnt8PrintXY((SCR_Width / 2) - ((sizeof(pRnk2) - 1) * 4), 40, pRnk2, e_Color_Gray)

		static char	pEnter[] = "ENTER YOUR NAME";
//		Font8_Print((SCR_Width / 2) - ((sizeof(pEnter) - 1) * 4), 32+16+8, pEnter, gVar.pnColors[e_Color_Green]);
		Fnt8PrintXY((SCR_Width / 2) - ((sizeof(pEnter) - 1) * 4), 32+16+8, pEnter, e_Color_Green)
	}
	else
	{	// Sans entrée du nom.
		static char	pBest[] = "- BEST METRO RUNNERS -";
//		Font8_Print((SCR_Width / 2) - ((sizeof(pBest) - 1) * 4), 32+16, pBest, gVar.pnColors[e_Color_Green]);
		Fnt8PrintXY((SCR_Width / 2) - ((sizeof(pBest) - 1) * 4), 32+16, pBest, e_Color_Green)
	}

	// Table des scores.
//	Font8_Print(16+8, 64+16, "         NAME       ROUND  SCORE", gVar.pnColors[e_Color_Red]);
	Fnt8PrintXY(16+8, 64+16, "         NAME       ROUND  SCORE", e_Color_Red)
	u32	i;
	for (i = 0; i < HISC_Nb; i++)
	{
//		u32	nColor = gVar.pnColors[(gMenuHS.nEntry && i == gMenuHS.nRank ? e_Color_Yellow : e_Color_Gray)];		// Scores en gris, celui en cours en jaune.
		u32	nColor = (gMenuHS.nEntry && i == gMenuHS.nRank ? e_Color_Yellow : e_Color_Gray);		// Scores en gris, celui en cours en jaune.

		// No.
		char	pNo[] = "1";
		pNo[0] += i;
//		Font8_Print(16+8, 96+(i*16), pNo, nColor);
		Fnt8PrintXY(16+8, 96+(i*16), pNo, nColor)
		// Name.
//		Font8_Print(16+8+24, 96+(i*16), gpHighScores[i].pName, nColor);
		Fnt8PrintXY(16+8+24, 96+(i*16), gpHighScores[i].pName, nColor)
		char	pDots[HISC_NameLg] = "................";
		u32	t = strlen(gpHighScores[i].pName);
//		Font8_Print(16+8+24 + (t*8), 96+(i*16), &pDots[t], nColor);
		Fnt8PrintXY(16+8+24 + (t*8), 96+(i*16), &pDots[t], nColor)
		// Curseur quand entrée du nom.
		if (gMenuHS.nEntry && i == gMenuHS.nRank)
		if (((++gMenuHS.nBlink) & 16) && gMenuHS.nPos < HISC_NameLg - 1)
		{
//			Font8_Print(16+8+24 + (gMenuHS.nPos * 8), 96+(i*16), "\x60", nColor);
			Fnt8PrintXY(16+8+24 + (gMenuHS.nPos * 8), 96+(i*16), "\x60", nColor)
		}
		// Round.
		if (gpHighScores[i].nRound < LVL_NB)
		{
			char	pRnd[] = "00";
			MyItoA(gpHighScores[i].nRound + 1, pRnd);
//			Font8_Print(16+8+24+(18*8)+4, 96+(i*16), pRnd, nColor);
			Fnt8PrintXY(16+8+24+(18*8)+4, 96+(i*16), pRnd, nColor)
		}
		else
		{
//			Font8_Print(16+8+24+(18*8), 96+(i*16), "ALL", nColor);
			Fnt8PrintXY(16+8+24+(18*8), 96+(i*16), "ALL", nColor)
		}
		// Score.
		char	pScr[] = "0000000";
		MyItoA(gpHighScores[i].nScore, pScr);
//		Font8_Print(16+8+24+(23*8), 96+(i*16), pScr, nColor);
		Fnt8PrintXY(16+8+24+(23*8), 96+(i*16), pScr, nColor)
	}

	HUD_Part1Disp();

	return (nRetVal);
}

//=============================================================================

// Renvoie le high-score.
u32 Scr_HighScore_Get(void)
{
	return (gpHighScores[0].nScore);
}

// Scores - Check si un score entre au Hall of Fame.
// Out : -1, pas dedans / >= 0, rang.
s32 Scr_CheckHighSc(u32 nScore)
{
	s32	i, nRank;

	nRank = -1;
	for (i = HISC_Nb - 1; i >= 0; i--)
	{
		if (nScore >= gpHighScores[i].nScore)
		{
			nRank = i;
		}
	}

	return (nRank);
}

// Insère un nom dans la table.
s32 Scr_PutNameInTable(char *pName, u32 nScore, u32 nRound)
{
	s32	nRank = Scr_CheckHighSc(nScore);
	s32	i;

	if (nRank < 0) return (nRank);		// Ne devrait pas arriver.

	// Décalage de la table.
	for (i = HISC_Nb - 2; i >= nRank; i--)
	{
		strcpy(gpHighScores[i + 1].pName, gpHighScores[i].pName);
		gpHighScores[i + 1].nScore = gpHighScores[i].nScore;
		gpHighScores[i + 1].nRound = gpHighScores[i].nRound;
	}
	// Le score à insérer.
	strcpy(gpHighScores[nRank].pName, pName);
	gpHighScores[nRank].nScore = nScore;
	gpHighScores[nRank].nRound = nRound;

	return (nRank);
}

// RAZ de la table des high scores.
void Scr_RazTable(void)
{
	static char	pDefStr[HISC_NameLg] = "0123456789ABCDEF";
	u32	i;

	for (i = 0; i < HISC_Nb; i++)
	{
		strcpy(gpHighScores[i].pName, pDefStr);
		gpHighScores[i].nScore = 0;
	}

	// Table de scores par défaut.
	static struct SScore pDefTb[] =
	{
//		{ "****++++****++++", 100000, LVL_NB },
//		{ "CLEMENT 1702",     364800, LVL_NB },
		{ "LUCAS SOCIAL",     100000,  9 },
		{ "ALBERT MUDA",       90000,  8 },
		{ "OSCAR AMBAR",       80000,  7 },
		{ "SOPHIE FONFEC",     70000,  6 },
		{ "THIERRY DICULE",    60000,  5 },
	};
// Gérard Mansoif / Odette Ritus / Paul Igone / Rachel Hemme
// Thomas Ouaque / Théo Jasmin / Walter Ofilly
	for (i = 0; i < NBELEM(pDefTb) && i < HISC_Nb; i++)
	{
		strcpy(gpHighScores[i].pName, pDefTb[i].pName);
		gpHighScores[i].nScore = pDefTb[i].nScore;
		gpHighScores[i].nRound = pDefTb[i].nRound;
	}
//c	Scr_Save();

}

// Calcule le checksum de la table des scores.
u32 Scr_CalcChecksum(void)
{
	u32	i, j;
	u32	nChk = 0;

	for (i = 0; i < HISC_Nb; i++)
	{
		nChk += gpHighScores[i].nScore;
		nChk += gpHighScores[i].nRound;
		for (j = 0; j < HISC_NameLg; j++) nChk ^= ((u32)gpHighScores[i].pName[j]) << (8 * (j & 3));
	}
	return (nChk);
}

// Lecture du fichier des high-scores.
void Scr_Load(void)
{
	FILE	*pFile;
	u32	nChk;
	u32	nSz;

	if ((pFile = fopen(HISC_Filename, "rb")) != NULL)
	{
		// Le fichier existe, lecture.
		nSz = fread(gpHighScores, 1, HISC_Nb * sizeof(struct SScore), pFile);
		nSz += fread(&nChk, 1, sizeof(u32), pFile);
		fclose(pFile);
		// Taille ok ?
		if (nSz != (HISC_Nb * sizeof(struct SScore)) + sizeof(u32))
		{
			fprintf(stderr, "Scr_Load(): fread error! Resetting table.\n");
			Scr_RazTable();
		}
		// Checksum ok ?
		if (nChk != Scr_CalcChecksum())
		{
			// Wrong checksum, RAZ table.
			fprintf(stderr, "Scr_Load(): Wrong checksum! Resetting table.\n");
			Scr_RazTable();
		}
	}
	else
	{
		// Le fichier n'existe pas, RAZ table.
		Scr_RazTable();
	}

}

// Sauvegarde du fichier des high-scores.
void Scr_Save(void)
{
	FILE	*pFile;
	u32	nChk;
	u32	nSz;

	if ((pFile = fopen(HISC_Filename, "wb")) == NULL)
	{
		fprintf(stderr, "Scr_Save(): fopen failed. Unable to save highscores table.\n");
		return;
	}
	// Sauvegarde des enregistrements.
	nSz = fwrite(gpHighScores, 1, HISC_Nb * sizeof(struct SScore), pFile);
	// Checksum.
	nChk = Scr_CalcChecksum();
	nSz += fwrite(&nChk, 1, sizeof(u32), pFile);
	fclose(pFile);
	// Taille ok ?
	if (nSz != (HISC_Nb * sizeof(struct SScore)) + sizeof(u32))
	{
		fprintf(stderr, "Scr_Save(): fwrite error!\n");
	}

}

//=============================================================================

#define	CFG_FILENAME	"metro.cfg"
struct SMetroCfg	gMetroCfg;

// Calcul d'un petit checksum.
u32	CfgChecksumCalc(void)
{
	u32	i;
	u32	nSum = 0;

	for (i = 0; i < e_CfgKey_MAX; i++) nSum += gMetroCfg.pKeys[i];
	nSum += gMetroCfg.nVideoMode;

	return (nSum);
}

// Lecture de la config.
u32 Cfg_Load(void)
{
	FILE	*fPt;
	u32	nSz;

	static struct SMetroCfg	sCfgDefault = { { SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT, SDLK_LCTRL, 0 }, 2/*0*/, 0 };	// Cfg par défaut.

	fPt = fopen(CFG_FILENAME, "rb");
	if (fPt == NULL)
	{
		fprintf(stderr, "Cfg_Load(): fopen error.\n");
		goto _CfgLoad_def;
	}
	nSz = fread(&gMetroCfg, 1, sizeof(struct SMetroCfg), fPt);
	fclose(fPt);
	if (nSz != sizeof(struct SMetroCfg))
	{
		fprintf(stderr, "Cfg_Load(): fread error.\n");
		goto _CfgLoad_def;
	}
	// Test du checksum.
	nSz = CfgChecksumCalc();
	if (gMetroCfg.nChecksum != (u16)nSz)
	{
		fprintf(stderr, "Cfg_Load(): Checksum error.\n");
		goto _CfgLoad_def;
	}
	return (0);
_CfgLoad_def:
	fprintf(stderr, "Cfg_Load(): Default configuration used.\n");
	memcpy(&gMetroCfg, &sCfgDefault, sizeof(struct SMetroCfg));
	return (1);
}

