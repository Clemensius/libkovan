#include "kovan/thread.hpp"

#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

Mutex::Mutex()
{
#ifdef WIN32
	InitializeCriticalSection(&m_handle);
#else
	pthread_mutex_init(&m_handle, NULL);
#endif
}

Mutex::~Mutex()
{
#ifdef WIN32
	DeleteCriticalSection(&m_handle);
#else
	pthread_mutex_destroy(&m_handle);
#endif
}

void Mutex::lock()
{
#ifdef WIN32
	EnterCriticalSection(&m_handle);
#else
	pthread_mutex_lock(&m_handle);
#endif
}

bool Mutex::tryLock()
{
#ifdef WIN32
	return TryEnterCriticalSection(&m_handle);
#else
	return pthread_mutex_trylock(&m_handle) == 0;
#endif
}

void Mutex::unlock()
{
#ifdef WIN32
	LeaveCriticalSection(&m_handle);
#else
	pthread_mutex_unlock(&m_handle);
#endif
}

Mutex::Mutex(const Mutex &)
{
}

static void *__runThread(void *data)
{
	Thread *t = reinterpret_cast<Thread *>(data);
	if(!t) return NULL;
	t->run();
	return NULL;
}


Thread::Thread()
#ifdef WIN32
	: m_thread(-1)
#else
	: m_started(false)
#endif
{
	
}

Thread::~Thread()
{
#ifdef WIN32
	if(m_thread != INVALID_HANDLE) CloseHandle(m_thread);
#else
	m_started = false;
	pthread_cancel(m_thread);
#endif
}

void Thread::start()
{
#ifdef WIN32
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)__runThread,
		reinterpret_cast<LPVOID>(this), 0, NULL);
#else
	if(m_started) return;
	pthread_create(&m_thread, NULL, &__runThread,
		reinterpret_cast<void *>(this));
	m_started = true;
#endif
}

void Thread::join()
{
#ifdef WIN32
	WaitForSingleObject(m_thread, INFINITE);
#else
	if(!m_started) return;
	pthread_join(m_thread, NULL);
	m_started = false;
#endif
}