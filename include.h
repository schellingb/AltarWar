/*
  Altar War
  Copyright (C) 2018-2019 Bernhard Schelling

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#ifndef _ALTARWAR_INCLUDE_
#define _ALTARWAR_INCLUDE_

#include <ZL_Application.h>
#include <ZL_Display.h>
#include <ZL_Surface.h>
#include <ZL_Audio.h>
#include <ZL_Font.h>
#include <ZL_Scene.h>
#include <ZL_Display3D.h>
#include <ZL_Input.h>
#include <ZL_SynthImc.h>
#include <vector>

extern ZL_Font Font;
extern ZL_Sound sndDeath, sndSwing, sndHit, sndSacrifice;
extern ZL_SynthImcTrack imcTheme, imcGameOver;
extern ZL_Mesh MeshAltar, MeshGround, MeshPlayer, MeshWall, MeshSword, MeshClaw, MeshDemon;
extern bool showTouchUI;

#define SCENE_TITLE 1
#define SCENE_GAME 2

static void DrawTextBordered(const ZL_Vector& p, const char* txt, scalar scale = 1, const ZL_Color& colfill = ZLWHITE, const ZL_Color& colborder = ZLBLACK, int border = 2, ZL_Origin::Type origin = ZL_Origin::Center)
{
	for (int i = 0; i < 9; i++) if (i != 4) Font.Draw(p.x+(border*((i%3)-1)), p.y+8+(border*((i/3)-1)), txt, scale, scale, colborder, origin);
	Font.Draw(p.x  , p.y+8  , txt, scale, scale, colfill, origin);
}

#endif //_ALTARWAR_INCLUDE_
