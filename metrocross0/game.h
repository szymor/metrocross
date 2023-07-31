

#define	BLK_SLOW	(2)
#define	BLK_PITFALL	(3)
#define	BLK_CRACKER	(4)

#define	HUD_TOP_HT		(16)//+8)
#define	HUD_BOTTOM_HT	(40)//-8)

#define FPS	(60)
#define TIME_1S	(1000 << 8)
#define TIME_1F	(TIME_1S / FPS)

#define	PLYR_GND_OFFS	(5)				// Offset min et max dans la bande de sol.
#define	GRAVITY	(0x54)//(0x40)

// Codes de sortie.
enum
{
	e_GameEC_Playing = 0,		// Jeu.
	e_GameEC_GameOver,			// Game over.
	e_GameEC_AllClear,			// Jeu terminé.
	e_GameEC_Aborted,			// Jeu abandonné.
};

// Phases.
enum
{
	e_Game_ReadyYourself = 0,
	e_Game_RoundInit,
	e_Game_ReadySetGo,
	e_Game_Play,
	e_Game_RoundCompleted,
};

struct SGame
{
	struct SMap	*pMap;				// Map en cours.
	struct SLoadedMst	*pMst;		// Mst en cours.
	u32	nRound;						// Level en cours.

	s32	nGndX;						// Position du sol ***sur la map***.

	s32	nPlayerPosX, nPlayerPosY, nPlayerPosZ;
	s32	nPlayerLastX, nPlayerLastY, nPlayerLastZ;
	s32	nPlayerSpdX, nPlayerSpdY, nPlayerSpdZ;
	u32	nPlayerGnd;					// Pas 0 => pas au sol.
	s32	nPlayerAnm;
	u32	nSkateboard;		// 1 = En skate.
	u32	nSpeedUp;			// Nb de frames de speed up.
	u32	nSlowDown;			// 0 => normal, 1 => slow down.
	u32	nLateJump;			// Cpt pour permettre le "late jump".

	u8	nM12SlowDown;		// Joueur accroché, il doit ralentir.
	u8	nM12Eject;			// Le joueur se débat, il éjecte les souris. (Flag pendant 1 frame).

	u8	nLRDir;
	u8	nLRTime;
	u8	nLRVar;

	s32	nTimeSparkleAnm;	// Pour étincelle qd time < 10 sec.
	s32	nTimeSparklePosX;
	s32	nTimeSparklePosY;
	s32	nTimeSparkleSide;	// Pour droite/gauche une fois sur deux.

	u32	nScore;				// Score en cours.
	u32	nHiScore;			// Hi-score pour affichage dans le HUD.

	u32	nPhase;
	s32	nPosXMax;			// X max atteint, pour limiter le recul.
	s32	nFinishLine;		// -1 quand pas initialisé.

	u32	nRoundOverTimer;	// Compteur, pour game over, pour round over...

	u32	nTimer1;			// Timer 1 (Normal).
	u32	nTimer2;			// Timer 2 (Additionnel, tous les 4 niveaux).
	u32	pnTimerR3[3];		// Temps restant des rounds 1,2,3.
	u32	nHUDTimersDisp;		// Affichage des x timers précédents dans le HUD.
	u32	nTimerStop;			// 0 = on compte / 1 = on ne compte pas.
	u32	nTimerFreeze;		// != 0 => nb de frames pendant lesquelles le compteur est bloqué (canettes).

	u32	nExitCode;			// Code de sortie.

};
extern struct SGame gGame;


void Game_Init(void);
void Game_Manage(void);

void Game_SprDisplay(u32 nSprNo, s32 nPosX, s32 nPosY, s32 nPosZ);
void Game_SprDisplayPrio(u32 nSprNo, s32 nPosX, s32 nPosY, s32 nPosZ, s32 nPrio);

void Game_Player_JumpInit(u32 nIdx);
u32 Game_Player_SkateboardSet(void);
void Game_Player_PitfallSet(void);
void Game_Player_FlattenedSet(void);
void Game_Player_SpeedUp(void);
void Game_Player_PointsBonus(u32 nPts, char *pStr);
void Game_Player_FootBumpInit(void);
void Game_Player_FallFrontInit(void);
void Game_Player_FallBackInit(void);
void Time_Freeze(void);

void HUD_Part1Disp(void);

