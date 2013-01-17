/*  Application defined data, etc */

/* comment/uncomment this as wished */
/* #define DEBUG_COMMS		 */

#define LOAD_DLLS

//#define CAPI_DEBUG
//#define DEBUG_RECEIVE
//#define DEBUG_SEND

#define COMPRESS_DATA
//#define CAPI_NET_DEBUG_FEATURES

// These compression functions must be defined if COMPRESS_DATA is defined
#ifdef COMPRESS_DATA
#include "utils/Lzss.h"
#define ComAPICompress		LZSS_Compress
#define ComAPIDecompress	LZSS_Expand
#endif


