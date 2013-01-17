#pragma once

#include "resource.h"

typedef	struct	{
	
	CString	CommandName;
	CString (*Exe)( char*);
	bool	Modify;

}	ParseItem;

