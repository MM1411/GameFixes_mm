//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: CBaseEntity
//
//=============================================================================

#ifndef CBASEENTITY_H
#define CBASEENTITY_H
#ifdef _WIN32
#pragma once
#endif

#include "CBodyComponent.h"
#include "CCollisionProperty.h"
#include "sdk/schemasystem_helper.h"

#include <const.h>
#include <entityhandle.h>
#include <entityinstance.h>
#include <mathlib/vector.h>

class CBaseEntity : public CEntityInstance
{
public:
    SCHEMA_FIELD(CBodyComponent*, CBaseEntity, m_CBodyComponent);
    SCHEMA_FIELD(uint32_t, CBaseEntity, m_fFlags);
    SCHEMA_FIELD(MoveType_t, CBaseEntity, m_MoveType);
    SCHEMA_FIELD(MoveType_t, CBaseEntity, m_nActualMoveType);
    SCHEMA_FIELD(Vector, CBaseEntity, m_vecAbsVelocity);
    SCHEMA_FIELD(CCollisionProperty*, CBaseEntity, m_pCollision);
    SCHEMA_FIELD(CEntityHandle, CBaseEntity, m_hOwnerEntity);
    SCHEMA_FIELD(CEntityHandle, CBaseEntity, m_hGroundEntity);
};

#endif // CBASEENTITY_H
