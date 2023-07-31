
#include "includes.h"

//#define	DEBUG_NO_READYSETGO	(1)		// Comment to remove.
//#define	DEBUG_NO_READYYOURSELF	(1)		// Comment to remove.
//#define DEBUG_KEYS	(1)		// A commenter.


struct SGame	gGame;

//=============================================================================

enum
{
	e_MsgPrio_TimerStop = 0,
	e_MsgPrio_SkateGood = 0,
	e_MsgPrio_ReadySetGo = 0,
	e_MsgPrio_CanKick = 0,
	e_MsgPrio_SpeedUp = 100,
	e_MsgPrio_GoalIn = 200,
	e_MsgPrio_Death = 200,
};

#define	MSG_SZ_MAX	((SCR_Width - 16) / 8)
#define	MSG_NB	(2)
struct SMsg
{
	char	pMsg[MSG_SZ_MAX + 1];
	s32	nPosX;		// Position X à l'écran.
	s32	nTime;		// Affiche le message pdt x frames.
	u16	nColor;
	u8	nPrio;

};
struct SMsg	gMsg[MSG_NB];

// RAZ msgs.
void Msg_Reset(void)
{
	u32	i;

	for (i = 0; i < MSG_NB; i++)
		gMsg[i].nTime = 0;		// RAZ message.

}

// Init d'un message.
void Msg_Init(u32 nLine, char *pMsg, s32 nTime, u16 nColor, u8 nPrio)
{
	if (nLine >= MSG_NB) { fprintf(stderr, "Msg_Init(): Invalid line number! (%d)\n", nLine); return; }

	if (!(gMsg[nLine].nTime == 0 || nPrio >= gMsg[nLine].nPrio)) return;

	s32	i;

	i = strlen(pMsg);
	if (i > MSG_SZ_MAX) i = MSG_SZ_MAX;
	strncpy(gMsg[nLine].pMsg, pMsg, i);
	gMsg[nLine].pMsg[i] = 0;

	gMsg[nLine].nPosX = (SCR_Width / 2) - (i * 4);

	gMsg[nLine].nTime = nTime;
	gMsg[nLine].nPrio = nPrio;
	gMsg[nLine].nColor = nColor;

}

#define	MSG_L1_Y	(BKG_Y_ORG + 8)
// Affichage d'un message.
void Msg_Display(void)
{
	u32	i;

	for (i = 0; i < MSG_NB; i++)
		if (gMsg[i].nTime)
		{
			Font8_Print(gMsg[i].nPosX + 1, (MSG_L1_Y + 1) + (i * 16), gMsg[i].pMsg, gVar.pnColors[e_Color_Black]);
			Font8_Print(gMsg[i].nPosX    , (MSG_L1_Y    ) + (i * 16), gMsg[i].pMsg, gMsg[i].nColor);

			if (gMsg[i].nTime > 0) gMsg[i].nTime--;
		}

}

//=============================================================================

// Conversion d'un temps en version affichable.
void Time_Convert2Display(char *pTime, u32 nTimer)
{
	static char	*pTimeInit = "0'00\"0";

	strcpy(pTime, pTimeInit);	// Init chaîne finale.

	u32	t, k;
	t = nTimer;
	// Minutes.
	if (t > TIME_1S * 60)
	{
		k = t / (TIME_1S * 60);
		t -= k * (TIME_1S * 60);
		pTime[1] = 0;
		MyItoA(k, pTime);
		pTime[1] = '\'';
	}
	// Secondes.
	if (t > TIME_1S)
	{
		k = t / TIME_1S;
		t -= k * TIME_1S;
		pTime[4] = 0;
		MyItoA(k, pTime);
		pTime[4] = '"';
	}
	// Millièmes.
	{
		k = (t >> 8) / 100;
		MyItoA(k, pTime);
	}
}

// Temps, tick.
void Time_Tick(void)
{
	// Si compteur arrêté...
	if (gGame.nTimerStop) return;

	// Temps freezé ?
	if (gGame.nTimerFreeze)
	{
		gGame.nTimerFreeze--;
		return;
	}

	// Si temps additionnel, on commence par ce temps.
	if (gGame.nTimer2)
	{
		if (gGame.nTimer2 >= TIME_1F)
			gGame.nTimer2 -= TIME_1F;
		else
		{
			gGame.nTimer1 -= gGame.nTimer2;
			gGame.nTimer2 = 0;
		}
	}
	else
	// Timer normal.
	{
		if (gGame.nTimer1 >= TIME_1F)
			gGame.nTimer1 -= TIME_1F;
		else
			gGame.nTimer1 = 0;

		// Cheat unlimited time.
		if ((gCCodes.nCheat & e_Cheat_UnlimitedTime) && gGame.nTimer1 <= TIME_1F) gGame.nTimer1 = TIME_1F;

	}

}

// Gèle le temps.
void Time_Freeze(void)
{
	gGame.nTimerFreeze = FPS;
	Msg_Init(0, "TIMER STOP", FPS, gVar.pnColors[e_Color_Red], e_MsgPrio_TimerStop);

}

//=============================================================================

// HUD.
//#define	HUD_TOP_HT		(16)//+8)
//#define	HUD_BOTTOM_HT	(40)//-8)
//
#define	HUD_SCORE_X	(48)
#define	HUD_SCORE_Y	(0)//+4)
#define	HUD_HISCORE_X	(SCR_Width - HUD_SCORE_X - (7 * 8))
//
#define	HUD_ROUND_X	(16)
#define	HUD_ROUND_Y	(200-8)//+4)
#define	HUD_AREA_X	(HUD_ROUND_X)
#define	HUD_AREA_Y	(HUD_ROUND_Y + 16)
#define	HUD_AREA_RCT_X	(HUD_AREA_X + (8*5))
#define	HUD_AREA_RCT_Y	(HUD_AREA_Y)
#define	HUD_AREA_RCT_HT	(8)
#define	HUD_TIME_X	(HUD_ROUND_X + (10*8))
#define	HUD_TIME_Y	(HUD_ROUND_Y)
#define	HUD_ROUND3_X	(SCR_Width-(10*8))
#define	HUD_ROUND3_Y	(HUD_ROUND_Y)

// HUD : Rectangles haut et bas + score.
void HUD_Part1Disp(void)
{
	// Les rectangles haut/bas.
	SDL_Rect	sRct;
	u32	nColor = gVar.pnColors[e_Color_HUDBkg];

	sRct.x = 0;
	sRct.y = 0;
	sRct.w = SCR_Width;
	sRct.h = HUD_TOP_HT;
	SDL_FillRect(gVar.pScreen, &sRct, nColor);

	sRct.x = 0;
	sRct.y = SCR_Height - HUD_BOTTOM_HT;
	sRct.w = SCR_Width;
	sRct.h = HUD_BOTTOM_HT;
	SDL_FillRect(gVar.pScreen, &sRct, nColor);

	// Score.
	nColor = gVar.pnColors[e_Color_Gray];
	char	pScore[] = "0000000";
	MyItoA(gGame.nScore, pScore);
	Font8_Print(HUD_SCORE_X, HUD_SCORE_Y, " SCORE", nColor);
	Font8_Print(HUD_SCORE_X, HUD_SCORE_Y + 8, pScore, nColor);

	// Hi-Score.
	char	pHiScore[] = "0000000";
	MyItoA(gGame.nHiScore, pHiScore);
	Font8_Print(HUD_HISCORE_X - 4, HUD_SCORE_Y, "HI-SCORE", nColor);
	Font8_Print(HUD_HISCORE_X, HUD_SCORE_Y + 8, pHiScore, nColor);

}

void Spr_DisplayOneNoList(u32 nSprNo, s32 nPosX, s32 nPosY);

// HUD in game.
void HUD_Display(void)
{
	SDL_Rect	sRct;
	u32	nColor;

	// MAJ du hi-score si nécessaire.
	if (gGame.nScore > gGame.nHiScore) gGame.nHiScore = gGame.nScore;

	// Rectangles gris + score.
	HUD_Part1Disp();

	// Round x.
	nColor = gVar.pnColors[e_Color_Gray];
	char	pRound[] = "ROUND 00";
	MyItoA(gGame.nRound + 1, pRound);
	Font8_Print(HUD_ROUND_X, HUD_ROUND_Y, pRound, nColor);

	// Area.
	nColor = gVar.pnColors[e_Color_Cyan];
	Font8_Print(HUD_AREA_X, HUD_AREA_Y, "AREA", nColor);

	s32	nSzMax = ((gGame.pMap->nMapLg - 10) / 2.5) + 8;
	s32	nSzCur = (((gGame.nPlayerPosX >> 8) / SCROLL_TILE_W) / 2.5) + 8;
	if (nSzCur > nSzMax) nSzCur = nSzMax;		// Oui, parce qu'on dépasse l'arrivée.

	nColor = gVar.pnColors[e_Color_AreaBlue];
	sRct.x = HUD_AREA_RCT_X;
	sRct.y = HUD_AREA_RCT_Y,
	sRct.w = nSzCur;
	sRct.h = HUD_AREA_RCT_HT;
	SDL_FillRect(gVar.pScreen, &sRct, nColor);

	static u64 pSprAdd[4] = { 0, 1, 0, 2 };
	Spr_DisplayOneNoList(e_Spr_Hero_HUDRun + pSprAdd[nSzCur & 3], HUD_AREA_RCT_X + nSzCur - 8, HUD_AREA_RCT_Y);

	nColor = gVar.pnColors[e_Color_AreaGray];
	sRct.x = HUD_AREA_RCT_X + nSzCur;
	sRct.y = HUD_AREA_RCT_Y,
	sRct.w = nSzMax - nSzCur;
	sRct.h = HUD_AREA_RCT_HT;
	SDL_FillRect(gVar.pScreen, &sRct, nColor);

	// Time.
	nColor = gVar.pnColors[e_Color_Yellow];
	Font8_Print(HUD_TIME_X, HUD_TIME_Y, "TIME", nColor);

	char	pTime[] = "      ";
	Time_Convert2Display(pTime, gGame.nTimer1);
	Font8_Print(HUD_TIME_X + (5*8), HUD_TIME_Y, pTime, nColor);

	if ((gGame.nRound & 3) == 3)	// Temps additionnel tous les 4 rounds.
	{
		Time_Convert2Display(pTime, gGame.nTimer2);

		// Total. (Sur le SET du ready, set, go).
		if (gGame.nHUDTimersDisp & 4)
		{
			nColor = gVar.pnColors[e_Color_CyanDark];
			Font8_Print(HUD_ROUND3_X - (2*8), HUD_ROUND3_Y, "TOTAL", nColor);
			//
			Time_Convert2Display(pTime, gGame.nTimer2);
			Font8_Print(HUD_ROUND3_X + (4*8), HUD_ROUND3_Y, &pTime[2], nColor);
		}
		else
		// Temps additionnel en phase de jeu (çàd à partir du GO).
		if (gGame.nPhase == e_Game_Play)
		{
			nColor = gVar.pnColors[e_Color_Yellow];
			pTime[0] = '+';
			pTime[1] = ' ';
			Font8_Print(HUD_TIME_X + (5*8)+(7*8), HUD_TIME_Y, pTime, nColor);
		}
	}

	// Temps des rounds précédents.
	nColor = gVar.pnColors[e_Color_CyanDark];
	u32	i;
	pRound[3] = 0;
	for (i = 0; i < (gGame.nHUDTimersDisp & 3); i++)
	{
		pRound[0] = ' ';
		pRound[1] = ' ';
		pRound[2] = 0;
		MyItoA((gGame.nRound & ~3) + i + 1, pRound);
		pRound[2] = 'R';
		Font8_Print(HUD_ROUND3_X, HUD_ROUND3_Y + (i*8), pRound, nColor);
		//
		Time_Convert2Display(pTime, gGame.pnTimerR3[i]);
		Font8_Print(HUD_ROUND3_X + (4*8), HUD_ROUND3_Y + (i*8), &pTime[2], nColor);
	}

	// Affichage du message sur l'aire de jeu.
	Msg_Display();

}

//=============================================================================

// Prio + 16 : Pour pouvoir placer une prio entre l'ombre et tous les sprites. Ex : Héros écrasé (flattened).

// Affichage d'un sprite sur le sol.
void Game_SprDisplay(u32 nSprNo, s32 nPosX, s32 nPosY, s32 nPosZ)
{
	s32	nX;
	nX = (nPosX >> 8) - (gGame.nGndX >> 8);		// Il ne faut pas mettre la virgule dans le calcul !
	SprDisplayAbsolute(nSprNo, nX - SCROLL_GND_OFFS + ((nPosY >> 8) / 2), (nPosY >> 8) + GND_Y_ORG + (nPosZ >> 8), (nPosY >> 8) + 16);
}

void Game_SprDisplayPrio(u32 nSprNo, s32 nPosX, s32 nPosY, s32 nPosZ, s32 nPrio)
{
	s32	nX;
	nX = (nPosX >> 8) - (gGame.nGndX >> 8);
	SprDisplayAbsolute(nSprNo, nX - SCROLL_GND_OFFS + ((nPosY >> 8) / 2), (nPosY >> 8) + GND_Y_ORG + (nPosZ >> 8), (nPrio != -1 ? nPrio : (nPosY >> 8) + 16));
}

//=============================================================================

// Initialisation d'un niveau.
void Game_RoundInit(void)
{
	Anm_InitEngine();

	Loader_MapPtr_Set(gGame.nRound, &gGame.pMap, &gGame.pMst);

	gGame.nPlayerPosX = ((2*32)+16) << 8;//0;//(3*32) << 8;
	gGame.nPosXMax = gGame.nPlayerPosX;
	gGame.nPlayerPosY = ((2*16) + 8) << 8;
	gGame.nPlayerPosZ = 0;
	gGame.nPlayerSpdX = gGame.nPlayerSpdY = gGame.nPlayerSpdZ = 0;
	gGame.nPlayerGnd = 1;
	gGame.nPlayerAnm = Anm_Set(gAnm_Hero_Idle, -1);

	gGame.nPlayerLastX = gGame.nPlayerPosX;
	gGame.nPlayerLastY = gGame.nPlayerPosY;
	gGame.nPlayerLastZ = gGame.nPlayerPosZ;

	gGame.nFinishLine = -1;
	gGame.nSpeedUp = 0;
	gGame.nSlowDown = 0;
	gGame.nM12SlowDown = 0;
	gGame.nM12Eject = 0;
	gGame.nSkateboard = 0;
	gGame.nLateJump = 0;

	gGame.nLRDir = 0;
	gGame.nLRTime = 0;
	gGame.nLRVar = 0;

	// Etincelle en fin de niveau, quand temps < 10 s.
	gGame.nTimeSparkleAnm = Anm_Set(gAnm_TimeSparkle, -1);
	gGame.nTimeSparkleSide = 0;

/*
//tmp, init à la con au cas ou on commence sur un round autre que 0.
gGame.pnTimerR3[0] = TIME_1S * 5;//15;
//gGame.pnTimerR3[0] += TIME_1S / 2;//15;
gGame.pnTimerR3[1] = TIME_1S * 3;//25;
gGame.pnTimerR3[2] = TIME_1S * 2;//35;
*/


	// Temps additionnel tous les 4 rounds.
	if ((gGame.nRound & 3) == 3)
		gGame.nTimer2 = gGame.pnTimerR3[0] + gGame.pnTimerR3[1] + gGame.pnTimerR3[2];
	else
		gGame.nTimer2 = 0;

	gGame.nTimer1 = TIME_1S * 60;		// Temps par défaut, qui normalement sera écrasé avec l'init du scroll (qui fera une passe sur les monstres).

	// Affichage des x timers précédents.
	gGame.nHUDTimersDisp = gGame.nRound & 3;
	if ((gGame.nRound & 3) == 3)
		gGame.nHUDTimersDisp = 0;

	gGame.nTimerStop = 0;
	gGame.nTimerFreeze = 0;
	gGame.nRoundOverTimer = 0;

	Msg_Reset();

	Mst_InitEngine();

	// Damier bleu (0) ou gris (1).
	{
	u32	nIdx = ((gGame.nRound & 3) == 3 ? 1 : 0);
	SDL_Rect	sSrc, sDst;
	sSrc.x = (5 + nIdx) * 32;
	sSrc.y = 0;
	sSrc.w = 32;
	sSrc.h = 16;
	sDst.x = 1 * 32;
	sDst.y = 0;
	SDL_BlitSurface(gGfxBlk.pBlocks, &sSrc, gGfxBlk.pBlocks, &sDst);
	}

	Scroll_Init();

//gGame.nTimer1 = TIME_1S * 3;//15;//160;	//tst

}

// Initialisation d'une partie.
void Game_Init(void)
{
	gGame.nRound = (gCCodes.nCheat & e_Cheat_LevelSelect ? gCCodes.nLevel : 0);
	gGame.pnTimerR3[0] = gGame.pnTimerR3[1] = gGame.pnTimerR3[2] = 0;				// Utile en cas de cheat "level".

	gGame.nScore = 0;

#if defined(DEBUG_NO_READYYOURSELF)
	gGame.nPhase = e_Game_RoundInit;
#else
	gGame.nPhase = e_Game_ReadyYourself;
	Transit_Init(e_Transit_FadeIn);
#endif

	gGame.nExitCode = e_GameEC_Playing;
}

//=============================================================================

#define	PLYR_ACC	(0x40)//(0x20)//0x40)				// Accélération.

#define	PLYR_SPDX_SLOWDOWN	(0x100)		// Vitesse max quand slowdown.
#define	PLYR_SPDX_NORM_MAX	(0x400)		// Vitesse max normale. (ST)
#define	PLYR_SPDX_SPDUP_MAX	(0x800)		// Vitesse max avec speed-up. (ST)
#define	PLYR_SPDX_BOOST	(0x400)			// Speed boost quand super saut.

#define	PLYR_SPDY_NORM_MAX	(0x100)		// Vitesse max normale en y. (ST)

// enum kb.
enum
{
	e_Kb_Up 		= 1 << 0,
	e_Kb_Right		= 1 << 1,
	e_Kb_Down		= 1 << 2,
	e_Kb_Left		= 1 << 3,
	e_Kb_ButtonA	= 1 << 4,
};

// enum anims joueur.
enum
{
	e_PlyrAnm_Idle = 0,
	e_PlyrAnm_Run,
	e_PlyrAnm_Jump,
	e_PlyrAnm_JumpForce1,
	e_PlyrAnm_JumpForce2,
	e_PlyrAnm_JumpForce3,
	e_PlyrAnm_LandOnBack,
	e_PlyrAnm_Flattened,
	e_PlyrAnm_Flattened2,
	e_PlyrAnm_Pitfall,
	e_PlyrAnm_WinBreathing,
	e_PlyrAnm_WinMuscleDisplay,
	e_PlyrAnm_WinJump,
	e_PlyrAnm_DeathSkeleton,
	e_PlyrAnm_DeathExhausted,
	e_PlyrAnm_FootBump,
	e_PlyrAnm_FallFront,
	e_PlyrAnm_FallBack,
};
u64 *gpHeroAnm[] =
{
	gAnm_Hero_Idle,
	gAnm_Hero_Run,
	gAnm_Hero_Jump,
	gAnm_Hero_JumpForce1,
	gAnm_Hero_JumpForce2,
	gAnm_Hero_JumpForce3,
	gAnm_Hero_LandOnBack,
	gAnm_Hero_Flattened,
	gAnm_Hero_Flattened2,
	gAnm_Hero_Pitfall,
	gAnm_Hero_WinBreathing,
	gAnm_Hero_MuscleDisplay,
	gAnm_Hero_WinJump,
	gAnm_Hero_DeathSkeleton,
	gAnm_Hero_DeathExhausted,
	gAnm_Hero_FootBump,
	gAnm_Hero_FallFront,
	gAnm_Hero_FallBack,
};

//void Scroll_Freeze(void);
//void Scroll_Unfreeze(void);


// Met le joueur en skateboard.
// Out: 1 = Ok / 0 = Not set.
u32 Game_Player_SkateboardSet(void)
{
	// Déjà en skate ?
	if (gGame.nSkateboard)
	{
		// Oui, bonus.
		gGame.nScore += 2000;		// Bonus 2000 pts.
		Msg_Init(0, "GOOD! 2000 POINTS", FPS, gVar.pnColors[e_Color_Yellow], e_MsgPrio_SkateGood);
		return (0);
	}

	// Si en train de chuter, pas de skate.
	u32	nKey = Anm_GetKey(gGame.nPlayerAnm);
	if (nKey == e_AnmKey_Hero_JumpForce2 ||
		nKey == e_AnmKey_Hero_FallFront ||
		nKey == e_AnmKey_Hero_FallBack ||
		nKey == e_AnmKey_Hero_LandOnBack ||
		nKey == e_AnmKey_Hero_LandOnBackStop)
		return (0);

	// Init skate. On colle le perso au sol.
	gGame.nSkateboard = 1;
	gGame.nPlayerGnd = 1;
	gGame.nPlayerPosZ = 0;
	gGame.nPlayerSpdZ = 0;

	return (1);
}

// Le joueur quitte le skateboard.
// In :
//    nFromPlayer = 0 => Appel depuis un monstre. On est dans le MstManage et il se peut donc que le skate soit ajouté à la liste des msts AVANT le mst qui fait
//						quitter le joueur. Dans ce cas, il faut que l'init du skate affiche un sprite pour éviter une éventuelle disparition pendant 1 frame.
//    nFromPlayer = 1 => Appel depuis les routines du joueur. Il ne faut pas que l'init du monstre affiche un sprite car il y a un décalage (à cause du scroll)
//						ET on est sûr que le monstre sera traité, puisqu'on n'est pas encore dans le MstManage.
void Game_Player_SkateboardLeave(u32 nFromPlayer)
{
	// Si pas en skate...
	if (gGame.nSkateboard == 0) return;

	// Init.
	gGame.nSkateboard = 0;
//	gGame.nPlayerGnd = 1;
//	gGame.nPlayerPosZ = 0;
//	gGame.nPlayerSpdZ = 0;

	/*
	[mst6] Skateboard
	phase = 0:3: Waiting - Go away - Go away no 1st spr
	prm = 4:7:			; tmp
	*/
	u8	nPrm;
	nPrm = (1 + nFromPlayer);
	Mst_Add(e_Mst6_Skateboard, gGame.nPlayerPosX >> 8, gGame.nPlayerPosY >> 8, (u8 *)&nPrm, -1);

}

// Init du saut par springboard/cracker.
void Game_Player_JumpInit(u32 nIdx)
{
	if (gGame.nPlayerGnd == 0) return;

	// Si nécessaire, lâche le skate.
	Game_Player_SkateboardLeave(0);
	// Dans tous les cas (spring board ou cracker), coupe le slow down.
	gGame.nSlowDown = 0;

	// En train de se casser la gueule => on empêche le déclenchement du super saut. (Ex : Hurdle, chute sur un cracker).
	u32	nLateJumpVal;
	u32	nKey = Anm_GetKey(gGame.nPlayerAnm);
	if (nKey == e_AnmKey_Hero_FallFront ||
		nKey == e_AnmKey_Hero_FallBack ||
		nKey == e_AnmKey_Hero_LandOnBack ||
		nKey == e_AnmKey_Hero_LandOnBackStop)
	{
		nLateJumpVal = 0;		// Pas de late jump possible.
		nIdx = 1;				// On forcera l'atterrissage sur le dos, y compris pour la springboard.
		goto _NoSuperJump;
	}

	// Cas normal.
	if (gVar.pKeysSDL[gMetroCfg.pKeys[e_CfgKey_ButtonA]])
	{
		// A enfoncé, => super saut.
		gGame.nPlayerGnd = 0;
		gGame.nPlayerSpdZ = -0x600;
		gGame.nPlayerAnm = Anm_SetIfNew(gpHeroAnm[e_PlyrAnm_JumpForce3], gGame.nPlayerAnm);
		gVar.pKeys[gMetroCfg.pKeys[e_CfgKey_ButtonA]] = 0;
	}
	else
	{
		nLateJumpVal = 8;
_NoSuperJump:
		// Saut 1 (pirouette) ou 2 (atterrissage sur le dos).
		gGame.nPlayerGnd = 0;
		gGame.nPlayerSpdZ = -0x600;
		gGame.nPlayerAnm = Anm_SetIfNew(gpHeroAnm[e_PlyrAnm_JumpForce1 + nIdx], gGame.nPlayerAnm);
//		gGame.nLateJump = 8;
		gGame.nLateJump = nLateJumpVal;
	}

}

// Test du late jump : Saut commencé, mais on a droit à quelques frames de plus pour déclencher le super saut. Sinon c'est impossible sans arriver avec le bouton de saut déjà enfoncé.
// Note : Ca marche parce que les anims sont identiques à la clef près et les inits de vitesse identiques.
void Anm_SetKey(s32 nSlotNo, u32 nKey);
void Game_Player_LateJumpCheck(void)
{
	if (gGame.nLateJump == 0) return;
	gGame.nLateJump--;

	if (Anm_GetKey(gGame.nPlayerAnm) != e_AnmKey_Hero_JumpForce1 &&
		Anm_GetKey(gGame.nPlayerAnm) != e_AnmKey_Hero_JumpForce2) return;	// Seulement sur pirouettes M2 et M3.

	if (gVar.pKeysSDL[gMetroCfg.pKeys[e_CfgKey_ButtonA]])
	{
		Anm_SetKey(gGame.nPlayerAnm, e_AnmKey_Hero_JumpForce3);
		gVar.pKeys[gMetroCfg.pKeys[e_CfgKey_ButtonA]] = 0;
		gGame.nLateJump = 0;
	}
}

// Le joueur tombe dans un pitfall.
void Game_Player_PitfallSet(void)
{
	gGame.nPlayerAnm = Anm_SetIfNew(gpHeroAnm[e_PlyrAnm_Pitfall], gGame.nPlayerAnm);
	if (Anm_GetKey(gGame.nPlayerAnm) != e_AnmKey_Hero_Pitfall) return;

	Game_Player_SkateboardLeave(0);		// Si nécessaire, lâche le skate.
	gGame.nSpeedUp = 0;					// Speed-up stop.

	gGame.nPlayerSpdX = 0;
	gGame.nPlayerPosX &= ~0xFF;		// !!! Bien clearer la virgule quand arrêt (pour scroll) !!!

}

// Le joueur se fait écraser.
void Game_Player_FlattenedSet(void)
{
	if (Anm_GetKey(gGame.nPlayerAnm) == e_AnmKey_Hero_Death) return;

	// Si nécessaire, lâche le skate.
	Game_Player_SkateboardLeave(0);
	gGame.nSpeedUp = 0;				// Speed-up stop.

	if (Anm_GetKey(gGame.nPlayerAnm) == e_AnmKey_Hero_Flattened)
		gGame.nPlayerAnm = Anm_Set(gpHeroAnm[e_PlyrAnm_Flattened2], gGame.nPlayerAnm);		// Re-flattened. !!! Anm-SET !!!
	else
		gGame.nPlayerAnm = Anm_SetIfNew(gpHeroAnm[e_PlyrAnm_Flattened], gGame.nPlayerAnm);

	gGame.nPlayerSpdX = 0;
	gGame.nPlayerPosX &= ~0xFF;		// !!! Bien clearer la virgule quand arrêt (pour scroll) !!!

}

// Prise de pieds dans un obstacle.
void Game_Player_FootBumpInit(void)
{
	gGame.nPlayerAnm = Anm_SetIfNew(gpHeroAnm[e_PlyrAnm_FootBump], gGame.nPlayerAnm);
	if (Anm_GetKey(gGame.nPlayerAnm) != e_AnmKey_Hero_FootBump) return;

	Game_Player_SkateboardLeave(0);		// Si nécessaire, lâche le skate.
	gGame.nSpeedUp = 0;					// Speed-up stop.

	// Vitesse X min/max.
	if (gGame.nPlayerSpdX > 0x200) gGame.nPlayerSpdX = 0x200;
	else
	if (gGame.nPlayerSpdX < 0x100) gGame.nPlayerSpdX = 0x100;

	// Init saut.
	gGame.nPlayerSpdZ = -0x400;

}

// Chute en avant (roulé boulé).
void Game_Player_FallFrontInit(void)
{
	gGame.nPlayerAnm = Anm_SetIfNew(gpHeroAnm[e_PlyrAnm_FallFront], gGame.nPlayerAnm);
	if (Anm_GetKey(gGame.nPlayerAnm) != e_AnmKey_Hero_FallFront) return;

	Game_Player_SkateboardLeave(0);		// Si nécessaire, lâche le skate.
	gGame.nSpeedUp = 0;					// Speed-up stop.

	// Vitesse X min/max.
	if (gGame.nPlayerSpdX > 0x200) gGame.nPlayerSpdX = 0x200;
	else
	if (gGame.nPlayerSpdX < 0x100) gGame.nPlayerSpdX = 0x100;

	// Coupe le saut si nécessaire.
	if (gGame.nPlayerSpdZ < 0) gGame.nPlayerSpdZ = 0;

}

// Chute en arrière.
void Game_Player_FallBackInit(void)
{
	gGame.nPlayerAnm = Anm_SetIfNew(gpHeroAnm[e_PlyrAnm_FallBack], gGame.nPlayerAnm);
	if (Anm_GetKey(gGame.nPlayerAnm) != e_AnmKey_Hero_FallBack) return;

	Game_Player_SkateboardLeave(0);		// Si nécessaire, lâche le skate.
	gGame.nSpeedUp = 0;					// Speed-up stop.

	// Recul.
	s32	nSpd = ABS(gGame.nPlayerSpdX);
	if (nSpd < 0x300) nSpd = 0x300;			// Assez élevé, car décrémentation rapide.
	else
	if (nSpd > PLYR_SPDX_NORM_MAX) nSpd = PLYR_SPDX_NORM_MAX;
	gGame.nPlayerSpdX = -nSpd;

	// Coupe le saut si nécessaire.
	if (gGame.nPlayerSpdZ < 0) gGame.nPlayerSpdZ = 0;

}

#define	GAME_SPEEDUP_TIME	(FPS * 4)
// Ramasse une cannette verte, speed-up.
void Game_Player_SpeedUp(void)
{
	gGame.nSpeedUp = GAME_SPEEDUP_TIME;
}

// Points bonus + message.
void Game_Player_PointsBonus(u32 nPts, char *pStr)
{
	gGame.nScore += nPts;
	if (pStr != NULL)
		Msg_Init(0, pStr, FPS, gVar.pnColors[e_Color_Red], e_MsgPrio_CanKick);
}

#define	GAME_ROUNDOVER_ANM_TIMER	(180)
// Gestion de la ligne d'arrivée.
void Game_FinishLineManage(int *pnKb)
{
	static u8	nBlink;

	if (gGame.nFinishLine < 0) return;	// Pas d'arrivée.
	if (gGame.nPlayerPosX < gGame.nFinishLine) return;	// Pas passée.

	if (gGame.nTimerStop == 0)			// Premier passage ?
	{
		gGame.nTimerStop = 1;					// Timer stop.
		Sfx_PlaySfx(e_Sfx_Go, e_SfxPrio_0);		// Sfx, masque la coupure sèche du YM.
		Music_Start(e_YmMusic_RoundOver, 0);

		// Arrivée en saut long sur un 'cracker'. (Si c'est juste le cracker sans saut long, ça ne fait rien).
		if (Anm_GetKey(gGame.nPlayerAnm) == e_AnmKey_Hero_JumpForce3)
		{
			gGame.nScore += 5000;		// Score + 5000.
			Msg_Init(1, "NICE! 5000 POINTS", FPS, gVar.pnColors[e_Color_Yellow], e_MsgPrio_GoalIn);
		}

		// Si skateboard, lâche le skate.
		if (gGame.nSkateboard)
		{
			Game_Player_SkateboardLeave(1);
			gGame.nScore += 10000;		// Arrivée avec skate, +10000.
			Msg_Init(1, "GOOD! 10000 POINTS", FPS, gVar.pnColors[e_Color_Yellow], e_MsgPrio_GoalIn);
		}

		gGame.nSpeedUp = 0;		// Coupe un éventuel speed-up.
		nBlink = 0;				// RAZ clignotement.
	}

	// Msg "Goal in!".
	if ((nBlink++ & 7) == 0)
		Msg_Init(0, (gGame.nRound == LVL_NB - 1 ? "ALL CLEAR!" : "GOAL IN!"), -1, gVar.pnColors[(nBlink & 8 ? e_Color_Yellow : e_Color_Red)], e_MsgPrio_GoalIn);

	// On déplace un peu le perso après la ligne.
	*pnKb = 0;
	if (gGame.nPlayerPosX < gGame.nFinishLine + ((2 * SCROLL_TILE_W) << 8))
		*pnKb |= e_Kb_Right;
	else
	if (gGame.nPlayerSpdX > 0)
		*pnKb |= e_Kb_Left;
	else
	{
		gGame.nPlayerSpdX = 0;
//org		if (gGame.nPlayerGnd)
//org			gGame.nPlayerAnm = Anm_SetIfNew(gpHeroAnm[((gGame.nRound & 3) == 3 ? e_PlyrAnm_WinMuscleDisplay : e_PlyrAnm_WinBreathing)], gGame.nPlayerAnm);

		if (gGame.nPlayerGnd)
		{
			if ((gGame.nRound & 3) == 3 && Anm_GetKey(gGame.nPlayerAnm) != e_AnmKey_Hero_Victory)	// Saut périlleux tous les 4 rounds avant le muscle display.
			{
				*pnKb |= e_Kb_ButtonA;			// Saut normal.
//				gGame.nPlayerGnd = 0;			// Super saut.
//				gGame.nPlayerSpdZ = -0x600;
				gGame.nPlayerAnm = Anm_SetIfNew(gpHeroAnm[e_PlyrAnm_WinJump], gGame.nPlayerAnm);
			}
			else
				gGame.nPlayerAnm = Anm_SetIfNew(gpHeroAnm[((gGame.nRound & 3) == 3 ? e_PlyrAnm_WinMuscleDisplay : e_PlyrAnm_WinBreathing)], gGame.nPlayerAnm);
		}

		if (++gGame.nRoundOverTimer > GAME_ROUNDOVER_ANM_TIMER)
		{
			// Si ce n'est pas encore fait, on rajoute l'affichage du timer du round + on note le temps restant de ce round.
			if ((gGame.nRound & 3) != 3)
			if (gGame.nHUDTimersDisp != ((gGame.nRound + 1) & 3))
			{
				gGame.pnTimerR3[gGame.nRound & 3] = gGame.nTimer1;
				gGame.nHUDTimersDisp = (gGame.nRound + 1) & 3;
			}

			// Score : Compteur de temps restant.
			if (gGame.nTimer2)		// Temps additionnel s'il en reste.
			{
				if (gGame.nTimer2 > TIME_1F * 6)
				{
					gGame.nTimer2 -= TIME_1F * 6;
					gGame.nScore += 100;
				}
				else
					gGame.nTimer2 = 0;
				//
				gGame.nRoundOverTimer--;		// Pour garder le timer à GAME_ROUNDOVER_ANM_TIMER.
			}
			else
			if (gGame.nTimer1)		// Ensuite le temps normal.
			{
				if (gGame.nTimer1 > TIME_1F * 6)
				{
					gGame.nTimer1 -= TIME_1F * 6;
					gGame.nScore += 100;
				}
				else
					gGame.nTimer1 = 0;
				//
				gGame.nRoundOverTimer--;		// Pour garder le timer à GAME_ROUNDOVER_ANM_TIMER.
			}
			else	// (gGame.nTimer1 == 0)
			{
				// Terminé, on quitte.
//org				if (gGame.nRoundOverTimer > GAME_ROUNDOVER_ANM_TIMER + (FPS * 3))
//org					gGame.nPhase = e_Game_RoundCompleted;

				if (gGame.nRoundOverTimer == GAME_ROUNDOVER_ANM_TIMER + (FPS * 3))
				{
					Transit_Init(e_Transit_FadeOut);				// On initialise la transition fermante...
				}
				else
				if (gGame.nRoundOverTimer > GAME_ROUNDOVER_ANM_TIMER + (FPS * 3))
				{
					if (Transit_IsDone())
						gGame.nPhase = e_Game_RoundCompleted;		// ...et on quitte quand elle est finie.
				}

			}


/*
if (gGame.nTimer1 == 0)
{
	if (gVar.pKeys[gMetroCfg.pKeys[e_CfgKey_ButtonA]])
		gGame.nPhase = e_Game_RoundCompleted;
}
*/


		} // if (++gGame.nRoundOverTimer > GAME_ROUNDOVER_ANM_TIMER)

	}

	// En y.
	if (*pnKb & (e_Kb_Left | e_Kb_Right))
	{
		if (gGame.nPlayerPosY > ((2 * SCROLL_TILE_H) + (SCROLL_TILE_H / 2)) << 8)
			*pnKb |= e_Kb_Up;
		if (gGame.nPlayerPosY < ((2 * SCROLL_TILE_H) + (SCROLL_TILE_H / 2)) << 8)
			*pnKb |= e_Kb_Down;
	}

}

#define	GAME_DEATH_ANM_TIMER	(160)	// Mieux avec valeur divisible par 16 ! (=> anim)
// Gestion de la mort.
void Game_DeathManage(int *pnKb)
{
	// Pas de mort quand compteur arrêté.
	if (gGame.nTimerStop) return;
	// Pas mort tant qu'il reste du temps.
	if (gGame.nTimer1) return;

	// Si nécessaire, lâche le skate.
	Game_Player_SkateboardLeave(1);
	gGame.nSpeedUp = 0;					// Speed-up stop.
	gGame.nLateJump = 0;				// RAZ late jump.

	// Stop !
	*pnKb = 0;
	gGame.nPlayerSpdX = 0;
	gGame.nPlayerPosX &= ~0xFF;

	gGame.nPlayerAnm = Anm_SetIfNew(gpHeroAnm[(++gGame.nRoundOverTimer > GAME_DEATH_ANM_TIMER ? e_PlyrAnm_DeathExhausted : e_PlyrAnm_DeathSkeleton)], gGame.nPlayerAnm);

	Msg_Init(0, "YOUR TIME IS UP", FPS, gVar.pnColors[e_Color_Red], e_MsgPrio_Death);	//_Yellow
	if (gGame.nRoundOverTimer > GAME_DEATH_ANM_TIMER)
		Msg_Init(1, "GAME OVER", FPS, gVar.pnColors[(gGame.nRoundOverTimer & 16 ? e_Color_Red : e_Color_Yellow)], e_MsgPrio_Death);
	Music_Start(e_YmMusic_GameOver, 0);

	// Musique finie ?
//org	if (Music_IsOver())
//org		gGame.nExitCode = e_GameEC_GameOver;		// Code de sortie.

	if (Music_IsOver())
	{
		if (Transit_StateGet() != e_Transit_FadeOut)
			Transit_Init(e_Transit_FadeOut);			// On initialise la transition fermante.
		else
		if (Transit_IsDone())
			gGame.nExitCode = e_GameEC_GameOver;		// Code de sortie.
	}


}

// Gestion du joueur.
void Game_PlayerManage(void)
{
	int	nKb;
	int	nAnm = e_PlyrAnm_Idle;

	// Last pos.
	gGame.nPlayerLastX = gGame.nPlayerPosX;
	gGame.nPlayerLastY = gGame.nPlayerPosY;
	gGame.nPlayerLastZ = gGame.nPlayerPosZ;

	// On récupère le clavier.
	nKb = 0;
	if (gVar.pKeys[gMetroCfg.pKeys[e_CfgKey_Up]]) nKb |= e_Kb_Up;
	if (gVar.pKeys[gMetroCfg.pKeys[e_CfgKey_Right]]) nKb |= e_Kb_Right;
	if (gVar.pKeys[gMetroCfg.pKeys[e_CfgKey_Down]]) nKb |= e_Kb_Down;
	if (gVar.pKeys[gMetroCfg.pKeys[e_CfgKey_Left]]) nKb |= e_Kb_Left;
	if (gVar.pKeys[gMetroCfg.pKeys[e_CfgKey_ButtonA]]) nKb |= e_Kb_ButtonA;
	// On élimine les cas Haut-Bas et Droite-Gauche.
	if ((nKb & (e_Kb_Up | e_Kb_Down)) == (e_Kb_Up | e_Kb_Down)) nKb &= ~(e_Kb_Up | e_Kb_Down);
	if ((nKb & (e_Kb_Left | e_Kb_Right)) == (e_Kb_Left | e_Kb_Right)) nKb &= ~(e_Kb_Left | e_Kb_Right);

	// On ralentit si le joueur est au sol et pas de kb. (Et cas normal, le reste sera traité plus tard).
	if ((gnFrame & 3) == 0 && gGame.nPlayerGnd && gGame.nPlayerSpdX && (nKb & (e_Kb_Left | e_Kb_Right)) == 0)
		nKb |= (gGame.nPlayerSpdX > 0 ? e_Kb_Left : e_Kb_Right);

	Game_FinishLineManage(&nKb);
	Game_DeathManage(&nKb);

	// LRLRLRLR ?
	gGame.nM12Eject = 0;	// RAZ éjection du M12 toutes les frames.
	if (nKb & (gGame.nLRDir ? e_Kb_Left : e_Kb_Right))
	{
		gGame.nLRVar <<= 1;
		gGame.nLRVar |= 1;
		gGame.nLRDir ^= 1;		// On attend l'autre direction.
		gGame.nLRTime = 0;		// RAZ Timer.
		// Séquence complète ?
		if (gGame.nLRVar == 0xFF)
		{
			gGame.nM12Eject = 1;	// Flag pour un monstre, au cas où il y en ait un d'accroché.
			gGame.nLRVar = 0;
		}
	}
	else
	{
		if (++gGame.nLRTime > 12)
		{
			gGame.nLRTime = 12;
			gGame.nLRVar = 0;
		}
	}
/*
{
char pBufConv[10];
snprintf(pBufConv, 10, "%02x - %2d", gGame.nLRVar, gGame.nLRTime);
Msg_Init(1, pBufConv,- 1, gVar.pnColors[e_Color_Red], e_MsgPrio_TimerStop);
}
*/

	u32	nKey = Anm_GetKey(gGame.nPlayerAnm);
	if (nKey == e_AnmKey_Hero_LandOnBack ||	// Pas de contrôle quand atterissage sur le dos.
		nKey == e_AnmKey_Hero_FallFront ||
		nKey == e_AnmKey_Hero_FallBack ||
		nKey == e_AnmKey_Hero_FootBump ||
		nKey == e_AnmKey_Hero_LandOnBackStop ||
		nKey == e_AnmKey_Hero_JumpForce2 ||	// Pas de contrôle sur les crackers.
		nKey == e_AnmKey_Hero_Pitfall ||	// Pas de contrôle pendant le pitfall.
		nKey == e_AnmKey_Hero_Flattened)	// Pas de contrôle quand applati.
		nKb = 0;

	// >>> Cas particuliers.

	if (gGame.nPhase == e_Game_ReadySetGo) nKb = 0;		// Pas de contrôle pendant le ready set go.

	// Sortie du pitfall ? => On déclenche un saut + vitesse x au max.
	if (nKey == e_AnmKey_Hero_Pitfall && Anm_CheckStepFlag(gGame.nPlayerAnm))
	{
		nKb |= e_Kb_ButtonA;
		gGame.nPlayerSpdX = PLYR_SPDX_NORM_MAX;
	}

	// Ralentissement à la fin du roulé boulé / pendant la chute arrière & fin de la chute arrière.
	if (nKey == e_AnmKey_Hero_LandOnBackStop || nKey == e_AnmKey_Hero_FallBack)
	if (gGame.nPlayerSpdX)
		gGame.nPlayerSpdX += PLYR_ACC * -SGN(gGame.nPlayerSpdX);

	// <<< Cas particuliers.

	// Déplacement.

	// *** En Z.
	// Init jump.
	if (gGame.nPlayerGnd && (nKb & e_Kb_ButtonA))
	{
		Game_Player_SkateboardLeave(1);		// Si skate, on le lâche.
		//
		gGame.nPlayerGnd = 0;
		gGame.nPlayerSpdZ = -0x400;
		gVar.pKeys[gMetroCfg.pKeys[e_CfgKey_ButtonA]] = 0;
//		Scroll_Freeze();
	}

	// Late (super) jump ?
	if (gGame.nPlayerGnd) gGame.nLateJump = 0;
	Game_Player_LateJumpCheck();

	// Gravité.
	gGame.nPlayerSpdZ += GRAVITY;
	gGame.nPlayerPosZ += gGame.nPlayerSpdZ;
	// Au sol ?
	if (gGame.nPlayerPosZ >= 0)
	{
		gGame.nPlayerPosZ = 0;
		gGame.nPlayerSpdZ = 0;
		gGame.nPlayerGnd = 1;
//		Scroll_Unfreeze();

		// Retombe de saut forcé.
		nKey = Anm_GetKey(gGame.nPlayerAnm);
		if (nKey == e_AnmKey_Hero_JumpForce2)
		{
			gGame.nPlayerAnm = Anm_Set(gpHeroAnm[e_PlyrAnm_LandOnBack], gGame.nPlayerAnm);
			gGame.nPlayerSpdX = gGame.nPlayerSpdY = 0;
			gGame.nPlayerPosX &= ~0xFF;
//ne suffit pas.			gVar.pKeys[gMetroCfg.pKeys[e_CfgKey_ButtonA]] = 0;		// Coupe un éventuel saut en sortie d'anim.
			gGame.nSpeedUp = 0;		// Coupe un éventuel speed-up.
		}
		else
		if (nKey == e_AnmKey_Hero_JumpForce1 ||
			nKey == e_AnmKey_Hero_JumpForce3 ||
			nKey == e_AnmKey_Hero_FootBump)			// < Atterrissage du foot bump.
			gGame.nPlayerAnm = Anm_Set(gpHeroAnm[e_PlyrAnm_Idle], gGame.nPlayerAnm);
	}

	// *** En X.
	// Speed max.
	s32	nSpdMax = PLYR_SPDX_NORM_MAX;		// Spd max.
	if (gGame.nSpeedUp)		// Speed-up ?
	{
		Msg_Init(0, (((GAME_SPEEDUP_TIME - gGame.nSpeedUp) & 16) == 0 ? "SPEED-UP!" : " "), 1, gVar.pnColors[e_Color_Green], e_MsgPrio_SpeedUp);
		nSpdMax = PLYR_SPDX_SPDUP_MAX -
			(gGame.nSpeedUp > 16 ? 0 : ((PLYR_SPDX_SPDUP_MAX - PLYR_SPDX_NORM_MAX) * (16 - gGame.nSpeedUp)) / 16);		// 16 = (PLYR_SPDX_SPDUP_MAX - PLYR_SPDX_NORM_MAX) / PLYR_ACC !
		gGame.nSpeedUp--;
	}

	// Left-right.
	if (nKb & e_Kb_Right)
	{
		gGame.nPlayerSpdX += PLYR_ACC;
//		if (gGame.nPlayerSpdX > nSpdMax)
//			gGame.nPlayerSpdX = nSpdMax;
	}
	else
	if (nKb & e_Kb_Left)
	{
		gGame.nPlayerSpdX -= PLYR_ACC;
//		if (gGame.nPlayerSpdX < -nSpdMax)
//			gGame.nPlayerSpdX = -nSpdMax;
	}
	// Limite de la vitesse.
	if (gGame.nPlayerSpdX > nSpdMax) gGame.nPlayerSpdX = nSpdMax;
	else
	if (gGame.nPlayerSpdX < -nSpdMax) gGame.nPlayerSpdX = -nSpdMax;


#if defined(DEBUG_KEYS)
if (gVar.pKeys[SDLK_s]) { gGame.nPlayerSpdX = 0; gGame.nPlayerPosX &= ~0xFF; }	// !!! bien clearer la virgule quand arrêt (pour scroll) !!!
#endif

	// *** En Y.
	if (nKb & e_Kb_Up)
	{
		gGame.nPlayerSpdY = -PLYR_SPDY_NORM_MAX;
	}
	else
	if (nKb & e_Kb_Down)
	{
		gGame.nPlayerSpdY = PLYR_SPDY_NORM_MAX;
	}
	else
	{
		gGame.nPlayerSpdY = 0;
	}



	s32	nSpdX = gGame.nPlayerSpdX;

	// Slow down ?
	if (gGame.nSkateboard == 0)
	{
		if (gGame.pMap->pMapBlocks[(((gGame.nPlayerPosY >> 8) / SCROLL_TILE_H) * gGame.pMap->nMapLg) + ((gGame.nPlayerPosX >> 8) / SCROLL_TILE_W)] == BLK_SLOW)
		{	// Sur du vert.
			if (gGame.nPlayerGnd)
				gGame.nSlowDown = 1;
		}
		else
		{	// Pas sur du vert. On ne remet à 0 qu'au sol pour rester ralenti quand on sort du vert en sautant.
			if (gGame.nPlayerGnd)
				gGame.nSlowDown = 0;
		}
	}
	else
	{	// En skate, pas de slow down.
		gGame.nSlowDown = 0;
	}

	if (gGame.nSlowDown || gGame.nM12SlowDown)
	{
		if (nSpdX > PLYR_SPDX_SLOWDOWN) nSpdX = PLYR_SPDX_SLOWDOWN;
		else
		if (nSpdX < -PLYR_SPDX_SLOWDOWN) nSpdX = -PLYR_SPDX_SLOWDOWN;
	}


	// Saut forcé ?
	nKey = Anm_GetKey(gGame.nPlayerAnm);
	if (nKey == e_AnmKey_Hero_JumpForce3)
	{
//		if (nSpdX < 0x200) nSpdX = 0x200;
//		nSpdX = 0x800;
//		nSpdX += 0x400;
		nSpdX += PLYR_SPDX_BOOST;
		if (nSpdX > PLYR_SPDX_SPDUP_MAX)	// Pour ne pas cumuler speed-up + speed-boost.
			nSpdX = PLYR_SPDX_SPDUP_MAX;
	}

//if (nKey == e_AnmKey_Hero_Jump)
//	if (nSpdX > 0x380) nSpdX = 0x380;


/*
{
char pBufConv[8];
snprintf(pBufConv, 8, "%04x", nSpdX);
Msg_Init(1, pBufConv,- 1, gVar.pnColors[e_Color_Red], e_MsgPrio_TimerStop);
}
{
char pBufConv[8];
snprintf(pBufConv, 8, "%04x", gGame_nM12SlowDown);
Msg_Init(1, pBufConv,- 1, gVar.pnColors[e_Color_Red], e_MsgPrio_TimerStop);
}
*/


	// Déplacement.
	gGame.nPlayerPosX += nSpdX;//gGame.nPlayerSpdX;
	gGame.nPlayerPosY += gGame.nPlayerSpdY;

//	if (gGame.nPlayerPosX > gGame.nPosXMax) gGame.nPosXMax = gGame.nPlayerPosX;
	if (gGame.nPlayerPosX > gGame.nPosXMax)
	{
//		if ((gGame.nPosXMax ^ gGame.nPlayerPosX) & (1 << (8+6))) gGame.nScore += 50;		// Score. 50 pts toutes les 2 cases.
		if ((gGame.nPosXMax ^ gGame.nPlayerPosX) & (1 << (8+7))) gGame.nScore += 50;		// Score. 50 pts toutes les 4 cases.
		gGame.nPosXMax = gGame.nPlayerPosX;
	}

	// Limite du déplacement en Y.
	if (gGame.nPlayerPosY < PLYR_GND_OFFS << 8)
	{
		gGame.nPlayerPosY = PLYR_GND_OFFS << 8;
		gGame.nPlayerSpdY = 0;
	}
	else
	if (gGame.nPlayerPosY > ((GND_BLK_HT * SCROLL_TILE_H) - PLYR_GND_OFFS) << 8)
	{
		gGame.nPlayerPosY = ((GND_BLK_HT * SCROLL_TILE_H) - PLYR_GND_OFFS) << 8;
		gGame.nPlayerSpdY = 0;
	}
	// Limite du déplacement en X.
	// 1 bloc max de retour arrière possible sur la position max atteinte.
	if (gGame.nPlayerPosX < gGame.nPosXMax - (SCROLL_TILE_W << 8))
	{
		gGame.nPlayerPosX = (gGame.nPosXMax - (SCROLL_TILE_W << 8));// & ~0xFF;
		gGame.nPlayerSpdX = 0;
	}
	// Sécurité, on ne dépasse jamais la map.
	if (gGame.nPlayerPosX >= ((gGame.pMap->nMapLg - 1) * SCROLL_TILE_W) << 8)
		gGame.nPlayerPosX = ((gGame.pMap->nMapLg - 1) * SCROLL_TILE_W) << 8;

//	Game_SprDisplay(e_Spr_Tstrct_Cross, gGame.nPlayerPosX, gGame.nPlayerPosY);

	// Anim.
	if (gGame.nPlayerSpdX || gGame.nPlayerSpdY) nAnm = e_PlyrAnm_Run;
	if (gGame.nPlayerGnd == 0) nAnm = e_PlyrAnm_Jump;

	gGame.nPlayerAnm = Anm_SetIfNew(gpHeroAnm[nAnm], gGame.nPlayerAnm);

#if defined(DEBUG_KEYS)
if (gVar.pKeys[SDLK_r]) gGame.nPhase = e_Game_RoundInit;	// *** test ***
#endif

}

// Affichage du joueur.
void Game_PlayerDisplay(void)
{
	u32	nSpr;
	u32	nKey = Anm_GetKey(gGame.nPlayerAnm);

	// Ombre.
	if (nKey != e_AnmKey_Hero_Pitfall)		// Pas d'ombre pendant le pitfall.
	{
		nSpr = -gGame.nPlayerPosZ >> (8 + 3);
		if (nSpr > 2) nSpr = 2;
		Game_SprDisplayPrio(e_Spr_Hero_Shadows + nSpr, gGame.nPlayerPosX - (gGame.nGndX & 0xFF), gGame.nPlayerPosY, 0, 0);
	}

	// Joueur.
	if (gGame.nSkateboard)
	{
		static	u32	nInc = 0;
		s32	nSpd = ABS(gGame.nPlayerSpdX) + ABS(gGame.nPlayerSpdY);
		if (nSpd > PLYR_SPDX_NORM_MAX) nSpd = PLYR_SPDX_NORM_MAX;
		nInc += nSpd;

		nSpd = (nInc >> (8 + 5)) & 1;
		Game_SprDisplay(e_Spr_Hero_Skateboarding + nSpd, gGame.nPlayerPosX - (gGame.nGndX & 0xFF) - (1 << 8), gGame.nPlayerPosY + (2 << 8), gGame.nPlayerPosZ);
		Game_SprDisplay(e_Spr_Skateboard + nSpd, gGame.nPlayerPosX - (gGame.nGndX & 0xFF), gGame.nPlayerPosY, gGame.nPlayerPosZ);

//		Game_SprDisplay(e_Spr_Hero_Skateboarding, gGame.nPlayerPosX - (gGame.nGndX & 0xFF) - (1 << 8), gGame.nPlayerPosY + (2 << 8), gGame.nPlayerPosZ);
//		Game_SprDisplay(e_Spr_Skateboard, gGame.nPlayerPosX - (gGame.nGndX & 0xFF), gGame.nPlayerPosY, gGame.nPlayerPosZ);
	}
	else
	{
/*
		if (nKey == e_AnmKey_Hero_Run)
		{
			s32	t;
			t = ABS(gGame.nPlayerSpdX) + ABS(gGame.nPlayerSpdY);
			t >>= 8;
//			if (t > 2) t = 2;
//			while (t) { Anm_GetImage(gGame.nPlayerAnm); t--; }
			if (t > 2) { Anm_GetImage(gGame.nPlayerAnm); }
		}
		Game_SprDisplay(Anm_GetImage(gGame.nPlayerAnm), gGame.nPlayerPosX - (gGame.nGndX & 0xFF), gGame.nPlayerPosY, gGame.nPlayerPosZ);
*/

		s32	nPrio = -1;
		if (nKey == e_AnmKey_Hero_Run)
		{
			if (gGame.nSlowDown == 0 && gGame.nM12SlowDown == 0)	// Pendant le slow-down, on garde l'anim à vitesse normale.
			{
				s32	t;
				t = ABS(gGame.nPlayerSpdX) + ABS(gGame.nPlayerSpdY);
				t >>= 8;
				if (t > 2) { Anm_GetImage(gGame.nPlayerAnm); }
			}
		}
		else
		if (nKey == e_AnmKey_Hero_Flattened)
			//nPrio = (gGame.nPlayerPosY >> 8);
			nPrio = 8;		// 8 : Sur l'ombre, mais au dessous de tout (les prios commencent à 16).

		Game_SprDisplayPrio(Anm_GetImage(gGame.nPlayerAnm), gGame.nPlayerPosX - (gGame.nGndX & 0xFF), gGame.nPlayerPosY, gGame.nPlayerPosZ, nPrio);
//Game_SprDisplayPrio(Anm_GetImage(gGame.nPlayerAnm), gGame.nPlayerPosX, gGame.nPlayerPosY, gGame.nPlayerPosZ, nPrio);

	}

	// Etincelle quand timer < 10 sec.
	if (gGame.nTimer1 && (gGame.nTimer1 + gGame.nTimer2) < (TIME_1S * 10) && gGame.nTimerStop == 0)
	{
		nSpr = Anm_GetImage(gGame.nTimeSparkleAnm);
		if (Anm_CheckEnd(gGame.nTimeSparkleAnm))
		{
			gGame.nTimeSparkleAnm = Anm_Set(gAnm_TimeSparkle, gGame.nTimeSparkleAnm);
			gGame.nTimeSparklePosX = (SCR_Width / 2) - 128 + (rand() & 127) + (gGame.nTimeSparkleSide ? 128 : 0);
			gGame.nTimeSparklePosY = (rand() & 127) + 32;
			gGame.nTimeSparkleSide ^= 1;	// Changera de côté au prochain tour.
		}
		else
			SprDisplayAbsolute(nSpr, gGame.nTimeSparklePosX, gGame.nTimeSparklePosY, 0x100);
	}

}

//=============================================================================

// Jeu.
void Game_Manage(void)
{
	switch (gGame.nPhase)
	{
	case e_Game_ReadyYourself:	// Ready yourself.
		Music_Start(e_YmMusic_ReadyYourself, 0);
		if (Music_IsOver()) gGame.nPhase = e_Game_RoundInit;

		{
			SDL_Rect	sRct;

			// HUD.
			HUD_Part1Disp();
			// Rectangle central.
			sRct.x = 0;
			sRct.y = HUD_TOP_HT;
			sRct.w = SCR_Width;
			sRct.h = SCR_Height - (HUD_TOP_HT + HUD_BOTTOM_HT);
			SDL_FillRect(gVar.pScreen, &sRct, gVar.pnColors[e_Color_Black]);

//RotoZoom_Draw();	// fonctionne, mais bon...

			// Logo.
			sRct.x = (SCR_Width / 2) - (gVar.pImgLogo->w / 2);
			sRct.y = 40;
			SDL_BlitSurface(gVar.pImgLogo, NULL, gVar.pScreen, &sRct);

			// Texte.
			Font8_Print((SCR_Width / 2) - ((15 * 8) / 2), 120, "READY YOURSELF!", gVar.pnColors[e_Color_Yellow]);

			char	pRound[] = "ROUND 00";
			MyItoA(gGame.nRound + 1, pRound);
			Font8_Print((SCR_Width / 2) - ((8 * 8) / 2), 144, pRound, gVar.pnColors[e_Color_Gray]);

		}

		break;

	case e_Game_RoundInit:		// Init round.
		Game_RoundInit();
		gGame.nPhase = e_Game_Play;
		Music_Start(e_YmMusic_MainTheme, 0);
//		break;		// ... on enchaîne.

//Transit_Init(e_Transit_FadeIn);
		Transit_Off();

		gGame.nTimerFreeze = FPS * 2;			// Temps du ready set go.
		gGame.nPhase = e_Game_ReadySetGo;
		// Pas de break.


#if defined(DEBUG_NO_READYSETGO)
		gGame.nPhase = e_Game_Play;	// Tst, pas de timer.
		gGame.nTimerFreeze = 0;
#endif


	case e_Game_ReadySetGo:		// Ready Set Go.
		if (gGame.nTimerFreeze == FPS * 2)
		{
			Msg_Init(0, "READY", FPS, gVar.pnColors[e_Color_Cyan], e_MsgPrio_ReadySetGo);
			Sfx_PlaySfx(e_Sfx_Countdown, e_SfxPrio_0);
			//
			if ((gGame.nRound & 3) == 3) gGame.nHUDTimersDisp = 3;		// Ready : Affichage des timers des 3 rounds précédents.
		}
		else if (gGame.nTimerFreeze == FPS)
		{
			Msg_Init(0, "SET", FPS, gVar.pnColors[e_Color_Cyan], e_MsgPrio_ReadySetGo);
			Sfx_PlaySfx(e_Sfx_Countdown, e_SfxPrio_0);
			//
			if ((gGame.nRound & 3) == 3) gGame.nHUDTimersDisp = 4;		// Set : Affiche le totale des 3 timers précédents.
		}
		else if (gGame.nTimerFreeze == 0)
		{
			Msg_Init(0, "GO!", FPS, gVar.pnColors[e_Color_Cyan], e_MsgPrio_ReadySetGo);
			Sfx_PlaySfx(e_Sfx_Go, e_SfxPrio_0);
			gGame.nPhase = e_Game_Play;
			//
			if ((gGame.nRound & 3) == 3) gGame.nHUDTimersDisp = 0;		// Go : Affichage normal : Time xxx + xxx.
		}
		// Pas de break.

	case e_Game_Play:			// Jeu normal.

		Time_Tick();
		Game_PlayerManage();

		Scroll_Manage();
		Mst_Manage();

		Game_PlayerDisplay();

		Scroll_Display();
		SprDisplayAll();
//		SprDisplayAll_Pass1();
		Scroll_BottomDisplay();
		HUD_Display();
//		SprDisplayAll_Pass2();

//Font8_Print(20+1, 160+1, "TEST 0123 789!?", 0);
//Font8_Print(20, 160, "TEST 0123 789!?", 0xFFFF);
//void mst_tst_dispnb(u32 nPosY);
//mst_tst_dispnb(16+8);

/*
		// Death ?
		if (gGame.nTimerStop == 0 && gGame.nTimer1 == 0)
		if (Music_IsOver())
			gGame.nExitCode = e_GameEC_GameOver;		// Code de sortie.
*/

		break;

	case e_Game_RoundCompleted:		// Round completed.
		// Fini ?
		if (++gGame.nRound >= LVL_NB)
		{
			// Tous les niveaux passés. Sortie.
			gGame.nExitCode = e_GameEC_AllClear;		// Code de sortie.
			break;
		}
		// On passe au niveau suivant.
#if defined(DEBUG_NO_READYYOURSELF)
		gGame.nPhase = e_Game_RoundInit;
#else
		gGame.nPhase = e_Game_ReadyYourself;//e_Game_RoundInit;
		Transit_Init(e_Transit_FadeIn);
#endif
		break;

	}


}

