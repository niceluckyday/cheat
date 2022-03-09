// Generated C++ file by Il2CppInspector - http://www.djkaty.com - https://github.com/djkaty
// Target Unity version: 2017.4.15 - 2017.4.40

// ******************************************************************************
// * IL2CPP application-specific method definition addresses and signatures
// ******************************************************************************
using namespace app;

DO_APP_FUNC(0x05CDB4C0, void, Object__ctor, (Object* __this, MethodInfo* method));
DO_APP_FUNC(0x067D4FB0, bool, Object_Equals, (Object* __this, Object* obj, MethodInfo* method));
DO_APP_FUNC(0x067D4F70, bool, Object_Equals_1, (void* __this, Object* objA, Object* objB, MethodInfo* method));
DO_APP_FUNC(0x05CDB4C0, void, Object_Finalize, (Object* __this, MethodInfo* method));
DO_APP_FUNC(0x067D4FC0, int32_t, Object_GetHashCode, (Object* __this, MethodInfo* method));
DO_APP_FUNC(0x067D4FD0, Type*, Object_GetType, (Object* __this, MethodInfo* method));
DO_APP_FUNC(0x067D4FF0, Object*, Object_MemberwiseClone, (Object* __this, MethodInfo* method));
DO_APP_FUNC(0x067D5000, String*, Object_ToString, (Object* __this, MethodInfo* method));
DO_APP_FUNC(0x079EA1F0, bool, Object_ReferenceEquals, (void* __this, Object* objA, Object* objB, MethodInfo* method));
DO_APP_FUNC(0x067D4FE0, int32_t, Object_InternalGetHashCode, (void* __this, Object* o, MethodInfo* method));
DO_APP_FUNC(0x067D5030, void*, Object_obj_address, (Object* __this, MethodInfo* method));

DO_APP_FUNC(0x052CC900, Byte__Array*, LuaManager_LoadCustomLuaFile, (LuaManager* __this, String** filePath, bool* recycleBytes, MethodInfo* method));

// ELIHHCKMPNE_ALEDHGAIDHN
DO_APP_FUNC(0x0175BD10, void, GameLogin_CheckGameIntegrity, (NetworkManager_1* __this, String* someString, uint32_t someInt, MethodInfo* method));

DO_APP_FUNC(0x06C66FF0, String*, DateTime_ToLongDateString, (DateTime__Boxed* __this, MethodInfo* method));
DO_APP_FUNC(0x06C67070, String*, DateTime_ToLongTimeString, (DateTime__Boxed* __this, MethodInfo* method));
DO_APP_FUNC(0x06C671B0, double, DateTime_ToOADate, (DateTime__Boxed* __this, MethodInfo* method));
DO_APP_FUNC(0x06C671C0, String*, DateTime_ToShortDateString, (DateTime__Boxed* __this, MethodInfo* method));
DO_APP_FUNC(0x06C67200, String*, DateTime_ToShortTimeString, (DateTime__Boxed* __this, MethodInfo* method));
DO_APP_FUNC(0x06C67500, String*, DateTime_ToString, (DateTime__Boxed* __this, MethodInfo* method));

DO_APP_FUNC(0x037B8580, void, GameLogin_SendInfo_2, (NetworkManager_1* __this, GKOJAICIOPA* info, MethodInfo* method));
DO_APP_FUNC_METHODINFO(0x0B25C800, GameLogin_SendInfo_2__MethodInfo);

DO_APP_FUNC(0x03747530, void, GKOJAICIOPA_JINHMKCMECH, (GKOJAICIOPA* __this, String* JOJAAEDKLKK, MethodInfo* method));
DO_APP_FUNC(0x03743E30, String*, GKOJAICIOPA_GJLJLNPHKBA, (GKOJAICIOPA* __this, MethodInfo* method));

DO_APP_FUNC(0x0705AB30, Array*, Array_CreateInstance, (void* __this, app::Type* elementType, int32_t length, MethodInfo* method));
DO_APP_FUNC(0x0705E3C0, int32_t, Array_get_Length, (Array* __this, MethodInfo* method));
DO_APP_FUNC(0x0705CC70, void, Array_SetValue_1, (Array* __this, Object* value, int64_t index, MethodInfo* method));
DO_APP_FUNC_METHODINFO(0x0B1AA908, Array_SetValue_1__MethodInfo);

DO_APP_FUNC(0x074AAAB0, Byte__Array*, Application_RecordUserData, (void* __this, int32_t nType, MethodInfo* method));
DO_APP_FUNC(0x070598A0, Object*, Array_Clone, (Array* __this, MethodInfo* method));

DO_APP_FUNC(0x04D57F50, bool, LuaHomeTask_get_isRealPause, (LuaHomeTask* __this, MethodInfo* method));
DO_APP_FUNC(0x07F63290, bool, LuaHomeTask_get_isBackGround, (LuaHomeTask* __this, MethodInfo* method));
DO_APP_FUNC(0x04D51360, void, LuaHomeTask_Init, (LuaHomeTask* __this, LuaTaskType__Enum taskType, int32_t taskQueueID, BaseActor* actor, String* tag, MethodInfo* method));
DO_APP_FUNC(0x04D559F0, void, LuaHomeTask_StartInit, (LuaHomeTask* __this, MethodInfo* method));
DO_APP_FUNC(0x04D57470, void, LuaHomeTask_TryLoadAnime, (LuaHomeTask* __this, MethodInfo* method));
DO_APP_FUNC(0x04D55F00, void, LuaHomeTask_StartTask, (LuaHomeTask* __this, MethodInfo* method));

DO_APP_FUNC(0x0140F770, Vector3, Miscs_GenWorldPos, (void* __this, Vector2 levelMapPos, MethodInfo* method));
DO_APP_FUNC(0x0140EE70, Vector2, Miscs_GenLevelPos_1, (void* __this, Vector3 worldPos, MethodInfo* method));
DO_APP_FUNC(0x018C3CD0, Vector3, ActorUtils_GetAvatarPos, (void* __this, MethodInfo* method));
DO_APP_FUNC(0x018CBAA0, void, ActorUtils_SetAvatarPos, (void* __this, Vector3 pos, MethodInfo* method));
DO_APP_FUNC(0x04BF7830, void, MonoMapMark_Init, (MonoMapMark* __this, HOOPGAGKHLD__Enum IGHPOMOHPNP, uint32_t DPLGNHJCBGB, NOIPNNCFAAH__Enum DDJHLKMOLEJ, GeneralMarkData* HBMFGMCKDNL, MethodInfo* method));
DO_APP_FUNC(0x01402500, float, Miscs_CalcCurrentGroundHeight, (void* __this, float x, float z, MethodInfo* method));
DO_APP_FUNC(0x01403060, float, Miscs_CalcCurrentWaterHeight, (void* __this, Vector3 pos, float rayUpLength, float rayDetectLength, MethodInfo* method));
DO_APP_FUNC(0x01402C90, float, Miscs_CalcCurrentGroundWaterHeight, (void* __this, float x, float z, MethodInfo* method));

DO_APP_FUNC(0x069A58A0, Vector3, WorldShiftManager_GetRelativePosition, (void* __this, Vector3 pos, MethodInfo* method));
DO_APP_FUNC(0x069A55E0, Vector3, WorldShiftManager_GetAbsolutePosition, (void* __this, Vector3 pos, MethodInfo* method));

// Teleport maybe
// DO_APP_FUNC(0x02B916D0, void, MJONCIENKGD_BLPCHLFNNCO, (MJONCIENKGD* __this, Vector3 OKHBIDCKLCD, float PGDBGIMJIFD, CMHGHBNDBMG_ECPNDLCPDIE__Enum CGKIEHJAOFM, MEPEDAIKBDI__Enum FFFNAKEFEOA, uint32_t AJILLILMGOK, uint32_t BDFOCMLADLB, MethodInfo* method));

// Teleport event starting
DO_APP_FUNC(0x0165CF20, void, TeleportStart_FJIDMGGJMEF_LFNOBNMJELN, (LoadingManager* __this, LDFJBNJCCCE* KJGLIPHELMG, MethodInfo* method));
DO_APP_FUNC(0x01658880, void, LoadingManager_PerformPlayerTransmit, (LoadingManager* __this, Vector3 targetPos, EnterType__Enum enterType, uint32_t token, CMHGHBNDBMG_ECPNDLCPDIE__Enum transType, uint32_t BDFOCMLADLB, MethodInfo* method));
DO_APP_FUNC(0x01656B10, void, LoadingManager_RequestSceneTransToPoint, (LoadingManager* __this, uint32_t sceneId, uint32_t pointId, void* finishCallBackInForceDrag, MethodInfo* method));

DO_APP_FUNC(0x01466540, MapModule_ScenePointData, GetWaypointInformation_MBHLOBDPKEC_BFKKBELGMLF, (MBHLOBDPKEC* __this, uint32_t waypointType, uint32_t waypointId, MethodInfo* method));
DO_APP_FUNC(0x014D3D80, void, TransportToMapMark_EAFLDMPLBHM_KBCBGBIDJMG, (EAFLDMPLBHM* __this, MonoMapMark* mark, MethodInfo* method));

DO_APP_FUNC(0x01E9F520, BaseEntity*, EntityManager_GetCurrentAvatar, (EntityManager* __this, MethodInfo* method));
DO_APP_FUNC(0x0152E720, void, Entity_SetPosition, (BaseEntity* __this, Vector3 position, bool someBool, MethodInfo* method));
DO_APP_FUNC(0x0749EB80, float, Vector3_Distance, (void* __this, Vector3 a, Vector3 b, MethodInfo* method));

DO_APP_FUNC(0x04E86BD0, MonoMapMark*, MonoInLevelMapPage_AddMark, (MonoInLevelMapPage* __this, BAEABLAJDLM PHDBMGDLIDJ, MethodInfo* method));

DO_APP_FUNC(0x014CC1F0, void, CreateCustomMapMark, (void* __this, Vector2 position, NOIPNNCFAAH__Enum iconType, MethodInfo* method));

DO_APP_FUNC(0x074BA270, void, Cursor_set_visible, (void* __this, bool value, MethodInfo* method));
DO_APP_FUNC(0x074BA220, void, Cursor_set_lockState, (void* __this, CursorLockMode__Enum value, MethodInfo* method));
DO_APP_FUNC(0x074BA1D0, bool, Cursor_get_visible, (void* __this, MethodInfo* method));

DO_APP_FUNC(0x01422720, int32_t, Miscs_GetHitLayerMask, (void* __this, MethodInfo* method));
DO_APP_FUNC(0x014285C0, int32_t, Miscs_GetSceneLayerMask, (void* __this, MethodInfo* method));
DO_APP_FUNC(0x01428440, int32_t, Miscs_GetSceneLayerMaskWithBothDynamicBarrierLayer, (void* __this, MethodInfo* method));
DO_APP_FUNC(0x01427FC0, int32_t, Miscs_GetSceneCameraLayerMask, (void* __this, MethodInfo* method));
DO_APP_FUNC(0x01429130, int32_t, Miscs_GetStaticSceneLayerMask, (void* __this, MethodInfo* method));
DO_APP_FUNC(0x014228A0, int32_t, Miscs_GetHitSceneLayerMask, (void* __this, MethodInfo* method));
DO_APP_FUNC(0x01425240, int32_t, Miscs_GetOccluderLayerMask, (void* __this, MethodInfo* method));
DO_APP_FUNC(0x0142BC70, int32_t, Miscs_GetWaterLayerMask, (void* __this, MethodInfo* method));
DO_APP_FUNC(0x014282C0, int32_t, Miscs_GetSceneGroundLayerMask, (void* __this, MethodInfo* method));
DO_APP_FUNC(0x01428140, int32_t, Miscs_GetSceneGroundLayerMaskWithoutTemp, (void* __this, MethodInfo* method));
DO_APP_FUNC(0x0141C6B0, int32_t, Miscs_GetCameraLayerMask, (void* __this, MethodInfo* method));
DO_APP_FUNC(0x01422A30, int32_t, Miscs_GetIKSceneLayerMask, (void* __this, MethodInfo* method));
DO_APP_FUNC(0x0141C830, int32_t, Miscs_GetCharacterLayerMask, (void* __this, MethodInfo* method));
DO_APP_FUNC(0x0141C060, int32_t, Miscs_GetBulletHitLayerMask, (void* __this, MethodInfo* method));
DO_APP_FUNC(0x0141F860, int32_t, Miscs_GetDynamicBarrierLayerMask, (void* __this, MethodInfo* method));
DO_APP_FUNC(0x014021F0, float, Miscs_CalcCurrentGroundHeight_1, (void* __this, float x, float z, float rayStartHeight, float rayDetectLength, int32_t layer, MethodInfo* method));
DO_APP_FUNC(0x014026D0, Vector3, Miscs_CalcCurrentGroundNorm, (void* __this, Vector3 pos, MethodInfo* method));

DO_APP_FUNC(0x01554680, void, BaseEntity__ctor, (BaseEntity* __this, MethodInfo* method));
DO_APP_FUNC(0x01554340, void, BaseEntity_ReturnToObjectPool, (BaseEntity* __this, MethodInfo* method));

DO_APP_FUNC(0x083FEF80, bool, Physics_Raycast_3, (void* __this, Vector3 origin, Vector3 direction, float maxDistance, MethodInfo* method));
DO_APP_FUNC(0x0168AD40, void, BaseActor_SpawnMonster, (BaseActor* __this, uint32_t configID, uint32_t level, Vector3 bornPos, Vector3 bornEuler, float scale, uint32_t sceneID, String* alias, uint32_t questId, uint32_t roomId, MethodInfo* method));
DO_APP_FUNC(0x074A1600, Vector3, Vector3_get_down, (void* __this, MethodInfo* method));

DO_APP_FUNC(0x02662A40, Notify, Notify_CreateNotify, (void* __this, AJAPIFPNFKP__Enum type, MethodInfo* method));

DO_APP_FUNC(0x01407160, bool, Miscs_CheckTargetAttackable, (void* __this, BaseEntity* attackerEntity, /* CBIKBDBKLEG */ BaseEntity* targetEntity, MethodInfo* method));

DO_APP_FUNC(0x07B4A950, void, AvatarPropDictionary_SetItem, (Dictionary_2_JNHGGGCKJNA_JKNLDEEBGLL_* __this, JNHGGGCKJNA key, JKNLDEEBGLL value, MethodInfo* method));
DO_APP_FUNC(0x047E6420, double, AvatarProp_DecodePropValue, (void* __this, JKNLDEEBGLL EODGKCKCMDM, MethodInfo* method));
DO_APP_FUNC(0x02707260, PropType__Enum, AvatarProp_DecodePropType, (void* __this, JNHGGGCKJNA EODGKCKCMDM, MethodInfo* method));

DO_APP_FUNC_METHODINFO(0x0B1E2F18, AvatarPropDictionary_SetItem__MethodInfo);

DO_APP_FUNC(0x0349DB90, void, SendAttackEventToServer, (BKFGGJFIIKC* __this, AttackLanded* attackInfo, MethodInfo* method));
DO_APP_FUNC(0x0349EF50, void, NetworkManager_1_RequestSceneEntityMoveReq, (BKFGGJFIIKC* __this, uint32_t entityId, MotionInfo* syncInfo, bool isReliable, uint32_t HAOCOEMOMBG, MethodInfo* method));
DO_APP_FUNC(0x02AB8E70, float, SafeFloat_GetValue, (SafeFloat__Boxed* __this, MethodInfo* method));
DO_APP_FUNC(0x02ABA450, void, SafeFloat_SetValue, (SafeFloat__Boxed* __this, float value, MethodInfo* method));
DO_APP_FUNC(0x047D6B60, AttackResult_1*, AttackResult_FillProtoAttackResult, (AttackResult* __this, uint32_t attackerID, uint32_t defenseId, AttackResult_1* protoAttackResult, MethodInfo* method));
DO_APP_FUNC(0x026B7290, void, VCBaseCombat_DoAttackLanded, (void* __this, AttackLanded* JGIKHCGLMOD, MethodInfo* method));
DO_APP_FUNC(0x047D1540, AttackResult*, AttackResult_CreateAttackResult_2, (void* __this, AttackResult_1* FAMCJNOEOFB, MethodInfo* method));

DO_APP_FUNC(0x0749AF60, Vector3, Transform_get_position, (Transform* __this, MethodInfo* method));
DO_APP_FUNC(0x0749BAA0, void, Transform_set_position, (Transform* __this, Vector3 value, MethodInfo* method));

DO_APP_FUNC(0x04A171C0, String*, EntityExcelConfig_GetName, (EntityExcelConfig* __this, MethodInfo* method));
DO_APP_FUNC(0x0154C5A0, String*, BaseEntity_ToStringRelease, (BaseEntity* __this, MethodInfo* method));
DO_APP_FUNC(0x03629D00, Vector3, LocalEntityInfoData_GetTargetPos, (LocalEntityInfoData* __this, MethodInfo* method));
DO_APP_FUNC(0x0165B8B0, bool, LoadingManager_IsFarTeleport, (LoadingManager* __this, uint32_t sceneId, Vector3 position, MethodInfo* method));
DO_APP_FUNC(0x0306D160, void, GameManager_Update, (GameManager* __this, MethodInfo* method));

DO_APP_FUNC(0x04D632C0, void, VCAnimatorMove_EnableCrashDamage, (VCAnimatorMove* __this, bool enable, MethodInfo* method));
DO_APP_FUNC(0x04564740, VCBaseMove*, BaseEntity_GetMoveComponent, (BaseEntity* __this, MethodInfo* method));
DO_APP_FUNC_METHODINFO(0x0B25D518, BaseEntity_GetBaseMoveComponent__MethodInfo);
DO_APP_FUNC_METHODINFO(0x0B25DCF0, BaseEntity_GetHumanoidMoveComponent__MethodInfo);
DO_APP_FUNC_METHODINFO(0x0B25E0B8, BaseEntity_GetAnimatorMoveComponent__MethodInfo);

DO_APP_FUNC(0x040EE200, void, VCBaseMove_HLODFCLMIFJ, (VCBaseMove* __this, bool enable, MethodInfo* method));
DO_APP_FUNC(0x040EB0E0, void, VCBaseMove_BMOMHINEADB, (VCBaseMove* __this, bool enable, MethodInfo* method));


DO_APP_FUNC(0x02ADE320, void, LCBaseCombat_FireBeingHitEvent, (LCBaseCombat* __this, uint32_t attackeeRuntimeID, AttackResult * attackResult, MethodInfo * method));
//DO_APP_FUNC(0x02ADE560, void, LCBaseCombat_FireBeingHitEvent_1, (LCBaseCombat* __this, PIGHJNDOIBE * evtBeingHitInfo, MethodInfo * method));
//DO_APP_FUNC(0x02ADE9D0, void, LCBaseCombat_FireBeingHitEvent_2, (LCBaseCombat* __this, IAEJBOCALMN * evt, MethodInfo * method));
DO_APP_FUNC(0x04597790, void, EvtFallOnGround_Init, (EvtFallOnGround* __this, uint32_t targetID, EvtFallOnGround_GroundType__Enum type, Vector3 velocity, float waterDepth, MethodInfo* method));
DO_APP_FUNC(0x04D22280, void, VCHumanoidMove_NotifyLandVelocity, (VCHumanoidMove* __this, Vector3 velocity, float reachMaxDownVelocityTime, MethodInfo* method));

DO_APP_FUNC(0x065A04E0, void*, Singleton_GetInstance, (void* __this, MethodInfo* method));
DO_APP_FUNC_METHODINFO(0x0B20F1E8, Singleton_1_MBHLOBDPKEC__get_Instance__MethodInfo);
DO_APP_FUNC_METHODINFO(0x0B1C5540, Singleton_1_LoadingManager__get_Instance__MethodInfo);
DO_APP_FUNC_METHODINFO(0x0B18EF88, Singleton_1_EntityManager__get_Instance__MethodInfo);