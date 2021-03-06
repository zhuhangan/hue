/************************************************************/
#ifndef _MSC_VER
/************************************************************/


#include <pthread.h>

/* don't include <semaphore.h>, it is not available on OS/X */

typedef struct {
    pthread_mutex_t mutex1;
    pthread_cond_t cond1;
    unsigned int value;
} sem_t;

static int sem_init(sem_t *sem, int pshared, unsigned int value)
{
    assert(pshared == 0);
    sem->value = value;
    return (pthread_mutex_init(&sem->mutex1, NULL) ||
            pthread_cond_init(&sem->cond1, NULL));
}

static int sem_post(sem_t *sem)
{
    pthread_mutex_lock(&sem->mutex1);
    sem->value += 1;
    pthread_cond_signal(&sem->cond1);
    pthread_mutex_unlock(&sem->mutex1);
    return 0;
}

static int sem_wait(sem_t *sem)
{
    pthread_mutex_lock(&sem->mutex1);
    while (sem->value == 0)
        pthread_cond_wait(&sem->cond1, &sem->mutex1);
    sem->value -= 1;
    pthread_mutex_unlock(&sem->mutex1);
    return 0;
}


/************************************************************/
#else
/************************************************************/


/* Very quick and dirty, just what I need for these tests.
   Don't use directly in any real code! 
*/

#include <Windows.h>
#include <assert.h>

typedef HANDLE sem_t;
typedef HANDLE pthread_t;

static int sem_init(sem_t *sem, int pshared, unsigned int value)
{
    assert(pshared == 0);
    assert(value == 0);
    *sem = CreateSemaphore(NULL, 0, 999, NULL);
    return *sem ? 0 : -1;
}

static int sem_post(sem_t *sem)
{
    return ReleaseSemaphore(*sem, 1, NULL) ? 0 : -1;
}

static int sem_wait(sem_t *sem)
{
    WaitForSingleObject(*sem, INFINITE);
    return 0;
}

static DWORD WINAPI myThreadProc(LPVOID lpParameter)
{
    void *(* start_routine)(void *) = (void *(*)(void *))lpParameter;
    start_routine(NULL);
    return 0;
}

static int pthread_create(pthread_t *thread, void *attr,
                          void *start_routine(void *), void *arg)
{
    assert(arg == NULL);
    *thread = CreateThread(NULL, 0, myThreadProc, start_routine, 0, NULL);
    return *thread ? 0 : -1;
}


/************************************************************/
#endif
/************************************************************/
