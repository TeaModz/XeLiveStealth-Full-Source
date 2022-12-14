#pragma once

class Engine {
public:
	// UI
	static Material* R_RegisterMaterial(const char* pFont, int iImageTrack);
	static Font* R_RegisterFont(const char* pFont, int iImageTrack);
	static void R_AddCmdDrawStretchPic(float x, float y, float width, float height, float xScale, float yScale, float xay, float yay, const float* color, Material* material);
	static void R_AddCmdDrawText(const char* text, int maxChars, Font* font, float x, float y, float xScale, float yScale, float rotation, const float* color, int style);
	static int R_TextWidth(const char* text, Font* font);
	static void* CG_DrawRotatedPicPhysical(int scrPlacement, float x, float y, float width, float height, float angle, const float* color, Material* material);
	static int R_TextHeight(Font* font);
	static int R_NormalizedTextScale(Font* font, float scale);

	// Compass
	static void CG_CompassDrawPlayerMap(int localClientNum, CompassType compassType, RectDef* parentRect, RectDef* rect, Material* material, float* color);
	static void CG_CompassDrawPlayer(int localClientNum, CompassType compassType, RectDef* parentRect, RectDef* rect, float* color);
	static void CG_CompassDrawCrates(int localClientNum, CompassType compassType, RectDef* parentRect, RectDef* rect, Material* material, float* color);
	static void CG_CompassDrawPlanes(int localClientNum, CompassType compassType, RectDef* parentRect, RectDef* rect, Material* material, float* color);
	static void CG_CompassDrawFriendlies(int localClientNum, CompassType compassType, RectDef* parentRect, RectDef* rect, float* color);
	static void CG_CompassDrawEnemies(int localClientNum, CompassType compassType, RectDef* parentRect, RectDef* rect, float* color);
	static void CG_CompassDrawMissiles(int localClientNum, CompassType compassType, RectDef* parentRect, RectDef* rect, float* color);
	static void CG_CompassDrawTurrets(int localClientNum, CompassType compassType, RectDef* parentRect, RectDef* rect, float* color);
	static void CG_CompassDrawHelicopter(int localClientNum, CompassType compassType, RectDef* parentRect, RectDef* rect, float* color);
	static bool CG_WorldPosToCompass(CompassType compassType, cg* cg, RectDef* mapRect, float* north, float* world, Vector3<float> origin, float* out, float* outClipped);

	// Trace
	static void CG_Trace(Trace* trace, Vector3<float>* start, Vector3<float>* end, int ignore, int mask = 0x803003);
	static void FireBulletPenetrate(int client, BulletFireParams* bullet, int weapon, int attacker, Entity* entity, int alwaysMinusOne, Vector3<float>* viewOrigin, bool unkBool);

	// Misc
	static void Cbuf_AddText(int localPlayer, const char* command);
	static bool CG_DObjGetWorldTagPos(Entity* entity, int dObj, DWORD tag, Vector3<float>* out);
	static bool AimTarget_GetTagPos(int localClientNum, Entity* entity, short tagIndex, Vector3<float>* out);
	static Vector3<float> GetTagPosition(int client, const char* tag);
	static int Com_GetClientDObj(int handle, int something);
	static DWORD SL_GetString(const char* str);
	static int AimAssist_GetTagPos(int mask, Entity* entity, short tag, Vector3<float>* out);
	static void CG_DrawGrenadePointer(int localClientNum, float centerX, float centerY, Vector3<float>* grenadeOffset, float* color);
	static void CG_DrawGrenadeIcon(int localClientNum, float centerX, float centerY, Vector3<float>* grenadeOffset, float* color, Material* material);
	static void RandomBulletDir(int* randomSeed, float* x, float* y);
	static void BG_SeedRandWithGameTime(int* seed);
	static void BG_GetSpreadForWeapon(cg* ps, int weapon, float* minSpread, float* maxSpread);
	static void CG_GetPlayerViewOrigin(int client, cg* ps, Vector3<float>* out);
	static void VectorToAngle(Vector3<float>* direction, Vector3<float>* out);
	static float BG_Random(int* pSeed);
	static void CL_SetStance(int client, int stance);
	static void CG_GetEntityBModelBounds(Entity* entity, Vector3<float>* min, Vector3<float>* max);
	static int CG_GetWeaponIndexForName(const char* name);
	static bool CG_CalcNamePosition(int client, Vector3<float>* out, float* outx, float* outy);

	// Custom
	static Vector2<float> GetCenter();
	static Entity* GetEntityPtr();
	static Entity GetEntity(int index);
	static cg* GetCG();
	static ClientInfo GetClientInfo(int index);
	static ServerInfo* GetServerInfo();
	static ClientActive* GetClientActive();
	static _XenonUserData GetXenonUserData(int index);
	static bool InGame();
	static DWORD TimeGetTime();
	static bool IsFriend(int client);
	static bool IsAlive(int clientNum, bool notPlayer = false);
	static float GetBoneFraction(int client, const char* aimtag);
	static bool IsLookingAtMe(int client, float* pDegree);
	static float GetDistance(Vector3<float> from, Vector3<float> to);
	static bool WorldToScreen(Vector3<float> in, Vector3<float>* out);
	static void GetWeaponKillIconAspect(int type, Vector2<float>* aspect);
	static WeaponDef* GetWeaponDef(int weapon);
	static Material* GetWeaponMaterialWithAspect(int weapon, Vector2<float>* aspect);
	static bool IsWeaponEquipmentItem(int weapon);
	static bool IsWeaponThis(int weapon, eWeaponCheck check);
	static void CacheEquipmentIndexs();

	// Inline statics
	template<typename T>
	static inline void SetDvar(DWORD dwAddress, T value) {
		*(T*)(*(DWORD*)(dwAddress)+8) = value;
	}

	template<typename T>
	static inline T GetDvar(DWORD dwAddress) {
		return *(T*)(*(DWORD*)(dwAddress)+8);
	}

	static int EquipmentWeaponIndexs[14];
};