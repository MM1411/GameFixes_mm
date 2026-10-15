//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: VPhysicsCollisionAttribute_t
//
//=============================================================================

#ifndef VPHYSICSCOLLISIONATTRIBUTE_T_H
#define VPHYSICSCOLLISIONATTRIBUTE_T_H
#ifdef _WIN32
#pragma once
#endif

#include "sdk/schemasystem_helper.h"

struct VPhysicsCollisionAttribute_t
{
    SCHEMA_FIELD(uint8, VPhysicsCollisionAttribute_t, m_nCollisionGroup);
    SCHEMA_FIELD(uint64, VPhysicsCollisionAttribute_t, m_nInteractsAs);
    SCHEMA_FIELD(uint64, VPhysicsCollisionAttribute_t, m_nInteractsWith);
    SCHEMA_FIELD(uint64, VPhysicsCollisionAttribute_t, m_nInteractsExclude);
    SCHEMA_FIELD(uint16, VPhysicsCollisionAttribute_t, m_nHierarchyId);
};

#endif // VPHYSICSCOLLISIONATTRIBUTE_T_H
