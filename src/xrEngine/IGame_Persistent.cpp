#include "stdafx.h"
#pragma hdrstop

#include "IGame_Persistent.h"
#include "GameFont.h"
#include "PerformanceAlert.hpp"

#ifndef _EDITOR
#include "Environment.h"
#include "x_ray.h"
#include "IGame_Level.h"
#include "XR_IOConsole.h"
#include "Render.h"
#include "PS_instance.h"
#include "CustomHUD.h"
#endif

#include "editor_environment_manager.hpp"

ENGINE_API IGame_Persistent* g_pGamePersistent = nullptr;

bool IGame_Persistent::IsMainMenuActive()
{
    return g_pGamePersistent && g_pGamePersistent->m_pMainMenu && g_pGamePersistent->m_pMainMenu->IsActive();
}

bool IGame_Persistent::MainMenuActiveOrLevelNotExist()
{
    return !g_pGameLevel || g_pGamePersistent->m_pMainMenu && g_pGamePersistent->m_pMainMenu->IsActive();
}

IGame_Persistent::IGame_Persistent()
{
    RDEVICE.seqAppStart.Add(this);
    RDEVICE.seqAppEnd.Add(this);
    RDEVICE.seqFrame.Add(this, REG_PRIORITY_HIGH + 1);
    RDEVICE.seqAppActivate.Add(this);
    RDEVICE.seqAppDeactivate.Add(this);

    m_pMainMenu = nullptr;

    if (RDEVICE.editor())
        pEnvironment = new editor::environment::manager();
    else
        pEnvironment = new CEnvironment();

    m_pGShaderConstants = new ShadersExternalData(); //--#SM+#--
}

IGame_Persistent::~IGame_Persistent()
{
    RDEVICE.seqFrame.Remove(this);
    RDEVICE.seqAppStart.Remove(this);
    RDEVICE.seqAppEnd.Remove(this);
    RDEVICE.seqAppActivate.Remove(this);
    RDEVICE.seqAppDeactivate.Remove(this);
#ifndef _EDITOR
    xr_delete(pEnvironment);
#endif

    xr_delete(m_pGShaderConstants); //--#SM+#--
}

void IGame_Persistent::OnAppActivate() {}
void IGame_Persistent::OnAppDeactivate() {}
void IGame_Persistent::OnAppStart()
{
#ifndef _EDITOR
    Environment().load();
#endif
}

void IGame_Persistent::OnAppEnd()
{
#ifndef _EDITOR
    Environment().unload();
#endif
    OnGameEnd();

#ifndef _EDITOR
    DEL_INSTANCE(g_hud);
#endif
}

void IGame_Persistent::PreStart(LPCSTR op)
{
    string256 prev_type;
    params new_game_params;
    xr_strcpy(prev_type, m_game_params.m_game_type);
    new_game_params.parse_cmd_line(op);

    // change game type
    if (0 != xr_strcmp(prev_type, new_game_params.m_game_type))
    {
        OnGameEnd();
    }
}
void IGame_Persistent::Start(LPCSTR op)
{
    string256 prev_type;
    xr_strcpy(prev_type, m_game_params.m_game_type);
    m_game_params.parse_cmd_line(op);
    // change game type
    if ((0 != xr_strcmp(prev_type, m_game_params.m_game_type)))
    {
        if (*m_game_params.m_game_type)
            OnGameStart();
#ifndef _EDITOR
        if (g_hud)
            DEL_INSTANCE(g_hud);
#endif
    }
    else
        UpdateGameType();

    VERIFY(ps_destroy.empty());
}

void IGame_Persistent::Disconnect()
{
#ifndef _EDITOR
    // clear "need to play" particles
    destroy_particles(true);

    if (g_hud)
        DEL_INSTANCE(g_hud);
//. g_hud->OnDisconnected ();
#endif
}

void IGame_Persistent::OnGameStart()
{
#ifndef _EDITOR
    SetLoadStageTitle("st_prefetching_objects");
    LoadTitle();
    if (!strstr(Core.Params, "-noprefetch"))
        Prefetch();
#endif
}

#ifndef _EDITOR
void IGame_Persistent::Prefetch()
{
    // prefetch game objects & models
    CTimer timer;
    timer.Start();
    const auto memoryBefore = Memory.mem_usage();

    Log("Loading objects...");
    ObjectPool.prefetch();

    Log("Loading models...");
    GEnv.Render->models_Prefetch();

    Log("Loading textures...");
    GEnv.Render->ResourcesDeferredUpload();

    const auto memoryAfter = Memory.mem_usage() - memoryBefore;

    Msg("* [prefetch] time:   %d ms", timer.GetElapsed_ms());
    Msg("* [prefetch] memory: %d Kb", memoryAfter / 1024);
}
#endif

void IGame_Persistent::OnGameEnd()
{
#ifndef _EDITOR
    ObjectPool.clear();
    GEnv.Render->models_Clear(TRUE);
#endif
}

void IGame_Persistent::OnFrame()
{
#ifndef _EDITOR
    if (!Device.Paused() || Device.dwPrecacheFrame)
        Environment().OnFrame();

    stats.Starting = ps_needtoplay.size();
    stats.Active = ps_active.size();
    stats.Destroying = ps_destroy.size();
    // Play req particle systems
    while (ps_needtoplay.size())
    {
        CPS_Instance* psi = ps_needtoplay.back();
        ps_needtoplay.pop_back();
        psi->Play(false);
    }
    // Destroy inactive particle systems
    while (ps_destroy.size())
    {
        // u32 cnt = ps_destroy.size();
        CPS_Instance* psi = ps_destroy.back();
        VERIFY(psi);
        if (psi->Locked())
        {
            Log("--locked");
            break;
        }
        ps_destroy.pop_back();
        psi->PSI_internal_delete();
    }
#endif
}

void IGame_Persistent::destroy_particles(const bool& all_particles)
{
#ifndef _EDITOR

    ps_needtoplay.clear();

	xr_set<CPS_Instance*>::iterator I = ps_active.begin();
    xr_set<CPS_Instance*>::iterator E = ps_active.end();
    for (; I != E; ++I)
    {
        if (all_particles || (*I)->destroy_on_game_load())
            (*I)->PSI_destroy();
    }
    while (ps_destroy.size())
    {
        CPS_Instance* psi = ps_destroy.back();
        VERIFY(psi);
        VERIFY(!psi->Locked());
        ps_destroy.pop_back();
        psi->PSI_internal_delete();
    }

    VERIFY(ps_needtoplay.empty() && ps_destroy.empty() && (!all_particles || ps_active.empty()));
#endif
}

void IGame_Persistent::OnAssetsChanged()
{
#ifndef _EDITOR
    GEnv.Render->OnAssetsChanged();
#endif
}

void IGame_Persistent::DumpStatistics(IGameFont& font, IPerformanceAlert* alert)
{
    // XXX: move to particle engine
    stats.FrameEnd();
    font.OutNext("Particles:");
    font.OutNext("- starting:   %u", stats.Starting);
    font.OutNext("- active:     %u", stats.Active);
    font.OutNext("- destroying: %u", stats.Destroying);
    stats.FrameStart();
}

bool IGame_Persistent::IsActorInHideout()
{
    if (Device.dwTimeGlobal > m_last_ray_pick_time)
    {
        m_last_ray_pick_time = Device.dwTimeGlobal + 1000;
        collide::rq_result RQ;
        m_isInHideout = !!g_pGameLevel->ObjectSpace.RayPick(Device.vCameraPosition, Fvector{0.f, 1.f, 0.f}, 50.f,
            collide::rqtBoth, RQ, g_pGameLevel->CurrentViewEntity());
    }
    return m_isInHideout;
}
