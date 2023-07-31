//
// Petit moteur de monstres.
//

#include "includes.h"

// externs.
extern	struct SMstTb gpMstTb[];

#define	MST_MAX_SLOTS	(32)
struct SMstCommon	gpMstSlots[MST_MAX_SLOTS];
u32	gnMstLastUsed;

u32	gnMstPrio;		// Pour priorité de l'affichage.

//u8	gpMstQuestItems[MST_QUEST_ITEMS_MAX];	// Items de quête.

//u8	gpnMstCount[e_Mst_MAX];		// Compte des monstres par type.

// RAZ moteur.
void Mst_InitEngine(void)
{
	u32	i;

	// RAZ de tous les slots.
	for(i = 0; i < MST_MAX_SLOTS; i++) gpMstSlots[i].nUsed = 0;
	gnMstLastUsed = 0;

	// RAZ des indexs de recherche dans les tables.
//	gLoadedMst.nMstRechIdxX = 0;
//	gLoadedMst.nMstRechIdxY = 0;
	gGame.pMst->nMstRechIdxX = 0;

	// RAZ states.
	memset(gGame.pMst->pMstState, e_MstState_Asleep, gGame.pMst->nMstNbInList * sizeof(u8));

//	// RAZ de items de quête.
//	for(i = 0; i < MST_QUEST_ITEMS_MAX; i++) gpMstQuestItems[i] = 0;

//	// RAZ du compte des monstres.
//	for(i = 0; i < e_Mst_MAX; i++) gpnMstCount[i] = 0;

}

// Cherche un slot libre.
// Out : N° d'un slot libre. -1 si erreur.
s32 Mst_GetSlot(void)
{
	u32	i;

	for (i = gnMstLastUsed; i < MST_MAX_SLOTS; i++)
	{
		if (gpMstSlots[i].nUsed == 0)
		{
			gpMstSlots[i].nUsed = 1;
			gnMstLastUsed = i + 1;		// La recherche commencera au suivant.
			return (i);
		}
	}
	return (-1);
}

// Libère un slot.
void Mst_ReleaseSlot(u32 nSlotNo)
{
	// Libère l'anim.
	if (gpMstSlots[nSlotNo].nAnm != -1) Anm_ReleaseSlot(gpMstSlots[nSlotNo].nAnm);
	// Pour accélérer la recherche des slots libres.
	if (nSlotNo < gnMstLastUsed)
	{
		gnMstLastUsed = nSlotNo;
	}
	gpMstSlots[nSlotNo].nUsed = 0;

}

// Ajoute un monstre dans la liste.
// In : nPosX et nPosY en pixels (pour placement au pixel).
// In : nLoadIdx : -1 pour les monstres générés.
// Out : N° du slot utilisé, -1 si erreur.
s32 Mst_Add(u32 nMstNo, s32 nPosX, s32 nPosY, u8 *pData, s32 nLoadIdx)
{
	s32	nSlotNo;

	// Slot mst.
	if ((nSlotNo = Mst_GetSlot()) == -1) { fprintf(stderr, "Mst_Add(): Out of monsters slots.\n"); return (-1); }
	// Slot anm.
	gpMstSlots[nSlotNo].nAnm = -1;
	if (gpMstTb[nMstNo].pAnm != NULL)	// Si NULL, on ne réserve pas d'anim.
	{
		// Pour réserver une anim.
		if ((gpMstSlots[nSlotNo].nAnm = Anm_Set(gpMstTb[nMstNo].pAnm, -1)) == -1) { Mst_ReleaseSlot(nSlotNo); return (-1); }
	}
	// Slots Ok.
	gpMstSlots[nSlotNo].nLoadIdx = nLoadIdx;
//	if (nLoadIdx >= 0) *(gLoadedMst.pMstState + nLoadIdx) = e_MstState_Managed;		// Le monstre est géré.
	if (nLoadIdx >= 0) gGame.pMst->pMstState[nLoadIdx] = e_MstState_Managed;		// Le monstre est géré.
	gpMstSlots[nSlotNo].nMstNo = nMstNo;
//	gpnMstCount[nMstNo]++;		// Compte des monstres par type.

	gpMstSlots[nSlotNo].nPhase = 0;
	gpMstSlots[nSlotNo].nFlipX = 0;
	gpMstSlots[nSlotNo].nPosX = nPosX << 8;		// Et on garde la position demandée.
	gpMstSlots[nSlotNo].nPosY = nPosY << 8;
	gpMstSlots[nSlotNo].nSpd = 0;
	gpMstSlots[nSlotNo].nAngle = 0;
	gpMstSlots[nSlotNo].nSpdY = 0;
	gpMstSlots[nSlotNo].nGround = 0;
	gpMstSlots[nSlotNo].pFctInit = gpMstTb[nMstNo].pFctInit;
	gpMstSlots[nSlotNo].pFctMain = gpMstTb[nMstNo].pFctMain;
	// Appel de la fonction d'init du monstre.
	gpMstSlots[nSlotNo].pFctInit(&gpMstSlots[nSlotNo], pData);

	return (nSlotNo);
}

// Gestion des monstres.
void Mst_Manage(void)
{
	u32	i;
	s32	nRetVal;

	gnMstPrio = 0;
//printf("frm: ");
	for (i = 0; i < MST_MAX_SLOTS; i++)
	{
		if (gpMstSlots[i].nUsed)
		{
//printf("%d(%d) ", gpMstSlots[i].nMstNo, gpMstSlots[i].nLoadIdx);
			if ((nRetVal = gpMstSlots[i].pFctMain(&gpMstSlots[i])) != e_MstState_Managed)
			{
				// Update de la table de status.
//				if (gpMstSlots[i].nLoadIdx >= 0) *(gLoadedMst.pMstState + gpMstSlots[i].nLoadIdx) = nRetVal;
				if (gpMstSlots[i].nLoadIdx >= 0) gGame.pMst->pMstState[ gpMstSlots[i].nLoadIdx ] = nRetVal;
//				// Compte des monstres par type.
//				gpnMstCount[gpMstSlots[i].nMstNo]--;
				// Le monstre sort de la gestion, on libère le slot.
				Mst_ReleaseSlot(i);
			}
			gnMstPrio = (gnMstPrio + 1) & MSTPRIO_AND;
		}
	}
//printf("\n");


/*
//> debug
u32	k = 0;
for (i = 0; i < MST_MAX_SLOTS; i++) if (gpMstSlots[i].nUsed) k++;
char	pScore[4+8+1] = "MST:0000";
MyItoA(k, pScore);
Font8_Print(10, 8, pScore, 0xFFFF);
//< debug
*/

}

/*
*/
#ifdef DEBUG_DISP
void mst_tst_dispnb(u32 nPosY)
{
	u32	i, k;
	// Compte.
	for (i = 0, k = 0; i < MST_MAX_SLOTS; i++) if (gpMstSlots[i].nUsed) k++;
	// Affichage.
	char	pTb[4+8+1] = "MST:00000000";
	MyItoA(k, pTb);
//	Font_Print(10, nPosY, pTb, 0);
	Font8_Print(10, nPosY, pTb, 0xFFFF);
}
#endif



/*
struct SMst0
{
	u32	nPosX, nPosY;
	u16	nMstNo;		// = No du bloc.
	u16	nNbBytes;	// Nb de bytes utilisés.
	u16	nIdx;		// Index du monstre, pour taper dans table pMstState.
	u16	_padding;
};

struct SLoadedMst
{
	u8	*pMstData;			// Les datas des monstres.
	struct	SMst0 **ppMstPtrX;	// Les monstres triés sur le X.
//	struct	SMst0 **ppMstPtrY;	// Les monstres triés sur le Y.
	u8	*pMstState;			// Etat des monstres (pas entré, déjà géré, mort...).
	u32	nMstNbInList;
//	s32	nMstRechIdxX, nMstRechIdxY;	// Indexs des recherches dans les tables ppMstPtrX et Y.

	s32	nMstRechIdxX;		// Index dans la liste de monstres.

};
*/

// Active tous les monstres sur une colonne de la map.
void Mst_CheckNewCol(s32 nCol)
{
	struct SMst0		*pMstCur;
	struct SLoadedMst	*pMst = gGame.pMst;

	if (pMst->nMstNbInList == 0) return;

//printf("mst rech COL : col = %d\n", nCol);
	// Vers la droite.
	for (; pMst->nMstRechIdxX < pMst->nMstNbInList; pMst->nMstRechIdxX++)
	{
		pMstCur = *(pMst->ppMstPtrX + pMst->nMstRechIdxX);
		if ((s32)pMstCur->nPosX > nCol) break;		// Stoppe la boucle.
		// Le monstre est sur la colonne et pas déjà géré ?
		if (pMstCur->nPosX == nCol)
		if (pMst->pMstState[pMstCur->nIdx] == e_MstState_Asleep)
			Mst_Add(pMstCur->nMstNo, (pMstCur->nPosX * SCROLL_TILE_W) /*+ MST_CTRX_DEF*/, (pMstCur->nPosY * SCROLL_TILE_H) /*+ MST_CTRY_DEF*/, (u8 *)pMstCur + sizeof(struct SMst0), pMstCur->nIdx);
	}
//	if (pMst->nMstRechIdxX >= pMst->nMstNbInList) pMst->nMstRechIdxX = pMst->nMstNbInList - 1;	// Inutile, scroll seulement vers la droite.
//printf("\n");

}


/*
#define	MST_CTRX_DEF	8		// Centrages en pixels dans un bloc.
#define	MST_CTRY_DEF	15

#define	MST_IF_ADD_X \
	if (pMstCur->nPosX == nCol) \
	if ((s32)pMstCur->nPosY >= nPosY && (s32)pMstCur->nPosY <= nPosY + (SCR_Height / 16) + (MST_CLIP_VAL * 2)) \
	if (*(gLoadedMst.pMstState + pMstCur->nIdx) == e_MstState_Asleep) \
	{ \
		MstAdd(pMstCur->nMstNo, (pMstCur->nPosX * 16) + MST_CTRX_DEF, (pMstCur->nPosY * 16) + MST_CTRY_DEF, (u8 *)pMstCur + sizeof(struct SMst0), pMstCur->nIdx); \
	}

// Teste si des monstres apparaissent sur une colonne.
void MstCheckNewCol(s32 nCol, s32 nPosY, s32 nSens)
{
	struct	SMst0 *pMstCur;

	if (gLoadedMst.nMstNbInList == 0) return;

//printf("mst rech COL : col = %d / yinit = %d\n", nCol, nPosY);
	if (nSens >= 0)
	{
		// Vers la droite.
//printf("mst rech d : ");
		for (; gLoadedMst.nMstRechIdxX < gLoadedMst.nMstNbInList; gLoadedMst.nMstRechIdxX++)
		{
			pMstCur = *(gLoadedMst.ppMstPtrX + gLoadedMst.nMstRechIdxX);
			if ((s32)pMstCur->nPosX > nCol) break;		// Stoppe la boucle.
//printf("%d ", gLoadedMst.nMstRechIdxX);
			// Un monstre au bon X et dans le Y demandé ?
			MST_IF_ADD_X
		}
		if (gLoadedMst.nMstRechIdxX >= gLoadedMst.nMstNbInList) gLoadedMst.nMstRechIdxX = gLoadedMst.nMstNbInList - 1;
//printf("\n");
	}
	else
	{
		// Vers la gauche.
//printf("mst rech g : ");
		for (; gLoadedMst.nMstRechIdxX >= 0; gLoadedMst.nMstRechIdxX--)
		{
			pMstCur = *(gLoadedMst.ppMstPtrX + gLoadedMst.nMstRechIdxX);
			if ((s32)pMstCur->nPosX < nCol) break;		// Stoppe la boucle.
//printf("%d ", gLoadedMst.nMstRechIdxX);
			// Un monstre au bon X et dans le Y demandé ?
			MST_IF_ADD_X
		}
		if (gLoadedMst.nMstRechIdxX < 0) gLoadedMst.nMstRechIdxX = 0;
//printf("\n");
	}

}


#define	MST_IF_ADD_Y \
	if (pMstCur->nPosY == nLine) \
	if ((s32)pMstCur->nPosX >= nPosX && (s32)pMstCur->nPosX <= nPosX + (SCR_Width / 16) + (MST_CLIP_VAL * 2)) \
	if (*(gLoadedMst.pMstState + pMstCur->nIdx) == e_MstState_Asleep) \
	{ \
		MstAdd(pMstCur->nMstNo, (pMstCur->nPosX * 16) + MST_CTRX_DEF, (pMstCur->nPosY * 16) + MST_CTRY_DEF, (u8 *)pMstCur + sizeof(struct SMst0), pMstCur->nIdx); \
	}

// Teste si des monstres apparaissent sur une ligne.
void MstCheckNewLine(s32 nLine, s32 nPosX, s32 nSens)
{
	struct	SMst0 *pMstCur;

	if (gLoadedMst.nMstNbInList == 0) return;

//printf("mst rech LINE : line = %d / xinit = %d\n", nLine, nPosX);
	if (nSens >= 0)
	{
		// Vers le bas.
//printf("mst rech b : ");
		for (; gLoadedMst.nMstRechIdxY < gLoadedMst.nMstNbInList; gLoadedMst.nMstRechIdxY++)
		{
			pMstCur = *(gLoadedMst.ppMstPtrY + gLoadedMst.nMstRechIdxY);
			if ((s32)pMstCur->nPosY > nLine) break;		// Stoppe la boucle.
//printf("%d ", gLoadedMst.nMstRechIdxY);
			// Un monstre au bon Y et dans le X demandé ?
			MST_IF_ADD_Y
		}
		if (gLoadedMst.nMstRechIdxY >= gLoadedMst.nMstNbInList) gLoadedMst.nMstRechIdxY = gLoadedMst.nMstNbInList - 1;
//printf("\n");
	}
	else
	{
		// Vers le haut.
//printf("mst rech h : ");
		for (; gLoadedMst.nMstRechIdxY >= 0; gLoadedMst.nMstRechIdxY--)
		{
			pMstCur = *(gLoadedMst.ppMstPtrY + gLoadedMst.nMstRechIdxY);
			if ((s32)pMstCur->nPosY < nLine) break;		// Stoppe la boucle.
//printf("%d ", gLoadedMst.nMstRechIdxY);
			// Un monstre au bon Y et dans le X demandé ?
			MST_IF_ADD_Y
		}
		if (gLoadedMst.nMstRechIdxY < 0) gLoadedMst.nMstRechIdxY = 0;
//printf("\n");
	}

}

//=============================================================================

// Renvoie le nombre de monstres de type x à l'écran.
// In : nMstType = Type du monstre, nOffset = Offset en blocs à ajouter à partir du bord de l'écran.
u32 MstOnScreenNb(u32 nMstType, s32 nBlkOffset)
{
	u32	nTot = 0;
	u32	i;

	for (i = 0; i < MST_MAX_SLOTS; i++)
	{
		if (gpMstSlots[i].nUsed && gpMstSlots[i].nMstNo == nMstType)
		if (gpMstSlots[i].nPosX >= gScrollPos.nPosX - (nBlkOffset << 12) &&
			gpMstSlots[i].nPosX < gScrollPos.nPosX + (SCR_Width << 8) + (nBlkOffset << 12) &&
			gpMstSlots[i].nPosY >= gScrollPos.nPosY - (nBlkOffset << 12) &&
			gpMstSlots[i].nPosY < gScrollPos.nPosY + (SCR_Height << 8) + (nBlkOffset << 12))
			nTot++;
	}
	return (nTot);

}

struct SMstCommon * MstGetPtrSlot(u32 nSlot)
{
	return (&gpMstSlots[nSlot]);
}

*/


