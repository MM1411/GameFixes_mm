//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: CPlayerPawnComponent
//
//=============================================================================

#ifndef CPLAYERPAWNCOMPONENT_H
#define CPLAYERPAWNCOMPONENT_H
#ifdef _WIN32
#pragma once
#endif

#include "CBasePlayerPawn.h"
#include "schemasystem_helper.h"

class CPlayerPawnComponent
{
public:
    SCHEMA_FIELD(CBasePlayerPawn*, CPlayerPawnComponent, __m_pChainEntity);

    CBasePlayerPawn* GetPawn() { return __m_pChainEntity(); }
};

#endif // CPLAYERPAWNCOMPONENT_H
