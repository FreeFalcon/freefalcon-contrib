#include "stdafx.h"
#include "LODConvert.h"
#include "LodInfo.h"
#include "Converter.h"
#include ".\lodinfo.h"
#include "Encoder.h"
#include ".\ctselector.h"

CCTSelector::CCTSelector()
{
	Create(IDD_CTSELECTOR, NULL);
	ShowWindow(SW_SHOW);
}

CCTSelector::~CCTSelector(void)
{
}
