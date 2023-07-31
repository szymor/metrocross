
// Prototypes.
void Sfx_SoundInit(void);
void Sfx_SoundOn(void);
void Sfx_SoundOff(void);
void Sfx_LoadWavFiles(void);
void Sfx_FreeWavFiles(void);
void Sfx_LoadYMFiles(void);
void Sfx_FreeYMFiles(void);

void Sfx_PlaySfx(u32 nSfxNo, u32 nSfxPrio);
void Sfx_StopSfx(u32 nSfxNo);
u32 Sfx_IsPlaying(u32 nSfxNo);
void Sfx_ClearChannels(void);

void Sfx_ChannelsSave(void);
void Sfx_ChannelsRestore(void);

void Music_Start(s32 nMusicNo, u32 nForceRestart);
s32 Music_GetMusicNo(void);
u32 Music_IsOver(void);


// Enum YM.
enum
{
	e_YmMusic_NoMusic = -1,
	e_YmMusic_Musics = 0,

	e_YmMusic_GameOver = e_YmMusic_Musics,
	e_YmMusic_RoundOver,
	e_YmMusic_HighScore,
	e_YmMusic_MainTheme,
	e_YmMusic_ReadyYourself,

	e_YmMusic_MAX
};

// Enums.
enum
{
	e_Sfx_Hero_Jump = 0,
	e_Sfx_Hero_Flattened,
	e_Sfx_SpringBoard,
	e_Sfx_Cracker,
	e_Sfx_Countdown,
	e_Sfx_Go,
	e_Sfx_Can_Crush,
	e_Sfx_Can_Kick,
	e_Sfx_Hurdle_Hit,
	e_Sfx_BigTire_Bounce,

	e_Sfx_LAST
};

enum
{
	e_SfxPrio_0 = 0,
	e_SfxPrio_10 = 10,
	e_SfxPrio_20 = 20,
	e_SfxPrio_30 = 30,
	e_SfxPrio_40 = 40,
	e_SfxPrio_Max = 254,
};

