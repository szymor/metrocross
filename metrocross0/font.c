
#include "includes.h"
#include "font8x8.h"

//#define FONT8_CHR_NB	(65)	// Nb de chars dans la fonte.
//u8	gpFont8[] = {

// itoa.
void MyItoA(s32 nNb, char *pDst)
{
	char	cMin = ' ';
	char	*pPtr;
	u32	nTmp;

	// Cas des nombres négatifs.
	if (nNb < 0)
	{
		cMin = '-';
		nNb = -nNb;
	}

	pPtr = pDst + strlen(pDst) - 1;
	nTmp = nNb;
	do
	{
		*pPtr-- = (char)((nTmp % 10) + '0');
	} while (pPtr >= pDst && (nTmp /= 10) > 0);

	// Négatifs.
	if (cMin != ' ' && pPtr >= pDst) *pPtr = cMin;

}


// => A bufferiser.

// Print, font 8x8.
void Font8_Print(u32 nPosX, u32 nPosY, char *pStr, u16 nColor)
{
	if (pStr == NULL) return;

	// Affichage.
	SDL_LockSurface(gVar.pScreen);
	u8	*pScr = (u8 *)gVar.pScreen->pixels;

/*
	while (*pStr && nPosX < SCR_Width)
	{
		char	cChr = *pStr;
		if (cChr >= 'a' && cChr <= 'z') cChr &= ~0x20;
		if (cChr > ' ' && cChr < ' ' + FONT8_CHR_NB)
		{
			u32	i, j;
			u32	nOffs = ((cChr) - ' ') * 64;
			for (j = 0; j < 8; j++)
			{
				if (nPosY + j >= SCR_Height) break;
				for (i = 0; i < 8; i++)
				{
					if (nPosX + i >= SCR_Width)
					{
						nOffs += 8 - i;
						break;	// Pas de poke en dehors de l'écran.
					}
					if (gpFont8[nOffs++])
					{
						*(u16 *)(pScr + ((nPosX + i) * sizeof(u16)) + (((nPosY + j) * gVar.pScreen->pitch))) = nColor;
					}
				}
			}
		}
		nPosX += 8;
		pStr++;
	}
*/

	u32	nPitch = gVar.pScreen->pitch;
	pScr += (nPosY * gVar.pScreen->pitch) + (nPosX * sizeof(u16));

	while (*pStr && nPosX < SCR_Width)
	{
		char	cChr = *pStr++;
		if (cChr >= 'a' && cChr <= 'z') cChr &= ~0x20;
		if (cChr > ' ' && cChr < ' ' + FONT8_CHR_NB)
		{
			u8	*pLetter = &gpFontData[gpFontIdx[(u32)(cChr - ' ')]];
			u8	*pScr2 = pScr;
			u32	j, nNb;
			for (j = 0; j < 8; j++)				// 8 lignes.
			{
				nNb = *pLetter++;				// Nb de pixels sur la ligne.
				while (nNb--)
				{
					*(u16 *)(pScr2 + *pLetter) = nColor;	// pLetter = offset du pixel sur la ligne (pré-multiplié par 2).
					pLetter++;
				}
				pScr2 += nPitch;
			}
		}
		pScr += 8 * sizeof(u16);
	}

	SDL_UnlockSurface(gVar.pScreen);

}


/*
#include "includes.h"

// Affichage d'une phrase en sprites.
// Renvoie la largeur en pixels de la phrase.
u32 Font_Print(s32 nPosX, s32 nPosY, char *pStr, u32 nFlags)
{
	char	cChr;
	struct SSprite	*pSpr;
	s32	nPosXOrg = nPosX;
	u32	nSprHiglight = (nFlags & FONT_Highlight ? SPR_Flag_HitPal : 0);

	while (*pStr)
	{
		cChr = *pStr++;
		cChr -= ' ';
		if (cChr != 0)
		{
			// Char normal.
			pSpr = SprGetDesc(e_Spr_FontSmall - 1 + cChr);
//			if ((nFlags & FONT_NoDisp) == 0) SprDisplayAbsolute(e_Spr_FontSmall - 1 + cChr, nPosX, nPosY, 0x100 + e_Prio_HUD);
			if ((nFlags & FONT_NoDisp) == 0) SprDisplayAbsolute((e_Spr_FontSmall - 1 + cChr) | nSprHiglight, nPosX, nPosY, 0x100 + e_Prio_HUD);
		}
		else
		{
			// Espace, on avance de la taille d'un 'I'.
			pSpr = SprGetDesc(e_Spr_FontSmall - 1 + 'I' - ' ');
		}
		nPosX += pSpr->nLg + 1;
	}

	return ((u32)(nPosX - nPosXOrg));
}

// Affichage d'une phrase en sprites, espacement entre les lettres fixé.
// Renvoie la largeur en pixels de la phrase.
u32 Font_PrintSpc(s32 nPosX, s32 nPosY, char *pStr, u32 nFlags, u32 nSpc)
{
	char	cChr;
	s32	nPosXOrg = nPosX;
	u32	nSprHiglight = (nFlags & FONT_Highlight ? SPR_Flag_HitPal : 0);

	while (*pStr)
	{
		cChr = *pStr++;
		cChr -= ' ';
		if (cChr != 0)
		{
			// Char normal.
//			if ((nFlags & FONT_NoDisp) == 0) SprDisplayAbsolute(e_Spr_FontSmall - 1 + cChr, nPosX, nPosY, 0x100 + e_Prio_HUD);
			if ((nFlags & FONT_NoDisp) == 0) SprDisplayAbsolute((e_Spr_FontSmall - 1 + cChr) | nSprHiglight, nPosX + (cChr == '1' - ' ' ? 2 : 0), nPosY, 0x100 + e_Prio_HUD);
		}
		nPosX += nSpc;
	}

	return ((u32)(nPosX - nPosXOrg));
}
*/
