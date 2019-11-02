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

#include "include.h"

ZL_Font Font;
#ifdef __SMARTPHONE__
bool showTouchUI = true;
#else
bool showTouchUI = false;
#endif

static struct sAltarWar : public ZL_Application
{
	sAltarWar() : ZL_Application() { }

	virtual void Load(int argc, char *argv[])
	{
		if (!ZL_Application::LoadReleaseDesktopDataBundle()) return;
		if (!ZL_Display::Init("Altar War", 1280, 720, ZL_DISPLAY_ALLOWRESIZEHORIZONTAL | ZL_DISPLAY_DEPTHBUFFER)) return;
		ZL_Display::ClearFill(ZL_Color::White);
		ZL_Display::SetAA(true);
		ZL_Audio::Init();
		ZL_Input::Init();
		ZL_Display3D::Init();
		ZL_Display3D::InitShadowMapping();
		Font = ZL_Font("Data/typomoderno.ttf.zip", 64);
		imcTheme.SetSongVolume(20);
		imcTheme.Play(true);
		ZL_SceneManager::Init(SCENE_TITLE);
		//ZL_SceneManager::Init(SCENE_GAME);
	}

	virtual void BeforeFrame()
	{
		if (showTouchUI && ZL_Input::KeyDownCount()) showTouchUI = false;
		else if (!showTouchUI && ZL_Input::Down(ZL_BUTTON_LEFT)) showTouchUI = true;
	}
} AltarWar;

//--------------------------------------------------------------------------------------------------------------------------------------------------------

static const unsigned int IMCDEATH_OrderTable[] = { 0x000000001, };
static const unsigned char IMCDEATH_PatternData[] = { 0x50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
static const unsigned char IMCDEATH_PatternLookupTable[] = { 0, 1, 1, 1, 1, 1, 1, 1, };
static const TImcSongEnvelope IMCDEATH_EnvList[] = {
	{ 0, 256, 21, 8, 16, 255, true, 255, },
	{ 0, 256, 21, 8, 9, 255, true, 255, },
	{ 0, 256, 29, 8, 255, 255, true, 255, },
	{ 0, 256, 56, 8, 16, 255, true, 255, },
	{ 0, 256, 13, 0, 24, 255, true, 255, },
	{ 0, 256, 697, 8, 255, 255, true, 255, },
};
static TImcSongEnvelopeCounter IMCDEATH_EnvCounterList[] = {
	{ 0, 0, 256 }, { -1, -1, 256 }, { 1, 0, 256 }, { 2, 0, 256 },
	{ 3, 0, 256 }, { 4, 0, 128 }, { 5, 0, 256 },
};
static const TImcSongOscillator IMCDEATH_OscillatorList[] = {
	{ 6, 15, IMCSONGOSCTYPE_SQUARE, 0, -1, 82, 1, 2 },
	{ 6, 66, IMCSONGOSCTYPE_SINE, 0, -1, 252, 1, 4 },
	{ 6, 31, IMCSONGOSCTYPE_NOISE, 0, 0, 22, 1, 3 },
	{ 12, 150, IMCSONGOSCTYPE_NOISE, 0, 1, 72, 5, 6 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 1, -1, 100, 0, 0 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 2, -1, 100, 0, 0 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 3, -1, 100, 0, 0 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 4, -1, 100, 0, 0 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 5, -1, 100, 0, 0 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 6, -1, 100, 0, 0 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 7, -1, 100, 0, 0 },
};
static const TImcSongEffect IMCDEATH_EffectList[] = {
	{ 90, 206, 1, 0, IMCSONGEFFECTTYPE_RESONANCE, 1, 1 },
	{ 51, 0, 1, 0, IMCSONGEFFECTTYPE_LOWPASS, 1, 0 },
};
static unsigned char IMCDEATH_ChannelVol[8] = { 100, 100, 100, 100, 100, 100, 100, 100 };
static const unsigned char IMCDEATH_ChannelEnvCounter[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
static const bool IMCDEATH_ChannelStopNote[8] = { true, false, false, false, false, false, false, false };
TImcSongData imcDataIMCDEATH = {
	/*LEN*/ 0x1, /*ROWLENSAMPLES*/ 4134, /*ENVLISTSIZE*/ 6, /*ENVCOUNTERLISTSIZE*/ 7, /*OSCLISTSIZE*/ 11, /*EFFECTLISTSIZE*/ 2, /*VOL*/ 100,
	IMCDEATH_OrderTable, IMCDEATH_PatternData, IMCDEATH_PatternLookupTable, IMCDEATH_EnvList, IMCDEATH_EnvCounterList, IMCDEATH_OscillatorList, IMCDEATH_EffectList,
	IMCDEATH_ChannelVol, IMCDEATH_ChannelEnvCounter, IMCDEATH_ChannelStopNote };
ZL_Sound sndDeath(ZL_SynthImcTrack::LoadAsSample(&imcDataIMCDEATH));

//--------------------------------------------------------------------------------------------------------------------------------------------------------

static const unsigned int IMCSACRIF_OrderTable[] = { 0x000000001, };
static const unsigned char IMCSACRIF_PatternData[] = {
	0x50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
static const unsigned char IMCSACRIF_PatternLookupTable[] = { 0, 1, 1, 1, 1, 1, 1, 1, };
static const TImcSongEnvelope IMCSACRIF_EnvList[] = {
	{ 0, 256, 15, 8, 16, 255, true, 255, },
	{ 150, 255, 42, 22, 19, 255, true, 255, },
	{ 0, 256, 42, 24, 16, 255, true, 255, },
	{ 200, 256, 297, 8, 255, 255, true, 255, },
	{ 150, 255, 40, 22, 19, 255, true, 255, },
};
static TImcSongEnvelopeCounter IMCSACRIF_EnvCounterList[] = {
	{ 0, 0, 256 }, { -1, -1, 256 }, { 1, 0, 153 }, { 2, 0, 0 },
	{ 3, 0, 256 }, { 4, 0, 153 }, { 2, 0, 0 }, { 3, 0, 256 },
};
static const TImcSongOscillator IMCSACRIF_OscillatorList[] = {
	{ 8, 150, IMCSONGOSCTYPE_SINE, 0, -1, 72, 1, 2 },
	{ 9, 0, IMCSONGOSCTYPE_SINE, 0, -1, 72, 1, 5 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 0, 0, 30, 3, 4 },
	{ 7, 174, IMCSONGOSCTYPE_SINE, 0, 1, 30, 6, 7 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 1, -1, 100, 0, 0 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 2, -1, 100, 0, 0 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 3, -1, 100, 0, 0 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 4, -1, 100, 0, 0 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 5, -1, 100, 0, 0 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 6, -1, 100, 0, 0 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 7, -1, 100, 0, 0 },
};
static const TImcSongEffect IMCSACRIF_EffectList[] = {
	{ 80, 0, 1, 0, IMCSONGEFFECTTYPE_DELAY, 0, 0 },
	{ 60, 0, 1, 0, IMCSONGEFFECTTYPE_DELAY, 0, 0 },
	{ 28, 0, 3307, 0, IMCSONGEFFECTTYPE_DELAY, 0, 0 },
	{ 28, 0, 4409, 0, IMCSONGEFFECTTYPE_DELAY, 0, 0 },
	{ 60, 232, 1, 0, IMCSONGEFFECTTYPE_RESONANCE, 1, 1 },
	{ 128, 0, 11024, 0, IMCSONGEFFECTTYPE_DELAY, 0, 0 },
};
static unsigned char IMCSACRIF_ChannelVol[8] = { 75, 100, 100, 100, 100, 100, 100, 100 };
static const unsigned char IMCSACRIF_ChannelEnvCounter[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
static const bool IMCSACRIF_ChannelStopNote[8] = { true, false, false, false, false, false, false, false };
TImcSongData imcDataIMCSACRIF = {
	/*LEN*/ 0x1, /*ROWLENSAMPLES*/ 5512, /*ENVLISTSIZE*/ 5, /*ENVCOUNTERLISTSIZE*/ 8, /*OSCLISTSIZE*/ 11, /*EFFECTLISTSIZE*/ 6, /*VOL*/ 100,
	IMCSACRIF_OrderTable, IMCSACRIF_PatternData, IMCSACRIF_PatternLookupTable, IMCSACRIF_EnvList, IMCSACRIF_EnvCounterList, IMCSACRIF_OscillatorList, IMCSACRIF_EffectList,
	IMCSACRIF_ChannelVol, IMCSACRIF_ChannelEnvCounter, IMCSACRIF_ChannelStopNote };

ZL_Sound sndSacrifice(ZL_SynthImcTrack::LoadAsSample(&imcDataIMCSACRIF));

//--------------------------------------------------------------------------------------------------------------------------------------------------------

static const unsigned int IMCSWING_OrderTable[] = { 0x000000001, };
static const unsigned char IMCSWING_PatternData[] = { 0x50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
static const unsigned char IMCSWING_PatternLookupTable[] = { 0, 1, 1, 1, 1, 1, 1, 1, };
static const TImcSongEnvelope IMCSWING_EnvList[] = {
	{ 0, 150, 107, 32, 24, 255, true, 255, },
	{ 128, 256, 1, 8, 16, 255, true, 255, },
	{ 128, 256, 174, 8, 16, 255, true, 255, },
	{ 0, 256, 697, 8, 16, 16, true, 255, },
	{ 0, 256, 216, 24, 31, 255, true, 255, },
	{ 0, 256, 107, 25, 31, 255, true, 255, },
};
static TImcSongEnvelopeCounter IMCSWING_EnvCounterList[] = {
	{ 0, 0, 75 }, { -1, -1, 256 }, { 1, 0, 256 }, { 2, 0, 256 },
	{ 3, 0, 256 }, { 4, 0, 0 }, { 5, 0, 2 },
};
static const TImcSongOscillator IMCSWING_OscillatorList[] = {
	{ 5, 48, IMCSONGOSCTYPE_SINE, 0, -1, 150, 1, 2 },
	{ 8, 0, IMCSONGOSCTYPE_NOISE, 0, -1, 0, 4, 1 },
	{ 6, 31, IMCSONGOSCTYPE_SINE, 0, -1, 78, 5, 1 },
	{ 9, 150, IMCSONGOSCTYPE_SQUARE, 0, -1, 22, 6, 1 },
	{ 5, 15, IMCSONGOSCTYPE_NOISE, 0, 0, 22, 1, 3 },
	{ 8, 0, IMCSONGOSCTYPE_NOISE, 0, 2, 72, 1, 1 },
	{ 9, 48, IMCSONGOSCTYPE_SQUARE, 0, 3, 32, 1, 1 },
};
static const TImcSongEffect IMCSWING_EffectList[] = {
	{ 96, 0, 1, 0, IMCSONGEFFECTTYPE_LOWPASS, 1, 0 },
	{ 12573, 664, 1, 0, IMCSONGEFFECTTYPE_OVERDRIVE, 0, 1 },
};
static unsigned char IMCSWING_ChannelVol[8] = { 84, 84, 100, 100, 100, 100, 100, 100 };
static const unsigned char IMCSWING_ChannelEnvCounter[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
static const bool IMCSWING_ChannelStopNote[8] = { true, true, false, false, false, false, false, false };
TImcSongData imcDataIMCSWING = {
	/*LEN*/ 0x1, /*ROWLENSAMPLES*/ 6615, /*ENVLISTSIZE*/ 6, /*ENVCOUNTERLISTSIZE*/ 7, /*OSCLISTSIZE*/ 7, /*EFFECTLISTSIZE*/ 2, /*VOL*/ 100,
	IMCSWING_OrderTable, IMCSWING_PatternData, IMCSWING_PatternLookupTable, IMCSWING_EnvList, IMCSWING_EnvCounterList, IMCSWING_OscillatorList, IMCSWING_EffectList,
	IMCSWING_ChannelVol, IMCSWING_ChannelEnvCounter, IMCSWING_ChannelStopNote };
ZL_Sound sndSwing(ZL_SynthImcTrack::LoadAsSample(&imcDataIMCSWING));

//--------------------------------------------------------------------------------------------------------------------------------------------------------

static const unsigned int IMCHIT_OrderTable[] = { 0x000000001, };
static const unsigned char IMCHIT_PatternData[] = { 0x50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
static const unsigned char IMCHIT_PatternLookupTable[] = { 0, 1, 1, 1, 1, 1, 1, 1, };
static const TImcSongEnvelope IMCHIT_EnvList[] = {
	{ 0, 256, 3, 7, 255, 255, true, 255, },
	{ 0, 256, 184, 23, 255, 255, true, 255, },
	{ 0, 256, 8, 8, 16, 255, true, 255, },
	{ 0, 256, 64, 8, 16, 255, true, 255, },
	{ 0, 256, 64, 5, 19, 255, true, 255, },
	{ 0, 256, 64, 8, 255, 255, true, 255, },
	{ 0, 256, 173, 1, 23, 255, true, 255, },
};
static TImcSongEnvelopeCounter IMCHIT_EnvCounterList[] = {
	{ -1, -1, 256 }, { 0, 0, 254 }, { 1, 0, 2 }, { 2, 0, 256 },
	{ 1, 0, 2 }, { 3, 0, 256 }, { 4, 0, 238 }, { 5, 0, 256 },
	{ 6, 0, 158 },
};
static const TImcSongOscillator IMCHIT_OscillatorList[] = {
	{ 6, 31, IMCSONGOSCTYPE_SAW, 0, -1, 178, 0, 1 },
	{ 7, 36, IMCSONGOSCTYPE_SAW, 0, -1, 60, 0, 3 },
	{ 6, 66, IMCSONGOSCTYPE_SINE, 0, -1, 148, 5, 6 },
	{ 7, 0, IMCSONGOSCTYPE_NOISE, 0, 0, 48, 2, 0 },
	{ 7, 0, IMCSONGOSCTYPE_NOISE, 0, 1, 136, 4, 0 },
	{ 7, 31, IMCSONGOSCTYPE_SINE, 0, 2, 104, 7, 0 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 1, -1, 100, 0, 0 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 2, -1, 100, 0, 0 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 3, -1, 100, 0, 0 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 4, -1, 100, 0, 0 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 5, -1, 100, 0, 0 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 6, -1, 100, 0, 0 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 7, -1, 100, 0, 0 },
};
static const TImcSongEffect IMCHIT_EffectList[] = {
	{ 236, 134, 1, 0, IMCSONGEFFECTTYPE_RESONANCE, 8, 0 },
};
static unsigned char IMCHIT_ChannelVol[8] = { 171, 100, 100, 100, 100, 100, 100, 100 };
static const unsigned char IMCHIT_ChannelEnvCounter[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
static const bool IMCHIT_ChannelStopNote[8] = { true, false, false, false, false, false, false, false };
TImcSongData imcDataIMCHIT = {
	/*LEN*/ 0x1, /*ROWLENSAMPLES*/ 5512, /*ENVLISTSIZE*/ 7, /*ENVCOUNTERLISTSIZE*/ 9, /*OSCLISTSIZE*/ 13, /*EFFECTLISTSIZE*/ 1, /*VOL*/ 11,
	IMCHIT_OrderTable, IMCHIT_PatternData, IMCHIT_PatternLookupTable, IMCHIT_EnvList, IMCHIT_EnvCounterList, IMCHIT_OscillatorList, IMCHIT_EffectList,
	IMCHIT_ChannelVol, IMCHIT_ChannelEnvCounter, IMCHIT_ChannelStopNote };
ZL_Sound sndHit(ZL_SynthImcTrack::LoadAsSample(&imcDataIMCHIT));

//--------------------------------------------------------------------------------------------------------------------------------------------------------

static const unsigned int IMCTHEME_OrderTable[] = {
	0x000000001, 0x000000001, 0x000000002, 0x010000003, 0x020000003, 0x030000001, 0x011000001, 0x001000004,
	0x001000005, 0x003000001, 0x012000000, 0x041000006,
};
static const unsigned char IMCTHEME_PatternData[] = {
	0x50, 0, 0x52, 0, 0x50, 0, 0x53, 0, 0x50, 0, 0x56, 0, 0x54, 0, 0x52, 0,
	0, 0, 0, 0, 0, 0x56, 0, 0x53, 0, 0x56, 0, 0, 0, 0, 0, 0,
	0x4B, 0, 0x4B, 0, 0, 0, 0x49, 0, 0x53, 0, 0, 0x45, 0x44, 0x42, 0x40, 0x40,
	0x48, 0, 0x4A, 0, 0x48, 0, 0x51, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0x48, 0, 0x4A, 0, 0x48, 0, 0x53, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0,
	0, 0, 0x40, 0, 0x42, 0, 0x44, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0x40, 0, 0x40, 0, 0x40, 0, 0x40, 0x42, 0,
	0x40, 0, 0, 0, 0x42, 0, 0, 0, 0x40, 0, 0, 0, 0x44, 0, 0, 0,
	0x22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0x24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0x1B, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0, 0, 0,
};
static const unsigned char IMCTHEME_PatternLookupTable[] = { 0, 6, 6, 6, 6, 6, 6, 9, };
static const TImcSongEnvelope IMCTHEME_EnvList[] = {
	{ 0, 256, 2266, 8, 16, 7, true, 255, },
	{ 0, 256, 152, 8, 16, 255, true, 255, },
	{ 10, 50, 32, 8, 255, 255, false, 255, },
	{ 0, 256, 58, 1, 23, 255, true, 255, },
	{ 128, 256, 174, 8, 16, 16, true, 255, },
	{ 0, 256, 2092, 8, 16, 16, true, 255, },
	{ 0, 256, 523, 8, 16, 255, true, 255, },
	{ 50, 100, 130, 24, 255, 255, false, 255, },
};
static TImcSongEnvelopeCounter IMCTHEME_EnvCounterList[] = {
	{ 0, 0, 256 }, { -1, -1, 256 }, { 1, 0, 256 }, { 2, 0, 50 },
	{ 3, 6, 158 }, { 4, 6, 256 }, { 5, 6, 256 }, { 6, 6, 256 },
	{ 7, 7, 50 },
};
static const TImcSongOscillator IMCTHEME_OscillatorList[] = {
	{ 8, 0, IMCSONGOSCTYPE_SINE, 0, -1, 100, 1, 1 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 0, -1, 66, 1, 1 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 0, -1, 24, 1, 1 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 0, -1, 88, 2, 1 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 0, 1, 36, 1, 1 },
	{ 8, 0, IMCSONGOSCTYPE_NOISE, 0, 3, 14, 1, 1 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 1, -1, 100, 0, 0 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 2, -1, 100, 0, 0 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 3, -1, 100, 0, 0 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 4, -1, 100, 0, 0 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 5, -1, 100, 0, 0 },
	{ 5, 15, IMCSONGOSCTYPE_SINE, 6, -1, 166, 1, 5 },
	{ 8, 0, IMCSONGOSCTYPE_NOISE, 6, -1, 176, 6, 1 },
	{ 5, 227, IMCSONGOSCTYPE_SINE, 6, -1, 174, 7, 1 },
	{ 8, 0, IMCSONGOSCTYPE_SAW, 7, -1, 255, 1, 1 },
	{ 9, 0, IMCSONGOSCTYPE_SINE, 7, -1, 255, 1, 1 },
	{ 7, 0, IMCSONGOSCTYPE_SQUARE, 7, -1, 255, 1, 1 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 7, -1, 130, 1, 1 },
	{ 9, 0, IMCSONGOSCTYPE_NOISE, 7, 14, 28, 1, 1 },
	{ 8, 0, IMCSONGOSCTYPE_SQUARE, 7, 15, 38, 1, 1 },
};
static const TImcSongEffect IMCTHEME_EffectList[] = {
	{ 255, 191, 1, 0, IMCSONGEFFECTTYPE_RESONANCE, 1, 1 },
	{ 96, 0, 1, 0, IMCSONGEFFECTTYPE_LOWPASS, 1, 0 },
	{ 0, 0, 51, 0, IMCSONGEFFECTTYPE_FLANGE, 3, 0 },
	{ 92, 0, 1, 6, IMCSONGEFFECTTYPE_LOWPASS, 1, 0 },
	{ 2286, 3669, 1, 6, IMCSONGEFFECTTYPE_OVERDRIVE, 0, 1 },
	{ 0, 0, 101, 7, IMCSONGEFFECTTYPE_FLANGE, 8, 0 },
	{ 136, 0, 8268, 7, IMCSONGEFFECTTYPE_DELAY, 0, 0 },
};
static unsigned char IMCTHEME_ChannelVol[8] = { 207, 100, 100, 100, 100, 100, 148, 30 };
static const unsigned char IMCTHEME_ChannelEnvCounter[8] = { 0, 0, 0, 0, 0, 0, 4, 1 };
static const bool IMCTHEME_ChannelStopNote[8] = { false, false, false, false, false, false, true, true };
TImcSongData imcDataIMCTHEME = {
	/*LEN*/ 0xC, /*ROWLENSAMPLES*/ 8268, /*ENVLISTSIZE*/ 8, /*ENVCOUNTERLISTSIZE*/ 9, /*OSCLISTSIZE*/ 20, /*EFFECTLISTSIZE*/ 7, /*VOL*/ 100,
	IMCTHEME_OrderTable, IMCTHEME_PatternData, IMCTHEME_PatternLookupTable, IMCTHEME_EnvList, IMCTHEME_EnvCounterList, IMCTHEME_OscillatorList, IMCTHEME_EffectList,
	IMCTHEME_ChannelVol, IMCTHEME_ChannelEnvCounter, IMCTHEME_ChannelStopNote };
ZL_SynthImcTrack imcTheme(&imcDataIMCTHEME);

//--------------------------------------------------------------------------------------------------------------------------------------------------------

static const unsigned int IMCGAMEOVER_OrderTable[] = { 0x011000001, 0x022000002, 0x000000003, };
static const unsigned char IMCGAMEOVER_PatternData[] = {
	0x20, 0, 0, 0, 0x20, 0, 0, 0x20, 0x20, 0, 0, 0, 0x24, 0, 0, 0x22,
	0x22, 0, 0, 0x20, 0x20, 0, 0, 0x1B, 0x20, 0, 0, 0, 0, 0, 0, 0,
	0x74, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0x40, 0, 0, 0, 0x40, 0, 0, 0x40, 0x40, 0, 0, 0, 0x44, 0, 0, 0x42,
	0x42, 0, 0, 0x40, 0x40, 0, 0, 0x3B, 0x40, 0, 0, 0, 0, 0, 0, 0,
	0x30, 0, 0, 0, 0x30, 0, 0, 0x30, 0x30, 0, 0, 0, 0x34, 0, 0, 0x32,
	0x32, 0, 0, 0x30, 0x30, 0, 0, 0x2B, 0x30, 0, 0, 0, 0, 0, 0, 0,
};
static const unsigned char IMCGAMEOVER_PatternLookupTable[] = { 0, 3, 3, 3, 3, 3, 3, 5, };
static const TImcSongEnvelope IMCGAMEOVER_EnvList[] = {
	{ 0, 256, 26, 8, 16, 255, true, 255, },
	{ 0, 256, 152, 8, 16, 255, true, 255, },
	{ 10, 50, 32, 8, 255, 255, false, 255, },
	{ 0, 256, 58, 1, 23, 255, true, 255, },
	{ 128, 256, 174, 8, 16, 16, true, 255, },
	{ 0, 256, 2092, 8, 16, 16, true, 255, },
	{ 0, 256, 523, 8, 16, 255, true, 255, },
	{ 0, 256, 13, 8, 15, 255, true, 255, },
	{ 50, 100, 130, 24, 255, 255, false, 255, },
};
static TImcSongEnvelopeCounter IMCGAMEOVER_EnvCounterList[] = {
	{ 0, 0, 256 }, { -1, -1, 256 }, { 1, 0, 256 }, { 2, 0, 50 },
	{ 3, 6, 158 }, { 4, 6, 256 }, { 5, 6, 256 }, { 6, 6, 256 },
	{ 7, 7, 256 }, { 8, 7, 50 },
};
static const TImcSongOscillator IMCGAMEOVER_OscillatorList[] = {
	{ 8, 0, IMCSONGOSCTYPE_SINE, 0, -1, 100, 1, 1 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 0, -1, 66, 1, 1 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 0, -1, 24, 1, 1 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 0, -1, 88, 2, 1 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 0, 1, 36, 1, 1 },
	{ 8, 0, IMCSONGOSCTYPE_NOISE, 0, 3, 14, 1, 1 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 1, -1, 100, 0, 0 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 2, -1, 100, 0, 0 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 3, -1, 100, 0, 0 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 4, -1, 100, 0, 0 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 5, -1, 100, 0, 0 },
	{ 5, 15, IMCSONGOSCTYPE_SINE, 6, -1, 166, 1, 5 },
	{ 8, 0, IMCSONGOSCTYPE_NOISE, 6, -1, 176, 6, 1 },
	{ 5, 227, IMCSONGOSCTYPE_SINE, 6, -1, 174, 7, 1 },
	{ 8, 0, IMCSONGOSCTYPE_SAW, 7, -1, 255, 1, 1 },
	{ 9, 0, IMCSONGOSCTYPE_SINE, 7, -1, 255, 1, 1 },
	{ 7, 0, IMCSONGOSCTYPE_SQUARE, 7, -1, 255, 1, 1 },
	{ 8, 0, IMCSONGOSCTYPE_SINE, 7, -1, 130, 1, 1 },
	{ 9, 0, IMCSONGOSCTYPE_NOISE, 7, 14, 28, 1, 1 },
	{ 8, 0, IMCSONGOSCTYPE_SQUARE, 7, 15, 38, 1, 1 },
};
static const TImcSongEffect IMCGAMEOVER_EffectList[] = {
	{ 255, 191, 1, 0, IMCSONGEFFECTTYPE_RESONANCE, 1, 1 },
	{ 96, 0, 1, 0, IMCSONGEFFECTTYPE_LOWPASS, 1, 0 },
	{ 0, 0, 51, 0, IMCSONGEFFECTTYPE_FLANGE, 3, 0 },
	{ 92, 0, 1, 6, IMCSONGEFFECTTYPE_LOWPASS, 1, 0 },
	{ 2286, 3669, 1, 6, IMCSONGEFFECTTYPE_OVERDRIVE, 0, 1 },
	{ 0, 0, 101, 7, IMCSONGEFFECTTYPE_FLANGE, 9, 0 },
	{ 136, 0, 6615, 7, IMCSONGEFFECTTYPE_DELAY, 0, 0 },
};
static unsigned char IMCGAMEOVER_ChannelVol[8] = { 227, 100, 100, 100, 100, 100, 148, 30 };
static const unsigned char IMCGAMEOVER_ChannelEnvCounter[8] = { 0, 0, 0, 0, 0, 0, 4, 8 };
static const bool IMCGAMEOVER_ChannelStopNote[8] = { true, false, false, false, false, false, true, true };
TImcSongData imcDataIMCGAMEOVER = {
	/*LEN*/ 0x3, /*ROWLENSAMPLES*/ 6615, /*ENVLISTSIZE*/ 9, /*ENVCOUNTERLISTSIZE*/ 10, /*OSCLISTSIZE*/ 20, /*EFFECTLISTSIZE*/ 7, /*VOL*/ 100,
	IMCGAMEOVER_OrderTable, IMCGAMEOVER_PatternData, IMCGAMEOVER_PatternLookupTable, IMCGAMEOVER_EnvList, IMCGAMEOVER_EnvCounterList, IMCGAMEOVER_OscillatorList, IMCGAMEOVER_EffectList,
	IMCGAMEOVER_ChannelVol, IMCGAMEOVER_ChannelEnvCounter, IMCGAMEOVER_ChannelStopNote };
ZL_SynthImcTrack imcGameOver(&imcDataIMCGAMEOVER);

//--------------------------------------------------------------------------------------------------------------------------------------------------------
