#include "stdafx.h"

int Engine::EquipmentWeaponIndexs[14];

Material* Engine::R_RegisterMaterial(const char* pFont, int iImageTrack) {
	return Invoke::Call<Material*>(XeLive.pPatches->MW3.R_RegisterMaterial, pFont, iImageTrack);
}

Font* Engine::R_RegisterFont(const char* pFont, int iImageTrack) {
	return Invoke::Call<Font*>(XeLive.pPatches->MW3.R_RegisterFont, pFont, iImageTrack);
}

void Engine::R_AddCmdDrawStretchPic(float x, float y, float width, float height, float xScale, float yScale, float xay, float yay, const float* color, Material* material) {
	Invoke::Call<DWORD>(XeLive.pPatches->MW3.R_AddCmdDrawStretchPic, x, y, width, height, xScale, yScale, xay, yay, color, material);
}

void Engine::R_AddCmdDrawText(const char* text, int maxChars, Font* font, float x, float y, float xScale, float yScale, float rotation, const float* color, int style) {
	Invoke::Call<DWORD>(XeLive.pPatches->MW3.R_AddCmdDrawText, text, maxChars, font, x, y, xScale, yScale, rotation, color, style);
};

int Engine::R_TextWidth(const char* text, Font* font) {
	return Invoke::Call<int>(XeLive.pPatches->MW3.R_TextWidth, text, 0x7FFFFFFF, font);
}

void* Engine::CG_DrawRotatedPicPhysical(int scrPlacement, float x, float y, float width, float height, float angle, const float* color, Material* material) {
	return Invoke::Call<void*>(XeLive.pPatches->MW3.CG_DrawRotatedPicPhysical, scrPlacement, x, y, width, height, angle, color, material);
}

int Engine::R_TextHeight(Font* font) {
	return font ? font->iPixelHeight : 0;
}

int Engine::R_NormalizedTextScale(Font* font, float scale) {
	return Invoke::Call<int>((XeLive.pPatches->MW3.R_NormalizedTextScale), font, scale);
}

void Engine::CG_CompassDrawPlayerMap(int localClientNum, CompassType compassType, RectDef* parentRect, RectDef* rect, Material* material, float* color) {
	Invoke::Call<DWORD>((XeLive.pPatches->MW3.CG_CompassDrawPlayerMap), localClientNum, compassType, parentRect, rect, material, color);
}

void Engine::CG_CompassDrawPlayer(int localClientNum, CompassType compassType, RectDef* parentRect, RectDef* rect, float* color) {
	Invoke::Call<DWORD>((XeLive.pPatches->MW3.CG_CompassDrawPlayer), localClientNum, compassType, parentRect, rect, color);
}

void Engine::CG_CompassDrawCrates(int localClientNum, CompassType compassType, RectDef* parentRect, RectDef* rect, Material* material, float* color) {
	Invoke::Call<DWORD>((XeLive.pPatches->MW3.CG_CompassDrawCrates), localClientNum, compassType, parentRect, rect, material, color);
}

void Engine::CG_CompassDrawPlanes(int localClientNum, CompassType compassType, RectDef* parentRect, RectDef* rect, Material* material, float* color) {
	Invoke::Call<DWORD>((XeLive.pPatches->MW3.CG_CompassDrawPlanes), localClientNum, compassType, parentRect, rect, material, color);
}

void Engine::CG_CompassDrawFriendlies(int localClientNum, CompassType compassType, RectDef* parentRect, RectDef* rect, float* color) {
	Invoke::Call<DWORD>((XeLive.pPatches->MW3.CG_CompassDrawFriendlies), localClientNum, compassType, parentRect, rect, color);
}

void Engine::CG_CompassDrawEnemies(int localClientNum, CompassType compassType, RectDef* parentRect, RectDef* rect, float* color) {
	Invoke::Call<DWORD>((XeLive.pPatches->MW3.CG_CompassDrawEnemies), localClientNum, compassType, parentRect, rect, color);
}

void Engine::CG_CompassDrawMissiles(int localClientNum, CompassType compassType, RectDef* parentRect, RectDef* rect, float* color) {
	Invoke::Call<DWORD>((XeLive.pPatches->MW3.CG_CompassDrawMissiles), localClientNum, compassType, parentRect, rect, color);
}

void Engine::CG_CompassDrawTurrets(int localClientNum, CompassType compassType, RectDef* parentRect, RectDef* rect, float* color) {
	Invoke::Call<DWORD>((XeLive.pPatches->MW3.CG_CompassDrawTurrets), localClientNum, compassType, parentRect, rect, color);
}

void Engine::CG_CompassDrawHelicopter(int localClientNum, CompassType compassType, RectDef* parentRect, RectDef* rect, float* color) {
	Invoke::Call<DWORD>((XeLive.pPatches->MW3.CG_CompassDrawHelicopter), localClientNum, compassType, parentRect, rect, color);
}

bool Engine::CG_WorldPosToCompass(CompassType compassType, cg* cg, RectDef* mapRect, float* north, float* world, Vector3<float> origin, float* out, float* outClipped) {
	return Invoke::Call<bool>((XeLive.pPatches->MW3.CG_WorldPosToCompass), compassType, cg, mapRect, north, world, origin, out, outClipped);
}

void Engine::CG_Trace(Trace* trace, Vector3<float>* start, Vector3<float>* end, int ignore, int mask) {
	Invoke::Call<DWORD>((XeLive.pPatches->MW3.CG_Trace), trace, start, end, ignore, mask);
}

void Engine::FireBulletPenetrate(int client, BulletFireParams* bullet, int weapon, int attacker, Entity* entity, int alwaysMinusOne, Vector3<float>* viewOrigin, bool unkBool) {
	Invoke::Call<DWORD>((XeLive.pPatches->MW3.FireBulletPenetrate), client, bullet, weapon, attacker, entity, alwaysMinusOne, viewOrigin, unkBool);
}

void Engine::CL_SetStance(int client, int stance) {
	Invoke::Call<DWORD>((XeLive.pPatches->MW3.CL_SetStance), client, stance);
}

void Engine::CG_GetEntityBModelBounds(Entity* entity, Vector3<float>* min, Vector3<float>* max) {
	Invoke::Call<DWORD>((XeLive.pPatches->MW3.CG_GetEntityBModelBounds), entity, min, max);
}

int Engine::CG_GetWeaponIndexForName(const char* name) {
	return Invoke::Call<int>((XeLive.pPatches->MW3.CG_GetWeaponIndexForName), name);
}

bool Engine::CG_CalcNamePosition(int client, Vector3<float>* out, float* outx, float* outy) {
	return Invoke::Call<bool>((XeLive.pPatches->MW3.CG_CalcNamePosition), client, out, outx, outy);
}

void Engine::Cbuf_AddText(int localPlayer, const char* command) {
	Invoke::Call<DWORD>((XeLive.pPatches->MW3.Cbuf_AddText), localPlayer, command);
}

bool Engine::CG_DObjGetWorldTagPos(Entity* entity, int dObj, DWORD tag, Vector3<float>* out) {
	return Invoke::Call<bool>((XeLive.pPatches->MW3.CG_DObjGetWorldTagPos), entity, dObj, tag, out);
}

bool Engine::AimTarget_GetTagPos(int localClientNum, Entity* entity, short tagIndex, Vector3<float>* out) {
	return Invoke::Call<bool>((XeLive.pPatches->MW3.AimTarget_GetTagPos), localClientNum, entity, tagIndex, out);
}

Vector3<float> Engine::GetTagPosition(int client, const char* tag) {
	if (!IsAlive(client)) {
		return Vector3<float>(0.f, 0.f, 0.f);
	}

	int dobj = Engine::Com_GetClientDObj(GetEntity(client).iHandle, 0);
	if (!dobj) {
		return Vector3<float>(0.f, 0.f, 0.f);
	}

	Vector3<float> out;
	if (Engine::CG_DObjGetWorldTagPos(&GetEntity(client), dobj, SL_GetString(tag), &out)) {
		return out;
	}

	return Vector3<float>(0.f, 0.f, 0.f);
}

int Engine::Com_GetClientDObj(int handle, int something) {
	return Invoke::Call<int>((XeLive.pPatches->MW3.Com_GetClientDObj), handle, something);
}

DWORD Engine::SL_GetString(const char* str) {
	return Invoke::Call<DWORD>((XeLive.pPatches->MW3.SL_GetString), str, 0);
}

int Engine::AimAssist_GetTagPos(int mask, Entity* entity, short tag, Vector3<float>* out) {
	return Invoke::Call<int>((XeLive.pPatches->MW3.AimAssist_GetTagPos), mask, entity, tag, out);
}

void Engine::CG_DrawGrenadePointer(int localClientNum, float centerX, float centerY, Vector3<float>* grenadeOffset, float* color) {
	Invoke::Call<DWORD>((XeLive.pPatches->MW3.CG_DrawGrenadePointer), localClientNum, centerX, centerY, grenadeOffset, color);
}

void Engine::CG_DrawGrenadeIcon(int localClientNum, float centerX, float centerY, Vector3<float>* grenadeOffset, float* color, Material* material) {
	Invoke::Call<DWORD>((XeLive.pPatches->MW3.CG_DrawGrenadeIcon), localClientNum, centerX, centerY, grenadeOffset, color, material);
}

void Engine::RandomBulletDir(int* randomSeed, float* x, float* y) {
	Invoke::Call<DWORD>((XeLive.pPatches->MW3.RandomBulletDir), randomSeed, x, y);
}

void Engine::BG_SeedRandWithGameTime(int* seed) {
	Invoke::Call<DWORD>((XeLive.pPatches->MW3.BG_SeedRandWithGameTime), seed);
}

void Engine::BG_GetSpreadForWeapon(cg* ps, int weapon, float* minSpread, float* maxSpread) {
	Invoke::Call<DWORD>((XeLive.pPatches->MW3.BG_GetSpreadForWeapon), ps, weapon, minSpread, maxSpread);
}

void Engine::CG_GetPlayerViewOrigin(int client, cg* ps, Vector3<float>* out) {
	Invoke::Call<DWORD>((XeLive.pPatches->MW3.CG_GetPlayerViewOrigin), client, ps, out);
}

void Engine::VectorToAngle(Vector3<float>* direction, Vector3<float>* out) {
	Invoke::Call<DWORD>((XeLive.pPatches->MW3.VectorToAngle), direction, out);
}

float Engine::BG_Random(int* pSeed) {
	return Invoke::Call<float>((XeLive.pPatches->MW3.BG_Random), pSeed);
}

Entity* Engine::GetEntityPtr() {
	return *(Entity**)(XeLive.pPatches->MW3.GetEntityPtr);
}

Entity Engine::GetEntity(int index) {
	if (GetEntityPtr()) {
		return GetEntityPtr()[index];
	}

	return Entity();
}

cg* Engine::GetCG() {
	return *(cg**)(XeLive.pPatches->MW3.GetCG);
}

ClientInfo Engine::GetClientInfo(int index) {
	auto cg = GetCG();
	if (cg) {
		if (cg->ClientInfo) {
			return cg->ClientInfo[index];
		}
	}

	return ClientInfo();
}

ServerInfo* Engine::GetServerInfo() {
	return *(ServerInfo**)(XeLive.pPatches->MW3.GetServerInfo);
}

ClientActive* Engine::GetClientActive() {
	return *(ClientActive**)(XeLive.pPatches->MW3.GetClientActive);
}

_XenonUserData Engine::GetXenonUserData(int index) {
	_XenonUserData* XenonUserData = (_XenonUserData*)(XeLive.pPatches->MW3.GetXenonUserData);
	if (XenonUserData) {
		return XenonUserData[index];
	}

	return _XenonUserData();
}

Vector2<float> Engine::GetCenter() {
	return Vector2<float>(Engine::GetCG()->Refdef.vScale.X * 0.5f, Engine::GetCG()->Refdef.vScale.Y * 0.5f);
}

bool Engine::InGame() {
	return GetDvar<bool>((XeLive.pPatches->MW3.cl_ingame)) && GetCG() != nullptr;
}

DWORD Engine::TimeGetTime() {
	return Invoke::Call<DWORD>((XeLive.pPatches->MW3.TimeGetTime));
}

bool Engine::IsFriend(int client) {
	auto cg = GetCG();
	if (!cg) return false;

	return cg->ClientInfo[client].iTeam == cg->ClientInfo[cg->iClientNumber].iTeam && strcmp(GetDvar<const char*>((XeLive.pPatches->MW3.ui_gametype)), "dm");
}

bool Engine::IsAlive(int clientNum, bool notPlayer) {
	auto cg = GetCG();
	if (!cg) return false;

	auto alive = GetEntity(clientNum).bAlive;

	if (notPlayer) {
		return (alive == 1 || alive == 3 || alive == 4);
	}

	return (alive == 1 || alive == 3 || alive == 4) && cg->ClientInfo[clientNum].szHeadModel && strcmp(cg->ClientInfo[clientNum].szHeadModel, "") && cg->ClientInfo[clientNum].szName;
}

float Engine::GetBoneFraction(int client, const char* aimtag) {
	if (!IsAlive(client)) 0.f;

	Trace t;

	auto cg = GetCG();
	if (!cg) return 0.f;

	auto pos = GetTagPosition(client, aimtag);
	if (!pos.IsNull()) {
		CG_Trace(&t, &cg->Refdef.vOrigin, &pos, client);
		return t.fFraction;
	}

	return 0.f;
}

bool Engine::IsLookingAtMe(int client, float* pDegree) {
	return false;
}

float Engine::GetDistance(Vector3<float> from, Vector3<float> to) {
	float dx = to.X - from.X;
	float dy = to.Y - from.Y;
	float dz = to.Z - from.Z;
	return (float)sqrt((dx * dx) + (dy * dy) + (dz * dz))/* * 0.03048f*/;
}

bool Engine::WorldToScreen(Vector3<float> world, Vector3<float>* out) {
	auto cg = Engine::GetCG();

	Vector3<float> position;
	position.X = world.X - cg->Refdef.vOrigin.X;
	position.Y = world.Y - cg->Refdef.vOrigin.Y;
	position.Z = world.Z - cg->Refdef.vOrigin.Z;

	Vector3<float> transform;
	transform.X = (position.X * cg->Refdef.vViewAxis[1].X) + (position.Y * cg->Refdef.vViewAxis[1].Y) + (position.Z * cg->Refdef.vViewAxis[1].Z);
	transform.Y = (position.X * cg->Refdef.vViewAxis[2].X) + (position.Y * cg->Refdef.vViewAxis[2].Y) + (position.Z * cg->Refdef.vViewAxis[2].Z);
	transform.Z = (position.X * cg->Refdef.vViewAxis[0].X) + (position.Y * cg->Refdef.vViewAxis[0].Y) + (position.Z * cg->Refdef.vViewAxis[0].Z);

	if (transform.Z < 0.1f) {
		return false;
	}

	out->X = (cg->Refdef.vScale.X * 0.5f) * (1 - (transform.X / cg->Refdef.vFOV.X / transform.Z));
	out->Y = (cg->Refdef.vScale.Y * 0.5f) * (1 - (transform.Y / cg->Refdef.vFOV.Y / transform.Z));

	return true;
}

void Engine::GetWeaponKillIconAspect(int type, Vector2<float>* aspect) {
	float width = 1.4f, height = 1.4f;

	if (type == 0) { //1:1
		width = 1.4f;
		height = 1.4f;
	}
	else if (type == 1) { //2:1
		width = 2.8f;
		height = 1.4f;
	}
	else if (type == 2) { //3:1
		width = 2.8f;
		height = 0.7f;
	}
	width *= 20.0f;
	height *= 20.0f;
	aspect->X = width;
	aspect->Y = height;
}

WeaponDef* Engine::GetWeaponDef(int weapon) {
	int weapDefId = ((int(*)(...))(XeLive.pPatches->MW3.GetWeaponDefID))(weapon, 0);
	return *(WeaponDef**)((XeLive.pPatches->MW3.GetWeaponDef) + ((weapDefId * 4) & 0x3FC));
}

Material* Engine::GetWeaponMaterialWithAspect(int weapon, Vector2<float>* aspect) {
	auto def = GetWeaponDef(weapon);
	GetWeaponKillIconAspect(*(DWORD*)((DWORD)def + 0x4C8), aspect);
	return def->pKillIcon;
}

void Engine::CacheEquipmentIndexs() {
	const char* names[] = {
		"frag_grenade_mp",
		"concussion_grenade_mp",
		"flash_grenade_mp",
		"smoke_grenade_mp",
		"semtex_mp",
		"emp_grenade_mp",
		"throwingknife_mp",
		"bouncingbetty_mp",
		"flare_mp",
		"claymore_mp",
		"portable_radar_mp",
		"c4_mp",
		"scavenger_bag_mp",
		"riotshield_mp"
	};

	for (int i = 0; i < 14; i++) {
		EquipmentWeaponIndexs[i] = CG_GetWeaponIndexForName(names[i]);
	}
}

bool Engine::IsWeaponEquipmentItem(int weapon) {
	for (int i = 0; i < 14; i++) {
		if (EquipmentWeaponIndexs[i] == weapon) {
			return true;
		}
	}

	return false;
}

bool Engine::IsWeaponThis(int weapon, eWeaponCheck check) {
	return weapon == EquipmentWeaponIndexs[check];
}