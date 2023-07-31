
#include "includes.h"

#define	SPHERICAL_PROJ	(1)		// Comment to remove effect.


#define NBELEM(tab) (sizeof(tab) / sizeof(tab[0]))

struct SRoto
{
	u8	pBuf[SCR_Width * SCR_Height * 2];	// 2 buffers pour avoir une trainée plus longue.
#if defined(SPHERICAL_PROJ)
	s32	pnSphOffs[SCR_Width * SCR_Height];	// Table d'offsets.
#endif
	u16	pnColors[256];
	u8	nAngle0, nAngle1, nAngle2;
};
struct SRoto	gRoto;


#if defined(SPHERICAL_PROJ)
// ***************************************
#define PI 3.14159265
#define	RAYON (200)			// = Demi diagonale sur un ecran 320 x 240.
#define RAYCALC (RAYON - 1)

// Précalcul des offsets pour projection sphérique.
void SphericalScreen_Preca(void)
{
	s32	ix, iy;
	double	dist;
	double	ht;
	double	angleh;
	s32	nOffsX, nOffsY;

	// Précalcul de la loupe (quart supérieur gauche).
	for (iy = 0; iy < SCR_Height; iy++)
	{
		// Petite correction pour éviter la ligne du centre, qui fait chier.
		//s32	iyy = (iy < (SCR_Height / 2) ? iy - 1 : iy + 1);
		s32	iyy = (iy < (SCR_Height / 2) ? iy - 2 : iy + 2);

		for (ix = 0; ix < SCR_Width; ix++)
		{
			nOffsX = nOffsY = 0;

			// Petite correction pour éviter la colonne du centre, qui fait chier.
			//s32	ixx = (ix < (SCR_Width / 2) ? ix - 1 : ix + 1);
			s32	ixx = (ix < (SCR_Width / 2) ? ix - 2 : ix + 2);

			// Est-on dans le cercle ?
			dist = (((SCR_Width / 2) - ixx) * ((SCR_Width / 2) - ixx)) + (((SCR_Height / 2) - iyy) * ((SCR_Height / 2) - iyy));
			if (dist < (RAYCALC * RAYCALC))
			{
				// Dans la loupe.
				ht = sqrt((RAYCALC * RAYCALC) - dist);		// Hauteur à ce point. (Pythagore ! Voir de côté : R^2 = dist^2 + h^2).

				// Calcul.
				angleh = acos(ht / RAYCALC);
				nOffsX = (s32)( (((SCR_Width / 2) - ixx) * cos(angleh))/4 );	// /2 pour sphere, /4 pour aplatir un peu.
				angleh = asin(ht / RAYCALC);
				nOffsY = (s32)( (((SCR_Height / 2) - iyy) * sin(angleh))/4 );	// /2 pour sphere, /4 pour aplatir un peu.
			}
			gRoto.pnSphOffs[(iy * SCR_Width) + ix] = ((iy + nOffsY) * SCR_Width) + (ix + nOffsX);
		}
	}
}
// ***************************************
#endif


// Init.
void RotoZoom_Init(void)
{
	u32	i, j;

	memset(gRoto.pBuf, 0, sizeof(gRoto.pBuf));	// RAZ buffer.
	gRoto.nAngle0 = gRoto.nAngle1 = gRoto.nAngle2 = 0;

	// Image en 2 bits, on les décale de 6 vers la gauche.
	u8	*pPic = (u8 *)gVar.pImgRoto->pixels;
	for (j = 0; j < gVar.pImgRoto->h; j++)
		for (i = 0; i < gVar.pImgRoto->w; i++)
			*(pPic + (j * gVar.pImgRoto->pitch) + i) <<= 6;

	// Création de la palette.
/*
	SDL_Color	pPal[4] = { { 0,0,0,0 }, { 0,0,222,0 }, { 0,222,222,0 }, { 206,206,206,0 } };

//	SDL_Color *pClr = gVar.pPicture->format->palette->colors;
	SDL_Color *pClr;// = gVar.pPicture->format->palette->colors;
	for (i = 0; i < 4; i++)
	{
//		pClr = &gVar.pPicture->format->palette->colors[i];
		pClr = &pPal[i];
		gpnColors[i] = SDL_MapRGB(gVar.pScreen->format, pClr->r, pClr->g, pClr->b);

fprintf(stderr, "r=%d / g=%d / b=%d\n", pClr->r, pClr->g, pClr->b);
//pClr++;
	}
*/
	SDL_Color *pClr;
	for (i = 0; i < 256; i++)
	{
		u8	pMsk[4] = { 0xC0, 0x30, 0x0C, 0x03 };	// Mask.
		u8	pShf[4] = {    6,    4,    2,    0 };	// Shift.
//		u16	pMul[4] = {  256,  192,  128,   64 };	// Multiplicateur (sur 256).
		u16	pMul[4] = {  128,   96,   64,   32 };	// Multiplicateur (sur 256).
		for (j = 0; j < 4; j++)
		{
			if (i & pMsk[j])
			{
//				pClr = &pPal[i >> pShf[j]];
				pClr = &gVar.pImgRoto->format->palette->colors[i >> pShf[j]];
//	gRoto.pnColors[i] = i*4;
				gRoto.pnColors[i] = SDL_MapRGB(gVar.pScreen->format,
					(pClr->r * pMul[j]) / 256,
					(pClr->g * pMul[j]) / 256,
//					(pClr->b * pMul[j]) / 256);
//					((pClr->b * pMul[j]) / 256) | i);
					((pClr->b * pMul[j]) / 256)
						| ((i & 0x30) == 0x10 ? 255 : 0) | ((i & 0x0C) == 0x04 ? 192 : 0) | ((i & 0x03) == 0x01 ? 128 : 0)	// Saturation clr 1.
//						| ((i & 0x30) == 0x30 ? 255 : 0) | ((i & 0xC0) == 0x04 ? 192 : 0) | ((i & 0x03) == 0x01 ? 128 : 0)	// Saturation clr 3.
					);
//	gRoto.pnColors[i] |= i*4;

/*
u8 rr, gg, bb;
SDL_GetRGB(gRoto.pnColors[i], gVar.pScreen->format, &rr, &gg, &bb);
fprintf(stderr, "clr=%3d(%d) - r=%3d g=%3d b=%3d > r=%3d g=%3d b=%3d > r=%3d g=%3d b=%3d - i*4=%d\n",
	i, j,
	pClr->r, pClr->g, pClr->b,
	(pClr->r * pMul[j]) / 256,
	(pClr->g * pMul[j]) / 256,
	(pClr->b * pMul[j]) / 256,
	rr, gg, bb, i*4);
*/

				break;
			}
		}
	}

#if defined(SPHERICAL_PROJ)
	SphericalScreen_Preca();		// Préca projection sphérique.
#endif

}

// RotoZoom.
void RotoZoom_Draw(void)
{
	u32	ix, iy;
	u16	*pScr;
	u8	*pPic;
	u8	*pBuf;
	u8	nClr;

	pPic = (u8 *)gVar.pImgRoto->pixels;

	// Scrolling de l'image.
	static u8	nPicInc = 0;
	pPic += (u32)nPicInc * 256;
	nPicInc += 4;

	// Buffer 0 ou 1.
//	pBuf = gpBuf;
	static	u8	nBuf = 0;
	pBuf = &gRoto.pBuf[(nBuf & 1 ? SCR_Width * SCR_Height : 0)];		// On alterne les buffers.
	nBuf ^= 1;
#if defined(SPHERICAL_PROJ)
	u8	*pBuf2 = pBuf;
#endif

	s32	nIncX, nIncY;
	s32	nCurXc, nCurYc;
	s32	nCurXl, nCurYl;

	//s32	nZoom = 0x120 + (gVar.pCos[(u8)(gRoto.nAngle1 + gRoto.nAngle2)] * 1);
	s32	nZoom = 0x220 + gVar.pCos[gRoto.nAngle1] + gVar.pCos[gRoto.nAngle2];

	u8 nAng;
	nAng = ((gVar.pCos[gRoto.nAngle0] * 16) + 0x80) >> 8;		// On fait varier l'angle entre 16 et -16. / +0x80, pour avoir à peu près autant de 240 que de 16 ! (sinon on a *un* 16 et plein de 240 à l'opposé, à cause des arrondis de virgule).
//	nAng = (((gVar.pCos[gRoto.nAngle0] + gVar.pSin[gRoto.nAngle2]) * 16) + 0x80) >> 8;		// On fait varier l'angle entre 16 et -16. / +0x80, pour avoir à peu près autant de 240 que de 16 ! (sinon on a *un* 16 et plein de 240 à l'opposé, à cause des arrondis de virgule).
	nAng -= 64;

	nIncX = (gVar.pCos[nAng] * nZoom);// >> 8;
	nIncY = (gVar.pSin[nAng] * nZoom);// >> 8;

	// Centrage.
	// x' = x.cos - y.sin
	// y' = x.sin + y.cos
	nCurXc = (( (-160 * gVar.pCos[nAng]) - (-120 * gVar.pSin[nAng]) ) * nZoom);// >> 8;
	nCurYc = (( (-160 * gVar.pSin[nAng]) + (-120 * gVar.pCos[nAng]) ) * nZoom);// >> 8;


#if !defined(SPHERICAL_PROJ)
	SDL_LockSurface(gVar.pScreen);

	pScr = (u16 *)gVar.pScreen->pixels;

	// On skippe les premières lignes (HUD).
	nCurXc -= nIncY * HUD_TOP_HT;
	nCurYc += nIncX * HUD_TOP_HT;
	pBuf += HUD_TOP_HT * SCR_Width;
	pScr += (HUD_TOP_HT * gVar.pScreen->pitch) / 2;
	// Loop.
	for (iy = HUD_TOP_HT; iy < SCR_Height - HUD_BOTTOM_HT; iy++)
	{
		nCurXl = nCurXc;
		nCurYl = nCurYc;
		for (ix = 0; ix < SCR_Width; ix++)
		{
//			nClr = *(pPic + (nCurYl & 0xFF00) + ((nCurXl >> 8) & 0xFF));
			nClr = *(pPic + ((nCurYl >> 8) & 0xFF00) + ((nCurXl >> 16) & 0xFF));

			nCurXl += nIncX;
			nCurYl += nIncY;

			nClr |= (*pBuf >> 2);
			*pBuf++ = nClr;
			*pScr++ = gRoto.pnColors[nClr];
		}

		nCurXc -= nIncY;
		nCurYc += nIncX;
	}

	SDL_UnlockSurface(gVar.pScreen);

#else

	// On skippe les premières lignes (HUD).
	nCurXc -= nIncY * HUD_TOP_HT;
	nCurYc += nIncX * HUD_TOP_HT;
	pBuf += HUD_TOP_HT * SCR_Width;
	// Loop.
	for (iy = HUD_TOP_HT; iy < SCR_Height - HUD_BOTTOM_HT; iy++)
	{
		nCurXl = nCurXc;
		nCurYl = nCurYc;
		for (ix = 0; ix < SCR_Width; ix++)
		{
//			nClr = *(pPic + (nCurYl & 0xFF00) + ((nCurXl >> 8) & 0xFF));
			nClr = *(pPic + ((nCurYl >> 8) & 0xFF00) + ((nCurXl >> 16) & 0xFF));

			nCurXl += nIncX;
			nCurYl += nIncY;

			nClr |= (*pBuf >> 2);
			*pBuf++ = nClr;
		}
		nCurXc -= nIncY;
		nCurYc += nIncX;
	}

	// Tracé avec projection sphérique.
	SDL_LockSurface(gVar.pScreen);
	pScr = (u16 *)gVar.pScreen->pixels;
/*
	for (iy = HUD_TOP_HT; iy < SCR_Height - HUD_BOTTOM_HT; iy++)
	{
		for (ix = 0; ix < SCR_Width; ix++)
		{
			*(pScr + ix + ((iy * gVar.pScreen->pitch) / 2)) = gRoto.pnColors[ *(pBuf2 + gRoto.pnSphOffs[ix + (iy * SCR_Width)]) ];
		}
	}
*/
	pScr += (HUD_TOP_HT * gVar.pScreen->pitch) / 2;
//	s32	*pnSphOffs = gRoto.pnSphOffs;
	for (ix = SCR_Width * HUD_TOP_HT; ix < SCR_Width * (SCR_Height - HUD_BOTTOM_HT); ix++)
	{
		*pScr++ = gRoto.pnColors[ *(pBuf2 + gRoto.pnSphOffs[ix]) ];
//		*pScr++ = gRoto.pnColors[ *(pBuf2 + (*pnSphOffs++)) ];
	}
	SDL_UnlockSurface(gVar.pScreen);
#endif


	// Balancement droite/pause/gauche/pause.
	static u8	nCnt = 0;
	if (nCnt == 0)
	{
		gRoto.nAngle0 += 1;
		if (gRoto.nAngle0 == 0 || gRoto.nAngle0 == 128) nCnt = 64;
	}
	else
		nCnt--;
	// Incréments angles.
	if (gRoto.nAngle0 & 1) gRoto.nAngle1 += 1;
	gRoto.nAngle2 -= 2;

}






