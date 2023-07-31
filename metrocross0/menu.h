

//=============================================================================

// Enum menus.
enum
{
	MENU_Null = 0,
	MENU_Main,
	MENU_Game,
	MENU_HallOfFame,
	MENU_HowToPlay,
	MENU_Quit,

};

void MenuMain_Init(void);
u32 MenuMain_Main(void);

void MenuHighScores_Init(void);
void MenuHighScores_Init2(void);
u32 MenuHighScores_Main(void);

void MenuCongratulations_Init(void);
u32 MenuCongratulations_Main(void);

//=============================================================================

void Scr_Load(void);
void Scr_Save(void);
void Scr_RazTable(void);
u32 Scr_HighScore_Get(void);
s32 Scr_CheckHighSc(u32 nScore);
s32 Scr_PutNameInTable(char *pName, u32 nScore, u32 nRound);

//=============================================================================

// Enum transit.
enum
{
	e_Transit_Off = 0,
	e_Transit_FadeIn,
	e_Transit_FadeOut,
};

void Transit_Init(u32 nOnOff);
void Transit_Manage(void);
void Transit_Off(void);

u32 Transit_IsDone(void);
u32 Transit_StateGet(void);

//=============================================================================

// Enum touches de la config.
enum
{
	e_CfgKey_Up = 0,
	e_CfgKey_Down,
	e_CfgKey_Left,
	e_CfgKey_Right,
	e_CfgKey_ButtonA,
//	e_CfgKey_ButtonB,

	e_CfgKey_LAST,

	e_CfgKey_JoyButtonA = e_CfgKey_LAST,
//	e_CfgKey_JoyButtonB,

	e_CfgKey_MAX
};

#pragma pack(1)
struct SMetroCfg
{
	u16	pKeys[e_CfgKey_MAX];
	u16	nVideoMode;			// 0 = 320x224 / 1 = x2 / 2 = TV2x.
	u16	nChecksum;
};
#pragma pack()
extern struct SMetroCfg	gMetroCfg;

u32 Cfg_Load(void);


