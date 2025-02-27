#include "stdafx.h"
#include "specific_character.h"

#ifdef XRGAME_EXPORTS
#include "PhraseDialog.h"
#include "string_table.h"

SSpecificCharacterData::SSpecificCharacterData()
{
    m_sGameName.clear();
    m_sBioText = NULL;
    m_sVisual.clear();
    m_sSupplySpawn.clear();
    m_sNpcConfigSect.clear();

    m_StartDialog = NULL;
    m_ActorDialogs.clear();

    m_Rank = NO_RANK;
    m_Reputation = NO_REPUTATION;

    money_def.inf_money = false;
    money_def.max_money = 0;
    money_def.min_money = 0;

    rank_def.min = NO_RANK;
    rank_def.max = NO_RANK;

    reputation_def.min = NO_REPUTATION;
    reputation_def.max = NO_REPUTATION;

    m_bNoRandom = false;
    m_bDefaultForCommunity = false;
    m_fPanic_threshold = 0.0f;
    m_fHitProbabilityFactor = 1.f;
    m_crouch_type = 0;
    m_upgrade_mechanic = false;
}

SSpecificCharacterData::~SSpecificCharacterData() {}
#endif

CSpecificCharacter::CSpecificCharacter() { m_OwnId = nullptr; }
CSpecificCharacter::~CSpecificCharacter() {}
void CSpecificCharacter::InitXmlIdToIndex()
{
    if (!id_to_index::tag_name)
        id_to_index::tag_name = "specific_character";
    if (!id_to_index::file_str)
        id_to_index::file_str = pSettings->r_string("profiles", "specific_characters_files");
}

void CSpecificCharacter::Load(shared_str id)
{
    R_ASSERT(id.size());
    m_OwnId = id;
    inherited_shared::load_shared(m_OwnId, nullptr);
}

void CSpecificCharacter::load_shared(LPCSTR)
{
#if 0
	CTimer			timer;
	timer.Start		();
#endif
    const ITEM_DATA& item_data = *id_to_index::GetById(m_OwnId);

    CUIXml* pXML = item_data._xml;

    pXML->SetLocalRoot(pXML->GetRoot());

    XML_NODE item_node = pXML->NavigateToNode(id_to_index::tag_name, item_data.pos_in_file);
    R_ASSERT3(item_node, "specific_character id=", *item_data.id);

    pXML->SetLocalRoot(item_node);

    int norandom = pXML->ReadAttribInt(item_node, "no_random", 0);
    if (1 == norandom)
        data()->m_bNoRandom = true;
    else
        data()->m_bNoRandom = false;

    int team_default = pXML->ReadAttribInt(item_node, "team_default", 0);
    if (1 == team_default)
        data()->m_bDefaultForCommunity = true;
    else
        data()->m_bDefaultForCommunity = false;

    R_ASSERT3(!(data()->m_bNoRandom && data()->m_bDefaultForCommunity),
        "cannot set 'no_random' and 'team_default' flags simultaneously, profile id", *shared_str(item_data.id));

#ifdef XRGAME_EXPORTS

    LPCSTR start_dialog = pXML->Read("start_dialog", 0, NULL);
    if (start_dialog)
    {
        data()->m_StartDialog = start_dialog;
    }
    else
        data()->m_StartDialog = NULL;

    int dialogs_num = pXML->GetNodesNum(pXML->GetLocalRoot(), "actor_dialog");
    data()->m_ActorDialogs.clear();
    for (int i = 0; i < dialogs_num; ++i)
    {
        shared_str dialog_name = pXML->Read(pXML->GetLocalRoot(), "actor_dialog", i, "");
        data()->m_ActorDialogs.push_back(dialog_name);
    }

    data()->m_icon_name = pXML->Read("icon", 0, "ui_npc_u_barman");

    //игровое имя персонажа
    data()->m_sGameName = pXML->Read("name", 0, "");
    data()->m_sBioText = StringTable().translate(pXML->Read("bio", 0, ""));

    data()->m_fPanic_threshold = pXML->ReadFlt("panic_threshold", 0, 0.f);
    data()->m_fHitProbabilityFactor = pXML->ReadFlt("hit_probability_factor", 0, 1.f);
    data()->m_crouch_type = pXML->ReadInt("crouch_type", 0, 0);
    data()->m_upgrade_mechanic = (pXML->ReadInt("mechanic_mode", 0, 0) == 1);

    data()->m_critical_wound_weights = pXML->Read("critical_wound_weights", 0, "1");

#endif

	data()->first_visual = pXML->ReadAttribInt("visual", 0, "first_visual", -1);
    data()->last_visual = pXML->ReadAttribInt("visual", 0, "last_visual", -1);
    int rnd_vis{};

    data()->m_sVisual = pXML->Read("visual", 0, "");

    if (data()->first_visual != -1 && data()->last_visual != -1)
    {
        data()->first_visual = _min(data()->first_visual, data()->last_visual);
        data()->last_visual = _max(data()->last_visual, data()->first_visual);
    }

#ifdef XRGAME_EXPORTS
    data()->m_sSupplySpawn = pXML->Read("supplies", 0, "");

    if (!data()->m_sSupplySpawn.empty())
    {
        xr_string& str = data()->m_sSupplySpawn;
        xr_string::size_type pos = str.find("\\n");

        while (xr_string::npos != pos)
        {
            str.replace(pos, 2, "\n");
            pos = str.find("\\n", pos + 1);
        }
    }

    data()->m_sNpcConfigSect = pXML->Read("npc_config", 0, "");
    data()->m_sound_voice_prefix = pXML->Read("snd_config", 0, "");

    data()->m_terrain_sect = pXML->Read("terrain_sect", 0, "");

#endif

    data()->m_Classes.clear();
    int classes_num = pXML->GetNodesNum(pXML->GetLocalRoot(), "class");
    for (int i = 0; i < classes_num; i++)
    {
        LPCSTR char_class = pXML->Read("class", 0, "");
        if (char_class)
        {
            char* buf_str = xr_strdup(char_class);
            xr_strlwr(buf_str);
            data()->m_Classes.push_back(buf_str);
            xr_free(buf_str);
        }
    }

#ifdef XRGAME_EXPORTS

    LPCSTR team = pXML->Read("community", 0, NULL);
    R_ASSERT3(team != NULL, "'community' field not fulfiled for specific character", *m_OwnId);

    char* buf_str = xr_strdup(team);
    xr_strlwr(buf_str);
    data()->m_Community.set(buf_str);
    xr_free(buf_str);

    if (data()->m_Community.index() == NO_COMMUNITY_INDEX)
        xrDebug::Fatal(DEBUG_INFO, "wrong 'community' '%s' in specific character %s ", team, *m_OwnId);

    int min_rank = pXML->ReadAttribInt("rank", 0, "min", NO_RANK);
    int max_rank = pXML->ReadAttribInt("rank", 0, "max", NO_RANK);
    if (min_rank != NO_RANK && max_rank != NO_RANK)
    {
        RankDef().min = _min(min_rank, max_rank);
        RankDef().max = _max(max_rank, min_rank);
    }
    else
    {
        int rank = pXML->ReadInt("rank", 0, NO_RANK);
        R_ASSERT3(rank != NO_RANK, "'rank' field not fulfiled for specific character", *m_OwnId);
        RankDef().min = rank;
        RankDef().max = rank;
    }
    
    int min_reputation = pXML->ReadAttribInt("reputation", 0, "min", NO_REPUTATION);
    int max_reputation = pXML->ReadAttribInt("reputation", 0, "max", NO_REPUTATION);
    if (min_reputation != NO_REPUTATION && max_reputation != NO_REPUTATION)
    {
        ReputationDef().min = _min(min_reputation, max_reputation);
        ReputationDef().max = _max(max_reputation, min_reputation);
    }
    else
    {
        int rep = pXML->ReadInt("reputation", 0, NO_REPUTATION);
        R_ASSERT3(rep != NO_REPUTATION, "'reputation' field not fulfiled for specific character", *m_OwnId);
        ReputationDef().min = rep;
        ReputationDef().max = rep;
    }

    if (pXML->NavigateToNode(pXML->GetLocalRoot(), "money", 0))
    {
        MoneyDef().min_money = pXML->ReadAttribInt("money", 0, "min");
        MoneyDef().max_money = pXML->ReadAttribInt("money", 0, "max");
        MoneyDef().inf_money = !!pXML->ReadAttribInt("money", 0, "infinitive");
        MoneyDef().max_money = _max(MoneyDef().max_money, MoneyDef().min_money); // :)
    }
    else
    {
        MoneyDef().min_money = 0;
        MoneyDef().max_money = 0;
        MoneyDef().inf_money = false;
    }

#endif

#if 0
	Msg			("CSpecificCharacter::load_shared() takes %f milliseconds",timer.GetElapsed_sec()*1000.f);
#endif
}

#ifdef XRGAME_EXPORTS

LPCSTR CSpecificCharacter::Name() const { return data()->m_sGameName.c_str(); }
shared_str CSpecificCharacter::Bio() const { return data()->m_sBioText; }
const CHARACTER_COMMUNITY& CSpecificCharacter::Community() const { return data()->m_Community; }
LPCSTR CSpecificCharacter::SupplySpawn() const { return data()->m_sSupplySpawn.c_str(); }
LPCSTR CSpecificCharacter::NpcConfigSect() const { return data()->m_sNpcConfigSect.c_str(); }
LPCSTR CSpecificCharacter::sound_voice_prefix() const { return data()->m_sound_voice_prefix.c_str(); }
float CSpecificCharacter::panic_threshold() const { return data()->m_fPanic_threshold; }
float CSpecificCharacter::hit_probability_factor() const { return data()->m_fHitProbabilityFactor; }
int CSpecificCharacter::crouch_type() const { return data()->m_crouch_type; }
bool CSpecificCharacter::upgrade_mechanic() const { return data()->m_upgrade_mechanic; }
LPCSTR CSpecificCharacter::critical_wound_weights() const { return data()->m_critical_wound_weights.c_str(); }
#endif

shared_str CSpecificCharacter::terrain_sect() const { return data()->m_terrain_sect; }
CHARACTER_RANK_VALUE CSpecificCharacter::Rank() const { return data()->m_Rank; }
CHARACTER_REPUTATION_VALUE CSpecificCharacter::Reputation() const { return data()->m_Reputation; }
LPCSTR CSpecificCharacter::Visual() const 
{ 
	string_path visual_randomized{};
    xr_string visual_name = data()->m_sVisual.c_str();
    int rnd_vis{};

    if (visual_name.back() == '_' && data()->first_visual != -1 && data()->last_visual != -1)
    {
        if (data()->last_visual != data()->first_visual)
            rnd_vis = ::Random.randI(data()->first_visual, data()->last_visual);
        else
            rnd_vis = data()->last_visual;

        strconcat(sizeof(visual_randomized), visual_randomized, visual_name.c_str(), std::to_string(rnd_vis).c_str());
        return visual_randomized;
    }

    return data()->m_sVisual.c_str(); 
}
