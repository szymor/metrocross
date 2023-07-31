
#include "includes.h"
//#include "monsters_inc.h"

//=============================================================================
void Mst0_Init_LevelData(struct SMstCommon *pMst, u8 *pData);
s32 Mst0_Main_LevelData(struct SMstCommon *pMst);
void Mst1_Init_EndOfLevel(struct SMstCommon *pMst, u8 *pData);
s32 Mst1_Main_EndOfLevel(struct SMstCommon *pMst);
void Mst2_Init_Springboard(struct SMstCommon *pMst, u8 *pData);
s32 Mst2_Main_Springboard(struct SMstCommon *pMst);
void Mst3_Init_Cracker(struct SMstCommon *pMst, u8 *pData);
s32 Mst3_Main_Cracker(struct SMstCommon *pMst);
void Mst4_Init_Pitfall(struct SMstCommon *pMst, u8 *pData);
s32 Mst4_Main_Pitfall(struct SMstCommon *pMst);
void Mst5_Init_Hurdle(struct SMstCommon *pMst, u8 *pData);
s32 Mst5_Main_Hurdle(struct SMstCommon *pMst);
void Mst6_Init_Skateboard(struct SMstCommon *pMst, u8 *pData);
s32 Mst6_Main_Skateboard(struct SMstCommon *pMst);
void Mst7_Init_SmallCan(struct SMstCommon *pMst, u8 *pData);
s32 Mst7_Main_SmallCan(struct SMstCommon *pMst);
void Mst8_Init_CokeCan(struct SMstCommon *pMst, u8 *pData);
s32 Mst8_Main_CokeCan(struct SMstCommon *pMst);
void Mst9_Init_BigTire(struct SMstCommon *pMst, u8 *pData);
s32 Mst9_Main_BigTire(struct SMstCommon *pMst);
void Mst10_Init_RedCube(struct SMstCommon *pMst, u8 *pData);
s32 Mst10_Main_RedCube(struct SMstCommon *pMst);
void Mst11_Init_TelescopicWall(struct SMstCommon *pMst, u8 *pData);
s32 Mst11_Main_TelescopicWall(struct SMstCommon *pMst);
void Mst12_Init_GreenMouse(struct SMstCommon *pMst, u8 *pData);
s32 Mst12_Main_GreenMouse(struct SMstCommon *pMst);
void Mst13_Init_ChessPiece(struct SMstCommon *pMst, u8 *pData);
s32 Mst13_Main_ChessPiece(struct SMstCommon *pMst);
void Mst14_Init_Sign(struct SMstCommon *pMst, u8 *pData);
s32 Mst14_Main_Sign(struct SMstCommon *pMst);

struct SMstTb gpMstTb[e_Mst_MAX] =
{
	{ Mst0_Init_LevelData, Mst0_Main_LevelData, NULL, 0 },	// Level data.
	{ Mst1_Init_EndOfLevel, Mst1_Main_EndOfLevel, NULL, 0 },	// End of level.
	{ Mst2_Init_Springboard, Mst2_Main_Springboard, gAnm_Springboard_Idle, 0 },	// Springboard.
	{ Mst3_Init_Cracker, Mst3_Main_Cracker, gAnm_Cracker_Idle, 0 },	// Cracker (auto).
	{ Mst4_Init_Pitfall, Mst4_Main_Pitfall, NULL, 0 },	// Pitfall (auto).
	{ Mst5_Init_Hurdle, Mst5_Main_Hurdle, gAnm_Hurdle_Idle, 0 },	// Hurdle.
	{ Mst6_Init_Skateboard, Mst6_Main_Skateboard, gAnm_Skateboard_Idle, 0 },	// Skateboard.
	{ Mst7_Init_SmallCan, Mst7_Main_SmallCan, NULL, 0 },	// Small cans.
	{ Mst8_Init_CokeCan, Mst8_Main_CokeCan, NULL, 0 },		// Coke cans.
	{ Mst9_Init_BigTire, Mst9_Main_BigTire, NULL, 0 },		// Gros pneu.
	{ Mst10_Init_RedCube, Mst10_Main_RedCube, gAnm_RedCube_MoveDown, 0 },		// Cube rouge.
	{ Mst11_Init_TelescopicWall, Mst11_Main_TelescopicWall, gAnm_TelescopicWall, 0 },		// Mur téléscopique.
	{ Mst12_Init_GreenMouse, Mst12_Main_GreenMouse, NULL, 0 },		// Souris verte.
	{ Mst13_Init_ChessPiece, Mst13_Main_ChessPiece, gAnm_ChessPiece_Normal, 0 },		// Pieces d'échecs.
	{ Mst14_Init_Sign, Mst14_Main_Sign, NULL, 0 },		// Panneau.

};

//=============================================================================
// Fonctions communes.

// Lecture d'un champ de bits (phase d'init).
s32 GetBits(u32 nBitStart, u32 nBitEnd, u8 *pData, u32 nSign)
{
	s32	nRet = 0;
	u8	nSrcMsk;
	u32	nDstMsk;
	u32	nCurByte;
	u32	i;

	if (nBitEnd < nBitStart) return (0);
	if (nBitEnd - nBitStart + 1 > 32) return (0);

	nDstMsk = 1;
	nSrcMsk = 1 << (nBitStart & 7);
	i = nBitEnd - nBitStart + 1;
	nCurByte = nBitStart >> 3;

	while (i)
	{
		if (pData[nCurByte] & nSrcMsk) nRet |= nDstMsk;
		nDstMsk <<= 1;
		nSrcMsk <<= 1;
		if (nSrcMsk == 0)
		{
			nSrcMsk = 1;
			nCurByte++;
		}
		i--;
	}

	// Faut-il étendre le signe ?
	if (nSign)
	{
		nDstMsk = ((u32)-1) << (nBitEnd - nBitStart);
		// Comme les bits supérieurs sont à 0, ça ne va matcher que le bit de signe (sur la taille demandée).
		if (nRet & nDstMsk) nRet |= nDstMsk;
	}

	return (nRet);
}

// Screen out ?
u32 Mst_ScreenOut(s32 nPosX, s32 nLg)
{
	return (nPosX + (nLg << 8) < gGame.nGndX ? 1 : 0);
}

//=============================================================================
// Mst 0 : Level misc data (background, valeur du timer...).

/*
[mst0] Level Data
Bkg = 0:7: bkg 1 - bkg 2
Time = 8:15:
*/

void Mst0_Init_LevelData(struct SMstCommon *pMst, u8 *pData)
{
	u32	nVal;

	nVal = GetBits(0, 7, pData, 0);		// No de background.
	Scroll_BkgNo_Set(nVal);

	nVal = GetBits(8, 15, pData, 0);	// Temps.
	gGame.nTimer1 = nVal * TIME_1S;

// tst
//gGame.nTimer1 = (gGame.nTimer1 * 80) / 100;

}

s32 Mst0_Main_LevelData(struct SMstCommon *pMst)
{
	return (e_MstState_Dead);	// On le tue immédiatement au premier passage, ne sert que pour l'init.
}

//=============================================================================
// Mst 1 : Fin de niveau.

/*
[mst1] End of level
prm = 0:7:			; tmp
*/

// Structure spécifique.
struct SMst0_EndOfLevel
{
	u8	nLightNo;		// Lumière qui clignote.

};

void Mst1_Init_EndOfLevel(struct SMstCommon *pMst, u8 *pData)
{
	struct SMst0_EndOfLevel	*pSpe = (struct SMst0_EndOfLevel *)pMst->pData;
	pSpe->nLightNo = 0;
	Scroll_SetMaxPos(pMst->nPosX - ((SCROLL_TILE_W * 2) << 8));
	gGame.nFinishLine = pMst->nPosX;
}

#define	MST1_BITS	(5)
s32 Mst1_Main_EndOfLevel(struct SMstCommon *pMst)
{
	struct SMst0_EndOfLevel	*pSpe = (struct SMst0_EndOfLevel *)pMst->pData;
	static u32 pnBits[MST1_BITS] =
		{ e_Spr_EndPortal, e_Spr_EndPortal+2, e_Spr_EndPortal+1, e_Spr_EndPortal+2, e_Spr_EndPortal+3 };
	u32	i;

	// Piliers.
	if (gGame.nPlayerPosX + 0x1000 > pMst->nPosX)
	if (gGame.nPlayerPosY < (SCROLL_TILE_H) << 8 || gGame.nPlayerPosY > (4 * SCROLL_TILE_H) << 8)
		gGame.nPlayerPosX = pMst->nPosX - 0x1000;

	// Affichage du portail.
	for (i = 0; i < MST1_BITS; i++)
		Game_SprDisplay(pnBits[i], pMst->nPosX, (i * 16) << 8, 0);
	// Lumières clignotantes.
	Game_SprDisplay(e_Spr_EndPortal_Lights + pSpe->nLightNo, pMst->nPosX, (i * 16) << 8, 0);
	if ((gnFrame & 7) == 0)
	{
		i = pSpe->nLightNo;
		pSpe->nLightNo = rand() & 3;
		if (i == pSpe->nLightNo) pSpe->nLightNo = (pSpe->nLightNo + 1) & 3;
	}

	return (e_MstState_Managed);
}

//=============================================================================
// Mst 2 : Springboard.

/*
[mst2] Springboard
prm = 0:7:			; tmp
*/

/*
// Structure spécifique.
struct SMst2_Springboard
{
	u8	nLateJmpCnt;	// Petit temps de latence pour permettre au joueur de lancer le super saut après avoir sauté.
};
	struct SMst2_Springboard	*pSpe = (struct SMst2_Springboard *)pMst->pData;
*/

void Mst2_Init_Springboard(struct SMstCommon *pMst, u8 *pData)
{
//printf("m2: x=%d / y=%d\n", pMst->nPosX >> 8, pMst->nPosY >> 8);
}

s32 Mst2_Main_Springboard(struct SMstCommon *pMst)
{
	// Screen out ?
	if (Mst_ScreenOut(pMst->nPosX, 32)) return (e_MstState_Dead);

	// Contact avec le perso ?
	if (Anm_GetKey(gGame.nPlayerAnm) != e_AnmKey_Hero_Death)
	{
		if (gGame.nPlayerGnd)
		{
			struct SSprRect sRct;
			if (SprGetRect(e_Spr_Springboard, e_SprRectZone_RectCol, &sRct))
			if (sRct.nType == e_SprRect_Rect)
/*
			{
				Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX1 << 8), pMst->nPosY + (sRct.nY1 << 8), 0);
				Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX2 << 8), pMst->nPosY + (sRct.nY1 << 8), 0);
				Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX1 << 8), pMst->nPosY + (sRct.nY2 << 8), 0);
				Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX2 << 8), pMst->nPosY + (sRct.nY2 << 8), 0);
			}
*/
			if (gGame.nPlayerPosX >= pMst->nPosX + (sRct.nX1 << 8) && gGame.nPlayerPosX <= pMst->nPosX + (sRct.nX2 << 8) &&
				gGame.nPlayerPosY >= pMst->nPosY + (sRct.nY1 << 8) && gGame.nPlayerPosY <= pMst->nPosY + (sRct.nY2 << 8))
			{
				Game_Player_JumpInit(0);
				pMst->nAnm = Anm_Set(gAnm_Springboard_Boing, pMst->nAnm);
			}
		}
	}

	// Affichage.
	Game_SprDisplay(Anm_GetImage(pMst->nAnm), pMst->nPosX, pMst->nPosY, 0);

	return (e_MstState_Managed);
}

//=============================================================================
// Mst 3 : Cracker.

/*
[mst3] Cracker (auto)
prm = 0:7:			; tmp
*/

/*
// Structure spécifique.
struct SMst3_Cracker
{
	u8	nLateJmpCnt;	// Petit temps de latence pour permettre au joueur de lancer le super saut après avoir sauté.
};
	struct SMst3_Cracker	*pSpe = (struct SMst3_Cracker *)pMst->pData;
*/

void Mst3_Init_Cracker(struct SMstCommon *pMst, u8 *pData)
{
	pMst->nPhase = 0;	// Sert de flag pour noter le déclenchement.
}

s32 Mst3_Main_Cracker(struct SMstCommon *pMst)
{
	// Screen out ?
	if (Mst_ScreenOut(pMst->nPosX, 32)) return (e_MstState_Dead);

	// Contact avec le perso ?
	if (pMst->nPhase == 0)
	if (Anm_GetKey(gGame.nPlayerAnm) != e_AnmKey_Hero_Death)
	{
		if (gGame.nPlayerGnd)
		{
			struct SSprRect sRct;
			if (SprGetRect(e_Spr_CrackerExplo, e_SprRectZone_RectCol, &sRct))
			if (sRct.nType == e_SprRect_Rect)
/*
			{
				Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX1 << 8), pMst->nPosY + (sRct.nY1 << 8), 0);
				Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX2 << 8), pMst->nPosY + (sRct.nY1 << 8), 0);
				Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX1 << 8), pMst->nPosY + (sRct.nY2 << 8), 0);
				Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX2 << 8), pMst->nPosY + (sRct.nY2 << 8), 0);
			}
*/
			if (gGame.nPlayerPosX >= pMst->nPosX + (sRct.nX1 << 8) && gGame.nPlayerPosX <= pMst->nPosX + (sRct.nX2 << 8) &&
				gGame.nPlayerPosY >= pMst->nPosY + (sRct.nY1 << 8) && gGame.nPlayerPosY <= pMst->nPosY + (sRct.nY2 << 8))
			{
				Game_Player_JumpInit(1);
				pMst->nAnm = Anm_Set(gAnm_Cracker_Explode, pMst->nAnm);
				pMst->nPhase = 1;		// Flag, déjà déclenché.
			}
		}
	}

	// On attend que le perso quitte la case pour réarmer.
	if (pMst->nPhase == 1)
	{
		if ((gGame.nPlayerPosY >> 8) / SCROLL_TILE_H != (pMst->nPosY >> 8) / SCROLL_TILE_H ||
			(gGame.nPlayerPosX >> 8) / SCROLL_TILE_W != (pMst->nPosX >> 8) / SCROLL_TILE_W)
			pMst->nPhase = 0;
	}

	Game_SprDisplay(Anm_GetImage(pMst->nAnm), pMst->nPosX, pMst->nPosY, 0);

	return (e_MstState_Managed);
}

//=============================================================================
// Mst 4 : Pitfall.

/*
[mst4] Pitfall (auto)
prm = 0:7:			; tmp
*/

void Mst4_Init_Pitfall(struct SMstCommon *pMst, u8 *pData)
{
	pMst->nPhase = 0;	// Sert de flag pour noter le déclenchement.
}

s32 Mst4_Main_Pitfall(struct SMstCommon *pMst)
{
	if (Mst_ScreenOut(pMst->nPosX, 32)) return (e_MstState_Dead);

	// Contact avec le perso ?
	if (pMst->nPhase == 0)
	if (gGame.nPlayerGnd)
	if (Anm_GetKey(gGame.nPlayerAnm) != e_AnmKey_Hero_Death)
	{
		struct SSprRect sRct;
		if (SprGetRect(e_Spr_CrackerExplo, e_SprRectZone_RectCol, &sRct))
		if (sRct.nType == e_SprRect_Rect)
/*
		{
			Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX1 << 8), pMst->nPosY + (sRct.nY1 << 8), 0);
			Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX2 << 8), pMst->nPosY + (sRct.nY1 << 8), 0);
			Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX1 << 8), pMst->nPosY + (sRct.nY2 << 8), 0);
			Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX2 << 8), pMst->nPosY + (sRct.nY2 << 8), 0);
		}
*/
		if (gGame.nPlayerPosX >= pMst->nPosX + (sRct.nX1 << 8) && gGame.nPlayerPosX <= pMst->nPosX + (sRct.nX2 << 8) &&
			gGame.nPlayerPosY >= pMst->nPosY + (sRct.nY1 << 8) && gGame.nPlayerPosY <= pMst->nPosY + (sRct.nY2 << 8))
		{
			Game_Player_PitfallSet();
			pMst->nPhase = 1;		// Flag, déjà déclenché.
		}
	}

	// On attend que le perso quitte la case pour réarmer.
	if (pMst->nPhase == 1)
	{
		if ((gGame.nPlayerPosY >> 8) / SCROLL_TILE_H != (pMst->nPosY >> 8) / SCROLL_TILE_H ||
			(gGame.nPlayerPosX >> 8) / SCROLL_TILE_W != (pMst->nPosX >> 8) / SCROLL_TILE_W)
			pMst->nPhase = 0;
	}

//	Game_SprDisplay(e_Spr_CrackerExplo+1, pMst->nPosX, pMst->nPosY, 0);

	return (e_MstState_Managed);
}

//=============================================================================
// Mst 5 : Hurdle.

/*
[mst5] Hurdle
posx = 0:4:			; pos (de 2 pix en 2 pix)
prm = 5:7:			; tmp
*/

enum
{
	e_Mst5_Phase_Waiting = 0,
	e_Mst5_Phase_Down,
};

void Mst5_Init_Hurdle(struct SMstCommon *pMst, u8 *pData)
{
//printf("m2: x=%d / y=%d\n", pMst->nPosX >> 8, pMst->nPosY >> 8);

	pMst->nPhase = e_Mst5_Phase_Waiting;

}

s32 Mst5_Main_Hurdle(struct SMstCommon *pMst)
{
	if (Mst_ScreenOut(pMst->nPosX, 32)) return (e_MstState_Dead);

	switch (pMst->nPhase)
	{
	case e_Mst5_Phase_Waiting:		// Debout, attente de contact.
		// Contact avec le perso ?
		if (gGame.nPlayerPosZ > -16 << 8)
		if (Anm_GetKey(gGame.nPlayerAnm) != e_AnmKey_Hero_Death)
		{
			struct SSprRect sRct;
			if (SprGetRect(e_Spr_Hurdle, e_SprRectZone_RectCol, &sRct))
			if (sRct.nType == e_SprRect_Rect)
/*
			{
				Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX1 << 8), pMst->nPosY + (sRct.nY1 << 8), 0);
				Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX2 << 8), pMst->nPosY + (sRct.nY1 << 8), 0);
				Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX1 << 8), pMst->nPosY + (sRct.nY2 << 8), 0);
				Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX2 << 8), pMst->nPosY + (sRct.nY2 << 8), 0);
			}
*/
			if (gGame.nPlayerPosX >= pMst->nPosX + (sRct.nX1 << 8) && gGame.nPlayerPosX <= pMst->nPosX + (sRct.nX2 << 8) &&
				gGame.nPlayerPosY >= pMst->nPosY + (sRct.nY1 << 8) && gGame.nPlayerPosY <= pMst->nPosY + (sRct.nY2 << 8))
			{
				if (gGame.nPlayerPosZ > -8 << 8)
					Game_Player_FallFrontInit();	// Chute.
				else
					Game_Player_FootBumpInit();		// Prise de pieds.

				pMst->nAnm = Anm_Set(gAnm_Hurdle_Falling, pMst->nAnm);
				pMst->nPhase = e_Mst5_Phase_Down;
			}

		}
		break;

	case e_Mst5_Phase_Down:			// Tombée, plus de tests.
		break;
	}

	Game_SprDisplay(Anm_GetImage(pMst->nAnm), pMst->nPosX, pMst->nPosY, 0);

	return (e_MstState_Managed);
}

//=============================================================================
// Mst 6 : Skateboard.

/*
[mst6] Skateboard
phase = 0:3: Waiting - Go away - Go away no 1st spr
prm = 4:7:			; tmp
*/

enum
{
	e_Mst6_Phase_Waiting = 0,
	e_Mst6_Phase_GoingAway,
	e_Mst6_Phase_GoingAway_No1stSpr,	// Phase inutilisée, seulement pour ne pas afficher le sprite lors de l'init.
};

void Mst6_Init_Skateboard(struct SMstCommon *pMst, u8 *pData)
{
	u32	nVal;

	nVal = GetBits(0, 3, pData, 0);		// Phase.
	pMst->nPhase = nVal;

	// Si en attente, recalage au milieu du bloc.
	if (pMst->nPhase == e_Mst6_Phase_Waiting)
	{
		pMst->nPosX += (SCROLL_TILE_W / 2) << 8;
		pMst->nPosY += (SCROLL_TILE_H - 6) << 8;
	}
	else
	if (pMst->nPhase > e_Mst6_Phase_GoingAway)
		pMst->nPhase = e_Mst6_Phase_GoingAway;	// Le monstre a été généré depuis les routines du joueur, il ne faut pas afficher le sprite en plus depuis l'init (décalage à cause du scroll).
	else
		// Au cas ou le monstre soit ajouté dans la liste *avant* le monstre qui a déclenché la perte du skate. Comme ça la planche ne disparait pas pendant 1 frame !
		Game_SprDisplay(Anm_GetImage(pMst->nAnm), pMst->nPosX - 0x400, pMst->nPosY, 0);

}

s32 Mst6_Main_Skateboard(struct SMstCommon *pMst)
{
	if (Mst_ScreenOut(pMst->nPosX, 32)) return (e_MstState_Dead);

	switch (pMst->nPhase)
	{
	case e_Mst6_Phase_Waiting:		// Attente d'un contact avec le joueur.
		// Contact avec le perso ?
		if (gGame.nPlayerGnd || (gGame.nPlayerSpdZ > 0 && gGame.nPlayerPosZ > -8 << 8))
		if (Anm_GetKey(gGame.nPlayerAnm) != e_AnmKey_Hero_Death)
		{
			struct SSprRect sRct;
			if (SprGetRect(e_Spr_Skateboard, e_SprRectZone_RectCol, &sRct))
			if (sRct.nType == e_SprRect_Rect)
/*
			{
				Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX1 << 8), pMst->nPosY + (sRct.nY1 << 8), 0);
				Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX2 << 8), pMst->nPosY + (sRct.nY1 << 8), 0);
				Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX1 << 8), pMst->nPosY + (sRct.nY2 << 8), 0);
				Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX2 << 8), pMst->nPosY + (sRct.nY2 << 8), 0);
			}
*/
			if (gGame.nPlayerPosX >= pMst->nPosX + (sRct.nX1 << 8) && gGame.nPlayerPosX <= pMst->nPosX + (sRct.nX2 << 8) &&
				gGame.nPlayerPosY >= pMst->nPosY + (sRct.nY1 << 8) && gGame.nPlayerPosY <= pMst->nPosY + (sRct.nY2 << 8))
			{
				if (Game_Player_SkateboardSet()) return (e_MstState_Dead);	// Le joueur est monté dessus.
				pMst->nPhase = e_Mst6_Phase_GoingAway;						// Sinon le skate se barre.
			}
		}
		break;

	case e_Mst6_Phase_GoingAway:	// La planche se barre.
		pMst->nPosX -= 0x400;
		break;
	}

	Game_SprDisplay(Anm_GetImage(pMst->nAnm), pMst->nPosX, pMst->nPosY, 0);

	return (e_MstState_Managed);
}

//=============================================================================
// Mst 7 : Small cans.

/*
[mst7] Small cans
posx = 0:4:			; pos (de 2 pix en 2 pix)
type = 5:6: Blue - Green -
prm = 7:7:			; tmp
*/

enum
{
	e_Mst7_Phase_Waiting = 0,
	e_Mst7_Phase_Crushed,
	e_Mst7_Phase_Kicked,
};

// Structure spécifique.
struct SMst7_SmallCan
{
	s32	nPosZ, nSpdZ;

	u8	nType;		// 0 = Blue / 1 = Green.
	u8	nKickNo;

	u8	nBounce;
};

#define	MST7_KICKNB_MAX	(6)

void Mst7_Init_SmallCan(struct SMstCommon *pMst, u8 *pData)
{
	struct SMst7_SmallCan	*pSpe = (struct SMst7_SmallCan *)pMst->pData;
	u32	nVal;

	nVal = GetBits(0, 4, pData, 0);		// Pos x.
	pMst->nPosX += (nVal * 2) << 8;
	pMst->nPosY += (SCROLL_TILE_H - 6) << 8;
	pSpe->nPosZ = 0;
	pSpe->nKickNo = 0;

	nVal = GetBits(5, 6, pData, 0);		// Type.
	pSpe->nType = nVal;

	pMst->nPhase = e_Mst7_Phase_Waiting;
}

s32 Mst7_Main_SmallCan(struct SMstCommon *pMst)
{
	struct SMst7_SmallCan	*pSpe = (struct SMst7_SmallCan *)pMst->pData;
	static u32	pnBaseSpr[] = { e_Spr_BlueCan, e_Spr_GreenCan };

	if (Mst_ScreenOut(pMst->nPosX, 32)) return (e_MstState_Dead);

	u32	nSpr = pnBaseSpr[pSpe->nType] + 2;

	switch (pMst->nPhase)
	{
	case e_Mst7_Phase_Waiting:		// Attente d'un contact avec le joueur.
		nSpr = pnBaseSpr[pSpe->nType] + (pSpe->nKickNo ? 0 : 2);

		// Contact avec le perso ?
		if (gGame.nPlayerPosZ > -8 << 8)
		if (Anm_GetKey(gGame.nPlayerAnm) != e_AnmKey_Hero_Death)
		{
			struct SSprRect sRct;
			if (SprGetRect(e_Spr_BlueCan+2, e_SprRectZone_RectCol, &sRct))
			if (sRct.nType == e_SprRect_Rect)
/*
			{
				Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX1 << 8), pMst->nPosY + (sRct.nY1 << 8), 0);
				Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX2 << 8), pMst->nPosY + (sRct.nY1 << 8), 0);
				Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX1 << 8), pMst->nPosY + (sRct.nY2 << 8), 0);
				Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX2 << 8), pMst->nPosY + (sRct.nY2 << 8), 0);
			}
*/
			if (gGame.nPlayerPosX >= pMst->nPosX + (sRct.nX1 << 8) && gGame.nPlayerPosX <= pMst->nPosX + (sRct.nX2 << 8) &&
				gGame.nPlayerPosY >= pMst->nPosY + (sRct.nY1 << 8) && gGame.nPlayerPosY <= pMst->nPosY + (sRct.nY2 << 8))
			{
				if (gGame.nPlayerGnd == 0 && gGame.nPlayerSpdZ > 0)
				{
					// Ecrasement.
					Time_Freeze();
					pMst->nPhase = e_Mst7_Phase_Crushed;
					Sfx_PlaySfx(e_Sfx_Can_Crush, e_SfxPrio_0);
				}
				else
				{
					// Kick.
					pMst->nPhase = e_Mst7_Phase_Kicked;
					pSpe->nBounce = 0;

					pMst->nSpd = (gGame.nSpeedUp ? 10 : 8) << 8;		// Init ici, avant de faire un éventuel speed-up.

					// Normal (blue) ou speed-up (green).
					if (pSpe->nType)
					{	// Green : Speed-up.
						Game_Player_SpeedUp();
						pSpe->nKickNo = MST7_KICKNB_MAX;		// Pour angle max.
					}
					else
					{	// Blue : Bonus points.
						static	u32	pnScores[MST7_KICKNB_MAX] = { 100, 200, 500, 1000, 2000, 5000 };
						static	char *ppStr[MST7_KICKNB_MAX] = { "+100 POINTS", "+200 POINTS", "+500 POINTS", "+1000 POINTS", "+2000 POINTS", "+5000 POINTS" };
						Game_Player_PointsBonus(pnScores[pSpe->nKickNo], ppStr[pSpe->nKickNo]);
						pSpe->nKickNo++;
					}

					// Angle du kick.
					// pMst->nSpd = 8 << 8; et pSpe->nSpdZ = -0x400;	=> pile poile 6 cases, mais pas de rebond.
					s32	nDX = pMst->nPosX - gGame.nPlayerPosX;
					pMst->nAngle = (nDX < 0 ? 128 : 0);
					if (gGame.nPlayerPosY < pMst->nPosY - (4 << 8)) pMst->nAngle = (nDX < 0 ? 128+4 : -4);//-4;
					else
					if (gGame.nPlayerPosY > pMst->nPosY + (2 << 8)) pMst->nAngle = (nDX < 0 ? 128-4 : 4);//4;

					if (pSpe->nKickNo >= MST7_KICKNB_MAX)		// Kick final, la cannette dégage par le bas.
						pMst->nAngle = 224;

					pSpe->nSpdZ = -0x350;

					Sfx_PlaySfx(e_Sfx_Can_Kick, e_SfxPrio_0);
				}
			}

		}
		break;

	case e_Mst7_Phase_Crushed:		// Canette écrasée, plus d'action.
		nSpr = pnBaseSpr[pSpe->nType] + 3;
		break;

	case e_Mst7_Phase_Kicked:		// Déplacement.
		nSpr = pnBaseSpr[pSpe->nType] + (pSpe->nBounce ^ 1);

		// Mouvement.
		pMst->nPosX += (gVar.pCos[pMst->nAngle] * pMst->nSpd) >> 8;
		pMst->nPosY += (gVar.pSin[pMst->nAngle] * pMst->nSpd) >> 8;

		// Rebond sur le mur haut.
		if (pMst->nPosY <= (8 << 8) && pMst->nAngle < 128) pMst->nAngle = -pMst->nAngle;

		// Gravité.
		pSpe->nSpdZ += GRAVITY;
		pSpe->nPosZ += pSpe->nSpdZ;
		// Au sol ?
		if (pSpe->nPosZ >= 0)
		{
			pSpe->nPosZ = 0;
			// Rebond ou arrêt.
			if (pSpe->nSpdZ > 0x200)
			{
				pSpe->nSpdZ = -pSpe->nSpdZ / 2;
				pMst->nSpd /= 2;
				//
				pSpe->nBounce = 1;		// Slt pour image.
			}
			else
			{
				pSpe->nSpdZ = 0;
				pMst->nPhase = e_Mst7_Phase_Waiting;
				// Arrêt, en dehors de l'écran ?
				if ((pMst->nPosY >> 8) > GND_BLK_HT * SCROLL_TILE_H) return (e_MstState_Dead);
			}
		}
		break;
	}

	Game_SprDisplay(nSpr, pMst->nPosX, pMst->nPosY, pSpe->nPosZ);
	if (pMst->nPhase == e_Mst7_Phase_Kicked)
		Game_SprDisplayPrio(e_Spr_Hero_Shadows + 2, pMst->nPosX, pMst->nPosY, 0, 0);		// Ombre quand vol.

	return (e_MstState_Managed);
}

//=============================================================================
// Mst 8 : Coke cans.

/*
[mst8] Coke cans
offset = 0:3:		; en dalles
bounce = 4:5:		; retour arrière
prm = 6:7:			; tmp
*/

enum
{
	e_Mst8_Phase_Normal = 0,
	e_Mst8_Phase_PlyrKeyWait,
	e_Mst8_Phase_WaitInitLvl,
};

// Structure spécifique.
struct SMst8_CokeCan
{
	u8	nBounce;
	u8	nBackCnt;		// Compteur de frames de recul.

};

void Mst8_Init_CokeCan(struct SMstCommon *pMst, u8 *pData)
{
	struct SMst8_CokeCan	*pSpe = (struct SMst8_CokeCan *)pMst->pData;

	u32	nVal;
	static s32 pnOffs[] = { -4, 0, 4, 0, 4, 0, -4 };	// 5 + 2, pour décalage de 2 dans la table en fct de la frame.

	nVal = GetBits(0, 3, pData, 0);		// Offset en dalles.
	pMst->nPosX += nVal << (5 + 8);
	pMst->nPosX += pnOffs[(pMst->nPosY >> 12) + ((gnFrame & 1) << 1)] << 7;//8;

	pMst->nSpd = -0x200;

//	pMst->nPhase = e_Mst8_Phase_Normal;
	pMst->nPhase = (gGame.nPhase == e_Game_Play ? e_Mst8_Phase_Normal : e_Mst8_Phase_WaitInitLvl);

	nVal = GetBits(4, 5, pData, 0);		// Bounce.
	pSpe->nBounce = nVal;
	pSpe->nBackCnt = 0;

}

s32 Mst8_Main_CokeCan(struct SMstCommon *pMst)
{
	struct SMst8_CokeCan	*pSpe = (struct SMst8_CokeCan *)pMst->pData;

	if (Mst_ScreenOut(pMst->nPosX, 32)) return (e_MstState_Dead);

	// Mouvement (indépendant des phases).
//	if (gGame.nPhase == e_Game_Play)			// Cas particulier du départ... >_< @#!
	if (pMst->nPhase != e_Mst8_Phase_WaitInitLvl)
	{
	if (pSpe->nBackCnt)
	{
		if (--pSpe->nBackCnt == 0)
		{
			pMst->nSpd = -(pMst->nSpd - 0x100);		// Repart à l'endroit.
		}
	}
	else
	if (pSpe->nBounce && pMst->nPosX < gGame.nGndX + ((SCR_Width / 2) << 8))
	{
		pMst->nSpd = -(pMst->nSpd - 0x100);			// Part en arrière.
		pSpe->nBackCnt = 16;
		pSpe->nBounce--;
	}
	pMst->nPosX += pMst->nSpd;
	}

	// Contact. (pas terrible, phases inutiles dans ce cas, tester e_AnmKey_Hero_Flattened en plus suffirait).
	switch (pMst->nPhase)
	{
	case e_Mst8_Phase_Normal:			// Phase normale.
		// Contact avec le perso ?
		if (gGame.nPlayerPosZ > -16 << 8)
		if (Anm_GetKey(gGame.nPlayerAnm) != e_AnmKey_Hero_Death)
		{
			struct SSprRect sRct;
			if (SprGetRect(e_Spr_CokeCan, e_SprRectZone_RectCol, &sRct))
			if (sRct.nType == e_SprRect_Rect)
/*
			{
				Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX1 << 8), pMst->nPosY + (sRct.nY1 << 8), 0);
				Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX2 << 8), pMst->nPosY + (sRct.nY1 << 8), 0);
				Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX1 << 8), pMst->nPosY + (sRct.nY2 << 8), 0);
				Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX2 << 8), pMst->nPosY + (sRct.nY2 << 8), 0);
			}
*/
			if (gGame.nPlayerPosX >= pMst->nPosX + (sRct.nX1 << 8) && gGame.nPlayerPosX <= pMst->nPosX + (sRct.nX2 << 8) &&
				gGame.nPlayerPosY >= pMst->nPosY + (sRct.nY1 << 8) && gGame.nPlayerPosY <= pMst->nPosY + (sRct.nY2 << 8))
			{
				if (gGame.nPlayerGnd)
				{
					Game_Player_FlattenedSet();
					pMst->nPhase = e_Mst8_Phase_PlyrKeyWait;
				}
				else
					Game_Player_FootBumpInit();
			}
		}

	case e_Mst8_Phase_PlyrKeyWait:		// Attente que le perso ne soit plus écrasé.
		if (Anm_GetKey(gGame.nPlayerAnm) != e_AnmKey_Hero_Flattened)
			pMst->nPhase = e_Mst8_Phase_Normal;
		break;

	case e_Mst8_Phase_WaitInitLvl:		// Cas particulier du départ. On attend le départ et que le joueur ait bougé.
		if (gGame.nPhase == e_Game_Play && gGame.nPlayerPosX != gGame.nPlayerLastX)
			pMst->nPhase = e_Mst8_Phase_Normal;
		break;
	}

//	Game_SprDisplay(e_Spr_CokeCan + 7 - ((pMst->nPosX >> 10) & 7), pMst->nPosX, pMst->nPosY, 0);
	Game_SprDisplay(e_Spr_CokeCan + 7 - (((pMst->nPosY >> 10)+(pMst->nPosY >> 12) + (pMst->nPosX >> 10)) & 7), pMst->nPosX, pMst->nPosY, 0);

	return (e_MstState_Managed);
}

//=============================================================================
// Mst 9 : Gros pneu.

/*
[mst9] BigTire
prm = 0:7:			; tmp
*/

// Structure spécifique.
struct SMst9_BigTire
{
	s32	nPosZ, nSpdZ;
	u8	nNoHit;			// Pour attendre que le joueur quitte la zone après un choc.
};

#define	MST9_SPDZ_INIT	(-0x600)

void Mst9_Init_BigTire(struct SMstCommon *pMst, u8 *pData)
{
	struct SMst9_BigTire	*pSpe = (struct SMst9_BigTire *)pMst->pData;

	pMst->nPosY += 11 << 8;
	pSpe->nPosZ = 0;
	pSpe->nSpdZ = MST9_SPDZ_INIT;

	pMst->nPhase = 0;	// La phase sert de compteur pour l'écrasement au sol.
	pSpe->nNoHit = 0;	// Pour attendre que le joueur quitte la zone après un choc.
}

s32 Mst9_Main_BigTire(struct SMstCommon *pMst)
{
	struct SMst9_BigTire	*pSpe = (struct SMst9_BigTire *)pMst->pData;

	if (Mst_ScreenOut(pMst->nPosX, 32)) return (e_MstState_Dead);

	u32	nSpr = e_Spr_BigTire;

	// En vol ?
	if (pMst->nPhase == 0)
	{
		pMst->nPosX -= 0x080;
		// Gravité.
		pSpe->nSpdZ += GRAVITY;
		pSpe->nPosZ += pSpe->nSpdZ;
		// Au sol ?
		if (pSpe->nPosZ >= 0)
		{
			pSpe->nPosZ = 0;
			pSpe->nSpdZ = MST9_SPDZ_INIT;
			Sfx_PlaySfx(e_Sfx_BigTire_Bounce, e_SfxPrio_0);
			pMst->nPhase++;
		}
	}
	else
	{	// Ecrasement du pneu pendant quelques frames.
		nSpr = e_Spr_BigTire + 1;
		pMst->nPhase++;
		pMst->nPhase &= 3;
	}


	// Héros à la bonne hauteur ?
	if (gGame.nPlayerPosZ > pSpe->nPosZ - (16 << 8) && gGame.nPlayerPosZ - (24 << 8) < pSpe->nPosZ)
	{
		struct SSprRect sRct;
		if (SprGetRect(e_Spr_BigTire, e_SprRectZone_RectCol, &sRct))
		if (sRct.nType == e_SprRect_Rect)
		{
/*
			Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX1 << 8), pMst->nPosY + (sRct.nY1 << 8), 0);
			Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX2 << 8), pMst->nPosY + (sRct.nY1 << 8), 0);
			Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX1 << 8), pMst->nPosY + (sRct.nY2 << 8), 0);
			Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX2 << 8), pMst->nPosY + (sRct.nY2 << 8), 0);
*/

			if (gGame.nPlayerPosX >= pMst->nPosX + (sRct.nX1 << 8) && gGame.nPlayerPosX <= pMst->nPosX + (sRct.nX2 << 8) &&
				gGame.nPlayerPosY >= pMst->nPosY + (sRct.nY1 << 8) && gGame.nPlayerPosY <= pMst->nPosY + (sRct.nY2 << 8))
			{
				if (pSpe->nNoHit == 0)
				{

				if (gGame.nPlayerGnd && pSpe->nPosZ > (-8 << 8) && pSpe->nSpdZ > 0)
				{	// Ecrasement.
					Game_Player_FlattenedSet();
					pSpe->nNoHit = 1;		// Plus de contact.
				}
				else
				if (gGame.nPlayerPosZ < pSpe->nPosZ - (8 << 8))
				{	// Bump.
					Game_Player_FootBumpInit();
					pSpe->nNoHit = 1;		// Plus de contact.
				}
				else
				{	// Chute. => Pas de nohit, au cas où un joueur chute en arrière et que le pneu atterrisse sur lui sans être sorti de la zone (est-ce que ça arrive, je ne sais pas).
					if (gGame.nPlayerLastX < pMst->nPosX)
						Game_Player_FallBackInit();
					else
						Game_Player_FallFrontInit();
				}

				}
			}	// if dans la zone
			else
				pSpe->nNoHit = 0;		// Joueur sorti de la zone de col x y, contact de nouveau.

		}	// if rect
	}	// if hauteur
	else
		pSpe->nNoHit = 0;		// Joueur sorti de la zone de col z, contact de nouveau.


	// Sprite.
	Game_SprDisplay(nSpr, pMst->nPosX, pMst->nPosY, pSpe->nPosZ);
	// Ombre.
	nSpr = -pSpe->nPosZ >> (8 + 4);
	if (nSpr > 2) nSpr = 2;
	Game_SprDisplayPrio(e_Spr_Hero_Shadows + nSpr, pMst->nPosX, pMst->nPosY, 0, 0);

	return (e_MstState_Managed);
}

//=============================================================================
// Mst 10 : Cube rouge.

/*
[mst10] RedCube
dist(5) = 0:2:		; Distance
dir = 3:3: Bas - Haut	; Direction originale
prm = 4:7:			; tmp
*/

// Structure spécifique.
struct SMst10_RedCube
{
	u8	nLimMin, nLimMax;	// Limite Y du déplacement.
	s8	nDir;		// 1 = bas / -1 = haut.
	u8	nMvt2;		// Pour compter 1 case tous les 2 déplacements.

};

void Mst10_Init_RedCube(struct SMstCommon *pMst, u8 *pData)
{
	struct SMst10_RedCube	*pSpe = (struct SMst10_RedCube *)pMst->pData;
	u32	nVal;

	// Décalage dans la case.
	pMst->nPosX += 7 << 8;
	pMst->nPosY += 4 << 8;

	nVal = GetBits(3, 3, pData, 0);		// Dir originale.
	pSpe->nDir = nVal;
	nVal = GetBits(0, 2, pData, 0) - 1;		// Distance.
	if (pSpe->nDir)
	{	// Haut.
		pSpe->nLimMax = (pMst->nPosY >> 8) / SCROLL_TILE_H;
		pSpe->nLimMin = pSpe->nLimMax - nVal;
		pSpe->nDir = -1;
		pMst->nAnm = Anm_Set(gAnm_RedCube_MoveUp, pMst->nAnm);
	}
	else
	{	// Bas.
		pSpe->nLimMin = (pMst->nPosY >> 8) / SCROLL_TILE_H;
		pSpe->nLimMax = pSpe->nLimMin + nVal;
		pSpe->nDir = 1;
		pMst->nAnm = Anm_Set(gAnm_RedCube_MoveDown, pMst->nAnm);
	}
	pSpe->nMvt2 = 0;

	pMst->nPhase = 0;
}

s32 Mst10_Main_RedCube(struct SMstCommon *pMst)
{
	struct SMst10_RedCube	*pSpe = (struct SMst10_RedCube *)pMst->pData;
	u32	nSpr;

	if (Mst_ScreenOut(pMst->nPosX, 32)) return (e_MstState_Dead);

	// Déplacement du cube.
	nSpr = Anm_GetImage(pMst->nAnm);
	if (Anm_CheckStepFlag(pMst->nAnm))
	{
		pMst->nPosY += ((s32)pSpe->nDir * 8) << 8;		// Avance réelle sur le flag step.
		pSpe->nMvt2 ^= 1;
		if (pSpe->nMvt2 == 0)
		{
			u32	nCurY = (pMst->nPosY >> 8) / SCROLL_TILE_H;
			if (nCurY <= pSpe->nLimMin || nCurY >= pSpe->nLimMax)
			{
				pSpe->nDir = -pSpe->nDir;
				pMst->nAnm = Anm_Set((pSpe->nDir > 0 ? gAnm_RedCube_MoveDown : gAnm_RedCube_MoveUp), pMst->nAnm);
			}
		}
	}

	// Contacts.
	// Joueur dans la zone de col ?
	struct SSprRect sRct;
	u32	nCtc = 0;
	if (gGame.nPlayerPosZ > -16 << 8)
	if (Anm_GetKey(gGame.nPlayerAnm) != e_AnmKey_Hero_Death)
	if (Anm_GetKey(gGame.nPlayerAnm) != e_AnmKey_Hero_Flattened)
	if (SprGetRect(nSpr, e_SprRectZone_RectCol, &sRct))
	if (sRct.nType == e_SprRect_Rect)
	{
/*
		// tst affichage zone de col.
		Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX1 << 8), pMst->nPosY + (sRct.nY1 << 8), 0);
		Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX2 << 8), pMst->nPosY + (sRct.nY1 << 8), 0);
		Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX1 << 8), pMst->nPosY + (sRct.nY2 << 8), 0);
		Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX2 << 8), pMst->nPosY + (sRct.nY2 << 8), 0);
*/
		if (gGame.nPlayerPosX >= pMst->nPosX + (sRct.nX1 << 8) && gGame.nPlayerPosX <= pMst->nPosX + (sRct.nX2 << 8) &&
			gGame.nPlayerPosY >= pMst->nPosY + (sRct.nY1 << 8) && gGame.nPlayerPosY <= pMst->nPosY + (sRct.nY2 << 8))
			nCtc = 1;
	}

	if (pMst->nPhase == 0)
	{	// Phase == 0 : Le joueur est écrasable.
		if (nCtc)
		{
			// En X.
			if (gGame.nPlayerLastX < pMst->nPosX + (sRct.nX1 << 8))
			{	// Cube percuté de face.
				// Chute arrière. Recalage au x du cube + init chute.
				gGame.nPlayerPosX = pMst->nPosX + (sRct.nX1 << 8);
				Game_Player_FallBackInit();
			}
			else
			if (gGame.nPlayerLastX > pMst->nPosX + (sRct.nX2 << 8))
			{	// Cube percuté par derrière. Recalage.
				gGame.nPlayerPosX = pMst->nPosX + ((sRct.nX2 + 1) << 8);	// (+1 pour se placer au 1er pixel après la col).
			}
			else
			// La collision est en Y.
			if (gGame.nPlayerGnd)
			{
				Game_Player_FlattenedSet();
				pMst->nPhase = 1;
			}
			else
				Game_Player_FootBumpInit();
		}
	}
	else
	{	// Phase 1 : On attend la fin de l'écrasement, et que le joueur soit sorti de la zone de col.
		if (Anm_GetKey(gGame.nPlayerAnm) != e_AnmKey_Hero_Flattened && nCtc == 0)
			pMst->nPhase = 0;
	}

	Game_SprDisplay(nSpr, pMst->nPosX, pMst->nPosY, 0);

	return (e_MstState_Managed);
}

//=============================================================================
// Mst 11 : Mur téléscopique.

/*
[mst11] TelescopicWall
prm = 0:7:			; tmp
*/

void Mst11_Init_TelescopicWall(struct SMstCommon *pMst, u8 *pData)
{

	// Inits à différentes hauteurs, en fct de la position.
	static u64 *pM11Anm[] = { gAnm_TelescopicWall, gAnm_TelescopicWall_Init0, gAnm_TelescopicWall_Init1, gAnm_TelescopicWall_Init2 };
	pMst->nAnm = Anm_Set(pM11Anm[
						( ((pMst->nPosX >> 8) / SCROLL_TILE_W) ^ ((pMst->nPosY >> 8) / SCROLL_TILE_H) ) & 3
						], pMst->nAnm);

}

s32 Mst11_Main_TelescopicWall(struct SMstCommon *pMst)
{
	u32	nSpr;
	s32	nHt;

	if (Mst_ScreenOut(pMst->nPosX, 32)) return (e_MstState_Dead);

	nSpr = Anm_GetImage(pMst->nAnm);
	nHt = -(nSpr * 4) << 8;

	if (nHt)
	{
		struct SSprRect sRct;
		if (SprGetRect(e_Spr_TelescopicWall, e_SprRectZone_RectCol, &sRct))
		if (sRct.nType == e_SprRect_Rect)
		{
			sRct.nY1--;		// Agrandissement artificiel de la zone de col.
			sRct.nY2++;
/*
			Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX1 << 8), pMst->nPosY + (sRct.nY1 << 8), 0);
			Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX2 << 8), pMst->nPosY + (sRct.nY1 << 8), 0);
			Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX1 << 8), pMst->nPosY + (sRct.nY2 << 8), 0);
			Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX2 << 8), pMst->nPosY + (sRct.nY2 << 8), 0);
*/

			if (gGame.nPlayerPosX >= pMst->nPosX + (sRct.nX1 << 8) && gGame.nPlayerPosX <= pMst->nPosX + (sRct.nX2 << 8) &&
				gGame.nPlayerPosY >= pMst->nPosY + (sRct.nY1 << 8) && gGame.nPlayerPosY <= pMst->nPosY + (sRct.nY2 << 8))
			{
				// Limite Y.
				if (gGame.nPlayerLastY <= pMst->nPosY + (sRct.nY1 << 8))
				{
					gGame.nPlayerPosY = pMst->nPosY + (sRct.nY1 << 8);
					goto _M11_l0;
				}
				else
				if (gGame.nPlayerLastY >= pMst->nPosY + (sRct.nY2 << 8))
				{
					gGame.nPlayerPosY = pMst->nPosY + (sRct.nY2 << 8);
					goto _M11_l0;
				}

				// En X.
				if (gGame.nPlayerLastX <= pMst->nPosX + (sRct.nX1 << 8))
				{
					// Mur percuté de face.
					if (gGame.nPlayerPosZ >= nHt)	// Si on est pas au dessus...
					{
						if (gGame.nPlayerPosZ <= nHt + (4 << 8))
						{
							// Bump.
							Game_Player_FootBumpInit();
						}
						else
						{
							// Chute arrière. Recalage au x du mur + init chute.
							gGame.nPlayerPosX = pMst->nPosX + (sRct.nX1 << 8);
							Game_Player_FallBackInit();
						}
					}
				}
				else
				if (gGame.nPlayerLastX >= pMst->nPosX + (sRct.nX2 << 8))
				{
					// Mur percuté par derrière. Recalage.
					gGame.nPlayerPosX = pMst->nPosX + (sRct.nX2 << 8);
				}
				else
				{
					// Le joueur est au dessus du mur (last et cur pos).
					if (gGame.nPlayerPosZ >= nHt)	// Si on est pas au dessus...
					{
						// Bump.
						Game_Player_FootBumpInit();
					}
				}

			}

		}
	}
_M11_l0:

	// Affichage.
	Game_SprDisplay(nSpr + e_Spr_TelescopicWall, pMst->nPosX, pMst->nPosY, 0);

	return (e_MstState_Managed);
}

//=============================================================================
// Mst 12 : Souris verte.

/*
[mst12] GreenMouse
prm = 0:7:			; tmp
*/

enum
{
	e_Mst12_Phase_Search = 0,
	e_Mst12_Phase_Attack,
	e_Mst12_Phase_Follows,
	e_Mst12_Phase_GoAway,
};

// Structure spécifique.
struct SMst12_GreenMouse
{
	s32	nPosZ, nSpdZ;
	u16	nXMax;			// En blocs.
	u8	nFeetPos;		// Pour position aux pieds du joueur.
};

void Mst12_Init_GreenMouse(struct SMstCommon *pMst, u8 *pData)
{
	struct SMst12_GreenMouse	*pSpe = (struct SMst12_GreenMouse *)pMst->pData;
	static	u8	nPos = 0;		// Pour position aux pieds du joueur.

	pSpe->nXMax = (pMst->nPosX >> 12) + 20;		// Zone max de suivi du joueur.
	pSpe->nFeetPos = nPos & 7;
	nPos++;

	pSpe->nPosZ = 0;
	pSpe->nSpdZ = 0;

	pMst->nPosX += (SCROLL_TILE_W / 2) << 8;
	pMst->nPosY += (SCROLL_TILE_H / 2) << 8;
	pMst->nFlipX = 0;

	pMst->nPhase = e_Mst12_Phase_Search;
}

#define	M12_SPDMAX_GND	(0x80)
#define	M12_JMP_SPD		(0x400)
#define	M12_GND_OFFS	(PLYR_GND_OFFS - 2)

s32 Mst12_Main_GreenMouse(struct SMstCommon *pMst)
{
	struct SMst12_GreenMouse	*pSpe = (struct SMst12_GreenMouse *)pMst->pData;

	// Plus de temps ou niveau fini ?
	if (gGame.nTimer1 == 0 || gGame.nTimerStop)
	{
		pMst->nPhase = e_Mst12_Phase_GoAway;	// On force le go away.
		gGame.nM12SlowDown = 0;					// Pas de cas spécial pour --, toutes les mice mettront la valeur à 0.
	}

	s32	nSpdX, nSpdY;
	nSpdX = nSpdY = 0;
	switch (pMst->nPhase)
	{
	case e_Mst12_Phase_Search:		// La souris se dirige vers le joueur.
		// Y.
		if ((gGame.nPlayerPosY >> 8) < (pMst->nPosY >> 8))
			nSpdY = -M12_SPDMAX_GND;
		else
		if ((gGame.nPlayerPosY >> 8) > (pMst->nPosY >> 8))
			nSpdY = M12_SPDMAX_GND;
//fprintf(stderr, "m=%d / y=%x yp=%x / spdy=%x\n", pMst->nLoadIdx, pMst->nPosY, gGame.nPlayerPosY, nSpdY);
		// X.
		if (gGame.nPlayerPosX < pMst->nPosX)
			nSpdX = -M12_SPDMAX_GND;
		else
		if (gGame.nPlayerPosX > pMst->nPosX)
			nSpdX = M12_SPDMAX_GND;
		// Attack ?
		{
			s32	nDX, nDY;
			s32	nDX8, nDY8;
			nDX8 = (pMst->nPosX - gGame.nPlayerPosX) / 2;	// /2 pour ellipse, aussi pour le fatan et la vitesse x*2 sur angle.
			nDY8 = (pMst->nPosY - gGame.nPlayerPosY);
			nDX = nDX8 >> 8;
			nDY = nDY8 >> 8;
			// Si la souris est assez proche du joueur, elle attaque.
			if ((nDX * nDX) + (nDY * nDY) < (24 * 24))
			{
				pSpe->nSpdZ = -M12_JMP_SPD;
				pMst->nAngle = fatan2(nDY8, -nDX8);
				//
				pMst->nPhase = e_Mst12_Phase_Attack;
			}
		}
		break;

	case e_Mst12_Phase_Attack:		// Saut vers le joueur.
		nSpdX = (0x200 * gVar.pCos[pMst->nAngle]) / 256;	// *2 en X.
		nSpdY = (0x100 * gVar.pSin[pMst->nAngle]) / 256;

		if (pSpe->nPosZ >= 0)		// Au sol, on repasse en "search".
			pMst->nPhase = e_Mst12_Phase_Search;

		// Joueur attrapé ?
		{
			s32	nDX, nDY;
			nDX = (pMst->nPosX - gGame.nPlayerPosX) >> 9;	// /2 pour ellipse.
			nDY = (pMst->nPosY - gGame.nPlayerPosY) >> 8;
			// Si la souris est assez proche du joueur, joueur attrapé.
			if ((nDX * nDX) + (nDY * nDY) < (8 * 8))
			{
				pMst->nPhase = e_Mst12_Phase_Follows;
				gGame.nM12SlowDown++;		// +1 suiveuse.
			}
		}
		break;

	case e_Mst12_Phase_Follows:		// La souris suit le joueur.
		{
			// Joueur plus "normal" ?
			u32	nKey = Anm_GetKey(gGame.nPlayerAnm);
			if ((!(nKey == e_AnmKey_Hero_Idle || nKey == e_AnmKey_Hero_Run || nKey == e_AnmKey_Hero_Jump)) || gGame.nM12Eject)
			{
				// On le lâche.
				nSpdX = -M12_SPDMAX_GND;
				pMst->nPhase = e_Mst12_Phase_GoAway;
				gGame.nM12SlowDown--;		// -1 suiveuse.

				// Si le suivi s'arrête sur un cracker ou une springboard, on fait aussi sauter le monstre.
				if ((pSpe->nPosZ >= 0 &&
					(nKey == e_AnmKey_Hero_JumpForce1 || nKey == e_AnmKey_Hero_JumpForce2 || nKey == e_AnmKey_Hero_JumpForce3)) || gGame.nM12Eject)
					pSpe->nSpdZ = -M12_JMP_SPD;

				break;
			}

			static	s32	pnOffsX[] = { -8 << 8, -4 << 8, -4 << 8, -8 << 8, -4 << 8, -4 << 8, -8 << 8, -4 << 8 };
			static	s32	pnOffsY[] = {  0 << 8, -4 << 8,  4 << 8,  0 << 8,  4 << 8, -4 << 8,  0 << 8,  4 << 8 };
			// Suivi x,y.
			s32	nDX, nDY;
			nDX = (gGame.nPlayerPosX + pnOffsX[pSpe->nFeetPos]) - pMst->nPosX;
			nDY = (gGame.nPlayerPosY + pnOffsY[pSpe->nFeetPos]) - pMst->nPosY;
			pMst->nPosX += nDX / 8;
			pMst->nPosY += nDY / 8;

			pMst->nFlipX = (nDX < 0 ? 0 : 1);	// Sens, pour affichage.

			// Saut ?
			if (pSpe->nPosZ >= 0 && gGame.nPlayerSpdZ < -0x300)
				pSpe->nSpdZ = -M12_JMP_SPD;

		}
		break;

	case e_Mst12_Phase_GoAway:		// La souris se barre.
		if (Mst_ScreenOut(pMst->nPosX, 32)) return (e_MstState_Dead);
		//
		nSpdX = -M12_SPDMAX_GND;
		break;
	}

	// Sens, pour affichage.
	if (nSpdX) pMst->nFlipX = (nSpdX < 0 ? 0 : 1);

	// Déplacement x,y.
	pMst->nPosX += nSpdX;
	pMst->nPosY += nSpdY;
	// Limite du déplacement en Y.
	if (pMst->nPosY < M12_GND_OFFS << 8)
		pMst->nPosY = M12_GND_OFFS << 8;
	else
	if (pMst->nPosY > ((GND_BLK_HT * SCROLL_TILE_H) - M12_GND_OFFS) << 8)
		pMst->nPosY = ((GND_BLK_HT * SCROLL_TILE_H) - M12_GND_OFFS) << 8;

	// Gravité.
	pSpe->nSpdZ += GRAVITY;
	pSpe->nPosZ += pSpe->nSpdZ;
	// Au sol ?
	if (pSpe->nPosZ >= 0)
	{
		pSpe->nPosZ = 0;
		pSpe->nSpdZ = 0;
	}

	// Sprite.
	Game_SprDisplay(e_Spr_GreenMouse ^ (pMst->nFlipX ? SPR_Flip_X : 0), pMst->nPosX, pMst->nPosY, pSpe->nPosZ);
	// Ombre.
	if (pSpe->nPosZ < 0)
	{
		u32	nSpr;
		nSpr = (-pSpe->nPosZ >> (8 + 4)) + 1;
		if (nSpr > 2) nSpr = 2;
		Game_SprDisplayPrio(e_Spr_Hero_Shadows + nSpr, pMst->nPosX + 0x100, pMst->nPosY - 0x100, 0, 0);
	}

	return (e_MstState_Managed);
}

//=============================================================================
// Mst 13 : Pieces d'échecs.

/*
[mst13] ChessPiece
type = 0:0: King - Knight
prm = 1:7:			; tmp
*/

enum
{
	e_Mst13_Phase_Attack = 0,
	e_Mst13_Phase_GoAway,
	e_Mst13_Phase_Fall,
};

// Structure spécifique.
struct SMst13_ChessPiece
{
	u32	nType;
	s32	nSpdZ;
	s32	nPosZ;
	s32	nLastX;
	s8	nLnGoAway;		// Ligne cible sur laquelle se barre la pièce.
	u16	nXMax;			// En blocs.

};

void Mst13_Init_ChessPiece(struct SMstCommon *pMst, u8 *pData)
{
	struct SMst13_ChessPiece	*pSpe = (struct SMst13_ChessPiece *)pMst->pData;
	u32	nVal;

	nVal = GetBits(0, 0, pData, 0);		// Type.
	pSpe->nType = nVal;

	pMst->nPosX += 16 << 8;
	pMst->nPosX |= 0x7F;
	pSpe->nLastX = pMst->nPosX;
	pMst->nPosY += 11 << 8;
	pMst->nPosY |= 0x7F;

	pMst->nSpd = 0;
	pMst->nSpdY = 0;

	pSpe->nSpdZ = 0;
	pSpe->nPosZ = 0;
	pSpe->nLnGoAway = -1;

	pSpe->nXMax = (pMst->nPosX >> 12) + 20;

	pMst->nPhase = e_Mst13_Phase_Attack;

}

// (0x400 * 2) / 0x54 = 24 frames de saut.
// 0x400 = jmp value / 0x54 = gravity.

s32 Mst13_Main_ChessPiece(struct SMstCommon *pMst)
{
	struct SMst13_ChessPiece	*pSpe = (struct SMst13_ChessPiece *)pMst->pData;
	u32	nCtc = 0;

	switch (pMst->nPhase)
	{
	case e_Mst13_Phase_Attack:
		// Hors zone ? Ligne d'arrivée passée ? Plus de temps ?
		if (pMst->nPosX >> 12 > pSpe->nXMax ||
			gGame.nTimerStop || gGame.nTimer1 == 0)
		{
			pSpe->nLnGoAway = pMst->nPosY >> 12;		// Ligne sur laquelle il faudra se barrer, pour target.
			pMst->nPhase = e_Mst13_Phase_GoAway;
		}
		else
			nCtc = 1;
		break;

	case e_Mst13_Phase_Fall:
		// Décélération (au sol).
		if (pSpe->nPosZ >= 0)
		{
			pMst->nSpd = (pMst->nSpd * 7) / 8;
			pMst->nSpdY = (pMst->nSpdY * 7) / 8;
		}
		// !!! Pas de break !!!
	case e_Mst13_Phase_GoAway:
		// Screen out ?
		if (Mst_ScreenOut(pMst->nPosX, 32)) return (e_MstState_Dead);
		break;
	}

	// Déplacement.
	pSpe->nLastX = pMst->nPosX;
	pMst->nPosX += pMst->nSpd;
	pMst->nPosY += pMst->nSpdY;
	// Gravité.
	pSpe->nSpdZ += GRAVITY;
	pSpe->nPosZ += pSpe->nSpdZ;
	// Rebond.
	if (pSpe->nPosZ >= 0)
	{
		// Recollage au sol, etc...
		pMst->nPosX &= ~0xFF;
		pMst->nPosX |= 0x7F;
		pMst->nPosY &= ~0xFF;
		pMst->nPosY |= 0x7F;

		pSpe->nPosZ = 0;
		if (pMst->nPhase == e_Mst13_Phase_Fall) goto _M13_Skip;
		pSpe->nSpdZ = -0x400;

		// Target.
		s32	nTargetX, nTargetY;
		if (pSpe->nLnGoAway >= 0)
		{	// Finish line || time over. On dégage vers l'arrière.
			nTargetX = pMst->nPosX - ((SCROLL_TILE_W * 5) << 8);
			nTargetY = ((s32)pSpe->nLnGoAway << 12) + ((SCROLL_TILE_H / 2) << 8);
		}
		else
		{	// Cas normal, on cible le joueur.
			nTargetX = gGame.nPlayerPosX;
			nTargetY = gGame.nPlayerPosY;
		}

		// Angle vers lequel se trouve le joueur.
		s32	nDX, nDY;
		u8	nAng;
		nDX = nTargetX - pMst->nPosX;
		nDY = nTargetY - pMst->nPosY;
		nAng = fatan2(-nDY, nDX);

		nAng = ((nAng + (pSpe->nType ? 0 : 16)) / 32) & 7;		// [0;7]

		// Cas particulier du Roi, pour se remettre au bon Y.
		if (pSpe->nType == 0 && (nAng & 3) == 0)    // King && dir = 0 ou 3 ?
		if (pMst->nPosY >> 12 != nTargetY >> 12)    // Pas sur la bonne ligne ?
		{
//SprDisplayAbsolute(e_Spr_FontSmall + 'C' - ' ' - 1, 32, 50, 0);
			if (nTargetY < pMst->nPosY)
				nAng += (nAng & 4 ? -1 : 1);
			else
				nAng += (nAng & 4 ? 1 : -1);
			nAng &= 7;
		}

		s32	nLn = pMst->nPosY >> 12;		// Ligne en cours [0;4].
		s32	nAdd = ((nAng & 2) ? 1 : -1);	// Sens dans lequel tourner si mur dans la direction demandée.
		static u8	pDirs[][5] = {
			{ (1<<1)|(1<<2)|(1<<3), 0, 0, 0, (1<<5)|(1<<6)|(1<<7) },
			{ (1<<0)|(1<<1)|(1<<2)|(1<<3), (1<<1)|(1<<2), 0, (1<<5)|(1<<6), (1<<4)|(1<<5)|(1<<6)|(1<<7) },
		};
		while (pDirs[pSpe->nType][nLn] & (1 << nAng)) nAng = (nAng + nAdd) & 7;

		// Ln 0 = Spd king / ln 1 = spd knight.
		static s32	pnSpdX[][8] = {
			{  (SCROLL_TILE_W << 8) / 24,  (SCROLL_TILE_W << 8) / 24,                          0, -(SCROLL_TILE_W << 8) / 24,
			  -(SCROLL_TILE_W << 8) / 24, -(SCROLL_TILE_W << 8) / 24,                          0,  (SCROLL_TILE_W << 8) / 24 },
			{  ((2 * SCROLL_TILE_W) << 8) / 24,        (SCROLL_TILE_W << 8) / 24,       -(SCROLL_TILE_W << 8) / 24, -((2 * SCROLL_TILE_W) << 8) / 24,
			  -((2 * SCROLL_TILE_W) << 8) / 24,       -(SCROLL_TILE_W << 8) / 24,        (SCROLL_TILE_W << 8) / 24,  ((2 * SCROLL_TILE_W) << 8) / 24 },
		};
		static s32	pnSpdY[][8] = {
			{                          0, -(SCROLL_TILE_H << 8) / 24, -(SCROLL_TILE_H << 8) / 24, -(SCROLL_TILE_H << 8) / 24,
									   0,  (SCROLL_TILE_H << 8) / 24,  (SCROLL_TILE_H << 8) / 24,  (SCROLL_TILE_H << 8) / 24 },
			{       -(SCROLL_TILE_H << 8) / 24, -((2 * SCROLL_TILE_H) << 8) / 24, -((2 * SCROLL_TILE_H) << 8) / 24,       -(SCROLL_TILE_H << 8) / 24,
					 (SCROLL_TILE_H << 8) / 24,  ((2 * SCROLL_TILE_H) << 8) / 24,  ((2 * SCROLL_TILE_H) << 8) / 24,        (SCROLL_TILE_H << 8) / 24 },
		};
		pMst->nSpd = pnSpdX[pSpe->nType][nAng];
		pMst->nSpdY = pnSpdY[pSpe->nType][nAng];

	}
_M13_Skip:

	// Contact avec le joueur.
	if (nCtc)
	if (gGame.nPlayerPosZ > pSpe->nPosZ - (24 << 8))	// Joueur pas au dessus du mst ?
	{
		struct SSprRect sRct;
		if (SprGetRect(e_Spr_ChessKing, e_SprRectZone_RectCol, &sRct))		// Knight & king, zone de col identique.
		if (sRct.nType == e_SprRect_Rect)
		{
/*
			// tst affichage zone de col.
			Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX1 << 8), pMst->nPosY + (sRct.nY1 << 8), 0);
			Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX2 << 8), pMst->nPosY + (sRct.nY1 << 8), 0);
			Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX1 << 8), pMst->nPosY + (sRct.nY2 << 8), 0);
			Game_SprDisplay(e_Spr_Tstrct_Cross, pMst->nPosX + (sRct.nX2 << 8), pMst->nPosY + (sRct.nY2 << 8), 0);
*/
			u32 nKey = Anm_GetKey(gGame.nPlayerAnm);

			if (nKey != e_AnmKey_Hero_Pitfall)
			if (gGame.nPlayerPosX >= pMst->nPosX + (sRct.nX1 << 8) && gGame.nPlayerPosX <= pMst->nPosX + (sRct.nX2 << 8) &&
				gGame.nPlayerPosY >= pMst->nPosY + (sRct.nY1 << 8) && gGame.nPlayerPosY <= pMst->nPosY + (sRct.nY2 << 8))
			{
				// Ecrasement du joueur ?
				if (gGame.nPlayerGnd &&
					pSpe->nPosZ > -0x800 && pSpe->nSpdZ >= 0)
				{
					Game_Player_FlattenedSet();
					// Mst, go away.
					pSpe->nLnGoAway = pMst->nPosY >> 12;		// Ligne sur laquelle il faudra se barrer, pour target.
					pMst->nPhase = e_Mst13_Phase_GoAway;
				}
				else
				{
					if (nKey != e_AnmKey_Hero_LandOnBack && nKey != e_AnmKey_Hero_LandOnBackStop &&		// Cas où on passe au dessus du joueur au sol.
						nKey != e_AnmKey_Hero_Flattened &&
						!(nKey == e_AnmKey_Hero_FallBack && gGame.nPlayerSpdX == 0) )
					{
						if (gGame.nPlayerLastX > pSpe->nLastX)
							Game_Player_FallFrontInit();
						else
							Game_Player_FallBackInit();
						// Mst, chute.
						pMst->nAnm = Anm_Set(gAnm_ChessPiece_Fall, pMst->nAnm);
						pMst->nPhase = e_Mst13_Phase_Fall;
						// On retourne les vitesses x et y.
						pMst->nSpd = -pMst->nSpd;
						pMst->nSpdY = -pMst->nSpdY;
						if (pMst->nSpd < 0) pMst->nFlipX = 1;
						// Limite spd Z.
						if (pSpe->nSpdZ < 0) pSpe->nSpdZ = 0;
					}
				} // if nKey
			} // if joueur dans la zone de col

		} // if (sRct.nType == e_SprRect_Rect)

	}

	// Sprite.
	static u32	pnSprBase[] = { e_Spr_ChessKing, e_Spr_ChessKnight };
	Game_SprDisplay((pnSprBase[pSpe->nType] + Anm_GetImage(pMst->nAnm)) ^ (pMst->nFlipX ? SPR_Flip_X : 0), pMst->nPosX, pMst->nPosY, pSpe->nPosZ);
	// Ombre.
//	if (pMst->nPhase != e_Mst13_Phase_Fall || pSpe->nPosZ < 0)		// Pas quand chhute + sol.
	if (pSpe->nPosZ < 0)		// Pas quand au sol. (Ca suffit en fait).
	{
		u32	nSpr;
		nSpr = -pSpe->nPosZ >> (8 + 3);
		if (nSpr > 2) nSpr = 2;
		Game_SprDisplayPrio(e_Spr_Hero_Shadows + nSpr, pMst->nPosX, pMst->nPosY - (2<<8), 0, 0);
	}

	return (e_MstState_Managed);
}

//=============================================================================
// Mst 14 : Panneaux.

/*
[mst14] Sign
sign = 0:3: Last 10m - Coke cans - Danger - Btl ctr - Btl right - Mice
prm = 4:7:			; tmp
*/

/*
// Affichage : On fait spr n°0 + type.
	e_Spr_Pannel_10M = e_Spr_GreenCan + 4,
	e_Spr_Pannel_CokeCan,
	e_Spr_Pannel_Attention,
	e_Spr_Pannel_RoadSmaller1,
	e_Spr_Pannel_RoadSmaller2,
	e_Spr_Pannel_GreenMouse,
*/

/*
enum
{
	e_Mst7_Phase_Waiting = 0,
	e_Mst7_Phase_Crushed,
	e_Mst7_Phase_Kicked,
};
*/

// Structure spécifique.
struct SMst14_Sign
{
	u32	nType;
};

void Mst14_Init_Sign(struct SMstCommon *pMst, u8 *pData)
{
	struct SMst14_Sign	*pSpe = (struct SMst14_Sign *)pMst->pData;
	u32	nVal;

	nVal = GetBits(0, 3, pData, 0);		// Type.
	pSpe->nType = nVal;

}

s32 Mst14_Main_Sign(struct SMstCommon *pMst)
{
	struct SMst14_Sign	*pSpe = (struct SMst14_Sign *)pMst->pData;

	if (Mst_ScreenOut(pMst->nPosX, 32)) return (e_MstState_Dead);

	Game_SprDisplayPrio(e_Spr_Pannel_10M + pSpe->nType, pMst->nPosX, 0, -2 << 12, 0);

	return (e_MstState_Managed);
}

//=============================================================================

/*
struct SAAA { u8	nTb0[80]; };
assert(sizeof(struct SAAA) < MST_COMMON_DATA_SZ);

#ifndef NDEBUG
// Debug, vérification de la taille des structures.
void Mst00CheckStructSizes(void)
{
//	assert(1 == 0);
	assert(sizeof(struct SMst0_EndOfLevel) < MST_COMMON_DATA_SZ);
	// Mst1: Pas de struct spécifique.
	// Mst2: Pas de struct spécifique.
	// Mst3: Pas de struct spécifique.
	// Mst4: Pas de struct spécifique.
	// Mst5: Pas de struct spécifique.
	// Mst6: Pas de struct spécifique.
	assert(sizeof(struct SMst7_SmallCan) < MST_COMMON_DATA_SZ);
	assert(sizeof(struct SMst8_CokeCan) < MST_COMMON_DATA_SZ);
	assert(sizeof(struct SMst9_BigTire) < MST_COMMON_DATA_SZ);
	assert(sizeof(struct SMst10_RedCube) < MST_COMMON_DATA_SZ);
	// Mst11: Pas de struct spécifique.
	assert(sizeof(struct SMst12_GreenMouse) < MST_COMMON_DATA_SZ);
	assert(sizeof(struct SMst13_ChessPiece) < MST_COMMON_DATA_SZ);
	assert(sizeof(struct SMst14_Sign) < MST_COMMON_DATA_SZ);
}
#endif
*/


