
#include "includes.h"

#define FPS_Default (1000 / 60)
#define FPS_OPENGL (1000 / 66)	// Ecran à 60 Hz, mais on regarde à 66 => Il ne faut pas rater le flip !


u32 gnTimer1;
u32 gnFrame;	// Compteur général.
//#define	FPS_MissMax	(2)
//u8	gnFrameMissed;

// Init timers.
void Frame_Init(void)
{
	gnTimer1 = SDL_GetTicks();
//	gnFrameMissed = 0;
}

// Attente de la frame.
void Frame_Wait(void)
{
	u32	nTimer2;

if (gRender.nUseGL)
{

/*
	nTimer2 = SDL_GetTicks() - gnTimer1;
	if (nTimer2 + 10 <= FPS_OPENGL)
	{
		SDL_Delay(10);
	}
*/

	nTimer2 = SDL_GetTicks() - gnTimer1;
	while (nTimer2 + 10 <= FPS_OPENGL)
	{
		SDL_Delay(10);
		nTimer2 = SDL_GetTicks() - gnTimer1;
	}

/*
	while (1)
	{
		nTimer2 = SDL_GetTicks() - gnTimer1;
		if (nTimer2 + 10 <= FPS_OPENGL) break;
		SDL_Delay(10);
	}
*/

	// Note : Avec OpenGL, la reinit se fait APRES SDL_GL_SwapBuffers() dans render.c.

}
else
{


/*
	// On a loupé des frames ?
	if (gnFrameMissed == 0)
	{
		nTimer2 = SDL_GetTicks() - gnTimer1;
		if (nTimer2 >= FPS_Default) gnFrameMissed = nTimer2 / FPS_Default;
//if (gnFrameMissed) printf("Frames missed: %d / %d\n", gnFrame, gnFrameMissed);
		if (gnFrameMissed > FPS_MissMax) gnFrameMissed = FPS_MissMax;	// On ne saute pas trop de frames quand même.
		if (gnFrameMissed) goto _FrameEnd;
	}
	else
	{
		gnFrameMissed--;
		goto _FrameEnd;
	}
*/

//nTimer2 = SDL_GetTicks() - gnTimer1;
//if (nTimer2 >= FPS_Default) printf("missed: %d\n", nTimer2 / FPS_Default);

	// On s'assure qu'on ne va pas trop vite...
//if (gVar.pKeys[SDLK_y]){
/*
	while (1)
	{
		nTimer2 = SDL_GetTicks() - gnTimer1;
		if (nTimer2 >= FPS_Default) break;
		SDL_Delay(3);
	}
*/
//}else{
/*
*/
	nTimer2 = SDL_GetTicks() - gnTimer1;
	if (nTimer2 < FPS_Default) SDL_Delay(FPS_Default - nTimer2);
//}
//_FrameEnd:
	gnTimer1 = SDL_GetTicks();

}

	gnFrame++;

}

