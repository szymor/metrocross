


// Structures.
struct SMstTb
{
	void (*pFctInit) (struct SMstCommon *pMst, u8 *pData);
	s32 (*pFctMain) (struct SMstCommon *pMst);
	u64	*pAnm;
	u16	nPoints;
};

// Liste des monstres.
enum
{
	e_Mst0_LevelData = 0,
	e_Mst1_EndOfLevel,
	e_Mst2_Springboard,
	e_Mst3_Cracker,
	e_Mst4_Pitfall,
	e_Mst5_Hurdle,
	e_Mst6_Skateboard,
	e_Mst7_SmallCan,
	e_Mst8_CokeCan,
	e_Mst9_BigTire,
	e_Mst10_RedCube,
	e_Mst11_TelescopicWall,
	e_Mst12_GreenMouse,
	e_Mst13_ChessPiece,
	e_Mst14_Sign,

	e_Mst_MAX
};




