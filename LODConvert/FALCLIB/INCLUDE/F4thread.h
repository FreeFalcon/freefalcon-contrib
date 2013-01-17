#ifndef _F4THREADS_H
#define _F4THREADS_H

#include "windows.h"

#define F4MAX_THREADS        20

#define F4THREAD_OK          0
#define F4THREAD_ERROR       -1

#define F4CREATE_SUSPENDED   0x01
#define F4IDLE_PRIORITY      0x02
#define F4NORMAL_PRIORITY    0x04
#define F4HIGH_PRIORITY      0x08
#define F4REALTIME_PRIORITY  0x10

typedef int F4THREADHANDLE;
class F4CSECTIONHANDLE
{
public:
   CRITICAL_SECTION criticalSection;
   HANDLE owningThread;
   int count;
   char *name;
#ifdef _DEBUG
   DWORD time;
#endif
};

// Thread Crontrol
int F4DestroyAllThreads (void);
int F4DestroyThread (F4THREADHANDLE theThread, int exitCode);
int F4ResumeThread (F4THREADHANDLE theThread);
int F4SuspendThread (F4THREADHANDLE theThread);
F4THREADHANDLE F4CreateThread (LPTHREAD_START_ROUTINE threadFunc, void* arg1, int threadFlags);
F4THREADHANDLE F4CreateThread (void threadFunc(void), int threadFlags);

// Critical Sections
void F4DestroyCriticalSection (F4CSECTIONHANDLE* theSection);
void F4EnterCriticalSection (F4CSECTIONHANDLE* theSection);
void F4LeaveCriticalSection (F4CSECTIONHANDLE* theSection);
bool F4CheckHasCriticalSection(F4CSECTIONHANDLE* theSection);
F4CSECTIONHANDLE* F4CreateCriticalSection (char *name);


// Processor Locking
int F4GetNumProcessors (void);
int F4SetThreadProcessor (F4THREADHANDLE theThread, int theProcessor);

#endif

