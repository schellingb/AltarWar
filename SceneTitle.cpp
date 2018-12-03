/*
  Altar War
  Copyright (C) 2018 Bernhard Schelling

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

#include "include.h"

static ZL_Surface srfLudumDare;
static ticks_t TimeStart;
static ZL_RenderList RenderList;
static ZL_Camera Camera;
static ZL_Light Light;

struct sSceneTitle : public ZL_Scene
{
	sSceneTitle() : ZL_Scene(SCENE_TITLE) { }

	int InitTransitionEnter(ZL_SceneType, void*)
	{
		TimeStart = ZLTICKS;
		srfLudumDare = ZL_Surface("Data/ludumdare.png").SetDrawOrigin(ZL_Origin::BottomRight);
		return 400;
	}

	void InitAfterTransition()
	{
		ZL_Display::sigPointerUp.connect(this, &sSceneTitle::OnPointerUp);
		ZL_Display::sigKeyDown.connect(this, &sSceneTitle::OnKeyDown);
	}

	int DeInitTransitionLeave(ZL_SceneType SceneTypeTo)
	{
		ZL_Display::AllSigDisconnect(this);
		return 400;
	}

	void DeInitAfterTransition()
	{
		srfLudumDare = ZL_Surface();
	}

	void OnPointerUp(ZL_PointerPressEvent& e)
	{
		if (e.y < 60 && e.x < 480) ZL_Application::OpenExternalUrl("https://zillalib.github.io/");
		if (e.x >= ZLFROMW(220) && e.y >= ZLFROMH(150)) ZL_Application::OpenExternalUrl("http://www.onegameamonth.com/B_Schelling");
		if (e.y < 60 && e.x > ZLFROMW(400)) ZL_Application::OpenExternalUrl("http://www.ludumdare.com/compo/ludum-dare-27/?action=preview&uid=20775");
	}

	void OnKeyDown(ZL_KeyboardEvent& e)
	{
		if (e.key == ZLK_ESCAPE) ZL_Application::Quit();
		else if (e.key == ZLK_SPACE || e.key == ZLK_RETURN) ZL_SceneManager::GoToScene(SCENE_GAME);
	}

	void Draw()
	{
		float colf =  .7f+ssin(ZLTICKS*s(.003))*s(.2);

		#ifdef ZILLALOG
		if (ZL_Input::Held(ZLK_TAB)) { ZLTICKS = (ticks_t)(ZL_Display::PointerX*3); }
		#endif

		Light.SetDirectionalLight(4);
		Light.SetLookAt(ZLV3(-3,2,3), ZLV3(0,0,.1));
		Light.SetColor(ZLRGB(colf,.4,.4));
		Camera.SetLookAt(ZL_Vector3(ZL_Vector::FromAngle(ZLTICKS*.001f)*.44f, .42f), ZLV3(0,0,.3));
		Camera.SetAmbientLightColor(ZLRGB(colf*.5f,.2,.2));

		ZL_Display::ClearFill(ZLRGB(colf*.5f, colf* s(.4), colf*s(.3)));

		RenderList.Reset();
		RenderList.Add(MeshGround, ZL_Matrix::Identity);
		RenderList.Add(MeshAltar, ZL_Matrix::MakeTranslate(                ZLV3(.8,-.25,0)));
		RenderList.Add(MeshPlayer, ZL_Matrix::MakeRotateZ(PI).SetTranslate(ZLV3(1.2,-.25,0)));
		RenderList.Add(MeshDemon, ZL_Matrix::MakeRotateZ(.4f+ZLTICKS*.0009f));
		ZL_Display3D::DrawListWithLight(RenderList, Camera, Light);

		ZL_Display::PushMatrix();
		ZL_Display::Translate(ZLHALFW, ZLHALFH);
		ZL_Display::Rotate(scos(ZLTICKS*s(.001))*s(.02));

		int index = ((ZLTICKS-TimeStart))/100;
		float scaletick = 1.5f + (s((ZLTICKS-TimeStart)%100)/s(100)*s(-.2));
		ZL_Display::Scale(1+ssin(ZLTICKS*s(.001))*s(.02));
		ZL_Display::Translate(-ZLHALFW, -ZLHALFH);

		const char Title[] = "ALTAR WAR";
		int LetterSpace[] = { 50, 35, 50, 55, 50, 80, 60, 50, 50 };
		ZL_SeededRand rnd(ZLTICKS/375);
		float LetterX = 0;
		for (int i = 0; i != sizeof(Title)-1; LetterX+= LetterSpace[i]*1.5f, i++)
		{
			if (Title[i] == ' ') continue;
			const char Letter[] = { Title[i], '\0' };
			bool isbig = ((int)(index % sizeof(Title)) == i);
			float x = 350 + LetterX + (isbig ? rnd.Range(-15, 15)*RAND_FACTOR : rnd.Range(-10, 10));
			float y = (isbig ? rnd.Range(-15, 15)*RAND_FACTOR : rnd.Range(-10, 10));
			float scl = (isbig ? scaletick*2 : 2.0f);
			DrawTextBordered(ZLV(x, y - 10 + ZLFROMH(110)),    Letter, scl, ZLLUMA(0,0.5), ZLLUMA(0,0.5), 20);
			DrawTextBordered(ZLV(x, y - 10 + ZLFROMH(110)),    Letter, scl, ZLRGB(.9,.1,.1), ZLBLACK, 10);
		}

		ZL_Display::PopMatrix();

		ZL_Color ColText = ZLRGBA(1,.5,.5,.5), ColBorder = ZLLUMA(0,.5);
		DrawTextBordered(ZL_Vector(ZLHALFW,110), "Slay and sacrifice the demons on the altar to keep the light ignited !", 0.5f, ColText, ColBorder);
		DrawTextBordered(ZL_Vector(ZLHALFW,75), "[WASD] Move  |  [SPACE] Attack  |  [ENTER] Pick up & Sacrifice", 0.5f, ColText, ColBorder);
		DrawTextBordered(ZL_Vector(ZLHALFW,40), "PRESS [ENTER] TO BEGIN THE SLAUGHTER", 0.4f, ColText, ColBorder);
		DrawTextBordered(ZL_Vector(ZLHALFW,15), "[ALT+ENTER] Toggle Fullscreen", 0.3f, ColText, ColBorder);
		DrawTextBordered(ZL_Vector(18, 12), "(C) 2018 Bernhard Schelling", s(.6), ZLRGBA(1,.8,.2,.5), ColBorder, 2, ZL_Origin::BottomLeft);

		srfLudumDare.Draw(ZLFROMW(10), 10);
	}

	void DrawTransition(float f, bool IsLeaveTransition)
	{
		imcTheme.SetSongVolume(80-(int)(f*60));
		ZL_Scene::DrawTransition(f, IsLeaveTransition);
	}
} SceneTitle;
