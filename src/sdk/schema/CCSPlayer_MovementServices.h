//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: CCSPlayer_MovementServices
//
//=============================================================================

#ifndef CCSPLAYER_MOVEMENTSERVICES_H
#define CCSPLAYER_MOVEMENTSERVICES_H
#ifdef _WIN32
#pragma once
#endif

#include "CPlayer_MovementServices_Humanoid.h"
#include "schemasystem_helper.h"

class CCSPlayer_MovementServices : public CPlayer_MovementServices_Humanoid
{
public:
    SCHEMA_FIELD(bool, CCSPlayer_MovementServices, m_bDucked);
};

#endif // CCSPLAYER_MOVEMENTSERVICES_H
