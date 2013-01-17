#include <windows.h>
#include <process.h>
#include "falclib.h"
#include "f4thread.h"

typedef struct
{
   int inUse;
   HANDLE handle;
   unsigned int  id;
} F4THREAD_TYPE;

static F4THREAD_TYPE F4Thread[F4MAX_THREADS] = {0};

BOOL F4TryEnterCriticalSection (F4CSECTIONHANDLE* theSection);

F4THREADHANDLE F4CreateThread (LPTHREAD_START_ROUTINE threadFunc, void* arg1, Int32 threadFlags)
{
F4THREADHANDLE retval;
DWORD cFlags = 0;
int i;

   // Find unused thread slot
   for (i=0; i<F4MAX_THREADS; i++)
   {
      if (F4Thread[i].inUse == FALSE)
         break;
   }
   if (i == F4MAX_THREADS)
      retval = 0;
   else
   {
      retval = i;
      if (threadFlags & F4CREATE_SUSPENDED)
         cFlags |= CREATE_SUSPENDED;
      if (threadFlags & F4IDLE_PRIORITY)
         cFlags |= IDLE_PRIORITY_CLASS;
      else if (threadFlags & F4NORMAL_PRIORITY)
         cFlags |= NORMAL_PRIORITY_CLASS;
      else if (threadFlags & F4HIGH_PRIORITY)
         cFlags |= HIGH_PRIORITY_CLASS;
      else if (threadFlags & F4REALTIME_PRIORITY)
         cFlags |= REALTIME_PRIORITY_CLASS;

#ifndef MISSILE_TEST_PROG
      F4Thread[retval].handle = (HANDLE)_beginthreadex (NULL, 0, (unsigned int (__stdcall *)(void*))threadFunc,
         arg1, cFlags, &F4Thread[retval].id);
#endif

      if (F4Thread[retval].handle)
      {
         F4Thread[retval].inUse = TRUE;
         if (threadFlags & F4REALTIME_PRIORITY)
            SetThreadPriority (F4Thread[retval].handle, THREAD_PRIORITY_TIME_CRITICAL);
      }
      else
         retval = F4THREAD_ERROR;
   }

   return (retval);
}

F4THREADHANDLE F4CreateThread (void threadFunc1(void), Int32 threadFlags)
{
   return (F4CreateThread ((LPTHREAD_START_ROUTINE)threadFunc1, NULL, threadFlags));
}

int F4SuspendThread (F4THREADHANDLE theThread)
{
   //F4Assert( "Don't call F4SuspendThread silly!" == NULL );
int retval = F4THREAD_ERROR;

   if (F4Thread[theThread].inUse)
   {
      if (SuspendThread(F4Thread[theThread].handle) != 0xFFFFFFFF)
         retval = F4THREAD_OK;
   }

   return (retval);
}

int F4ResumeThread (F4THREADHANDLE theThread)
{
   //F4Assert( "Don't call F4ResumeThread silly!" == NULL );
int retval = F4THREAD_ERROR;

   if (F4Thread[theThread].inUse)
   {
      if (ResumeThread(F4Thread[theThread].handle) != 0xFFFFFFFF)
         retval = F4THREAD_OK;
   }

   return (retval);
}

int F4DestroyThread (F4THREADHANDLE theThread, Int32 exitCode)
{
int retval = F4THREAD_ERROR;

//   F4Assert (FALSE);
   if (F4Thread[theThread].inUse)
   {
      if (TerminateThread(F4Thread[theThread].handle, exitCode))
      {
         F4Thread[theThread].inUse = FALSE;
         retval = F4THREAD_OK;
      }
   }

   return (retval);
}

int F4DestroyAllThreads (void)
{
int i;
int retval = F4THREAD_OK;

   for (i=0; i<F4MAX_THREADS; i++)
   {
      if (F4Thread[i].inUse)
      {
         if (TerminateThread(F4Thread[i].handle, 0))
            F4Thread[i].inUse = FALSE;
         else
            retval = F4THREAD_OK;
      }
   }

   return (retval);
}

void F4DestroyCriticalSection (F4CSECTIONHANDLE* theSection)
{
   if (theSection)
   {
      F4Assert (theSection->owningThread == (HANDLE)-2 ||
         theSection->owningThread == (HANDLE)-1 );
      F4Assert (theSection->count == 0)
      DeleteCriticalSection (&(theSection->criticalSection));
      delete theSection->name;
      delete (theSection);
			theSection = NULL; // JB 001203
   }
}

void F4EnterCriticalSection (F4CSECTIONHANDLE* theSection)
{
   if (theSection)
   {
      EnterCriticalSection (&(theSection->criticalSection));
      theSection->count ++;
      theSection->owningThread = (HANDLE)GetCurrentThreadId();
#ifdef _TIMEDEBUG
      if (theSection->count == 1)
	  theSection->time = GetTickCount();
#endif
   }
}

BOOL F4TryEnterCriticalSection (F4CSECTIONHANDLE* theSection)
{
   HANDLE tid = (HANDLE)GetCurrentThreadId();

   if (theSection)
   {
      if ( (int)theSection->owningThread < 0 || theSection->owningThread == tid )
	  {
      	 EnterCriticalSection (&(theSection->criticalSection));
      	 theSection->count ++;
      	 theSection->owningThread = tid;
#ifdef _TIMEDEBUG
		 if (theSection->count == 1)
			 theSection->time = GetTickCount();
#endif
		 return TRUE;
	  }
   }

   return FALSE;
}

// JPO - utility routine to check that a routine that
// expects to be in critical section really is.
bool F4CheckHasCriticalSection(F4CSECTIONHANDLE* theSection)
{
    HANDLE tid = (HANDLE)GetCurrentThreadId();
    
    if (theSection && 
	theSection->owningThread == tid &&
	theSection->count > 0)
	return true;
    return false;
}

void F4LeaveCriticalSection (F4CSECTIONHANDLE* theSection)
{
   if (theSection)
   {
      theSection->count --;
      if (theSection->count == 0)
      {
         F4Assert (theSection->owningThread == (HANDLE)GetCurrentThreadId());
         theSection->owningThread = (HANDLE)-2;
      }
      LeaveCriticalSection (&(theSection->criticalSection));
#ifdef _TIMEDEBUG
      static int interesttime = 100;
      DWORD endt = GetTickCount();
      int time = endt - theSection->time;
	  
      if (theSection->count == 0 &&time > interesttime) {
		  MonoPrint ("Has held critical section %s for %d\n", theSection->name, time);
      }
#endif

   }
}

F4CSECTIONHANDLE* F4CreateCriticalSection (char *name)
{
F4CSECTIONHANDLE* theSection;

   theSection = new F4CSECTIONHANDLE;

   if (theSection)
   {
      memset (&(theSection->criticalSection), 0, sizeof (CRITICAL_SECTION));
      InitializeCriticalSection (&(theSection->criticalSection));
      theSection->owningThread = (HANDLE)-1;
      theSection->count = 0;
      theSection->name = new char[strlen(name) + 1];
      strcpy(theSection->name, name);
   }

   return (theSection);

}

int F4GetNumProcessors (void)
{
int i;
DWORD pmask, smask;

   GetProcessAffinityMask (GetCurrentProcess(), &pmask, &smask);

   smask = 0;
   for (i=0; i<32; i++)
      if (pmask & (1 << i))
         smask ++;

   return (smask);
}

int F4SetThreadProcessor (F4THREADHANDLE theThread, int theProcessor)
{
   if (SetThreadAffinityMask (F4Thread[theThread].handle, (DWORD)(1 << (theProcessor - 1))))
      return (F4THREAD_OK);
   else
      return (F4THREAD_ERROR);
}
