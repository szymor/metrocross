
#include "includes.h"


#define	SCROLL_BUF_LG	(384)		// Taille du buffer (scroll type scroll text, lg * 2).
#define	SCROLL_BUF_HT	(SCROLL_TILE_H * GND_BLK_HT)


struct SScroll
{
	SDL_Surface	*pScrollBuf;	// Buffers de scroll (1 plan).

	u32	nBackgroundNo;			// No du background.
	s32	nPosMax;				// -1 si pas utilisé.

	u8	nPosXSav;				// Sauvegarde pour l'affichage de la bande en bas par dessus les sprites.
};
struct SScroll	gScroll;

// Allocation des buffers de scroll.
void Scroll_Allocate(void)
{
	gScroll.pScrollBuf = SDL_CreateRGBSurface(SDL_SWSURFACE, SCROLL_BUF_LG * 2, SCROLL_BUF_HT, 16, gVar.pScreen->format->Rmask, gVar.pScreen->format->Gmask, gVar.pScreen->format->Bmask, 0);
	if (gScroll.pScrollBuf == NULL)
	{
		fprintf(stderr, "Scroll_Allocate(): Unable to allocate scroll buffer: %s\n", SDL_GetError());
		exit(1);
	}
}

// Libère les buffers de scroll.
void Scroll_Release(void)
{
	SDL_FreeSurface(gScroll.pScrollBuf);
	gScroll.pScrollBuf = NULL;
}


// Init scroll.
void Scroll_Init(void)
{
	struct SMap	*pMap = gGame.pMap;
	u32	i, j;
	SDL_Rect	sSrc, sDst;
	u32	nBlk;

	gScroll.nBackgroundNo = 0;			// No du background par défaut.

	// Init du premier écran.
	for (j = 0; j < GND_BLK_HT; j++)
	for (i = 0; i < (SCROLL_BUF_LG / SCROLL_TILE_W) + 1; i++)
	{
		nBlk = pMap->pMapBlocks[(j * pMap->nMapLg) + i];

		// Ajout des "crackers".
		if (nBlk == BLK_CRACKER)
			Mst_Add(e_Mst3_Cracker, i * SCROLL_TILE_W, j * SCROLL_TILE_H, NULL, -1);
		else if (nBlk == BLK_PITFALL)
			Mst_Add(e_Mst4_Pitfall, i * SCROLL_TILE_W, j * SCROLL_TILE_H, NULL, -1);

		sSrc.x = nBlk * SCROLL_TILE_W;
		sSrc.w = SCROLL_TILE_W;
		sSrc.y = 0;
		sSrc.h = SCROLL_TILE_H;
		//
		sDst.x = i * SCROLL_TILE_W;
		sDst.y = j * SCROLL_TILE_H;
		//
		SDL_BlitSurface(gGfxBlk.pBlocks, &sSrc, gScroll.pScrollBuf, &sDst);
	}

	// Passe sur les monstres.
	for (i = 0; i < (SCROLL_BUF_LG / SCROLL_TILE_W) + 1; i++)
		Mst_CheckNewCol(i);

	gGame.nGndX = 0;

	gScroll.nPosMax = -1;		// Sera initialisé par les portails de fin de niveau.

}

// Met le numéro du background (passé via un monstre de la map, à l'init).
void Scroll_BkgNo_Set(u32 nBackgroundNo)
{
	gScroll.nBackgroundNo = nBackgroundNo;
}


// Affichage de la bande du bas AU DESSUS des sprites.
void Scroll_BottomDisplay(void)
{
	SDL_Rect	sSrc, sDst;

	// La bordure en bas.
	sSrc.x = gScroll.nPosXSav;		// [0;255]
	sSrc.w = SCR_Width;
	sSrc.y = 0;
	sSrc.h = BKG2_HT;
	//
	sDst.x = 0;
	sDst.y = BKG2_Y_ORG;
	//
	SDL_BlitSurface(gGfxBlk.pBackground, &sSrc, gVar.pScreen, &sDst);
}

// Affichage du niveau en cours.
void Scroll_Display(void)
{
	SDL_Rect	sSrc, sDst;

	gScroll.nPosXSav = (gGame.nGndX >> 8) & 0xFF;		// Sauvegarde pour l'affichage de la bande en bas par dessus les sprites.

	// Le mur.
	sSrc.x = gScroll.nPosXSav;		// [0;255]
	sSrc.w = SCR_Width;
	sSrc.y = BKG2_HT + (gScroll.nBackgroundNo * BKG_HT);
	sSrc.h = BKG_HT;
	//
	sDst.x = 0;
	sDst.y = BKG_Y_ORG;
	//
	SDL_BlitSurface(gGfxBlk.pBackground, &sSrc, gVar.pScreen, &sDst);

	// La bordure en bas est affichée APRES l'affichage des sprites.

	// Le sol, en italique.
	SDL_LockSurface(gScroll.pScrollBuf);
	SDL_LockSurface(gVar.pScreen);

	u32	i, j;
	u8	*pSrc, *pDst;
	u32	nOffs = (SCROLL_GND_OFFS << 1);

	pSrc = gScroll.pScrollBuf->pixels;
	pSrc += ((gGame.nGndX >> 8) % SCROLL_BUF_LG) * 2;
	pDst = gVar.pScreen->pixels;
	pDst += GND_Y_ORG * gVar.pScreen->pitch;

	for (j = 0; j < GND_BLK_HT * SCROLL_TILE_H; j++)
	{
		u8	*pSrc2;
		pSrc2 = pSrc + (nOffs & ~1);		// Masque le bit de virgule et fait x2 en même temps.
		for (i = 0; i < SCR_Width; i++)
		{
			*((u16 *)pDst + i) = *((u16 *)pSrc2 + i);
		}
		pSrc += gScroll.pScrollBuf->pitch;
		pDst += gVar.pScreen->pitch;
		nOffs--;
	}

	SDL_UnlockSurface(gVar.pScreen);
	SDL_UnlockSurface(gScroll.pScrollBuf);

}

// Fixe la position max du scroll. (Pour fin de niveaux).
void Scroll_SetMaxPos(s32 nPos)
{
	gScroll.nPosMax = nPos;
}

// Gestion du scroll.
void Scroll_Manage(void)
{
	struct SMap	*pMap = gGame.pMap;
	s32	nLastX = gGame.nGndX;

	// Détermination de la position de la fenêtre par rapport au joueur.
	s32	nNewPosX;
	nNewPosX = gGame.nGndX;

	s32	nOffs;
	nOffs = (gGame.nPlayerPosX & ~0xFF) - (((2 * SCROLL_TILE_W) + (SCROLL_TILE_W / 2) + 4) << 8) - (gGame.nGndX & ~0xFF);

	if (nOffs > 0)
	{
		nNewPosX = gGame.nGndX + (nOffs / 20);
		// Si le joueur dépasse la colonne xxx, le scroll suit.
		if (gGame.nPlayerPosX - nNewPosX > (SCR_Width / 2) << 8)
		{
			nNewPosX = gGame.nPlayerPosX - ((SCR_Width / 2) << 8);
		}
	}


	// On ne dépasse pas la map.
	if ((nNewPosX >> 8) + SCROLL_BUF_LG >= pMap->nMapLg * SCROLL_TILE_W)
		nNewPosX = ((pMap->nMapLg * SCROLL_TILE_W) - SCROLL_BUF_LG - 1) << 8;
	// Si limite fixée dans le jeu...
	if (gScroll.nPosMax != -1)
	if (nNewPosX >= gScroll.nPosMax)
		nNewPosX = gScroll.nPosMax;


	gGame.nGndX = nNewPosX;


	// Changement de bloc ?
	while (nLastX >> (8+5) < gGame.nGndX >> (8+5))
	{
		// Copie de la dernière colonne affichée au début du buffer.
		SDL_Rect	sSrc, sDst;
		//
		sSrc.x = (((nLastX >> 8) % SCROLL_BUF_LG) & ~0x1F) + SCROLL_BUF_LG;
		sSrc.y = 0;
		sSrc.w = SCROLL_TILE_W;
		sSrc.h = SCROLL_TILE_H * GND_BLK_HT;
		//
		sDst.x = (((nLastX >> 8) % SCROLL_BUF_LG) & ~0x1F);
		sDst.y = 0;
		//
		SDL_BlitSurface(gScroll.pScrollBuf, &sSrc, gScroll.pScrollBuf, &sDst);

		// Avance du 'last x'.
		nLastX += SCROLL_TILE_W << 8;

		// Nouvelle colonne.
		u32	j;
		u32	nBlk;
		u32	nCol;

		nCol = (nLastX + (SCROLL_BUF_LG << 8)) >> (8 + 5);
		for (j = 0; j < GND_BLK_HT; j++)
		{
			nBlk = pMap->pMapBlocks[(j * pMap->nMapLg) + nCol];

			// Ajout des "crackers".
			if (nBlk == BLK_CRACKER)
				Mst_Add(e_Mst3_Cracker, nCol * SCROLL_TILE_W, j * SCROLL_TILE_H, NULL, -1);
			else if (nBlk == BLK_PITFALL)
				Mst_Add(e_Mst4_Pitfall, nCol * SCROLL_TILE_W, j * SCROLL_TILE_H, NULL, -1);

			sSrc.x = nBlk * SCROLL_TILE_W;
			sSrc.w = SCROLL_TILE_W;
			sSrc.y = 0;
			sSrc.h = SCROLL_TILE_H;
			//
			sDst.x = (((nLastX >> 8) % SCROLL_BUF_LG) + SCROLL_BUF_LG) & ~0x1F;
			sDst.y = j * SCROLL_TILE_H;
			//
			SDL_BlitSurface(gGfxBlk.pBlocks, &sSrc, gScroll.pScrollBuf, &sDst);
		}

		// Et on active les monstres sur la colonne qui entre.
		Mst_CheckNewCol(nCol);

	}

}


