
// Codes de contrôle.
#define	BIT31	(1 << 31)
enum
{
	e_Anm_Jump		= BIT31 | 1,	// Ptr + offset. e_Anm_Jump, offset.
	e_Anm_Goto		= BIT31 | 2,	// Initialise une autre anim. e_Anm_Goto, adresse.
	e_Anm_End		= BIT31 | 3,	// Fin d'une anim. Renvoie SPR_NoSprite, place e_AnmFlag_End, ne libère pas le slot.
	e_Anm_Kill		= BIT31 | 4,	// Fin d'une anim. Renvoie -1 et libère le slot (ex: dust).
	e_Anm_Sfx		= BIT31 | 5,	// Joue un son. e_Anm_Sfx, No sfx, Prio sfx.
	e_Anm_StepFlag	= BIT31 | 6,	// Flag pour laisser des traces de pas.
	e_Anm_RndGoto	= BIT31 | 7,	// Goto aléatoire. e_Anm_RndGoto, proba (sur 255), adr si <, adr si >.
	e_Anm_RndJump	= BIT31 | 8,	// Jump aléatoire. e_Anm_RndJump, proba (sur 255), offs si <, offs si >.
	e_Anm_SfxStop	= BIT31 | 9,	// Stoppe un son. e_Anm_SfxStop, No sfx.

};

// Clefs d'anim.	16b = Priorité (à faire) | 16b = No.
#define	ANMPRIO(x)	(x << 16)
enum
{
	e_AnmKey_Null			= 0,

	e_AnmKey_Hero_Idle		= 1,
	e_AnmKey_Hero_Run		= 2,
	e_AnmKey_Hero_Jump		= 5,
	e_AnmKey_Hero_JumpForce1	= ANMPRIO(1) + 1,
	e_AnmKey_Hero_JumpForce2	= ANMPRIO(1) + 2,
	e_AnmKey_Hero_JumpForce3	= ANMPRIO(1) + 3,
	e_AnmKey_Hero_FallFront		= ANMPRIO(1) + 4,
	e_AnmKey_Hero_FootBump		= ANMPRIO(1) + 5,
	e_AnmKey_Hero_FallBack		= ANMPRIO(1) + 6,
	e_AnmKey_Hero_LandOnBack	= ANMPRIO(2) + 1,
	e_AnmKey_Hero_Pitfall		= ANMPRIO(2) + 2,
	e_AnmKey_Hero_Flattened		= ANMPRIO(2) + 3,
	e_AnmKey_Hero_LandOnBackStop	= ANMPRIO(2) + 4,
	e_AnmKey_Hero_Death		= ANMPRIO(3) + 1,
	e_AnmKey_Hero_Victory	= ANMPRIO(4) + 1,

};


// Définition des anims.

extern u64	gAnm_Hero_Idle[];
extern u64	gAnm_Hero_Run[];
extern u64	gAnm_Hero_Jump[];
extern u64	gAnm_Hero_JumpForce1[];
extern u64	gAnm_Hero_JumpForce2[];
extern u64	gAnm_Hero_JumpForce3[];
extern u64	gAnm_Hero_LandOnBack[];
extern u64	gAnm_Hero_Flattened[];
extern u64	gAnm_Hero_Flattened2[];
extern u64	gAnm_Hero_WinBreathing[];
extern u64	gAnm_Hero_MuscleDisplay[];
extern u64	gAnm_Hero_WinJump[];
extern u64	gAnm_Hero_DeathSkeleton[];
extern u64	gAnm_Hero_Pitfall[];
extern u64	gAnm_Hero_DeathExhausted[];
extern u64	gAnm_Hero_FootBump[];
extern u64	gAnm_Hero_FallFront[];
extern u64	gAnm_Hero_FallBack[];

extern u64	gAnm_TimeSparkle[];
extern u64	gAnm_Springboard_Idle[];
extern u64	gAnm_Springboard_Boing[];
extern u64	gAnm_Cracker_Idle[];
extern u64	gAnm_Cracker_Explode[];
extern u64	gAnm_Hurdle_Idle[];
extern u64	gAnm_Hurdle_Falling[];
extern u64	gAnm_Skateboard_Idle[];
extern u64	gAnm_Skateboard_Rolling[];
extern u64	gAnm_RedCube_MoveDown[];
extern u64	gAnm_RedCube_MoveUp[];
extern u64	gAnm_TelescopicWall[];
extern u64	gAnm_TelescopicWall_Init0[];
extern u64	gAnm_TelescopicWall_Init1[];
extern u64	gAnm_TelescopicWall_Init2[];
extern u64	gAnm_ChessPiece_Normal[];
extern u64	gAnm_ChessPiece_Fall[];



