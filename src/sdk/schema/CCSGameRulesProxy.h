//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: CCSGameRulesProxy
//
//=============================================================================

#ifndef CCSGAMERULESPROXY_H
#define CCSGAMERULESPROXY_H
#ifdef _WIN32
#pragma once
#endif

#include "CBaseEntity.h"
#include "CCSGameRules.h"
#include "sdk/schemasystem_helper.h"

class CCSGameRulesProxy : public CBaseEntity
{
public:
    SCHEMA_FIELD(CCSGameRules*, CCSGameRulesProxy, m_pGameRules);
};

#endif // CCSGAMERULESPROXY_H
