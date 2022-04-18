// Generated C++ file by Il2CppInspector - http://www.djkaty.com - https://github.com/djkaty
// Target Unity version: 2017.4.15 - 2017.4.40

// ******************************************************************************
// * IL2CPP application-specific method definition addresses and signatures
// ******************************************************************************
using namespace app;


// Map hooks
DO_APP_FUNC(0x014CF980, void, InLevelMapPageContext_OnMarkClicked, (InLevelMapPageContext* __this, MonoMapMark* mark, MethodInfo* method));
DO_APP_FUNC(0x014E0420, void, InLevelMapPageContext_OnMapClicked, (InLevelMapPageContext* __this, Vector2 screenPos, MethodInfo* method));
DO_APP_FUNC(0x0158AD40, bool, MapModule_IsAreaUnlock, (MBHLOBDPKEC* __this, uint32_t sceneID, uint32_t areaID, MethodInfo* method));
DO_APP_FUNC(0x05036C50, uint16_t, SimpleSafeUInt16_get_Value, (void* __this, LAFKDOLNGNA rawValue, MethodInfo* method));


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


// Unlimited stamina
DO_APP_FUNC(0x0349EF50, void, LevelSyncCombatPlugin_RequestSceneEntityMoveReq, (BKFGGJFIIKC* __this, uint32_t entityId, MotionInfo* syncInfo, bool isReliable, uint32_t HAOCOEMOMBG, MethodInfo* method));
DO_APP_FUNC(0x07B4A950, void, AvatarPropDictionary_SetItem, (Dictionary_2_JNHGGGCKJNA_JKNLDEEBGLL_* __this, JNHGGGCKJNA key, JKNLDEEBGLL value, MethodInfo* method));
DO_APP_FUNC(0x047E6420, double, AvatarProp_DecodePropValue, (void* __this, JKNLDEEBGLL EODGKCKCMDM, MethodInfo* method));
DO_APP_FUNC(0x02707260, PropType__Enum, AvatarProp_DecodePropType, (void* __this, JNHGGGCKJNA EODGKCKCMDM, MethodInfo* method));
DO_APP_FUNC_METHODINFO(0x0B1E2F18, AvatarPropDictionary_SetItem__MethodInfo);


// God mode
DO_APP_FUNC(0x04D22280, void, VCHumanoidMove_NotifyLandVelocity, (VCHumanoidMove* __this, Vector3 velocity, float reachMaxDownVelocityTime, MethodInfo* method));
DO_APP_FUNC(0x01407160, bool, Miscs_CheckTargetAttackable, (void* __this, BaseEntity* attackerEntity, /* CBIKBDBKLEG */ BaseEntity* targetEntity, MethodInfo* method));
// DO_APP_FUNC(0x02ADE320, void, LCBaseCombat_FireBeingHitEvent, (LCBaseCombat* __this, uint32_t attackeeRuntimeID, AttackResult* attackResult, MethodInfo* method));


// Cooldown cheats
DO_APP_FUNC(0x01F10360, bool, HumanoidMoveFSM_CheckSprintCooldown, (/* HumanoidMoveFSM */void* __this, MethodInfo* method));
DO_APP_FUNC(0x0145AB40, bool, LCAvatarCombat_IsEnergyMax, (void* __this, MethodInfo* method));
DO_APP_FUNC(0x01458C60, bool, LCAvatarCombat_IsSkillInCD_1, (void* __this, void* skillInfo, MethodInfo* method));
DO_APP_FUNC(0x01034EE0, void, ActorAbilityPlugin_AddDynamicFloatWithRange, (void* __this, String* key, float value, float min, float max, bool forceDoAtRemote, MethodInfo* method));


// Rapid fire
DO_APP_FUNC(0x02AE15E0, void, LCBaseCombat_DoHitEntity, (LCBaseCombat* __this, uint32_t targetID, AttackResult* attackResult, bool ignoreCheckCanBeHitInMP, MethodInfo* method));
DO_APP_FUNC(0x03BB1030, void, Formula_CalcAttackResult, (void* __this, CombatProperty* attackCombatProperty, CombatProperty* defenseCombatProperty, AttackResult* attackResult, BaseEntity* attackerEntity, BaseEntity* attackeeEntity, MethodInfo* method));


// World cheats
DO_APP_FUNC(0x01A56020, void, VCMonsterAIController_TryDoSkill, (/* VCMonsterAIController */ void* __this, uint32_t skillID, MethodInfo* method)); // Manual
DO_APP_FUNC(0x04D96080, void, LCSelectPickup_AddInteeBtnByID, (void* __this, BaseEntity* entity, MethodInfo* method));
DO_APP_FUNC(0x04D94930, bool, LCSelectPickup_IsInPosition, (void* __this, BaseEntity* entity, MethodInfo* method));
DO_APP_FUNC(0x04D946E0, bool, LCSelectPickup_IsOutPosition, (void* __this, BaseEntity* entity, MethodInfo* method));
DO_APP_FUNC(0x012316E0, void, ItemModule_PickItem, (ItemModule* __this, uint32_t entityID, MethodInfo* method));


// Dialog skipping
DO_APP_FUNC(0x02B9E6A0, bool, TalkDialogContext_get_canClick, (TalkDialogContext* __this, MethodInfo* method)); // delete
DO_APP_FUNC(0x02B9BB50, bool, TalkDialogContext_get_canAutoClick, (TalkDialogContext* __this, MethodInfo* method));
DO_APP_FUNC(0x02BA24C0, void, TalkDialogContext_OnDialogSelectItem, (TalkDialogContext* __this, Notify* notify, MethodInfo* method));
DO_APP_FUNC(0x0419D8A0, void, InLevelCutScenePageContext_OnFreeClick, (InLevelCutScenePageContext* __this, MethodInfo* method)); // manual
DO_APP_FUNC(0x0419F270, void, InLevelCutScenePageContext_UpdateView, (InLevelCutScenePageContext* __this, MethodInfo* method)); // manual (find by InLevelCutScenePageContext_ShowSkipBtn)


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
DO_APP_FUNC(0x034A0660, void, LevelSyncCombatPlugin_SendFaceToDir, (void* __this, uint32_t runtimeID, Vector3 forward, MethodInfo* method));
DO_APP_FUNC(0x01514030, void, BaseEntity_FireEvent, (BaseEntity* __this, BaseEvent* e, bool immediately, MethodInfo* method));
DO_APP_FUNC(0x01092510, bool, ActorAbilityPlugin_OnEvent, (void* __this, BaseEvent* e, MethodInfo* method));


// Kill aura
DO_APP_FUNC(0x04F8C3A0, void, EvtCrash_Init, (EvtCrash* __this, uint32_t targetID, MethodInfo* method)); // Manual offset
DO_APP_FUNC(0x02F2A3E0, void, EventManager_FireEvent, (EventManager* __this, BaseEvent* e, bool immediately, MethodInfo* method));
DO_APP_FUNC(0x050ABF60, bool, FixedBoolStack_get_value, (FixedBoolStack* __this, MethodInfo* method));
DO_APP_FUNC(0x04566900, EvtCrash*, CreateCrashEvent, (void* __this, MethodInfo* method));
DO_APP_FUNC_METHODINFO(0x0B1CE4B0, CreateCrashEvent__MethodInfo);


// Utility
DO_APP_FUNC(0x01402C90, float,   Miscs_CalcCurrentGroundWaterHeight, (void* __this, float x, float z, MethodInfo* method));
DO_APP_FUNC(0x01402500, float,   Miscs_CalcCurrentGroundHeight,      (void* __this, float x, float z, MethodInfo* method));
DO_APP_FUNC(0x014021F0, float,   Miscs_CalcCurrentGroundHeight_1,    (void* __this, float x, float z, float rayStartHeight, float rayDetectLength, int32_t layer, MethodInfo* method));
DO_APP_FUNC(0x014026D0, Vector3, Miscs_CalcCurrentGroundNorm,        (void* __this, Vector3 pos, MethodInfo* method));
DO_APP_FUNC(0x0140F770, Vector3, Miscs_GenWorldPos,                  (void* __this, Vector2 levelMapPos, MethodInfo* method));
DO_APP_FUNC(0x0140EE70, Vector2, Miscs_GenLevelPos_1,                (void* __this, Vector3 worldPos, MethodInfo* method));
DO_APP_FUNC(0x01601D90, int32_t, Miscs_GetSceneGroundLayerMask,      (void* __this, MethodInfo* method));

DO_APP_FUNC(0x069A58A0, Vector3, WorldShiftManager_GetRelativePosition, (void* __this, Vector3 pos, MethodInfo* method));
DO_APP_FUNC(0x069A55E0, Vector3, WorldShiftManager_GetAbsolutePosition, (void* __this, Vector3 pos, MethodInfo* method));

DO_APP_FUNC(0x0849DB30, bool,    RectTransformUtility_ScreenPointToLocalPointInRectangle, (void* __this, void* rect, Vector2 screenPoint, void* cam, Vector2* localPoint, MethodInfo* method));

DO_APP_FUNC(0x018C3CD0, Vector3, ActorUtils_GetAvatarPos,     (void* __this, MethodInfo* method));
DO_APP_FUNC(0x018CBAA0, void,    ActorUtils_SetAvatarPos,     (void* __this, Vector3 pos, MethodInfo* method));
DO_APP_FUNC(0x018D0AE0, void,    ActorUtils_SyncAvatarMotion, (void* __this, int32_t state, MethodInfo* method));

DO_APP_FUNC(0x0749AF60, Vector3, Transform_get_position, (Transform* __this, MethodInfo* method));
DO_APP_FUNC(0x0749BAA0, void,    Transform_set_position, (Transform* __this, Vector3 value, MethodInfo* method));
DO_APP_FUNC(0x0749EB80, float,   Vector3_Distance, (void* __this, Vector3 a, Vector3 b, MethodInfo* method));

DO_APP_FUNC(0x074BA270, void, Cursor_set_visible,   (void* __this, bool value, MethodInfo* method));
DO_APP_FUNC(0x074BA220, void, Cursor_set_lockState, (void* __this, CursorLockMode__Enum value, MethodInfo* method));
DO_APP_FUNC(0x074BA1D0, bool, Cursor_get_visible,   (void* __this, MethodInfo* method));

DO_APP_FUNC(0x02662BD0, Notify, Notify_CreateNotify_1, (void* __this, AJAPIFPNFKP__Enum type, Object* body, MethodInfo* method));

DO_APP_FUNC(0x02F571D0, float, SafeFloat_GetValue, (void* __this, SafeFloat safeFloat, MethodInfo* method));

DO_APP_FUNC(0x0152E720, void,          Entity_SetPosition,             (BaseEntity* __this, Vector3 position, bool someBool, MethodInfo* method));
DO_APP_FUNC(0x04564540, LCBaseCombat*, BaseEntity_GetBaseCombat,       (BaseEntity* __this, MethodInfo* method));
DO_APP_FUNC(0x0154C5A0, String*,       BaseEntity_ToStringRelease,     (BaseEntity* __this, MethodInfo* method));
DO_APP_FUNC(0x0151C910, void,          BaseEntity_SetRelativePosition, (BaseEntity* __this, Vector3 position, bool forceSyncToRigidbody, MethodInfo* method));
DO_APP_FUNC(0x0152E720, void,          BaseEntity_SetAbsolutePosition, (BaseEntity* __this, Vector3 abpos, bool forceSyncToRigidbody, MethodInfo* method));
DO_APP_FUNC(0x0151D390, Vector3,       BaseEntity_GetAbsolutePosition, (BaseEntity* __this, MethodInfo* method));
DO_APP_FUNC(0x01516150, Vector3,       BaseEntity_GetRelativePosition, (BaseEntity* __this, MethodInfo* method));
DO_APP_FUNC(0x0151BD00, Vector3,       BaseEntity_GetForward,          (BaseEntity* __this, MethodInfo* method));
DO_APP_FUNC(0x0154D020, Vector3,       BaseEntity_GetForwardFast,      (BaseEntity* __this, MethodInfo* method));
DO_APP_FUNC(0x015175D0, Vector3,       BaseEntity_GetRight,            (BaseEntity* __this, MethodInfo* method));
DO_APP_FUNC(0x01515110, Vector3,       BaseEntity_GetUp,               (BaseEntity* __this, MethodInfo* method));
DO_APP_FUNC(0x0152ABB0, bool,          BaseEntity_IsActive,            (BaseEntity* __this, MethodInfo* method));
DO_APP_FUNC(0x0153AA10, Rigidbody*,    BaseEntity_GetRigidbody,        (BaseEntity* __this, MethodInfo* method));
DO_APP_FUNC(0x04564740, VCBaseMove*,   BaseEntity_GetMoveComponent_1,  (BaseEntity* __this, MethodInfo* method));
DO_APP_FUNC(0x01659D20, List_1_MoleMole_BaseComponent_*, BaseEntity_GetAllLogicComponents, (BaseEntity* __this, MethodInfo* method));
// v 2.6
// DO_APP_FUNC(0x01667880, List_1_System_UInt32_*, BaseEntity_GetChildrenRuntimeIds, (BaseEntity* __this, MethodInfo* method));
DO_APP_FUNC(0x0163DCF0, GameObject*, BaseEntity_get_gameObject, (BaseEntity* __this, MethodInfo* method));

DO_APP_FUNC_METHODINFO(0x0B25D518,     BaseEntity_GetMoveComponent_1__MethodInfo);
DO_APP_FUNC_METHODINFO(0x0B19B338,     BaseEntity_GetBaseCombat__MethodInfo);

DO_APP_FUNC(0x01E9F520, BaseEntity*,   EntityManager_GetCurrentAvatar,    (EntityManager* __this, MethodInfo* method));
DO_APP_FUNC(0x01EBAB90, CameraEntity*, EntityManager_GetMainCameraEntity, (EntityManager* __this, MethodInfo* method));
DO_APP_FUNC(0x01EA44F0, BaseEntity*,   EntityManager_GetValidEntity,      (EntityManager* __this, uint32_t runtimeID, MethodInfo* method));
DO_APP_FUNC(0x01E9F090, List_1_MoleMole_BaseEntity_*, EntityManager_GetEntities, (EntityManager* __this, MethodInfo* method));

DO_APP_FUNC(0x084045B0, void, Rigidbody_set_detectCollisions, (Rigidbody* __this, bool value, MethodInfo* method));
DO_APP_FUNC(0x08404790, void, Rigidbody_set_isKinematic,      (Rigidbody* __this, bool value, MethodInfo* method));

DO_APP_FUNC(0x07496F70, float, Time_get_deltaTime, (void* __this, MethodInfo* method));

DO_APP_FUNC(0x01F14D00, void, HumanoidMoveFSM_LateTick, (void* __this, float deltaTime, MethodInfo* method));

DO_APP_FUNC(0x03511760, bool, ScenePropManager_GetTreeTypeByPattern, (ScenePropManager* __this, String* pattern, ECGLPBEEEAA__Enum* treeType, MethodInfo* method));

DO_APP_FUNC(0x01997D90, void, NetworkManager_1_RequestHitTreeDropNotify, (NetworkManager_1* __this, Vector3 position, Vector3 hitPostion, ECGLPBEEEAA__Enum treeType, MethodInfo* method));
DO_APP_FUNC(0x0332CD30, uint64_t, GetTimestamp, (void* __this, MethodInfo* method));

DO_APP_FUNC(0x0743D0A0, app::Vector3, Camera_WorldToScreenPoint, (app::Camera* __this, app::Vector3 position, MethodInfo* method));
DO_APP_FUNC(0x0743DBF0, Camera*, Camera_get_main, (void* __this, MethodInfo* method));
DO_APP_FUNC(0x0743DF30, int32_t, Camera_get_pixelWidth, (Camera* __this, MethodInfo* method));
DO_APP_FUNC(0x0743DE60, int32_t, Camera_get_pixelHeight, (Camera* __this, MethodInfo* method));

DO_APP_FUNC(0x073C9A50, int32_t, Screen_get_width, (void* __this, MethodInfo* method));
DO_APP_FUNC(0x073C98F0, int32_t, Screen_get_height, (void* __this, MethodInfo* method));

DO_APP_FUNC(0x05D07B50, Bounds, Utils_1_GetBounds, (void* __this, GameObject* go, MethodInfo* method));
DO_APP_FUNC(0x07438F50, bool, Behaviour_get_isActiveAndEnabled, (Behaviour* __this, MethodInfo* method));

DO_APP_FUNC(0x017F43F0, bool, LoadingManager_IsLoaded, (LoadingManager* __this, MethodInfo* method));
DO_APP_FUNC(0x0155A580, bool, EntityManager_RemoveEntity, (EntityManager* __this, BaseEntity* entity, uint32_t specifiedRuntimeID, MethodInfo* method));
DO_APP_FUNC(0x0154F520, bool, EntityManager_IsCachedEntity, (EntityManager* __this, BaseEntity* entity, MethodInfo* method));
DO_APP_FUNC(0x013B9810, GameObject*, BaseEntity_get_rootGameObject, (BaseEntity* __this, MethodInfo* method));
// Thanks to @RyujinZX
DO_APP_FUNC(0x019C5D50, void, LCAbilityElement_ReduceModifierDurability, (LCAbilityElement* __this, int32_t modifierDurabilityIndex, float reduceDurability, Nullable_1_Single_ deltaTime, MethodInfo* method));

DO_APP_FUNC(0x035D8B70, BaseEntity*, GadgetEntity_GetOwnerEntity, (GadgetEntity* __this, MethodInfo* method));

// Singletons
DO_APP_FUNC(0x065A04E0, void*, Singleton_GetInstance, (void* __this, MethodInfo* method));
DO_APP_FUNC_METHODINFO(0x0B20F1E8, Singleton_1_MBHLOBDPKEC__get_Instance__MethodInfo);
DO_APP_FUNC_METHODINFO(0x0B1C5540, Singleton_1_LoadingManager__get_Instance__MethodInfo);
DO_APP_FUNC_METHODINFO(0x0B18EF88, Singleton_1_EntityManager__get_Instance__MethodInfo);
DO_APP_FUNC_METHODINFO(0x0B29B0F0, Singleton_1_InteractionManager__get_Instance__MethodInfo);
DO_APP_FUNC_METHODINFO(0x0B23BCF0, Singleton_1_MessageInfo__get_Instance__MethodInfo);
DO_APP_FUNC_METHODINFO(0x0B18C3C0, Singleton_1_UIManager_1__get_Instance__MethodInfo);
DO_APP_FUNC_METHODINFO(0x0B20B748, Singleton_1_ItemModule__get_Instance__MethodInfo);
DO_APP_FUNC_METHODINFO(0x0B2648E0, Singleton_1_EventManager__get_Instance__MethodInfo);

// 2.6
DO_APP_FUNC_METHODINFO(0x0B2EC728, Singleton_1_PlayerModule__get_Instance__MethodInfo);
DO_APP_FUNC_METHODINFO(0x0B2FD3D0, Singleton_1_MapManager__get_Instance__MethodInfo);
DO_APP_FUNC_METHODINFO(0x0B321D50, Singleton_1_ScenePropManager__get_Instance__MethodInfo);
DO_APP_FUNC_METHODINFO(0x0B3A9738, Singleton_1_NetworkManager_1__get_Instance__MethodInfo);
