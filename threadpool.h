
/*
 *  Name: threadpool
 *  Date: 8-12-2015
 *  Author: Sumkee
 *  Brief: A thread pool which created more than one threads
 *         to process some events
 *
 */

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <pthread.h>
#include <deque>


using namespace std;
typedef void (* CALLBACK_FN)(void*);

class ThreadPool {
public:
    // Event
    typedef struct _event {
        CALLBACK_FN callback;        // Callback function
        void *args;                     // Arguments
    } event;

    ThreadPool();
    ~ThreadPool();

    /*
     *  Name: init  
     *  Brief: Initialize the thread pool
     *  @thread_count: Number of thread, the default value is 4
     *  return: Success return true , else return false 
     */ 
    bool init(const int thread_count=4);
    /*
     *  Name: destroy
     *  Brief: Destroy and free the thread pool after all events done
     *
     */ 
    void destroy();

    /*
     *  Name: add_event
     *  Brief: Add event to event buffer
     *  @ent: The event that include callback function and argument
     *  return: Success return true, else return false
     *
     */
    bool add_event(const ThreadPool::event ent);

	bool add_event(CALLBACK_FN callback, void *args);
private:
    bool m_is_init;                 // Determine that if the thread pool initialized
    pthread_mutex_t m_mutex;        // The mutex for thread-safe
    pthread_cond_t m_cond;          // The cond value
    deque<pthread_t> m_thrs;        // The threads id
    deque<event> m_events;           // The events
    bool m_is_closing;              // When thread pool is closing

    /*
     *  Name: put_error
     *  Brief: Output error string
     *  @err: Error string
     *
     */ 
    void put_error(const string err);

    /*
     *  Name: thread_procee
     *  Brief: Threads wait here for event
     *  @m_this: Point to thread pool because this is static function
     *
     */ 
    static void* thread_process(void *m_this);
};

#endif //THREADPOOL_H
