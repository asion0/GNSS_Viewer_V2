#include "stdafx.h"
#include "GPS.h"
#include "MaskedBitmap.h"
#include "GPSDlg.h"
#include "UISetting.h"
#include "SnrBarChart.h"
#include "Pic_Earth.h"

static CSize GpsBarChartBg(685, 170);
static int StartGpsBarChartX = 3;
static int GpsBarChartGapX = 22;

static CSize GnssBarChartBg(685, 94);
static int StartGnssBarChartX = 3;
static int StartGnssBarChartY = 170;
static int GnssBarChartGapX = 22;

static int MaxGpsId = 61;
static int HalfMaxGpsId = (MaxGpsId + 1) / 2;
static int GnssIdStart = 65;

CGPSDlg::PrnType CGPSDlg::GetPrnType(int id)
{
	if(id <= 51 || (id >= 193 && id <= 197))
	{
		return Gps;
	}

	if(id >= 65 && id <= 96)
	{
		return Glonass;
	}

	return Unknown;
}

int CGPSDlg::CreateSubMenu(const HMENU hMenu, const MenuItemEntry* menuItemTable, LPCSTR pszSubMenuText)
{
	HMENU hMenuPopup = CreateMenu();

	const MenuItemEntry *p = menuItemTable;
	while(p->type || p->id || p->pszText || p->subMenu)
	{
		if(!p->showOption)
		{
			++p;
			continue;
		}
		if(MF_SEPARATOR == p->type)
		{
			AppendMenu(hMenuPopup, MF_SEPARATOR, 0, NULL);
		}
		else if(MF_STRING == p->type)
		{
			AppendMenu(hMenuPopup, MF_STRING,  p->id, p->pszText);
		}
		else if(MF_POPUP == p->type)
		{
			CreateSubMenu(hMenuPopup, p->subMenu, p->pszText);
		}
		else
		{
			ASSERT(FALSE);
		}
		++p;
	}
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hMenuPopup, pszSubMenuText);
	return 0;
}
