#pragma once

class IRender;
class IRenderFactory;
class IDebugRender;
class CDUInterface;
struct xr_token;
class IUIRender;
class CGameMtlLibrary;
class CRender;
class CScriptEngine;
class AISpaceBase;
class ISoundManager;
class ui_core;

//extern XRAPI_API IDebugRender* DRender;

class XRAPI_API EngineGlobalEnvironment
{
public:
    IRender* Render;
    IDebugRender* DRender;
    CDUInterface* DU;
    IUIRender* UIRender;
    CGameMtlLibrary* PGMLib;
    IRenderFactory* RenderFactory;
    CScriptEngine* ScriptEngine;
    AISpaceBase* AISpace;
    ISoundManager* Sound;
    ui_core* UICore;

    bool isEditor;
    bool isDedicatedServer;

    int CurrentRenderer;
};

extern XRAPI_API EngineGlobalEnvironment GEnv;
