// Generated C++ file by Il2CppInspector - http://www.djkaty.com - https://github.com/djkaty
// Target Unity version: 2017.4.15 - 2017.4.40

// ******************************************************************************
// * IL2CPP application-specific method definition addresses and signatures
// ******************************************************************************
using namespace app;


// Map hooks
DO_APP_FUNC(0x014CF980, void, InLevelMapPageContext_OnMarkClicked, (InLevelMapPageContext* __this, MonoMapMark* mark, MethodInfo* method));
DO_APP_FUNC(0x014E0420, void, InLevelMapPageContext_OnMapClicked, (InLevelMapPageContext* __this, Vector2 screenPos, MethodInfo* method));


// Map utility
DO_APP_FUNC(0x04E88810, Rect, MonoInLevelMapPage_get_mapRect, (MonoInLevelMapPage* __this, MethodInfo* method));
DO_APP_FUNC(0x04E887E0, Transform*, MonoInLevelMapPage_get_mapBackground, (MonoInLevelMapPage* __this, MethodInfo* method));


// Teleport hooks
DO_APP_FUNC(0x0306D160, void, GameManager_Update, (GameManager* __this, MethodInfo* method));
DO_APP_FUNC(0x0165CF20, void, LoadingManager_SceneGoto, (LoadingManager* __this, PlayerEnterSceneNotify* notify, MethodInfo* method));
DO_APP_FUNC(0x01658880, void, LoadingManager_PerformPlayerTransmit, (LoadingManager* __this, Vector3 targetPos, EnterType__Enum enterType, uint32_t token, CMHGHBNDBMG_ECPNDLCPDIE__Enum transType, uint32_t BDFOCMLADLB, MethodInfo* method));
DO_APP_FUNC(0x01656B10, void, LoadingManager_RequestSceneTransToPoint, (LoadingManager* __this, uint32_t sceneId, uint32_t pointId, void* finishCallBackInForceDrag, MethodInfo* method));
DO_APP_FUNC(0x0165B8B0, bool, LoadingManager_NeedTransByServer, (LoadingManager* __this, uint32_t sceneId, Vector3 position, MethodInfo* method));
DO_APP_FUNC(0x03629D00, Vector3, LocalEntityInfoData_GetTargetPos, (LocalEntityInfoData* __this, MethodInfo* method));
DO_APP_FUNC(0x0152E720, void, Entity_SetPosition, (BaseEntity* __this, Vector3 position, bool someBool, MethodInfo* method));


// Unlimited stamina
DO_APP_FUNC(0x0349EF50, void, NetworkManager_1_RequestSceneEntityMoveReq, (BKFGGJFIIKC* __this, uint32_t entityId, MotionInfo* syncInfo, bool isReliable, uint32_t HAOCOEMOMBG, MethodInfo* method));
DO_APP_FUNC(0x07B4A950, void, AvatarPropDictionary_SetItem, (Dictionary_2_JNHGGGCKJNA_JKNLDEEBGLL_* __this, JNHGGGCKJNA key, JKNLDEEBGLL value, MethodInfo* method));
DO_APP_FUNC(0x047E6420, double, AvatarProp_DecodePropValue, (void* __this, JKNLDEEBGLL EODGKCKCMDM, MethodInfo* method));
DO_APP_FUNC(0x02707260, PropType__Enum, AvatarProp_DecodePropType, (void* __this, JNHGGGCKJNA EODGKCKCMDM, MethodInfo* method));
DO_APP_FUNC_METHODINFO(0x0B1E2F18, AvatarPropDictionary_SetItem__MethodInfo);


// God mode
DO_APP_FUNC(0x04D22280, void, VCHumanoidMove_NotifyLandVelocity, (VCHumanoidMove* __this, Vector3 velocity, float reachMaxDownVelocityTime, MethodInfo* method));
DO_APP_FUNC(0x01407160, bool, Miscs_CheckTargetAttackable, (void* __this, BaseEntity* attackerEntity, /* CBIKBDBKLEG */ BaseEntity* targetEntity, MethodInfo* method));
// DO_APP_FUNC(0x02ADE320, void, LCBaseCombat_FireBeingHitEvent, (LCBaseCombat* __this, uint32_t attackeeRuntimeID, AttackResult* attackResult, MethodInfo* method));


// World cheats
DO_APP_FUNC(0x04691F40, void, VCMonsterAIController_TryDoSkill, (/* VCMonsterAIController */ void* __this, uint32_t skillID, MethodInfo* method));


// Dialog skipping
DO_APP_FUNC(0x02B9E6A0, bool, TalkDialogContext_get_canClick, (TalkDialogContext* __this, MethodInfo* method)); // delete
DO_APP_FUNC(0x02B9BB50, bool, TalkDialogContext_get_canAutoClick, (TalkDialogContext* __this, MethodInfo* method));
DO_APP_FUNC(0x02BA24C0, void, TalkDialogContext_OnDialogSelectItem, (TalkDialogContext* __this, Notify* notify, MethodInfo* method));
DO_APP_FUNC(0x02F60300, void, InLevelCutScenePageContext_OnFreeClick, (InLevelCutScenePageContext* __this, MethodInfo* method));
DO_APP_FUNC(0x02F64030, void, InLevelCutScenePageContext_UpdateView, (InLevelCutScenePageContext* __this, MethodInfo* method));


// Protection bypass
DO_APP_FUNC(0x074AAAB0, Byte__Array*, Application_RecordUserData, (void* __this, int32_t nType, MethodInfo* method));


// Networking
DO_APP_FUNC(0x03E58110, int32_t, KcpNative_kcp_client_send_packet, (void* __this, void* kcp_client, KcpPacket_1* packet, MethodInfo* method));
DO_APP_FUNC(0x03ABDEB0, bool, KcpClient_TryDequeueEvent, (void* __this, ClientKcpEvent* evt, MethodInfo* method));
DO_APP_FUNC(0x022C0E40, void, Packet_XorEncrypt, (void* __this, Byte__Array** bytes, int32_t length, MethodInfo* method));


// Lua functions
DO_APP_FUNC(0x052CC900, Byte__Array*, LuaManager_LoadCustomLuaFile, (LuaManager* __this, String** filePath, bool* recycleBytes, MethodInfo* method));
DO_APP_FUNC(0x060FFD90, void, Lua_xlua_pushasciistring, (void* __this, void* L, String* str, MethodInfo* method));
DO_APP_FUNC(0x01CBBD70, void, LuaShellManager_DoString, (void* __this, Byte__Array* byteArray, MethodInfo* method));
DO_APP_FUNC(0x060F5D10, void*, LuaEnv_DoString, (void* __this, Byte__Array* chunk, String* chunkName, void* env, MethodInfo* method));
DO_APP_FUNC(0x01CBB760, void, LuaShellManager_ReportLuaShellResult, (void* __this, String* type, String* value, MethodInfo* method)); // Anticheat info


// Debug hooks
DO_APP_FUNC(0x047D6B60, AttackResult_1*, AttackResult_FillProtoAttackResult, (AttackResult* __this, uint32_t attackerID, uint32_t defenseId, AttackResult_1* protoAttackResult, MethodInfo* method));


// Utility
DO_APP_FUNC(0x01402C90, float,   Miscs_CalcCurrentGroundWaterHeight, (void* __this, float x, float z, MethodInfo* method));
DO_APP_FUNC(0x01402500, float,   Miscs_CalcCurrentGroundHeight, (void* __this, float x, float z, MethodInfo* method));
DO_APP_FUNC(0x014021F0, float,   Miscs_CalcCurrentGroundHeight_1, (void* __this, float x, float z, float rayStartHeight, float rayDetectLength, int32_t layer, MethodInfo* method));
DO_APP_FUNC(0x014026D0, Vector3, Miscs_CalcCurrentGroundNorm, (void* __this, Vector3 pos, MethodInfo* method));

DO_APP_FUNC(0x0140F770, Vector3, Miscs_GenWorldPos, (void* __this, Vector2 levelMapPos, MethodInfo* method));
DO_APP_FUNC(0x0140EE70, Vector2, Miscs_GenLevelPos_1, (void* __this, Vector3 worldPos, MethodInfo* method));

DO_APP_FUNC(0x069A58A0, Vector3, WorldShiftManager_GetRelativePosition, (void* __this, Vector3 pos, MethodInfo* method));
DO_APP_FUNC(0x069A55E0, Vector3, WorldShiftManager_GetAbsolutePosition, (void* __this, Vector3 pos, MethodInfo* method));

DO_APP_FUNC(0x0849DB30, bool,    RectTransformUtility_ScreenPointToLocalPointInRectangle, (void* __this, void* rect, Vector2 screenPoint, void* cam, Vector2* localPoint, MethodInfo* method));

DO_APP_FUNC(0x018C3CD0, Vector3, ActorUtils_GetAvatarPos, (void* __this, MethodInfo* method));
DO_APP_FUNC(0x018CBAA0, void,    ActorUtils_SetAvatarPos, (void* __this, Vector3 pos, MethodInfo* method));

DO_APP_FUNC(0x0749AF60, Vector3, Transform_get_position, (Transform* __this, MethodInfo* method));
DO_APP_FUNC(0x0749BAA0, void,    Transform_set_position, (Transform* __this, Vector3 value, MethodInfo* method));
DO_APP_FUNC(0x0749EB80, float,   Vector3_Distance, (void* __this, Vector3 a, Vector3 b, MethodInfo* method));

DO_APP_FUNC(0x074BA270, void, Cursor_set_visible, (void* __this, bool value, MethodInfo* method));
DO_APP_FUNC(0x074BA220, void, Cursor_set_lockState, (void* __this, CursorLockMode__Enum value, MethodInfo* method));
DO_APP_FUNC(0x074BA1D0, bool, Cursor_get_visible, (void* __this, MethodInfo* method));

DO_APP_FUNC(0x02662BD0, Notify, Notify_CreateNotify_1, (void* __this, AJAPIFPNFKP__Enum type, Object* body, MethodInfo* method));

DO_APP_FUNC(0x0154C5A0, String*, BaseEntity_ToStringRelease, (BaseEntity* __this, MethodInfo* method));
DO_APP_FUNC(0x01E9F520, BaseEntity*, EntityManager_GetCurrentAvatar, (EntityManager* __this, MethodInfo* method));


// Singletons
DO_APP_FUNC(0x065A04E0, void*, Singleton_GetInstance, (void* __this, MethodInfo* method));
DO_APP_FUNC_METHODINFO(0x0B20F1E8, Singleton_1_MBHLOBDPKEC__get_Instance__MethodInfo);
DO_APP_FUNC_METHODINFO(0x0B1C5540, Singleton_1_LoadingManager__get_Instance__MethodInfo);
DO_APP_FUNC_METHODINFO(0x0B18EF88, Singleton_1_EntityManager__get_Instance__MethodInfo);
DO_APP_FUNC_METHODINFO(0x0B29B0F0, Singleton_1_InteractionManager__get_Instance__MethodInfo);
DO_APP_FUNC_METHODINFO(0x0B23BCF0, Singleton_1_MessageInfo__get_Instance__MethodInfo);
DO_APP_FUNC_METHODINFO(0x0B18C3C0, Singleton_1_UIManager_1__get_Instance__MethodInfo);