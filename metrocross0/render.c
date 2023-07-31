
#include "includes.h"


struct SRender	gRender;

// Scaling 2x.
void Render_Scale2x(SDL_Surface *pSDL_Src, SDL_Surface *pSDL_Dst)
{
	SDL_LockSurface(pSDL_Src);
	SDL_LockSurface(pSDL_Dst);

	u32	y, x;
	u8	*pSrc = pSDL_Src->pixels;
	u8	*pDst = pSDL_Dst->pixels;
	u32	nClr;

	u16	*pSrc2;
	u32	*pDst2a, *pDst2b;

	for (y = 0; y < SCR_Height; y++)
	{
		pSrc2 = (u16 *)pSrc;
		pDst2a = (u32 *)pDst;
		pDst2b = (u32 *)(pDst + pSDL_Dst->pitch);
		for (x = 0; x < SCR_Width; x++)
		{
			nClr = *pSrc2++;
			nClr |= (nClr << 16);
			*pDst2a++ = nClr;
			*pDst2b++ = nClr;
		}
		pSrc += pSDL_Src->pitch;
		pDst += pSDL_Dst->pitch * 2;
	}

	SDL_UnlockSurface(pSDL_Src);
	SDL_UnlockSurface(pSDL_Dst);
}

u16	gpTV2xCLUT[65536];

// Calcul de la grosse CLUT pour le mode TV2x.
#define	TV2x_FACTOR	200		// * factor, / 256
void TV2x_CalculateCLUT(SDL_Surface *pSDL_Dst)
{
	u32	i;
	u8	r, g, b;

	for (i = 0; i < 65536; i++)
	{
		SDL_GetRGB(i, pSDL_Dst->format, &r, &g, &b);
		r = ((u32)r * TV2x_FACTOR) / 256;
		g = ((u32)g * TV2x_FACTOR) / 256;
		b = ((u32)b * TV2x_FACTOR) / 256;
		gpTV2xCLUT[i] = SDL_MapRGB(pSDL_Dst->format, r, g, b);
	}
}

// TV2x.
void Render_TV2x(SDL_Surface *pSDL_Src, SDL_Surface *pSDL_Dst)
{
	SDL_LockSurface(pSDL_Src);
	SDL_LockSurface(pSDL_Dst);

	u32	y, x;
	u8	*pSrc = pSDL_Src->pixels;
	u8	*pDst = pSDL_Dst->pixels;
	u32	nClra, nClrb;

	u16	*pSrc2;
	u32	*pDst2a, *pDst2b;

	for (y = 0; y < SCR_Height; y++)
	{
		pSrc2 = (u16 *)pSrc;
		pDst2a = (u32 *)pDst;
		pDst2b = (u32 *)(pDst + pSDL_Dst->pitch);
		for (x = 0; x < SCR_Width; x++)
		{
			nClra = *pSrc2++;
			nClrb = gpTV2xCLUT[nClra];
			nClra |= (nClra << 16);
			nClrb |= (nClrb << 16);
			*pDst2a++ = nClra;
			*pDst2b++ = nClrb;
		}
		pSrc += pSDL_Src->pitch;
		pDst += pSDL_Dst->pitch * 2;
	}

	SDL_UnlockSurface(pSDL_Src);
	SDL_UnlockSurface(pSDL_Dst);
}


#if defined(RENDER_BPP)
u32	gpR2x_32_CLUT[65536];
u32	gpTV2x_32_CLUT[65536];

// Calcul des CLUT 32 bits.
void R2x_32_CalculateCLUT(void)
{
	u32	i;
	u8	r, g, b;

	SDL_Surface *pSrc;
	SDL_Surface *pDst;
/*
//> *** BPP Solution 1. > Sur Mac, pose problème à cause des masques des surfaces et écran qui sont différents.
	pSrc = SDL_CreateRGBSurface(SDL_SWSURFACE, SCR_Width, SCR_Height, 16, 0, 0, 0, 0);
	pDst = SDL_CreateRGBSurface(SDL_SWSURFACE, SCR_Width, SCR_Height, 32, 0, 0, 0, 0);
//<
*/

/*
//> *** BPP Solution 2.
//> Modif Pouet pour Mac. > Donne du bleu sur PC.
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
	int mask16r = 0x001F;
	int mask16g = 0x07E0;
	int mask16b = 0xF800;
	int mask16a = 0x0000;
	int mask32r = 0xFF000000;
	int mask32g = 0x00FF0000;
	int mask32b = 0x0000FF00;
	int mask32a = 0x000000FF;
#else
	int mask16r = 0xF800;
	int mask16g = 0x07E0;
	int mask16b = 0x001F;
	int mask16a = 0x0000;
	int mask32r = 0x000000FF;
	int mask32g = 0x0000FF00;
	int mask32b = 0x00FF0000;
	int mask32a = 0xFF000000;
#endif
	pSrc = SDL_CreateRGBSurface(SDL_SWSURFACE, SCR_Width, SCR_Height, 16, mask16r, mask16g, mask16b, mask16a);
	pDst = SDL_CreateRGBSurface(SDL_SWSURFACE, SCR_Width, SCR_Height, 32, mask32r, mask32g, mask32b, mask32a);
//< Modif Pouet pour Mac.
//<
*/

//> *** BPP Solution 3. Ok partout, mais la manip dans InitVideo n'est pas très jolie...
if (gRender.nUseGL)
{
	pSrc = gVar.pScreen;
	pDst = gRender.pSdlGlBuffer;
}
else
{
	pSrc = SDL_CreateRGBSurface(SDL_SWSURFACE, SCR_Width, SCR_Height, 16, gRender.pScreenBuf2->format->Rmask, gRender.pScreenBuf2->format->Gmask, gRender.pScreenBuf2->format->Bmask, 0);
	pDst = SDL_CreateRGBSurface(SDL_SWSURFACE, SCR_Width, SCR_Height, 32, gVar.pScreen->format->Rmask, gVar.pScreen->format->Gmask, gVar.pScreen->format->Bmask, 0);
}
//<
	if (pSrc == NULL || pDst == NULL)
	{
		fprintf(stderr, "R2x_32_CalculateCLUT(): Unable to allocate SDL surfaces.\n");
		exit(1);
	}

	for (i = 0; i < 65536; i++)
	{
		SDL_GetRGB(i, pSrc->format, &r, &g, &b);
		gpR2x_32_CLUT[i] = SDL_MapRGB(pDst->format, r, g, b);
		r = ((u32)r * TV2x_FACTOR) / 256;
		g = ((u32)g * TV2x_FACTOR) / 256;
		b = ((u32)b * TV2x_FACTOR) / 256;
		gpTV2x_32_CLUT[i] = SDL_MapRGB(pDst->format, r, g, b);
	}

if (gRender.nUseGL == 0)
{
	SDL_FreeSurface(pSrc);
	SDL_FreeSurface(pDst);
}

}

// Scaling 2x - 16 > 32.
void Render_Scale2x_16to32(SDL_Surface *pSDL_Src, SDL_Surface *pSDL_Dst)
{
	SDL_LockSurface(pSDL_Src);
	SDL_LockSurface(pSDL_Dst);

	u32	y, x;
	u8	*pSrc = pSDL_Src->pixels;
	u8	*pDst = pSDL_Dst->pixels;
	u32	nClr;

	u16	*pSrc2;
	u32	*pDst2a, *pDst2b;

	for (y = 0; y < SCR_Height; y++)
	{
		pSrc2 = (u16 *)pSrc;
		pDst2a = (u32 *)pDst;
		pDst2b = (u32 *)(pDst + pSDL_Dst->pitch);
		for (x = 0; x < SCR_Width; x++)
		{
			nClr = gpR2x_32_CLUT[*pSrc2++];
			*pDst2a++ = nClr;
			*pDst2a++ = nClr;
			*pDst2b++ = nClr;
			*pDst2b++ = nClr;
		}
		pSrc += pSDL_Src->pitch;
		pDst += pSDL_Dst->pitch * 2;
	}

	SDL_UnlockSurface(pSDL_Src);
	SDL_UnlockSurface(pSDL_Dst);
}

// TV2x - 16 > 32.
void Render_TV2x_16to32(SDL_Surface *pSDL_Src, SDL_Surface *pSDL_Dst)
{
	SDL_LockSurface(pSDL_Src);
	SDL_LockSurface(pSDL_Dst);

	u32	y, x;
	u8	*pSrc = pSDL_Src->pixels;
	u8	*pDst = pSDL_Dst->pixels;
	u32	nClra, nClrb;

	u16	*pSrc2;
	u32	*pDst2a, *pDst2b;

	for (y = 0; y < SCR_Height; y++)
	{
		pSrc2 = (u16 *)pSrc;
		pDst2a = (u32 *)pDst;
		pDst2b = (u32 *)(pDst + pSDL_Dst->pitch);
		for (x = 0; x < SCR_Width; x++)
		{
			nClra = gpR2x_32_CLUT[*pSrc2];
			nClrb = gpTV2x_32_CLUT[*pSrc2++];
			*pDst2a++ = nClra;
			*pDst2a++ = nClra;
			*pDst2b++ = nClrb;
			*pDst2b++ = nClrb;
		}
		pSrc += pSDL_Src->pitch;
		pDst += pSDL_Dst->pitch * 2;
	}

	SDL_UnlockSurface(pSDL_Src);
	SDL_UnlockSurface(pSDL_Dst);
}

#endif



// Buffer SDL 16 > Buffer OpenGL 32.
void Render_SDL16toOpenGL32(SDL_Surface *pSDL_Src, SDL_Surface *pSDL_Dst)
{
	SDL_LockSurface(pSDL_Src);
	SDL_LockSurface(pSDL_Dst);

	u32	y, x;
	u8	*pSrc = pSDL_Src->pixels;
	u8	*pDst = pSDL_Dst->pixels + (pSDL_Dst->pitch * 8);	// 224 de haut pour 240 de buffer.

	u16	*pSrc2;
	u32	*pDst2;

	for (y = 0; y < SCR_Height; y++)
	{
		pSrc2 = (u16 *)pSrc;
		pDst2 = (u32 *)pDst;
		for (x = 0; x < SCR_Width; x++)
		{
			*pDst2++ = gpR2x_32_CLUT[*pSrc2++];
		}
		pSrc += pSDL_Src->pitch;
		pDst += pSDL_Dst->pitch;
	}

	SDL_UnlockSurface(pSDL_Src);
	SDL_UnlockSurface(pSDL_Dst);
}


//extern	u8	gnFrameMissed;
typedef void (*pRenderFct)(SDL_Surface *pSDL_Src, SDL_Surface *pSDL_Dst);
// Rendu + Flip.
void Render_Flip(u32 nSync)
{

if (gRender.nUseGL)
{

/*
	// SDL game frame buffer => buffer 32 bits à tranférer en VRAM.
	SDL_Rect	sSrc;
	SDL_Rect	sDst;
	sSrc.x = 0;
	sSrc.y = 0;
	sSrc.w = 320;
	sSrc.h = 224;//240;
	sDst.x = 0;
	sDst.y = 8;
	SDL_BlitSurface(gVar.pScreen, &sSrc, pSdlGlBuffer, &sDst);
*/
	// SDL game frame buffer 16 => buffer 32 bits à tranférer en VRAM.
	Render_SDL16toOpenGL32(gVar.pScreen, gRender.pSdlGlBuffer);
//

	// Bind the texture object
	glBindTexture(GL_TEXTURE_2D, gRender.nGlTextureId);

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 320, 240,
		GL_BGRA, GL_UNSIGNED_BYTE, gRender.pSdlGlBuffer->pixels);
//	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 8, 320, 224,
//		GL_BGRA, GL_UNSIGNED_BYTE, pSdlGlBuffer->pixels + (8 * pSdlGlBuffer->pitch));
// il reste des traces en haut ?

	glBegin(GL_QUADS);
	// Top-left vertex (corner)
	glTexCoord2f(0, 0);
//	glVertex3f(0, 0+8, 0);
	glVertex3f(0, 0, 0);
	// Top-right vertex (corner)
	glTexCoord2f(0.625, 0);	// 320/512 = 0.625
//	glVertex3f(320, 0+8, 0);
	glVertex3f(320, 0, 0);
	// Bottom-right vertex (corner)
//	glTexCoord2f(0.625, 0.875);		// 224/256 = 0.875
	glTexCoord2f(0.625, 0.9375);	// 240/256 = 0.9375
//	glVertex3f(320, 240-8, 0);
	glVertex3f(320, 240, 0);
	// Bottom-left vertex (corner)
//	glTexCoord2f(0, 0.875);
	glTexCoord2f(0, 0.9375);
//	glVertex3f(0, 240-8, 0);
	glVertex3f(0, 240, 0);
	glEnd();



#if defined(GL_TV2x)
if (gRender.nRenderMode == e_RenderMode_TV2x)
{
	glBindTexture(GL_TEXTURE_2D, gRender.nGlTextureTVId);

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBegin(GL_QUADS);
	// Top-left vertex (corner)
	glTexCoord2f(0, 0);
	glVertex3f(0, 0, 0);//(0, 0+50, 0);
	// Top-right vertex (corner)
	//glTexCoord2f(0.625, 0);	// 320/512 = 0.625
	glTexCoord2f(1.0, 0);	// 320/512 = 0.625
	glVertex3f(320, 0, 0);
	// Bottom-right vertex (corner)
	//glTexCoord2f(0.625, 0.9375);	// 240/256 = 0.9375
	glTexCoord2f(1.0, 0.9375);	// 240/256 = 0.9375 == 480/512 = 0.9375
	glVertex3f(320, 240, 0);//(320, 240-50, 0);
	// Bottom-left vertex (corner)
	glTexCoord2f(0, 0.9375);
	glVertex3f(0, 240, 0);
	glEnd();

	glBlendFunc(GL_ONE, GL_ZERO);
	glDisable(GL_BLEND);
}
#endif




//SDL_Delay(10);
//	Frame_Wait();	// Wait entre transfert et flip.
	SDL_GL_SwapBuffers();
// >> tst
	SDL_Delay(10);	// Meilleur résultat avec le delay ici. Plus de frames loupées.
	gnFrame++;		// Plus d'appel à FrameWait() => incrémentation du frame counter ici.
// << tst
//	Frame_Wait();	// Wait entre transfert et flip.
	Frame_Init();	// Re-init cnt après l'attente du flip.

}
else
{

#if defined(RENDER_BPP)
	static pRenderFct	pFctTb[2][e_RenderMode_MAX] =
	{
		{ NULL, Render_Scale2x, Render_TV2x },	// Screen 16 bits.
		{ NULL, Render_Scale2x_16to32, Render_TV2x_16to32 },	// Screen 32 bits.
	};
#else
	static pRenderFct	pFctTb[e_RenderMode_MAX] = { NULL, Render_Scale2x, Render_TV2x };
#endif

/*
	// Frames loupées ? => Pas de Rendu/Flip.
	if (nSync && gnFrameMissed)
	{
		Frame_Wait();
		return;
	}
*/

#if defined(RENDER_BPP)
	if (pFctTb[gRender.nRenderBPP][gRender.nRenderMode] != NULL) pFctTb[gRender.nRenderBPP][gRender.nRenderMode](gVar.pScreen, gRender.pScreen2x);
#else
	if (pFctTb[gRender.nRenderMode] != NULL) pFctTb[gRender.nRenderMode](gVar.pScreen, gRender.pScreen2x);
#endif
	if (nSync) Frame_Wait();
	SDL_Flip(gRender.nRenderMode == e_RenderMode_Normal ? gVar.pScreen : gRender.pScreen2x);

}

}

// Set video mode.
#if defined(RENDER_BPP)
SDL_Surface * VideoModeSet(u32 nScrWidth, u32 nScrHeight, u32 nBPP, u32 nSDL_Flags)
#else
SDL_Surface * VideoModeSet(u32 nScrWidth, u32 nScrHeight, u32 nSDL_Flags)
#endif
{
	if (gRender.nUseGL) return NULL;

	SDL_Surface *pSurf;

#if defined(RENDER_BPP)
	pSurf = SDL_SetVideoMode(nScrWidth, nScrHeight, nBPP, SDL_SWSURFACE | nSDL_Flags);
//	pSurf = SDL_SetVideoMode(nScrWidth, nScrHeight, nBPP, SDL_HWSURFACE | SDL_FULLSCREEN | SDL_DOUBLEBUF);
#else
	pSurf = SDL_SetVideoMode(nScrWidth, nScrHeight, 16, SDL_SWSURFACE | nSDL_Flags);
#endif
	if (pSurf == NULL)
	{
		fprintf(stderr, "VideoModeSet(): Couldn't set video mode: %sn",SDL_GetError());
		//exit(1);
	}
	return (pSurf);
}

// Met le mode video qui va bien.
void Render_SetVideoMode(void)
{
#if defined(GL_TV2x)
	if (gRender.nUseGL)
	{
		if (gRender.nRenderMode == e_RenderMode_Normal)
			gRender.nRenderMode = e_RenderMode_Scale2x;		// Juste pour éviter un double appui sur F9 pour changer de mode.
		return;
	}
#else
	if (gRender.nUseGL) return;
#endif

	switch (gRender.nRenderMode)
	{
	case e_RenderMode_Scale2x:
	case e_RenderMode_TV2x:
#if defined(RENDER_BPP)
		gRender.pScreen2x = VideoModeSet(SCR_Width * 2, SCR_Height * 2, 16 << gRender.nRenderBPP, (gRender.nFullscreenMode ? SDL_FULLSCREEN : 0));
#else
		gRender.pScreen2x = VideoModeSet(SCR_Width * 2, SCR_Height * 2, (gRender.nFullscreenMode ? SDL_FULLSCREEN : 0));
#endif
		gVar.pScreen = gRender.pScreenBuf2;
		if (gRender.pScreen2x != NULL) return;		// Ok.
		// Erreur => On repasse en mode Normal et Windowed.
		gRender.nRenderMode = e_RenderMode_Normal;
		gRender.nFullscreenMode = 0;
		// ... et pas de break.
	case e_RenderMode_Normal:
	default:
#if defined(RENDER_BPP)
		gVar.pScreen = VideoModeSet(SCR_Width, SCR_Height, 16, (gRender.nFullscreenMode ? SDL_FULLSCREEN : 0));
#else
		gVar.pScreen = VideoModeSet(SCR_Width, SCR_Height, (gRender.nFullscreenMode ? SDL_FULLSCREEN : 0));
#endif
		gRender.pScreen2x = NULL;
		if (gVar.pScreen == NULL) exit(1);	// Message d'erreur dans VideoModeSet.
		break;
	}

}


// Precalcul des couleurs 16 bits pour le texte.
void Colors_Precalculate(void)
{
	static SDL_Color	pnSDLColors[e_Color_MAX] =
	{
		{ 81, 81, 98, 0 },		// e_Color_HUDBkg
		{ 81, 122, 241, 0 },	// e_Color_AreaBlue
		{ 210, 224, 241, 0 },	// e_Color_AreaGray
		{ 0, 0, 0, 0 },			// e_Color_Black
		{ 255, 255, 255, 0 },	// e_Color_White
		{ 224, 224, 224, 0 },	// e_Color_Gray
		{ 0, 224, 224, 0 },		// e_Color_Cyan
		{ 241, 241, 0, 0 },		// e_Color_Yellow
		{ 0, 174, 210, 0 },		// e_Color_CyanDark
		{ 241, 0, 0, 0 },		// e_Color_Red
		{ 0, 241, 0, 0 },		// e_Color_Green
	};
	u32	i;

	for (i = 0; i < e_Color_MAX; i++)
		gVar.pnColors[i] = SDL_MapRGB(gVar.pScreen->format, pnSDLColors[i].r, pnSDLColors[i].g, pnSDLColors[i].b);

}

// Init de la vidéo.
void Render_InitVideo(void)
{

if (gRender.nUseGL)
{

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);	// *new*
	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);	// V-Sync.

	SDL_Surface	*pScreen;
//	pScreen = SDL_SetVideoMode(640, 480, 16, SDL_OPENGL | SDL_FULLSCREEN); // *changed*
	pScreen = SDL_SetVideoMode(640, 480, 32, SDL_OPENGL | SDL_FULLSCREEN); // *changed*
	if (pScreen == NULL)
	{
		fprintf(stderr, "Render_InitVideo(): %s\n", SDL_GetError());
		exit(1);
	}

	// Set the OpenGL state after creating the context with SDL_SetVideoMode
	glClearColor(0, 0, 0, 0);

	glEnable(GL_TEXTURE_2D);	// Need this to display a texture

	glViewport(0, 0, 640, 480);	// dim écran.

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

//	glOrtho(0, 640, 480, 0, -1, 1);
	glOrtho(0, 320, 240, 0, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

//

	// Ecran => buffer 16 bits.
	gVar.pScreen = SDL_CreateRGBSurface(SDL_SWSURFACE, SCR_Width, SCR_Height, 16, 0,0,0,0);//gVar.pScreen->format->Rmask, gVar.pScreen->format->Gmask, gVar.pScreen->format->Bmask, 0);
	if (gVar.pScreen == NULL)
	{
		fprintf(stderr, "Render_InitVideo(): Unable to allocate SDL surface: %s\n", SDL_GetError());
		exit(1);
	}
	Colors_Precalculate();

	// Buffer 32 bits pour passage de la "texture" à OpenGL.
	gRender.pSdlGlBuffer = SDL_CreateRGBSurface(SDL_SWSURFACE, 320, 240, 32, 0,0,0,0);
	if (gRender.pSdlGlBuffer == NULL)
	{
		fprintf(stderr, "SDL_CreateRGBSurface error.\n");
		exit(1);
	}
//	SDL_FillRect(gRender.pSdlGlBuffer, NULL, SDL_MapRGB(gRender.pSdlGlBuffer->format, 255, 0, 0));
	SDL_FillRect(gRender.pSdlGlBuffer, NULL, 0);	// Pour effacer les bordures en haut et en bas.

	R2x_32_CalculateCLUT();

//

	// Have OpenGL generate a texture object handle for us
	glGenTextures(1, &gRender.nGlTextureId);
	// Creation de la texture de base.
	// Bind the texture object
	glBindTexture(GL_TEXTURE_2D, gRender.nGlTextureId);
	// Set the texture's stretching properties
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Bilinear filtering.
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);	// Sans bilinear filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Création de la texture "vide".
//	glTexImage2D(GL_TEXTURE_2D, 0, 4/*3*/, 512, 256, 0,
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 256, 0,
		GL_BGRA/*GL_BGR*/, /*GL_UNSIGNED_SHORT_5_6_5*/GL_UNSIGNED_BYTE, NULL);
//		GL_RGBA/*GL_BGR*/, /*GL_UNSIGNED_SHORT_5_6_5*/GL_UNSIGNED_BYTE, NULL);




#if defined(GL_TV2x)
//>>
u32	rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif
/*
    bmask = 0xff000000;
    gmask = 0x00ff0000;
    rmask = 0x0000ff00;
    amask = 0x000000ff;
*/
// Texture 32 bits pour effet TV avec OpenGL.
gRender.pSdlGlTextureTV = SDL_CreateRGBSurface(SDL_SWSURFACE, 8, 240*2, 32, rmask, gmask, bmask, amask);
if (gRender.pSdlGlTextureTV == NULL)
{
	fprintf(stderr, "SDL_CreateRGBSurface error.\n");
	exit(1);
}
//SDL_FillRect(gRender.pSdlGlTextureTV, NULL, SDL_MapRGBA(gRender.pSdlGlTextureTV->format, 128, 128, 128, 128));//0xFFFFFFFF);//0);
SDL_FillRect(gRender.pSdlGlTextureTV, NULL, 0);

u32	i, j, clr;
//clr = SDL_MapRGBA(gRender.pSdlGlTextureTV->format, 255, 255, 255, 128);
clr = SDL_MapRGBA(gRender.pSdlGlTextureTV->format, 0, 0, 0, 256-TV2x_FACTOR);
SDL_LockSurface(gRender.pSdlGlTextureTV);
u8	*pDst = gRender.pSdlGlTextureTV->pixels;
for (j = 0; j < 240*2; j += 2)
	for (i = 0; i < 8; i++)
		*(u32 *)(pDst + (i * 4) + (j * gRender.pSdlGlTextureTV->pitch)) = clr;
SDL_UnlockSurface(gRender.pSdlGlTextureTV);

/*
fprintf(stderr, "r=%08X / g=%08X / b=%08X / a=%08X\n"
		, gRender.pSdlGlTextureTV->format->Rmask
		, gRender.pSdlGlTextureTV->format->Gmask
		, gRender.pSdlGlTextureTV->format->Bmask
		, gRender.pSdlGlTextureTV->format->Amask);
*/

// Have OpenGL generate a texture object handle for us
glGenTextures(1, &gRender.nGlTextureTVId);
// Creation de la texture de base.
// Bind the texture object
glBindTexture(GL_TEXTURE_2D, gRender.nGlTextureTVId);
// Set the texture's stretching properties
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Bilinear filtering.
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);	// Sans bilinear filtering.
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

// Création de la texture "vide".
//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 256*2, 0,
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 8, 256*2, 0,
	GL_BGRA, GL_UNSIGNED_BYTE, NULL);

// Chargement de la texture en VRAM une fois pour toutes.
glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 8, 240*2,
	GL_BGRA, GL_UNSIGNED_BYTE, gRender.pSdlGlTextureTV->pixels);
//<<
#endif



}
else
{

	gRender.nRenderMode = e_RenderMode_Normal;
	gRender.nFullscreenMode = 0;

	gRender.pScreen2x = NULL;		// En mode 2x, ptr sur la surface écran.
	gRender.pScreenBuf2 = NULL;		// Buffer de rendu pour le jeu en mode 2x (à la place de la surface écran réelle).

	// On initialise d'abord un écran en mode e_RenderMode_Normal. Important, car on fait un CreateRGBSurface à partir de cette surface.
#if defined(RENDER_BPP)
	gVar.pScreen = VideoModeSet(SCR_Width, SCR_Height, 16, gRender.nFullscreenMode ? SDL_FULLSCREEN : 0);
#else
	gVar.pScreen = VideoModeSet(SCR_Width, SCR_Height, gRender.nFullscreenMode ? SDL_FULLSCREEN : 0);
#endif
	if (gVar.pScreen == NULL) exit(1);
	// On créé un buffer de la taille de l'écran.
	// => En mode 2x, on switche le ptr pScreen sur cette surface, les rendus du jeu se font donc dedans. Puis on fait le scale/filtre du buffer vers la vraie surface écran.
	gRender.pScreenBuf2 = SDL_CreateRGBSurface(SDL_SWSURFACE, SCR_Width, SCR_Height, 16, gVar.pScreen->format->Rmask, gVar.pScreen->format->Gmask, gVar.pScreen->format->Bmask, 0);
	if (gRender.pScreenBuf2 == NULL)
	{
		fprintf(stderr, "Render_InitVideo(): Unable to allocate SDL surface: %s\n", SDL_GetError());
		exit(1);
	}

	// Calcul de la CLUT pour le mode TV2x.
	TV2x_CalculateCLUT(gVar.pScreen);
	Colors_Precalculate();
#if defined(RENDER_BPP)
	// Calcul des CLUTs pour les mode 32 bits.
	if (gRender.nRenderBPP)
	{
/*
//> *** BPP Solution 1.
		R2x_32_CalculateCLUT();
//<
*/
//> *** BPP Solution 3.
		gVar.pScreen = VideoModeSet(SCR_Width, SCR_Height, 32, gRender.nFullscreenMode ? SDL_FULLSCREEN : 0);
		R2x_32_CalculateCLUT();
		gVar.pScreen = VideoModeSet(SCR_Width, SCR_Height, 16, gRender.nFullscreenMode ? SDL_FULLSCREEN : 0);
//<
	}
#endif

}

}

// Libère les ressources du rendu. (1 fois !).
void Render_Release(void)
{
if (gRender.nUseGL)
{
	SDL_FreeSurface(gVar.pScreen);
	SDL_FreeSurface(gRender.pSdlGlBuffer);
	// Now we can delete the OpenGL texture and close down SDL.
	glDeleteTextures(1, &gRender.nGlTextureId);
}
else
{
	SDL_FreeSurface(gRender.pScreenBuf2);
}

}



