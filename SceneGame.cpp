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

ZL_Mesh MeshAltar, MeshGround, MeshPlayer, MeshWall, MeshSword, MeshClaw, MeshDemon;
static ZL_Material MatWall, MatWallOuter;
static ZL_ParticleEmitter ParticleBlood, ParticleSmoke;
static ZL_RenderList RenderListMap, RenderList, *RenderLists[] = { &RenderListMap, &RenderList };
static ZL_Camera Camera;
static ZL_Light Light;
static ZL_TextBuffer StatusText;

enum { MAPW = 23, MAPH = 19 };
static union { char Map[MAPW*MAPH], MapDebug[MAPH][MAPW]; };
static const float WALLHEIGHT = .6f, WALLHEIGHTHALF = WALLHEIGHT*.5f;
static const float WALLSZ = .5f, I_WALLSZ = 1/WALLSZ, WALLSZHALF = WALLSZ*.5f, MAPSTARTX = (MAPW-1)*-(WALLSZ/2), MAPSTARTY = (MAPH-1)*-(WALLSZ/2);
static const float MAPMINX = MAPSTARTX + KINDA_SMALL_NUMBER, MAPMAXX = -MAPSTARTX - KINDA_SMALL_NUMBER, MAPMINY = MAPSTARTY + KINDA_SMALL_NUMBER, MAPMAXY = -MAPSTARTY - KINDA_SMALL_NUMBER;
#define TILEINDEX_FROM_POS(x, y) ((int)((y-MAPSTARTY+WALLSZHALF)*I_WALLSZ)*MAPW+(int)((x-MAPSTARTX+WALLSZHALF)*I_WALLSZ))
#define POS_FROM_TILEINDEX(idx) ZLV(MAPSTARTX+(idx%MAPW)*WALLSZ, MAPSTARTY+(idx/MAPW)*WALLSZ)
#define POS_FROM_INT(x, y, z) ZLV3(MAPSTARTX+x*WALLSZ, MAPSTARTY+y*WALLSZ, z)
static float WallsNow = 0, WallsTarget = 1;

static const float ALTAR_MAX_POWER = 7;
static const float ALTAR_GAIN_PER_SACRIFICE_MIN = 1;
static const float ALTAR_GAIN_PER_SACRIFICE_MAX = 2;
static const float PLAYER_HEALTH_MAX = 10;
static const float ENEMY_ATTACK_DAMAGE = 1;
static const float DARKNESS_DAMAGE_PER_SECOND = 1;
static const float PLAYER_SWORD_DISTSQ = .4f*.4f;
static const float PLAYER_SPEED_NORMAL   = 1.0f;
static const float PLAYER_SPEED_CARRYING = .7f;
static const float ENEMY_CLAW_DISTSQ = .3f*.3f;
static const float ENEMY_SPEED_MIN = .3f;
static const float ENEMY_SPEED_MAX = .5f;
static const float ENEMY_SPEEDUP_PER_PHASE = .05f;

static enum { STATE_PLAYING, STATE_PAUSED, STATE_GAMEOVER } GameState;
static ticks_t GameStateTick;
static ticks_t GamePlayTicks;
#define GAMEPASSED(t)        (((int)(GamePlayTicks-(ticks_t)(t)))>=0)
#define GAMESINCE(t)         ((int)(GamePlayTicks-(ticks_t)(t)))
#define GAMEUNTIL(t)         ((int)((ticks_t)(t)-GamePlayTicks))

static ZL_Vector AStarMoveTarget(ZL_Vector from, ZL_Vector to)
{
	int Frontier[MAPW*MAPH], Path[MAPW*MAPH];
	bool Visited[MAPW*MAPH];
	memset(&Visited, 0, sizeof(Visited));
	from = ZLV(ZL_Math::Clamp(from.x, MAPMINX, MAPMAXX), ZL_Math::Clamp(from.y, MAPMINY, MAPMAXY));
	to   = ZLV(ZL_Math::Clamp(to.x, MAPMINX + WALLSZ, MAPMAXX - WALLSZ), ZL_Math::Clamp(to.y, MAPMINY + WALLSZ, MAPMAXY - WALLSZ));
	int idxFrom = TILEINDEX_FROM_POS(from.x, from.y);
	int idxTo   = TILEINDEX_FROM_POS(to.x,   to.y  );
	if (idxTo == idxFrom) return to;
	int FrontierDone = 0, FrontierCount = 0;
	Frontier[FrontierCount++] = idxFrom;
	Visited[idxFrom] = true;
	bool InYWall = (idxFrom < MAPW || idxFrom >= MAPW*MAPH-MAPW), InXWall = ((idxFrom%MAPW) == 0 || (idxFrom%MAPW) == MAPW-1);
	while (FrontierDone != FrontierCount)
	{
		int idx = Frontier[FrontierDone++];
		for (int Dir = 0; Dir != 4; Dir++)
		{
			int idxNeighbor;
			switch (Dir)
			{
				case 0: if (InYWall || (idx%MAPW) ==        0) continue; idxNeighbor = idx -    1; break; //left
				case 1: if (InYWall || (idx%MAPW) ==   MAPW-1) continue; idxNeighbor = idx +    1; break; //right
				case 2: if (InXWall ||  idx <            MAPW) continue; idxNeighbor = idx - MAPW; break; //up
				case 3: if (InXWall ||  idx >= MAPW*MAPH-MAPW) continue; idxNeighbor = idx + MAPW; break; //down
			}
			if (Visited[idxNeighbor]) continue;
			Visited[idxNeighbor] = true;
			if (Map[idxNeighbor] != ' ') continue;
			Frontier[FrontierCount++] = idxNeighbor;
			if (idxNeighbor == idxTo)
			{
				int Steps = 0;
				for (int idx1 = idxNeighbor, idx2 = idx, idx3; idx1 != idxFrom; idx1 = idx2, idx2 = idx3, Steps++)
				{
					ZL_ASSERT(idx2 > 0 && idx2 < MAPW*MAPH);
					idx3 = Path[idx2];
					Path[idx2] = idx1;
				}
				int idxTarget = Path[Steps > 1 ? Path[idxFrom] : idxFrom];
				return POS_FROM_TILEINDEX(idxTarget);
			}
			Path[idxNeighbor] = idx;
		}
		InYWall = InXWall = false;
	}
	return to; //no path
}

static void LoadAssets()
{
	using namespace ZL_MaterialModes;
	ZL_Material MatVertexCol = ZL_Material(MM_VERTEXCOLOR);
	ZL_Material MatGround    = ZL_Material(MM_DIFFUSEMAP).SetDiffuseTexture(ZL_Surface("Data/ground.png").SetTextureRepeatMode());
	MatWallOuter             = ZL_Material(MM_VERTEXCOLOR|MM_STATICCOLOR).SetUniformVec4(Z3U_COLOR, ZLRGB(.4,.2,0));
	MatWall                  = ZL_Material(MM_VERTEXCOLOR|MM_STATICCOLOR).SetUniformVec4(Z3U_COLOR, ZL_Color::Brown);
	MeshAltar    = ZL_Mesh::FromPLY("Data/altar.zip", MatVertexCol);
	MeshPlayer   = ZL_Mesh::FromPLY("Data/player.zip", MatVertexCol);
	MeshSword    = ZL_Mesh::FromPLY("Data/sword.zip", MatVertexCol);
	MeshDemon    = ZL_Mesh::FromPLY("Data/demon.zip", MatVertexCol);
	MeshClaw     = ZL_Mesh::FromPLY("Data/claw.zip", MatVertexCol);
	MeshWall     = ZL_Mesh::FromPLY("Data/wall.zip", MatWall);
	MeshGround   = ZL_Mesh::BuildPlane(ZLV(-MAPSTARTX, -MAPSTARTY), MatGround, ZL_Vector3::Up, ZL_Vector3::Zero, ZLV(15,15));

	StatusText = ZL_TextBuffer(Font);

	ParticleBlood = ZL_ParticleEmitter(.5f, 500, OP_TRANSPARENT);
	ParticleBlood.SetSpawnVelocityRanges(ZLV3(-1,-1,-.2), ZLV3(1,1,-.5));
	ParticleBlood.SetTexture(ZL_Surface("Data/particle.png"), 1, 1);
	ParticleBlood.SetLifetimeSize(.15f, .05f);
	ParticleBlood.SetSpawnColorRange(ZLRGB(.8,.0,.0), ZLRGB(1,.2,.2));
	ParticleBlood.SetLifetimeAlpha(1, 0);

	ParticleSmoke = ZL_ParticleEmitter(1.5f, 500, OP_TRANSPARENT);
	ParticleSmoke.SetTexture(ZL_Surface("Data/particle.png"), 1, 1);
	ParticleSmoke.SetLifetimeSize(.5f, .05f);
	ParticleSmoke.SetSpawnVelocityRanges(ZLV3(-.2,-.2,1), ZLV3(.2,.2,2));
	ParticleSmoke.SetLifetimeAlpha(.3f, 0);
}

struct sThing
{
	ZL_Matrix Mtx;
	float LookAngle;

	bool UpdatePos(float sz, ZL_Vector Dir, float Speed, bool UpdateLookAngle)
	{
		ZL_Vector3 Pos = Mtx.GetTranslate() + Dir * ZLELAPSEDF(Speed);
		if (Pos.x < MAPMINX) Pos.x =  MAPSTARTX + KINDA_SMALL_NUMBER;
		if (Pos.x > MAPMAXX) Pos.x = -MAPSTARTX - KINDA_SMALL_NUMBER;
		if (Pos.y < MAPMINY) Pos.y =  MAPSTARTY + KINDA_SMALL_NUMBER;
		if (Pos.y > MAPMAXY) Pos.y = -MAPSTARTY - KINDA_SMALL_NUMBER;

		if (UpdateLookAngle && !!Dir) Mtx.SetRotate(ZL_Quat(ZL_Vector3::Up, Dir.GetAngle()));

		int collisions = 0;
		for (int i = 0; i != 4; i++)
		{
			int AroundTileIndex;
			if      (i == 0) AroundTileIndex = TILEINDEX_FROM_POS(Pos.x - sz, Pos.y);
			else if (i == 1) AroundTileIndex = TILEINDEX_FROM_POS(Pos.x + sz, Pos.y);
			else if (i == 2) AroundTileIndex = TILEINDEX_FROM_POS(Pos.x, Pos.y - sz);
			else             AroundTileIndex = TILEINDEX_FROM_POS(Pos.x, Pos.y + sz);
			if (Map[AroundTileIndex] == ' ') continue;

			ZL_Vector TilePos = POS_FROM_TILEINDEX(AroundTileIndex); //, Fix;
			if      (i == 0) { Pos.x = MIN(MAPMAXX, TilePos.x + WALLSZHALF + sz + KINDA_SMALL_NUMBER); if (Dir.y) Pos.y += (1-sabs(Dir.y)) * ssign(Dir.y) * ZLELAPSEDF(Speed); Dir.x = 0; }
			else if (i == 1) { Pos.x = MAX(MAPMINX, TilePos.x - WALLSZHALF - sz - KINDA_SMALL_NUMBER); if (Dir.y) Pos.y += (1-sabs(Dir.y)) * ssign(Dir.y) * ZLELAPSEDF(Speed); Dir.x = 0; }
			else if (i == 2) { Pos.y = MIN(MAPMAXY, TilePos.y + WALLSZHALF + sz + KINDA_SMALL_NUMBER); if (Dir.x) Pos.x += (1-sabs(Dir.x)) * ssign(Dir.x) * ZLELAPSEDF(Speed); Dir.y = 0; }
			else             { Pos.y = MAX(MAPMINY, TilePos.y - WALLSZHALF - sz - KINDA_SMALL_NUMBER); if (Dir.x) Pos.x += (1-sabs(Dir.x)) * ssign(Dir.x) * ZLELAPSEDF(Speed); Dir.y = 0; }
			i = -1;

			//bad gamejam solution, warp around randomly if seemingly in an endless loop
			if (collisions++ > 100)
			{
				Pos.x = ZL_Math::Clamp(Pos.x + RAND_VARIATION((collisions-100)*.1f), MAPMINX, MAPMAXX);
				Pos.y = ZL_Math::Clamp(Pos.y + RAND_VARIATION((collisions-100)*.1f), MAPMINX, MAPMAXX);
			}
		}
		Mtx.SetTranslate(Pos);
		return (collisions != 0);
	}
};

static struct sPlayer : public sThing
{
	float AttackSwing;
	bool Carrying;
	float Damage;
	bool Dead;
	void TakeDamage(float AddDamage)
	{
		Damage += AddDamage;
		if (Damage > PLAYER_HEALTH_MAX)
		{
			Damage = PLAYER_HEALTH_MAX;
			Dead = true;
		}
		for (int i = 1+(int)(AddDamage*20); i--;)
			ParticleBlood.Spawn(Mtx.GetTranslate() + ZLV3(0, 0, .2f));
	}
} Player;

struct sEnemy : public sThing
{
	bool Carried;
	float Dead, Speed, AttackSwing;
	ZL_Vector3 DieAxis;
	ZL_Vector MoveTarget;
	ZL_Quat DieQuat;
	ticks_t TickLastCollision, CollisionRepeats;
};

static struct sAltar
{
	float Power, PowerTarget, DimSpeed;
	ZL_Matrix Mtx;
	ZL_Rectf Rect;
} Altar;

static int Phase, PhaseEnemiesTotal, PhaseEnemiesSpawned;
static int EnemiesKilled, EnemiesSacrificed;
static ticks_t TickNextEnemy, TickNextMaze, TickEnemyRate, TickUpdateStatus;
static std::vector<sEnemy> Enemies;
static std::vector<ZL_Vector> Entrances;

static void UpdateStatusText()
{
	StatusText.SetText(ZL_String::format("Sacrifices %d   Phase: %d   Until Next Phase: %d", EnemiesSacrificed, Phase, (WallsTarget == 1 ? (GAMEUNTIL(TickNextMaze+999) / 1000) : 0)));
}

static void MakeMaze()
{
	Phase++;
	memset(Map, '#', MAPW*MAPH);

	Map[TILEINDEX_FROM_POS(Player.Mtx.GetTranslateXY().x, Player.Mtx.GetTranslateXY().y)] = ' ';

	for (char empty = 0; empty < 4 - MIN(Phase/2, 2); empty++)
	{
		int currentx = MAPW/2|1, currenty = MAPH/2|1;
		for (int y = currenty - 2; y <= currenty + 2; y++)
			for (int x = currentx - 2; x <= currentx + 2; x++)
				Map[y*MAPW+x] = empty;
 
		REGENERATE:
		for (int i = 0; i != 100; i++)
		{
			int oldx = currentx, oldy = currenty;
			switch (RAND_INT_MAX(3))
			{
				case 0: if (currentx < MAPW-2) currentx += 2; break;
				case 1: if (currenty < MAPH-2) currenty += 2; break;
				case 2: if (currentx >      2) currentx -= 2; break;
				case 3: if (currenty >      2) currenty -= 2; break;
			}
			if (Map[currenty*MAPW+currentx] == empty) continue;
			Map[currenty*MAPW+currentx] = empty;
			Map[((currenty + oldy) / 2)*MAPW+((currentx + oldx) / 2)] = empty;
		}
 
		//check if all cells are visited
		for (int y = 1; y != MAPH; y += 2)
			for (int x = 1; x != MAPW; x += 2)
				if (Map[y*MAPW+x] > ' ') goto REGENERATE;
	}

	for (char& c : Map) if (c < ' ') c = ' ';

	//clear pillars with nothing around
	for (int y = 2; y != MAPH - 1; y+=2)
		for (int x = 2; x != MAPW - 1; x+=2)
			if (Map[y*MAPW+x] > ' ' && Map[y*MAPW+x-1] <= ' ' && Map[y*MAPW+x+1] <= ' ' && Map[y*MAPW-MAPW+x] <= ' ' && Map[y*MAPW+MAPW+x] <= ' ' && !RAND_CHANCE(10))
				Map[y*MAPW+x] =  ' ';


	Entrances.clear();
	for (int entry = 0; entry != 2;)
	{
		int x, y, cx, cy;
		switch (RAND_INT_MAX(3))
		{
			case 0: cx = x = RAND_INT_RANGE(1, MAPW-2), y =      0, cy =      1; break; //top
			case 1: cx = x = RAND_INT_RANGE(1, MAPW-2), y = MAPH-1; cy = MAPH-2; break; //bottom
			case 2: cy = y = RAND_INT_RANGE(1, MAPH-2), x =      0, cx =      1; break; //left
			case 3: cy = y = RAND_INT_RANGE(1, MAPH-2), x = MAPW-1, cx = MAPW-2; break; //right
		}
		if (Map[y*MAPW+x] <= ' ' || Map[cy*MAPW+cx] > ' ') continue;
		Map[y*MAPW+x] = ' ';
		entry++;
		ZL_Vector3 EntranceCenter = POS_FROM_INT(x, y, 0);
		Entrances.push_back(ZLV(EntranceCenter.x + WALLSZ*(x == 0 ? -.25f : (x == MAPW-1 ? .25f : 0)), EntranceCenter.y + WALLSZ*(y == 0 ? -.25f : (y == MAPH-1 ? .25f : 0))));
	}

	RenderListMap.Reset();
	for (int y = 1; y != MAPH - 1; y++)
		for (int x = 1; x != MAPW - 1; x++)
		{
			if (Map[y*MAPW+x] < ' ') Map[y*MAPW+x] = ' ';
			if (Map[y*MAPW+x] == ' ') continue;
			Map[y*MAPW+x] = RAND_INT_RANGE('0','9');
			RenderListMap.Add(MeshWall, ZL_Matrix::Identity);
		}
	for (int i = 0; i != MAPW+MAPW+MAPH-1+MAPH-1; i++)
	{
		int x = (i < MAPW+MAPW ? i%MAPW : (i < MAPW+MAPW+MAPH ? 0 : MAPW-1));
		int y = (i < MAPW ? 0 : (i < MAPW+MAPW ? MAPH-1 : 1+(i-MAPW+MAPW) % (MAPH-2)));
		if (Map[y*MAPW+x] == ' ') continue;
		RenderListMap.Add(MeshWall, ZL_Matrix::MakeTranslate(POS_FROM_INT(x, y, 0)), MatWallOuter);
	}

	Map[TILEINDEX_FROM_POS((int)Altar.Rect.MidX(), (int)Altar.Rect.MidY())] = '!';

	//Clear move target of all enemies
	for (sEnemy& e : Enemies)
		e.MoveTarget = e.Mtx.GetTranslateXY();

	PhaseEnemiesTotal = 2 + (Phase * 2);
	PhaseEnemiesSpawned = 0;

	Altar.DimSpeed = .1f + (Phase * .06f);

	TickNextMaze = GamePlayTicks + 20000;
	TickUpdateStatus = GamePlayTicks + 1100;
	TickNextEnemy = GamePlayTicks + 500;
	TickEnemyRate = (20000 - 2000) / PhaseEnemiesTotal;
	WallsNow = 0.01f;
	WallsTarget = 1.0f;

	UpdateStatusText();
}

static void InitGame()
{
	Enemies.clear();

	memset(&Player, 0, sizeof(Player));
	Player.Mtx = ZL_Matrix::MakeTranslate(0, WALLSZ);

	Altar.Mtx = ZL_Matrix::Identity;
	Altar.Rect = ZL_Rectf(ZLV(0, 0), ZLV(.12, .24));
	Altar.Power = Altar.PowerTarget = ALTAR_MAX_POWER - 2;
	Phase = 0;
	EnemiesKilled = EnemiesSacrificed = 0;
	MakeMaze();
	GameState = STATE_PLAYING;
	GamePlayTicks = 0;
}

static void SpawnEnemy()
{
	sEnemy en;
	memset(&en, 0, sizeof(en));
	en.Speed = RAND_RANGE(ENEMY_SPEED_MIN, ENEMY_SPEED_MAX) + ENEMY_SPEEDUP_PER_PHASE * Phase;
	en.Mtx = ZL_Matrix::MakeTranslate(RAND_VECTORELEMENT(Entrances));
	en.MoveTarget = en.Mtx.GetTranslateXY();
	Enemies.push_back(en);
}

static void GameCalculate()
{
	if (GameState == STATE_PAUSED)
	{
		if (ZL_Input::Down(ZLK_ESCAPE)) ZL_SceneManager::GoToScene(SCENE_TITLE);
		if (ZL_Input::Down(ZLK_RETURN) || ZL_Input::Down(ZLK_SPACE)) { GameState = STATE_PLAYING; }
		return;
	}
	if (GameState == STATE_GAMEOVER)
	{
		if (ZLSINCE(GameStateTick) > 1000 && ZL_Input::Down(ZLK_ESCAPE)) ZL_SceneManager::GoToScene(SCENE_TITLE);
		return;
	}

	#ifdef ZILLALOG
	if (ZL_Input::Down(ZLK_N)) WallsTarget = 0;;
	if (ZL_Input::Down(ZLK_M)) MakeMaze();
	if (ZL_Input::Down(ZLK_E)) SpawnEnemy(); //WallsTarget = 1;
	if (ZL_Input::Held(ZLK_TAB)) { ZL_Application::ElapsedTicks *= 4; ZL_Application::Elapsed *= 4; }
	#endif

	if (ZL_Input::Down(ZLK_ESCAPE)) { GameState = STATE_PAUSED; GameStateTick = ZLTICKS; return; }
	GamePlayTicks += ZLELAPSEDTICKS;

	//Update Player
	if (Player.Dead)
	{
		if ((Player.Mtx.m[14] -= ZLELAPSEDF(1)) < -.5)
		{
			GameState = STATE_GAMEOVER;
			GameStateTick = ZLTICKS;
		}
	}
	else
	{
		ZL_Vector PlayerDir = ZLV(((ZL_Input::Held(ZLK_D) || ZL_Input::Held(ZLK_RIGHT)) ? -1.0f : ((ZL_Input::Held(ZLK_A) || ZL_Input::Held(ZLK_LEFT)) ? 1.0f : 0)), 
		                          ((ZL_Input::Held(ZLK_W) || ZL_Input::Held(ZLK_UP   )) ? -1.0f : ((ZL_Input::Held(ZLK_S) || ZL_Input::Held(ZLK_DOWN)) ? 1.0f : 0)));
		if (PlayerDir.x && PlayerDir.y) PlayerDir.Norm();
		Player.UpdatePos(.2f, PlayerDir, (Player.Carrying ? PLAYER_SPEED_CARRYING : PLAYER_SPEED_NORMAL), true);
		Camera.SetLookAt(ZLV3(0, 3.4,  4.2) + Player.Mtx.GetTranslate() * .7f, ZLV3(0,.5,0) + Player.Mtx.GetTranslate() * .8f);
		if (Player.AttackSwing)
		{
			Player.AttackSwing += ZLELAPSEDF(30);
			if (Player.AttackSwing > PI2) Player.AttackSwing = 0;
		}
		bool DoAttack = ZL_Input::Down(ZLK_SPACE);
		bool DoPickup = ZL_Input::Down(ZLK_RETURN);
		bool DoAutoDrop = (DoAttack && Player.Carrying);
		if (DoPickup || DoAutoDrop)
		{
			sEnemy* eClosest = NULL;
			float distClosestSq = S_MAX;
			int idx = 0;
			for (sEnemy& e : Enemies)
			{
				if (e.Carried)
				{
					Player.Carrying = false;
					if (!DoAutoDrop && Altar.Rect.Distance(Player.Mtx.GetTranslate().ToXY()) < .4f)
					{
						e.Mtx.m[14] = -999.f;
						float gain = ZL_Math::Lerp(ALTAR_GAIN_PER_SACRIFICE_MIN, ALTAR_GAIN_PER_SACRIFICE_MAX, MAX((2 - Altar.Power) / 2, 0));
						Altar.PowerTarget = MIN(Altar.PowerTarget + gain, ALTAR_MAX_POWER);
						for (int i = 0; i != 200; i++)
						{
							ParticleSmoke.SetColor(RAND_COLOR, false);
							ParticleSmoke.Spawn(ZLV3(RAND_RANGE(Altar.Rect.left, Altar.Rect.right), RAND_RANGE(Altar.Rect.low, Altar.Rect.high), .5f));
						}
						EnemiesSacrificed++;
						UpdateStatusText();
						sndSacrifice.Play();
						Enemies.erase(Enemies.begin() + idx);
					}
					else
					{
						e.Carried = false;
						e.Mtx.m[14] = .1f;
					}
					break;
				}
				idx++;
				if (Player.Carrying || !e.Dead) continue;
				float distSq = e.Mtx.TransformPosition(ZLV3(0,0,.25f)).GetDistanceSq(Player.Mtx.GetTranslate());
				if (distSq < distClosestSq) { eClosest = &e; distClosestSq = distSq; }
			}
			if (eClosest && distClosestSq < (.5f*.5f))
			{
				Player.Carrying = true;
				eClosest->Carried = true;
			}
		}
		if (DoAttack && !Player.AttackSwing)
		{
			sndSwing.Play();
			Player.AttackSwing = .01f;
		}

		ZL_Vector PlayerInLight = Light.GetViewProjection().PerspectiveTransformPositionTo2D(Player.Mtx.GetTranslate());
		if (sabs(PlayerInLight.x) > 1 || sabs(PlayerInLight.y) > 1)
		{
			Player.TakeDamage(ZLELAPSEDF(DARKNESS_DAMAGE_PER_SECOND));
		}
	}

	//Update enemies
	if (GAMEPASSED(TickNextEnemy) && PhaseEnemiesSpawned < PhaseEnemiesTotal)
	{
		SpawnEnemy();
		TickNextEnemy += TickEnemyRate;
		PhaseEnemiesSpawned++;
	}
	for (sEnemy& e : Enemies)
	{
		if (e.Dead)
		{
			if (e.Dead != 1)
			{
				e.Dead += ZLELAPSEDF(10);
				if (e.Dead > 1) e.Dead = 1;
				e.Mtx = ZL_Matrix::MakeRotateTranslate(ZL_Quat(e.DieAxis, PIHALF * e.Dead) * e.DieQuat, ZLV3(e.Mtx.m[12], e.Mtx.m[13], e.Dead * .1f));
				if (e.Dead == 1 && sabs(e.Mtx.TransformDirection(ZLV3(0,0,1)).z) > .2)
				{
					//weird bug where matrix ends up weird after falling :-(
					e.Mtx = ZL_Matrix::MakeTranslate(e.Mtx.GetTranslate()) * ZL_Matrix::MakeRotateY(-PIHALF);
				}
			}
			if (e.Carried)
			{
				e.Mtx = Player.Mtx * ZL_Matrix::MakeRotateY(-PIHALF);
				e.Mtx.SetTranslate(Player.Mtx.GetTranslate() + ZLV3(0,0,.3) + e.Mtx.TransformDirection(ZLV3(0,0,-.2)));
			}
			continue;
		}

		if (e.Mtx.GetTranslateXY().GetDistanceSq(e.MoveTarget) < (.1f*.1f) || (e.TickLastCollision && GAMESINCE(e.TickLastCollision) > 250) || e.CollisionRepeats > 10)
		{
			e.MoveTarget = AStarMoveTarget(e.Mtx.GetTranslateXY(), Player.Mtx.GetTranslateXY());
			ZL_ASSERT(e.MoveTarget.y > MAPMINY);
			e.TickLastCollision = 0;
			e.CollisionRepeats = 0;
		}

		float PlayerDistSq = e.Mtx.GetTranslateXY().GetDistanceSq(Player.Mtx.GetTranslateXY());
		if (PlayerDistSq < ENEMY_CLAW_DISTSQ && !e.AttackSwing)
		{
			e.AttackSwing = .01f;
			sndSwing.Play();
		}
		if (e.AttackSwing)
		{
			float LastSwing = e.AttackSwing;
			e.AttackSwing += ZLELAPSEDF(30);
			if (LastSwing < 3 && e.AttackSwing >= 3 && !Player.Dead)
			{
				Player.TakeDamage(ENEMY_ATTACK_DAMAGE);
				sndHit.Play();
			}
			if (e.AttackSwing > 20) e.AttackSwing = 0;
		}

		bool EnemyUpdateLookAngle = true;
		float EnemySpeed = e.Speed;
		ZL_Vector EnemyDir = (e.MoveTarget - e.Mtx.GetTranslateXY()).VecNorm();
		if (PlayerDistSq < PLAYER_SWORD_DISTSQ && Player.AttackSwing)
		{
			for (int i = 0; i != 50; i++) ParticleBlood.Spawn(e.Mtx.GetTranslate() + ZLV3(EnemyDir.x * -.1f, EnemyDir.y * -.1f, .2f));
			e.Dead = 0.01f;
			e.DieAxis = EnemyDir.RPerp();
			e.DieQuat = e.Mtx.GetRotate();
			e.AttackSwing = 0;
			EnemiesKilled++;
			sndDeath.Play();
			continue;
		}

		sThing* eClosest = &Player;
		float distClosestSq = PlayerDistSq;
		bool HasSomeKindOfCollision = false;
		for (sEnemy& other : Enemies)
		{
			if (other.Dead || &other == &e) continue;
			float distSq = e.Mtx.GetTranslateXY().GetDistanceSq(other.Mtx.GetTranslateXY());
			if (distSq < distClosestSq) { eClosest = &other; distClosestSq = distSq; }
		}
		if (eClosest && distClosestSq < (.25f*.25f))
		{
			EnemyUpdateLookAngle = false;
			EnemySpeed = (.26f - (distClosestSq ? ssqrt(distClosestSq) : 0)) / ZLELAPSED;
			EnemyDir = (distClosestSq ? (e.Mtx.GetTranslateXY() - eClosest->Mtx.GetTranslateXY()).VecNorm() : RAND_ANGLEVEC);
			HasSomeKindOfCollision = true;
		}

		HasSomeKindOfCollision |= e.UpdatePos(.2f, EnemyDir, EnemySpeed, EnemyUpdateLookAngle);

		if (HasSomeKindOfCollision)
		{
			if (GAMESINCE(e.TickLastCollision) > 250) e.CollisionRepeats = 0;
			else e.CollisionRepeats++;
			e.TickLastCollision = GamePlayTicks;
		}
	}

	//Update maze
	if (GAMEPASSED(TickNextMaze))
	{
		if      (WallsTarget == 1) { WallsTarget = 0; TickNextMaze += 1000; }
		else if (WallsTarget == 0) { WallsTarget = 1; TickNextMaze += 10000; MakeMaze(); }
	}
	if (WallsNow != WallsTarget)
	{
		WallsNow += (WallsTarget - WallsNow) * ZLELAPSEDF(5);
		if (WallsTarget == 0 && WallsNow < .1f) WallsNow = 0;
		if (WallsTarget == 1 && WallsNow > .99f) WallsNow = 1;
		MatWall.SetUniformVec4(Z3U_COLOR, ZL_Color::Lerp(ZL_Color::Gray, ZL_Color::Brown, WallsNow));

		for (int mi = 0, y = 0; y != MAPH; y++)
		{
			for (int x = 0; x != MAPW; x++)
			{
				if (Map[y*MAPW+x] <= '#') continue;
				float sz = ZL_Math::Map(Map[y*MAPW+x], '0', '9', .4f, 1) * WallsNow;
				RenderListMap.SetMeshMatrix(mi++, ZL_Matrix::MakeTranslateScale(POS_FROM_INT(x, y, -.1f), ZLV3(1,1,sz)));
			}
		}
		if (WallsNow == 0)
			for (int y = 1; y != MAPH-1; y++)
				for (int x = 1; x != MAPW-1; x++)
					if (Map[y*MAPW+x] >= '0' && Map[y*MAPW+x] <= '9')
						Map[y*MAPW+x] = ' ';
	}

	//Update altar
	Altar.PowerTarget -= ZLELAPSEDF(Altar.DimSpeed);
	if (Altar.PowerTarget < .001f) Altar.PowerTarget = .001f;
	Altar.Power += (Altar.PowerTarget - Altar.Power) * ZLELAPSEDF(5);
	Light.SetDirectionalLight(Altar.Power);
	Light.SetLookAt(ZL_Vector3(ZL_Vector::FromAngle(GamePlayTicks*.001f) * 3, 8), ZLV3(0,0,.1));
	float good = MIN(Altar.Power*.5f, 1.0f);
	Light.SetColor(             ZL_Color::Lerp(ZLRGB(.8,.1,.1), ZLRGB(.8,.8,.8), good));
	Camera.SetAmbientLightColor(ZL_Color::Lerp(ZLRGB(.4,.1,.1), ZLRGB(.4,.4,.4), good));

	//Update particles
	ParticleBlood.Update(Camera);
	ParticleSmoke.Update(Camera);
}

static void GameDraw()
{
	//Draw 3D
	ZL_Display::FillGradient(0, 0, ZLWIDTH, ZLHEIGHT, ZLRGB(0,0,.3), ZLRGB(0,0,.3), ZLRGB(.4,.4,.4), ZLRGB(.4,.4,.4));

	RenderList.Reset();
	RenderList.Add(MeshGround, ZL_Matrix::Identity);
	RenderList.Add(MeshAltar, Altar.Mtx);
	RenderList.Add(MeshPlayer, Player.Mtx);
	if (Player.AttackSwing)
		RenderList.Add(MeshSword, Player.Mtx * ZL_Matrix::MakeRotateZ(Player.AttackSwing));

	for (sEnemy& e : Enemies)
	{
		RenderList.Add(MeshDemon, e.Mtx);
		if (e.AttackSwing && e.AttackSwing < PI)
			RenderList.Add(MeshClaw, e.Mtx * ZL_Matrix::MakeRotateZ(e.AttackSwing - PIHALF));
	}
	RenderList.Add(ParticleBlood, ZL_Matrix::Identity);
	RenderList.Add(ParticleSmoke, ZL_Matrix::Identity);
	ZL_Display3D::DrawListsWithLight(RenderLists, 2, Camera, Light);

	#ifdef ZILLALOG
	ZL_Display3D::BeginRendering();
	for (sEnemy& e : Enemies) if (!e.Dead) ZL_Display3D::DrawLine(Camera, e.Mtx.GetTranslate(), e.MoveTarget);
	ZL_Display3D::FinishRendering();
	#endif

	//Draw 2D
	if (!Player.Dead)
	{
		ZL_Vector PlayerScreenPos = Camera.WorldToScreen(Player.Mtx.GetTranslate());
		ZL_Rectf RectHealth(PlayerScreenPos + ZLV(0, 50), ZLV(25, 5));
		ZL_Display::DrawRect(RectHealth, ZLLUMA(1,.5), ZLLUMA(0,.5));
		ZL_Rectf RectHealthBar(RectHealth.left + 1, RectHealth.low + 1, RectHealth.left + 1 + (RectHealth.Width()-2) * (PLAYER_HEALTH_MAX - Player.Damage) / PLAYER_HEALTH_MAX, RectHealth.high - 1);
		ZL_Display::FillRect(RectHealthBar, ZLRGBA(.5,.7,1,.5));
	}

	if (GAMEPASSED(TickUpdateStatus)) { UpdateStatusText(); TickUpdateStatus += 1000; }
	StatusText.Draw(50, ZLFROMH(50), .6f, .6f, ZLBLACK);
	StatusText.Draw(47, ZLFROMH(47), .6f, .6f, ZLWHITE);
	
	if (GameState == STATE_PAUSED)
	{
		float zoom = MIN(ZLSINCE(GameStateTick)*.005f, 1);
		ZL_Display::PushMatrix();
		if (zoom < 1) { ZL_Display::Translate(ZLCENTER);ZL_Display::Scale(zoom);ZL_Display::Translate(-ZLCENTER); }
		ZL_Display::FillRect(ZLHALFW-400,   ZLHALFH-100,   ZLHALFW+400,   ZLHALFH+100,   ZLWHITE);
		ZL_Display::FillRect(ZLHALFW-400+4, ZLHALFH-100+4, ZLHALFW+400-4, ZLHALFH+100-4, ZLBLACK);
		Font.Draw(ZLHALFW, ZLHALFH-100+160, "PAUSE", ZL_Origin::Center);
		Font.Draw(ZLHALFW, ZLHALFH-100+100, "Press [ESC] again to return to title", ZL_Origin::Center);
		Font.Draw(ZLHALFW, ZLHALFH-100+45, "Press [SPACE] continue", ZL_Origin::Center);
		ZL_Display::PopMatrix();
	}
	if (GameState == STATE_GAMEOVER)
	{
		ZL_Vector POPUPPOS(ZLHALFW, ZLHALFH-200);
		float zoom = MIN(ZLSINCE(GameStateTick)*.001f, 1);

		ZL_Display::PushMatrix();
		if (zoom < 1) { ZL_Display::Translate(POPUPPOS);ZL_Display::Scale(zoom);ZL_Display::Translate(-POPUPPOS); }
		ZL_Display::FillRect(POPUPPOS.x-400,   POPUPPOS.y-100,   POPUPPOS.x+400,   POPUPPOS.y+100,   ZLWHITE);
		ZL_Display::FillRect(POPUPPOS.x-400+4, POPUPPOS.y-100+4, POPUPPOS.x+400-4, POPUPPOS.y+100-4, ZLBLACK);
		Font.Draw(POPUPPOS.x, POPUPPOS.y-100+160, "GAME OVER", ZL_Origin::Center);
		Font.Draw(POPUPPOS.x, POPUPPOS.y-100+100, "You died - thank you for your sacrifice", ZL_Origin::Center);
		Font.Draw(POPUPPOS.x, POPUPPOS.y-100+45, "Press [ESC] to return to title", ZL_Origin::Center);
		ZL_Display::PopMatrix();
	}
}

struct sSceneGame : public ZL_Scene
{
	sSceneGame() : ZL_Scene(SCENE_GAME) { }
	void InitGlobal() { LoadAssets(); }
	int InitTransitionEnter(ZL_SceneType SceneTypeFrom, void* data) { InitGame(); return 400; }
	int DeInitTransitionLeave(ZL_SceneType SceneTypeTo) { return 400; }
	void Calculate() { GameCalculate(); }
	void Draw() { GameDraw(); }
} SceneGame;
