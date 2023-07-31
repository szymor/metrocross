

#define	RENDER_BPP	(1)
#define	GL_TV2x	(1)


enum
{
	e_RenderMode_Normal = 0,
	e_RenderMode_Scale2x,
	e_RenderMode_TV2x,
	//
	e_RenderMode_MAX
};

// Paramètres de rendu.
struct SRender
{
	// Note : Je laisse le pointeur pScreen dans gVar.
	// On fait dans tous les cas le rendu "normal" dans pScreen. Si on a un post-effect à faire, on redirige pScreen dans un buffer secondaire, qu'on recopiera avec l'effet voulu dans l'écran réel une fois le tracé du jeu fini.

	SDL_Surface *pScreen2x;		// En modes 2x, ptr sur la surface écran réelle.
	SDL_Surface *pScreenBuf2;	// Buffer de rendu pour le jeu en modes 2x (à la place de la surface écran réelle).

	u8	nRenderMode;			// Mode en cours : normal / 2x / TV2x.
	u8	nFullscreenMode;		// Fullscreen ou pas.

#if defined(RENDER_BPP)
	u8	nRenderBPP;		// 0 = 16 bits / 1 = 32 bits.
#endif

	u8	nUseGL;			// 1 = Use OpenGL / 0 = SDL rendering.
	GLuint	nGlTextureId;		// Texture object handle
	SDL_Surface	*pSdlGlBuffer;	// Frame buffer.
#if defined(GL_TV2x)
	GLuint	nGlTextureTVId;		// Id de la texture pour le filtre TV.
	SDL_Surface *pSdlGlTextureTV;	// La surface contenant le filtre.
#endif

};
extern struct SRender	gRender;


// Prototypes.
#if defined(RENDER_BPP)
SDL_Surface * VideoModeSet(u32 nScrWidth, u32 nScrHeight, u32 nBPP, u32 nSDL_Flags);
#else
SDL_Surface * VideoModeSet(u32 nScrWidth, u32 nScrHeight, u32 nSDL_Flags);
#endif
void Render_SetVideoMode(void);
void Render_InitVideo(void);
void Render_Release(void);
void Render_Flip(u32 nSync);

