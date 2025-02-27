// Rain.h: interface for the CRain class.
//
//////////////////////////////////////////////////////////////////////

#ifndef RainH
#define RainH
#pragma once

#include "xrCDB/xr_collide_defs.h"

// refs
class ENGINE_API IRender_DetailModel;

#include "Include/xrRender/FactoryPtr.h"
#include "Include/xrRender/RainRender.h"
//
class ENGINE_API CEffect_Rain
{
    friend class dxRainRender;

private:
    struct Item
    {
        Fvector P;
        Fvector Phit;
        Fvector D;
        float fSpeed;
        u32 dwTime_Life;
        u32 dwTime_Hit;
        u32 uv_set;
        void invalidate() { dwTime_Life = 0; }
    };
    struct Particle
    {
        Particle *next, *prev;
        Fmatrix mXForm;
        Fsphere bounds;
        float time;
    };
    enum States
    {
        stIdle = 0,
        stWorking
    };

private:
    // Visualization (rain) and (drops)
    FactoryPtr<IRainRender> m_pRender;
    /*
    // Visualization (rain)
    ref_shader SH_Rain;
    ref_geom hGeom_Rain;

    // Visualization (drops)
    IRender_DetailModel* DM_Drop;
    ref_geom hGeom_Drops;
    */

    // Data and logic
    xr_vector<Item> items;
    States state;

    // Particles
    xr_vector<Particle> particle_pool;
    Particle* particle_active;
    Particle* particle_idle;

	// Rain params
    float drop_speed_min;
    float drop_speed_max;
    float drop_length;
    float drop_width;
    float drop_angle;
    float drop_max_wind_vel;
    float drop_max_angle;

    bool m_bWindWorking;

	public:

     // Sounds
     ref_sound snd_Ambient;
     ref_sound snd_Wind;
     ref_sound snd_RainOnMask;

     float rain_hemi = 0.0f;

	private:
    // Utilities
    void p_create();
    void p_destroy();

    void p_remove(Particle* P, Particle*& LST);
    void p_insert(Particle* P, Particle*& LST);
    int p_size(Particle* LST);
    Particle* p_allocate();
    void p_free(Particle* P);

    // Some methods
    void Born(Item& dest, float radius, float speed);
    void Hit(Fvector& pos);
    BOOL RayPick(const Fvector& s, const Fvector& d, float& range, collide::rq_target tgt);
    void RenewItem(Item& dest, float height, BOOL bHit);

public:
    CEffect_Rain();
    ~CEffect_Rain();

    void Render();
    void OnFrame();
    float GetRainHemi() { return rain_hemi; }

    void StopAmbient();
    void SetInvalidateState();
	void InvalidateState()		{ state = stIdle; }
    bool bWinterMode;
};

#endif // RainH
