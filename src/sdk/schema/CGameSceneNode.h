//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: CGameSceneNode
//
//=============================================================================

#ifndef CGAMESCENENODE_H
#define CGAMESCENENODE_H
#ifdef _WIN32
#pragma once
#endif

#include "schemasystem_helper.h"

#include <mathlib/vector.h>

class CGameSceneNode
{
public:
    SCHEMA_FIELD(CGameSceneNode*, CGameSceneNode, m_pParent);
    SCHEMA_FIELD(Vector, CGameSceneNode, m_vecAbsOrigin);
};

#endif // CGAMESCENENODE_H
