#include "kernel/srr.h"
#include "kernel/syscall.h"
#include "pthread.h"


//---------------------------------------------------------------------------
// Channels
//---------------------------------------------------------------------------
sysCallUser1r(int, channelCreate,          unsigned, iFlags);
sysCallUser1r(int, channelDestroy,         int, iChannelID);
sysCallUser4r(int, channelConnectAttach,   uint32_t, iNodeID, pid_t, iProcessID, int, iChannelID, int, iFlags);
sysCallUser1r(int, channelConnectDetach,   int, iConnectionID);

//---------------------------------------------------------------------------
// Names
//---------------------------------------------------------------------------
sysCallUser2r(int, registerName,           int, channelID, const char *, name);
sysCallUser3r(int, lookupName,             const char *, name, int *, pid, int *, channelID);

//---------------------------------------------------------------------------
// Threads
//---------------------------------------------------------------------------
sysCallUser4r(int, pthread_create,         pthread_t *, thread, const pthread_attr_t *, attr, pthread_func_entry, start_routine, void *, arg);
sysCallUser1 (     pthread_exit,           void *, status);
sysCallUser2r(int, pthread_join,           pthread_t, thread, void **, value_ptr);

//---------------------------------------------------------------------------
// Condition
//---------------------------------------------------------------------------
sysCallUser2r(int, pthread_cond_init,      pthread_cond_t *, cond, const pthread_condattr_t *, attr);
sysCallUser1r(int, pthread_cond_destroy,   pthread_cond_t *, cond);
sysCallUser2r(int, pthread_cond_wait,      pthread_cond_t *, cond, pthread_mutex_t *, mutex);
sysCallUser3r(int, pthread_cond_timedwait, pthread_cond_t *, cond, pthread_mutex_t *, mutex, const struct timespec *, ts);
sysCallUser1r(int, pthread_cond_signal,    pthread_cond_t *, cond);
sysCallUser1r(int, pthread_cond_broadcast, pthread_cond_t *, cond);

//---------------------------------------------------------------------------
// Mutex
//---------------------------------------------------------------------------
sysCallUser2r(int, pthread_mutex_init,     pthread_mutex_t *, mutex, const pthread_mutexattr_t *, attr);
sysCallUser1r(int, pthread_mutex_destroy,  pthread_mutex_t *, mutex);
sysCallUser1r(int, pthread_mutex_lock,     pthread_mutex_t *, mutex);
sysCallUser1r(int, pthread_mutex_trylock,  pthread_mutex_t *, mutex);
sysCallUser1r(int, pthread_mutex_unlock,   pthread_mutex_t *, mutex);

//---------------------------------------------------------------------------
// Other
//---------------------------------------------------------------------------
//sysCallUser1r(int,             brk, void *, addr);
//sysCallUser1 (                 _exit, int, iStatus);
sysCallUser0r(pid_t,           getpid);
//sysCallUser1r(char *,          sbrk, intptr_t, increment);
sysCallUser1r(unsigned int,    sleep, unsigned int, iSeconds);
sysCallUser1r(int,             usleep, useconds_t, useconds);
