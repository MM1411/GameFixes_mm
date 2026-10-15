//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: CBodyComponent
//
//=============================================================================

#ifndef CBODYCOMPONENT_H
#define CBODYCOMPONENT_H
#ifdef _WIN32
#pragma once
#endif

#include "CGameSceneNode.h"
#include "sdk/schemasystem_helper.h"

class CBodyComponent
{
public:
    SCHEMA_FIELD(CGameSceneNode*, CBodyComponent, m_pSceneNode);
};

#endif // CBODYCOMPONENT_H
