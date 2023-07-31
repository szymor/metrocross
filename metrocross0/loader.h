

struct SMap
{
	u32	nMapLg;
	u32	nMapHt;

	u32	nPlanesNb;

	u8	*pMapBlocks;		// Lg x Ht.

/*
	SDL_Surface	*ppPlanesGfx[MAP_PLANES_MAX];	// Graphs des plans.
	s32	*ppPlanesBlocks[MAP_PLANES_MAX];		// Les plans (n° de blocs).
	s32	pPlanesLg[MAP_PLANES_MAX];		// Largeur et hauteur de chaque plan en blocs 16, dans une surface de nMapLg * nMapHt.
	s32	pPlanesHt[MAP_PLANES_MAX];
	u32	nHeroPlane;						// Plan du héros, servira de référence pour le scroll différentiel.

	//u8	*ppColCodes[MAP_PLANES_MAX];	// Les codes de collision.
	struct SBlockCol	*ppColCodes[MAP_PLANES_MAX];	// Les codes de collision.

	struct SPathBlock	*pPath;
	struct SPathBlock	*pPathGnd;	// Ptr dans pPath pour les blocs au sol.
	u32	nPathGndNb;					// Nb de blocs de path.
	struct SPathBlock	*pPathAir;	// Ptr dans pPath pour les blocs en l'air.
	u32	nPathAirNb;					// Nb de blocs de path.

	u32	nPlayerStartPosX, nPlayerStartPosY;

	u32	nTranspColorKey;			// Valeur à passer à SDL_SetColorKey() pour la transparence des plans.

	u8	*pBlkAnmMem;	// Bloc mémoire pour les 'plans' d'anims de blocs.
	u8	*ppBlkAnmPlanes[MAP_PLANES_MAX];	// Les plans d'anim de blocs (les pointeurs vont pointer dans pBlkAnmMem).
*/
};

//extern	struct SMap	gMap;
/*
extern	struct SLoadedMst	gLoadedMst;

void LevelLoad(u32 nLevelNo);
void LevelRelease(void);

*/

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

enum
{
	e_MstState_Asleep = 0,		// Le monstre est endormi, il attend d'être réveillé par le scroll.
	e_MstState_Managed,			// Le monstre est géré (il est dans le jeu).
	e_MstState_Dead,			// Le monstre est mort.

};

//=============================================================================

struct SGfxBlk
{
	SDL_Surface	*pBlocks;
	SDL_Surface	*pBackground;

};
extern struct SGfxBlk	gGfxBlk;


void Loader_BlocksLoad(char *pFilename, SDL_Surface **ppSurf);
void Loader_BlocksRelease(SDL_Surface **ppSurf);

void Loader_LevelsLoad(void);
void Loader_LevelsRelease(void);

void Loader_MapPtr_Set(u32 nLevelNo, struct SMap **ppMap, struct SLoadedMst **ppMst);


