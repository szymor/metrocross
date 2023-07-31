
// Define priorités.
enum
{
	e_Prio_EnnemiesBg = 16,		// Plateformes, etc... derrière le reste
	e_Prio_DustUnder = 24,
	e_Prio_Ennemies = 32,		// 32 valeurs.
	e_Prio_DustOver = e_Prio_Ennemies + 32 + 16,
	e_Prio_Tirs = 140,
	e_Prio_Joueur = 192,

	e_Prio_HUD = 250,

};

// Define sprites.
enum
{
	e_Spr_FontSmall = 0,

	e_Spr_Hero_Idle = e_Spr_FontSmall + 95+2,
	e_Spr_Hero_Shadows = e_Spr_Hero_Idle + 1,
	e_Spr_Hero_HUDRun = e_Spr_Hero_Shadows + 3,
	e_Spr_Hero_TimeSparkle = e_Spr_Hero_HUDRun + 3,
	e_Spr_Hero_Run = e_Spr_Hero_TimeSparkle + 2,
	e_Spr_Hero_Jump = e_Spr_Hero_Run + 6,
	e_Spr_Hero_Skateboarding = e_Spr_Hero_Jump + 1,
	e_Spr_Hero_Fly1 = e_Spr_Hero_Skateboarding + 2,
	e_Spr_Hero_FallFront = e_Spr_Hero_Fly1 + 4,
	e_Spr_Hero_Death_Skeleton = e_Spr_Hero_FallFront + 3,
	e_Spr_Hero_Death_Exhausted = e_Spr_Hero_Death_Skeleton + 2,
	e_Spr_Hero_Flattened = e_Spr_Hero_Death_Exhausted + 2,
	e_Spr_Hero_FallBack = e_Spr_Hero_Flattened + 1,
	e_Spr_Hero_Win_Breathing = e_Spr_Hero_FallBack + 3,
	e_Spr_Hero_Win_MuscleDisplay = e_Spr_Hero_Win_Breathing + 2,
	e_Spr_Hero_Pitfall = e_Spr_Hero_Win_MuscleDisplay + 1,

	e_Spr_Springboard = e_Spr_Hero_Pitfall + 32,
	e_Spr_CrackerExplo = e_Spr_Springboard + 3,
	e_Spr_CokeCan = e_Spr_CrackerExplo + 2,
	e_Spr_Hurdle = e_Spr_CokeCan + 8,
	e_Spr_EndPortal = e_Spr_Hurdle + 3,
	e_Spr_EndPortal_Lights = e_Spr_EndPortal + 4,
	e_Spr_Skateboard = e_Spr_EndPortal_Lights + 4,
	e_Spr_GreenMouse = e_Spr_Skateboard + 2,
	e_Spr_BigTire = e_Spr_GreenMouse + 2,
	e_Spr_BlueCan = e_Spr_BigTire + 2,
	e_Spr_GreenCan = e_Spr_BlueCan + 4,
	e_Spr_Pannel_10M = e_Spr_GreenCan + 4,
	e_Spr_Pannel_CokeCan,
	e_Spr_Pannel_Attention,
	e_Spr_Pannel_RoadSmaller1,
	e_Spr_Pannel_RoadSmaller2,
	e_Spr_Pannel_GreenMouse,
	e_Spr_RedCube,
	e_Spr_TelescopicWall = e_Spr_RedCube + 8,
	e_Spr_ChessKing = e_Spr_TelescopicWall + 9,
	e_Spr_ChessKnight = e_Spr_ChessKing + 4,

	e_Spr_Tstrct_Cross = e_Spr_ChessKnight + 4,
	e_Spr_Tstrct_CornerUL,
	e_Spr_Tstrct_CornerUR,
	e_Spr_Tstrct_CornerDL,
	e_Spr_Tstrct_CornerDR,

	e_Spr_NEXT = e_Spr_Tstrct_CornerDR + 1,

};

