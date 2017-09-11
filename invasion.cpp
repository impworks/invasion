#include <math.h>
#include <sys/stat.h>
#include <errno.h>

#include <hge.h>
#include <hgesprite.h>
#include <hgeparticle.h>
#include <hgefont.h>

HGE *hge = 0;

#include "predefine.h"
#include "list.h"
#include "core.h"

CORE *core = 0;

#include "sprites.h"
#include "threads.h"

bool FrameFuncLost()
{
    if(hge->Input_KeyDown(HGEK_ESCAPE)) return true;

    if(hge->Input_KeyDown(HGEK_ENTER) || hge->Input_KeyDown(HGEK_LBUTTON))
    {
        core->init();
        core->fnt->SetColor(0xFFFFFFFF);
        hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
        hge->System_SetState(HGE_RENDERFUNC, RenderFunc);
    }

    return false;
}

bool RenderFuncLost()
{
    hge->Gfx_BeginScene();
	core->sky->Render(0, 0);
    core->fnt->printf(400, 250, HGETEXT_CENTER, "You have lost!\n\nYour score was %i points.\n\nBetter luck next time!", core->score);
    hge->Gfx_EndScene();
    return false;
}

bool FrameFunc()
{
	// quit if Escape pressed
	if(hge->Input_KeyDown(HGEK_ESCAPE)) return true;

    if(hge->Input_KeyDown(HGEK_P))
        core->pause = core->pause ? false : true;

    if(hge->Input_KeyDown(HGEK_S))
        if(hge->Channel_IsPlaying(core->channel))
            hge->Channel_Pause(core->channel);
        else
            hge->Channel_Resume(core->channel);

	if(!core->pause)
	{
	    // rotate turret
		hge->Input_GetMousePos(&(core->mouseX), &(core->mouseY));
		core->rotate_turret();

		// fire rocket?
		if(hge->Input_KeyDown(HGEK_LBUTTON))
			core->fire_rocket();

		// create bombs
		core->bombTimer -= hge->Timer_GetDelta();
		if(core->bombTimer <= 0)
		{
			core->drop_bomb();
			core->bombTimer = core->bombInterval;
		}

		// create nukes
		core->nukeTimer -= hge->Timer_GetDelta();
		if(core->nukeTimer <= 0)
		{
			core->drop_nuke();
			core->nukeTimer = 15;
		}
		
		// speed up bombs
		core->bombSpeedup -= hge->Timer_GetDelta();
		if(core->bombSpeedup <=0)
		{
			core->bombSpeedup = 4;
			if(core->bombInterval > 0.8) core->bombInterval -= 0.075;
		}

		core->explode_bombs();

		if(hge->Timer_GetTime() > 20 && (char)(core->starcolor) < 0x70) core->starcolor+=0.1;

		if(core->collect_garbage())
		{
			// convert sky
			core->sky->SetColor(0x0FFFFFFF, 0);
			core->sky->SetColor(0x0FFFFFFF, 1);
			core->sky->SetColor(0x15FFFFFF, 2);
			core->sky->SetColor(0x15FFFFFF, 3);

			core->fnt->SetColor(0xFF000000);

			hge->System_SetState(HGE_FRAMEFUNC, FrameFuncLost);
			hge->System_SetState(HGE_RENDERFUNC, RenderFuncLost);
		}

		// debug info
		if(hge->Input_KeyDown(HGEK_CTRL))
			core->debug = (core->debug ? false : true);
	}

	return false;
}

bool RenderFunc()
{
    hge->Gfx_BeginScene();

	if(!core->pause)
	{
		// render BG
		core->repaint_sky();
		core->sky->Render(0, 0);
		for(int idx=0; idx<100; idx++)
		{
			core->stars[idx].y+=0.1;
            core->stars[idx].x+=hge->Random_Float(-0.2, 0.2);
			if(core->stars[idx].y > 530)
			{
				core->stars[idx].x = hge->Random_Int(0, 800);
				core->stars[idx].y = -10;
			}
			hge->Gfx_RenderLine(core->stars[idx].x, core->stars[idx].y, core->stars[idx].x+1, core->stars[idx].y, ARGB((char)core->starcolor, 0xFF, 0xFF, 0xFF));
		}

		// render objects
		core->rockets->render();
		core->bombs->render();
		core->nukes->render();

		// render gun
		core->gun->RenderEx(398, 420, core->turretAngle, 1);

			// signs
		core->sign->RenderEx(64, 470, -0.1, 0.7);
		core->sign->RenderEx(264, 485, 0.1, 0.3);
		core->sign->RenderEx(736, 460, 0.25, 0.8);

		// render FG
		core->turret->Render(336, 400);
		core->ground->Render(0, 500);

		// crosshair
		core->crosshair->Render(core->mouseX, core->mouseY);

		if(hge->Timer_GetTime() < 3)
        {
			core->fnt->printf(400, 250, HGETEXT_CENTER, "Destroy all the bombs before they hit the ground!");
        }
		else if(hge->Timer_GetTime() < 4)
        {
            if(hge->Timer_GetTime() > 3.5)
            {
                core->textalpha-=5;
                core->fnt->SetColor(ARGB((char)core->textalpha, 0xFF, 0xFF, 0xFF));
            }
			core->fnt->printf(400, 250, HGETEXT_CENTER, "HERE THEY COME!");
        }

        if(hge->Timer_GetTime() < 5) core->fnt->SetColor(0xE0FFFFFF);
		core->fnt->printf(5, 5, HGETEXT_LEFT, "Score: %i points", core->score);

		if(hge->Input_GetKeyState(HGEK_C)) core->fnt->printf(795, 5, HGETEXT_RIGHT, "Made by Imp, 2008\nGreetings to natasha,\nbr, blew, demix, zabava\n\nПЫЩЬ ПЫЩЬ!!!!1\n\nА ткач не ня :]");
		if(core->debug) core->debug_render();
	}
	else
	{
		core->fnt->printf(400, 300, HGETEXT_CENTER, "Paused");
	}

    hge->Gfx_EndScene();
	return false;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    HANDLE exists = CreateMutex(NULL, false, TEXT("invasion1337"));
    if(GetLastError() == ERROR_ALREADY_EXISTS)
    {
        MessageBox(NULL, "Don't run me twice!", "Error", MB_OK | MB_ICONERROR | MB_APPLMODAL);
        return 0;
    }

    CreateMutex(NULL, false, TEXT("invasion1337"));
    hge = hgeCreate(HGE_VERSION);
    core = new CORE();

    hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
    hge->System_SetState(HGE_RENDERFUNC, RenderFunc);
    hge->System_SetState(HGE_USESOUND, true);

    hge->System_SetState(HGE_SHOWSPLASH, false);
    hge->System_SetState(HGE_FPS, 100);
    hge->System_SetState(HGE_WINDOWED, true);
    hge->System_SetState(HGE_SCREENWIDTH, 800);
    hge->System_SetState(HGE_SCREENHEIGHT, 600);
    hge->System_SetState(HGE_SCREENBPP, 32);
    hge->System_SetState(HGE_TITLE, "Invasion");

    if(hge->System_Initiate())
    {
        if(core->load_data())
            hge->System_Start();
        else
            MessageBox(NULL, "Game data files are missing!", "Error", MB_OK | MB_ICONERROR | MB_APPLMODAL);
    }
    else
        MessageBox(NULL, hge->System_GetErrorMessage(), "Error", MB_OK | MB_ICONERROR | MB_APPLMODAL);

    hge->System_Shutdown();
    hge->Release();

    return 0;
}
