#ifndef NEO_HUD_GHOST_MARKER_H
#define NEO_HUD_GHOST_MARKER_H
#ifdef _WIN32
#pragma once
#endif

#include "neo_hud_childelement.h"
#include "hudelement.h"
#include <vgui_controls/Panel.h>

class CNEOHud_GhostMarker : public CNEOHud_ChildElement, public CHudElement, public vgui::Panel
{
	DECLARE_CLASS_SIMPLE(CNEOHud_GhostMarker, Panel);

public:
	CNEOHud_GhostMarker(const char *pElemName, vgui::Panel *parent = NULL);

	virtual void Paint();

	void SetGhostingTeam(int team);
	void SetScreenPosition(int x, int y);
	void SetGhostDistance(float distance);

private:
	int m_iMarkerTexWidth, m_iMarkerTexHeight;
	int m_iPosX, m_iPosY;
	int m_iGhostingTeam;

	float m_flDistMeters;

	vgui::HTexture m_hTex;

	vgui::HFont m_hFont;
};

#endif // NEO_HUD_GHOST_MARKER_H