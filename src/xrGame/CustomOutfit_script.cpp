#include "pch_script.h"
#include "CustomOutfit.h"
#include "ActorHelmet.h"
#include "xrScriptEngine/ScriptExporter.hpp"
#include "ActorBackpack.h"
#include "ActorUnvest.h"

using namespace luabind;

SCRIPT_EXPORT(CCustomOutfit, (CGameObject), {
    module(luaState)
    [
        class_<CCustomOutfit, CGameObject>("CCustomOutfit")
            .def(constructor<>()),

        class_<CHelmet, CGameObject>("CHelmet")
            .def(constructor<>()),

        class_<CBackpack, CGameObject>("CBackpack")
            .def(constructor<>()),

        class_<CUnvest, CGameObject>("CUnvest")
            .def(constructor<>())
    ];
});
