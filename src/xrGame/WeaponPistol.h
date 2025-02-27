#pragma once
//#include "WeaponCustomPistol.h"
#include "WeaponCustomPistolAuto.h"

class CWeaponPistol : public CWeaponCustomPistolAuto
{
    using inherited = CWeaponCustomPistolAuto;

public:
    CWeaponPistol();
    virtual ~CWeaponPistol();

    virtual void Load(LPCSTR section);

    virtual void switch2_Reload();

    virtual void OnShot();
    virtual void OnAnimationEnd(u32 state);
    virtual void net_Destroy();
    virtual void OnH_B_Chield();

    //анимации
    virtual void PlayAnimShow();
    virtual void PlayAnimIdle();
    virtual void PlayAnimIdleMoving();
    virtual void PlayAnimIdleMovingCrouch();
    virtual void PlayAnimIdleSprint();
    virtual void PlayAnimHide();
    virtual void PlayAnimReload();
    virtual void PlayAnimShoot();
    virtual void PlayAnimBore();
    virtual void PlayAnimAim();

    virtual void UpdateSounds();

protected:
    virtual bool AllowFireWhileWorking() { return true; }
};
