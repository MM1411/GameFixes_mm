//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: CCollisionProperty
//
//=============================================================================

#ifndef CCOLLISIONPROPERTY_H
#define CCOLLISIONPROPERTY_H
#ifdef _WIN32
#pragma once
#endif

#include "VPhysicsCollisionAttribute_t.h"
#include "schemasystem_helper.h"

class CCollisionProperty
{
public:
    SCHEMA_FIELD(VPhysicsCollisionAttribute_t, CCollisionProperty, m_collisionAttribute);
    SCHEMA_FIELD(uint8, CCollisionProperty, m_CollisionGroup);
};

#endif // CCOLLISIONPROPERTY_H
