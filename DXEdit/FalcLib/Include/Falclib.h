// Falcon 4 Project wide header file
#ifndef _PROJECT_INCLUDE_H
#define _PROJECT_INCLUDE_H

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <fcntl.h>
#include <io.h>
#include "debuggr.h"
//#include "shi/shi.h"
//#include "shi/int.h"
//#include "shi/float.h"
//#include "shi/ConvFtoI.h"
//#include "shi/SHIerror.h"
#include "f4error.h"
#include "IsBad.h" // JB 010515

#ifdef USE_SH_POOLS
#include "SmartHeap/Include/shmalloc.h"
#include "SmartHeap/Include/smrtheap.hpp"
#endif

#include "F4vu.h"
#include "mltrig.h"

#endif // _PROJECT_INCLUDE_H          
