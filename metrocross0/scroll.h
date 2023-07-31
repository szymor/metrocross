
#define	SCROLL_TILE_W	(32)
#define	SCROLL_TILE_H	(16)

#define	SCROLL_GND_OFFS	(40)

#define	LVL_NB	(32)			// Nombre de niveaux.

#define	BKG_HT	(72)			// Hauteur de la bande de décor.
#define	BKG_Y_ORG	(16)//+8)	// Premier Y d'affichage.

#define	GND_BLK_HT	(5)			// Hauteur du sol : 5 lignes de 16 pixels.
#define	GND_Y_ORG	(BKG_Y_ORG + BKG_HT)						// Y min du sol.
#define	GND_Y_MAX	(GND_Y_ORG + (GND_BLK_HT * SCROLL_TILE_H))	// Y max du sol.

#define	BKG2_HT	(16)			// Hauteur de la bande de décor sous le sol.
#define	BKG2_Y_ORG	(GND_Y_ORG + (GND_BLK_HT * SCROLL_TILE_H))	// Y d'affichage.


void Scroll_Allocate(void);
void Scroll_Release(void);

void Scroll_Init(void);
void Scroll_Display(void);
void Scroll_Manage(void);

void Scroll_BkgNo_Set(u32 nBackgroundNo);
void Scroll_BottomDisplay(void);
void Scroll_SetMaxPos(s32 nPos);


