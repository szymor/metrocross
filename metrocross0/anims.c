
#include "includes.h"

// 1 u32/u64 : 1 clef.
// Ensuite :
// 1 u32/u64 : Nb de frames d'affichage
//  Si b31, code de contrôle : Voir liste en haut de anims.h.
// 1 u32/u64 : N° du sprite.


// Macro pour le clignotement de mort.
#define	ANIM_CLIGNO(sprno)	\
6, SPR_NoSprite,	\
6, sprno,	\
6, SPR_NoSprite,	\
6, sprno,	\
6, SPR_NoSprite,	\
6, sprno,	\
6, SPR_NoSprite,	\
6, sprno,

// Macro pour jouer un son ou l'autre.
#define	ANIM_SFXRND(proba, sfx1no, sfx1prio, sfx2no, sfx2prio)	\
e_Anm_RndJump, proba, 4+0, 4+5,	\
e_Anm_Sfx, sfx1no, sfx1prio,	\
e_Anm_Jump, 5,					\
e_Anm_Sfx, sfx2no, sfx2prio,




// Héros - Idle.
u64	gAnm_Hero_Idle[] =
{
e_AnmKey_Hero_Idle,
128, e_Spr_Hero_Idle,
e_Anm_Jump, (u64)-2
};

// Héros - Run.
u64	gAnm_Hero_Run[] =
{
e_AnmKey_Hero_Run,
9, e_Spr_Hero_Run,
9, e_Spr_Hero_Run + 1,
9, e_Spr_Hero_Run + 2,
9, e_Spr_Hero_Run + 3,
9, e_Spr_Hero_Run + 4,
9, e_Spr_Hero_Run + 5,
e_Anm_Goto, (u64)gAnm_Hero_Run
};

// Héros - Jump.
u64	gAnm_Hero_Jump[] =
{
e_AnmKey_Hero_Jump,
e_Anm_Sfx, e_Sfx_Hero_Jump, e_SfxPrio_10,
128, e_Spr_Hero_Jump,
e_Anm_Jump, (u64)-2
};

// Héros - Jump forcé 1 (springboard, no jump => pirouette simple, pas de gain de temps).
u64	gAnm_Hero_JumpForce1[] =
{
e_AnmKey_Hero_JumpForce1,
4, e_Spr_Hero_Fly1,
4, e_Spr_Hero_Fly1 + 1,
4, e_Spr_Hero_Fly1 + 2,
4, e_Spr_Hero_Fly1 + 3,
e_Anm_Jump, (u64)-8
};

// Héros - Jump forcé 2 (cracker, no jump, land on back).
u64	gAnm_Hero_JumpForce2[] =
{
e_AnmKey_Hero_JumpForce2,
4, e_Spr_Hero_Fly1,
4, e_Spr_Hero_Fly1 + 1,
4, e_Spr_Hero_Fly1 + 2,
4, e_Spr_Hero_Fly1 + 3,
e_Anm_Jump, (u64)-8
};

// Héros - Jump forcé 3 (springboard or cracker + jump => super saut).
u64	gAnm_Hero_JumpForce3[] =
{
e_AnmKey_Hero_JumpForce3,
4, e_Spr_Hero_Fly1,
4, e_Spr_Hero_Fly1 + 1,
4, e_Spr_Hero_Fly1 + 2,
4, e_Spr_Hero_Fly1 + 3,
e_Anm_Jump, (u64)-8
};

// Héros - Atterrissage sur le dos.
u64	gAnm_Hero_LandOnBack[] =
{
e_AnmKey_Hero_LandOnBack,
60, e_Spr_Hero_FallBack + 2,
e_Anm_Goto, (u64)gAnm_Hero_Idle
};

// Héros - Ecrasement par grosse cannette de coca, gros pneu...
u64	gAnm_Hero_Flattened2[] =
{
e_AnmKey_Hero_Flattened,
e_Anm_Sfx, e_Sfx_Hero_Flattened, e_SfxPrio_0,
60, e_Spr_Hero_Flattened,
e_Anm_Goto, (u64)gAnm_Hero_Idle
};
u64	gAnm_Hero_Flattened[] =
{
e_AnmKey_Hero_Flattened,
1, e_Spr_Hero_FallBack,
1, e_Spr_Hero_FallBack + 1,
e_Anm_Goto, (u64)gAnm_Hero_Flattened2
};

// Héros - Win breathing.
u64	gAnm_Hero_WinBreathing[] =
{
e_AnmKey_Hero_Victory,
16, e_Spr_Hero_Win_Breathing,
16, e_Spr_Hero_Win_Breathing + 1,
e_Anm_Jump, (u64)-4
};

// Héros - Win breathing.
u64	gAnm_Hero_MuscleDisplay[] =
{
e_AnmKey_Hero_Victory,
128, e_Spr_Hero_Win_MuscleDisplay,
e_Anm_Jump, (u64)-2
};

// Héros - Win saut périlleux.
u64	gAnm_Hero_WinJump[] =
{
e_AnmKey_Hero_Victory,
4, e_Spr_Hero_Fly1,
4, e_Spr_Hero_Fly1 + 1,
4, e_Spr_Hero_Fly1 + 2,
4, e_Spr_Hero_Fly1 + 3,
e_Anm_Jump, (u64)-8
};

// Héros - Death skeleton.
u64	gAnm_Hero_DeathSkeleton[] =
{
e_AnmKey_Hero_Death,
16, e_Spr_Hero_Death_Skeleton,
16, e_Spr_Hero_Death_Skeleton + 1,
e_Anm_Jump, (u64)-4
};
// Héros - Death exhausted (après skeleton).
u64	gAnm_Hero_DeathExhausted[] =
{
e_AnmKey_Hero_Death,
16, e_Spr_Hero_Death_Exhausted,
16, e_Spr_Hero_Death_Exhausted + 1,
e_Anm_Jump, (u64)-4
};

// Héros - Pitfall.
u64	gAnm_Hero_Pitfall[] =
{
e_AnmKey_Hero_Pitfall,
e_Anm_Sfx, e_Sfx_Cracker, e_SfxPrio_0,
1, e_Spr_Hero_Pitfall + 0,		1, e_Spr_Hero_Pitfall + 1,		1, e_Spr_Hero_Pitfall + 2,
1, e_Spr_Hero_Pitfall + 3,		1, e_Spr_Hero_Pitfall + 4,		1, e_Spr_Hero_Pitfall + 5,
1, e_Spr_Hero_Pitfall + 6,		1, e_Spr_Hero_Pitfall + 7,		1, e_Spr_Hero_Pitfall + 8,
1, e_Spr_Hero_Pitfall + 9,
1, e_Spr_Hero_Pitfall + 10,		1, e_Spr_Hero_Pitfall + 11,		1, e_Spr_Hero_Pitfall + 12,
1, e_Spr_Hero_Pitfall + 13,		1, e_Spr_Hero_Pitfall + 14,		1, e_Spr_Hero_Pitfall + 15,
1, e_Spr_Hero_Pitfall + 16,		1, e_Spr_Hero_Pitfall + 17,		1, e_Spr_Hero_Pitfall + 18,
1, e_Spr_Hero_Pitfall + 19,
1, e_Spr_Hero_Pitfall + 20,		1, e_Spr_Hero_Pitfall + 21,		1, e_Spr_Hero_Pitfall + 22,
1, e_Spr_Hero_Pitfall + 23,		1, e_Spr_Hero_Pitfall + 24,		1, e_Spr_Hero_Pitfall + 25,
1, e_Spr_Hero_Pitfall + 26,		1, e_Spr_Hero_Pitfall + 27,		1, e_Spr_Hero_Pitfall + 28,
1, e_Spr_Hero_Pitfall + 29,
1, e_Spr_Hero_Pitfall + 30,		1, e_Spr_Hero_Pitfall + 31,
32, SPR_NoSprite,
1, e_Spr_Hero_Pitfall + 31,		1, e_Spr_Hero_Pitfall + 30,
1, e_Spr_Hero_Pitfall + 29,		1, e_Spr_Hero_Pitfall + 28,		1, e_Spr_Hero_Pitfall + 27,
1, e_Spr_Hero_Pitfall + 26,		1, e_Spr_Hero_Pitfall + 25,		1, e_Spr_Hero_Pitfall + 24,
1, e_Spr_Hero_Pitfall + 23,		1, e_Spr_Hero_Pitfall + 22,		1, e_Spr_Hero_Pitfall + 21,
1, e_Spr_Hero_Pitfall + 20,
1, e_Spr_Hero_Pitfall + 19,		1, e_Spr_Hero_Pitfall + 18,		1, e_Spr_Hero_Pitfall + 17,
1, e_Spr_Hero_Pitfall + 16,		1, e_Spr_Hero_Pitfall + 15,		1, e_Spr_Hero_Pitfall + 14,
1, e_Spr_Hero_Pitfall + 13,		1, e_Spr_Hero_Pitfall + 12,		1, e_Spr_Hero_Pitfall + 11,
1, e_Spr_Hero_Pitfall + 10,
1, e_Spr_Hero_Pitfall + 9,		1, e_Spr_Hero_Pitfall + 8,		1, e_Spr_Hero_Pitfall + 7,
1, e_Spr_Hero_Pitfall + 6,		1, e_Spr_Hero_Pitfall + 5,		1, e_Spr_Hero_Pitfall + 4,
1, e_Spr_Hero_Pitfall + 3,		1, e_Spr_Hero_Pitfall + 2,		1, e_Spr_Hero_Pitfall + 1,
//1, e_Spr_Hero_Pitfall + 0,
e_Anm_StepFlag,
1, e_Spr_Hero_Pitfall + 0,
//e_Anm_Jump, (u64)-3
e_Anm_Goto, (u64)gAnm_Hero_Idle
};

// Héros - Prise de pieds dans un obstacle.
u64	gAnm_Hero_FootBump[] =
{
e_AnmKey_Hero_FootBump,
10, e_Spr_Hero_FallFront + 0,
10, e_Spr_Hero_FallFront + 1,
64, e_Spr_Hero_FallFront + 2,
e_Anm_Jump, (u64)-2
};

// Héros - Chute avant (roulé boulé). Fin.
u64	gAnm_Hero_FallFront_End[] =
{
e_AnmKey_Hero_LandOnBackStop,		// Clef pour freiner.
60, e_Spr_Hero_FallBack + 2,
e_Anm_Goto, (u64)gAnm_Hero_Idle
};
// Héros - Chute avant (roulé boulé).
u64	gAnm_Hero_FallFront[] =
{
e_AnmKey_Hero_FallFront,
8, e_Spr_Hero_Fly1 + 0,
8, e_Spr_Hero_Fly1 + 1,
8, e_Spr_Hero_Fly1 + 2,
e_Anm_Goto, (u64)gAnm_Hero_FallFront_End
};

// Héros - Chute arrière.
u64	gAnm_Hero_FallBack[] =
{
e_AnmKey_Hero_FallBack,
8, e_Spr_Hero_FallBack + 0,
8, e_Spr_Hero_FallBack + 1,
8, e_Spr_Hero_FallBack + 2,
e_Anm_Goto, (u64)gAnm_Hero_FallFront_End	//gAnm_Hero_LandOnBack
};


// Etincelle pour temps < 10s.
u64	gAnm_TimeSparkle[] =
{
e_AnmKey_Null,
2, e_Spr_Hero_TimeSparkle,
2, e_Spr_Hero_TimeSparkle + 1,
2, e_Spr_Hero_TimeSparkle,
e_Anm_End
};

// Springboard - Idle.
u64	gAnm_Springboard_Idle[] =
{
e_AnmKey_Null,
128, e_Spr_Springboard,
e_Anm_Jump, (u64)-2
};

// Springboard - Boing.
u64	gAnm_Springboard_Boing[] =
{
e_AnmKey_Null,
e_Anm_Sfx, e_Sfx_SpringBoard, e_SfxPrio_0,
3, e_Spr_Springboard + 1,
3, e_Spr_Springboard + 2,
3, e_Spr_Springboard + 1,
e_Anm_Goto, (u64)gAnm_Springboard_Idle
};

// Cracker - Idle.
u64	gAnm_Cracker_Idle[] =
{
e_AnmKey_Null,
128, SPR_NoSprite,
e_Anm_Jump, (u64)-2
};

// Cracker - Explode.
u64	gAnm_Cracker_Explode[] =
{
e_AnmKey_Null,
e_Anm_Sfx, e_Sfx_Cracker, e_SfxPrio_0,
8, e_Spr_CrackerExplo,
8, e_Spr_CrackerExplo + 1,
8, e_Spr_CrackerExplo,
8, e_Spr_CrackerExplo + 1,
8, e_Spr_CrackerExplo,
8, e_Spr_CrackerExplo + 1,
8, e_Spr_CrackerExplo,
8, e_Spr_CrackerExplo + 1,
8, e_Spr_CrackerExplo,
e_Anm_Goto, (u64)gAnm_Cracker_Idle
};

// Hurdle - Idle.
u64	gAnm_Hurdle_Idle[] =
{
e_AnmKey_Null,
128, e_Spr_Hurdle,
e_Anm_Jump, (u64)-2
};

// Hurdle - Falling.
u64	gAnm_Hurdle_Falling[] =
{
e_AnmKey_Null,
e_Anm_Sfx, e_Sfx_Hurdle_Hit, e_SfxPrio_0,
8, e_Spr_Hurdle + 1,
128, e_Spr_Hurdle + 2,
e_Anm_Jump, (u64)-2
};

// Skateboard - Idle.
u64	gAnm_Skateboard_Idle[] =
{
e_AnmKey_Null,
128, e_Spr_Skateboard,
e_Anm_Jump, (u64)-2
};

// Skateboard - Rolling.
u64	gAnm_Skateboard_Rolling[] =
{
e_AnmKey_Null,
9, e_Spr_Skateboard,
9, e_Spr_Skateboard + 1,
e_Anm_Jump, (u64)-4
};

#define	ANM_REDCUBE_SPD1	(5)
// Red cube - Move down.
u64	gAnm_RedCube_MoveDown[] =
{
e_AnmKey_Null,
ANM_REDCUBE_SPD1, e_Spr_RedCube,
ANM_REDCUBE_SPD1, e_Spr_RedCube + 1,
ANM_REDCUBE_SPD1, e_Spr_RedCube + 2,
ANM_REDCUBE_SPD1, e_Spr_RedCube + 3,
e_Anm_StepFlag,
e_Anm_Jump, (u64)-9
};

// Red cube - Move up.
u64	gAnm_RedCube_MoveUp[] =
{
e_AnmKey_Null,
ANM_REDCUBE_SPD1, e_Spr_RedCube + 4,
ANM_REDCUBE_SPD1, e_Spr_RedCube + 5,
ANM_REDCUBE_SPD1, e_Spr_RedCube + 6,
ANM_REDCUBE_SPD1, e_Spr_RedCube + 7,
e_Anm_StepFlag,
e_Anm_Jump, (u64)-9
};

// Telescopic wall.
u64	gAnm_TelescopicWall[] =
{
e_AnmKey_Null,
14, 0,
4, 1,
4, 2,
4, 3,
4, 4,
4, 5,
4, 6,
4, 7,
4, 8,
4, 7,
4, 6,
4, 5,
4, 4,
4, 3,
4, 2,
4, 1,
e_Anm_Goto, (u64)gAnm_TelescopicWall
};
// Telescopic wall - Init 0.
u64	gAnm_TelescopicWall_Init0[] =
{
e_AnmKey_Null,
4, 8,
4, 7,
4, 6,
4, 5,
4, 4,
4, 3,
4, 2,
4, 1,
e_Anm_Goto, (u64)gAnm_TelescopicWall
};
// Telescopic wall - Init 1.
u64	gAnm_TelescopicWall_Init1[] =
{
e_AnmKey_Null,
4, 4,
4, 5,
4, 6,
4, 7,
4, 8,
4, 7,
4, 6,
4, 5,
4, 4,
4, 3,
4, 2,
4, 1,
e_Anm_Goto, (u64)gAnm_TelescopicWall
};
// Telescopic wall - Init 2.
u64	gAnm_TelescopicWall_Init2[] =
{
e_AnmKey_Null,
4, 4,
4, 3,
4, 2,
4, 1,
e_Anm_Goto, (u64)gAnm_TelescopicWall
};

// Chess piece - Normal.
u64	gAnm_ChessPiece_Normal[] =
{
e_AnmKey_Null,
128, 0,
e_Anm_Jump, (u64)-2
};
// Chess piece - Fall.
u64	gAnm_ChessPiece_Fall[] =
{
e_AnmKey_Null,
4, 1,
4, 2,
128, 3,
e_Anm_Jump, (u64)-2
};




