#include "threadpool.h"
ThreadPool::ThreadPool() {
    // Initialize some variables
    m_is_init = false;
    m_is_closing = false;
    // Initialize mutex
    pthread_mutex_init(&m_mutex, 0);
    // Initialize the cond
    pthread_cond_init(&m_cond, 0);
}

ThreadPool::~ThreadPool() {
    // Free variables
    pthread_mutex_destroy(&m_mutex);
    pthread_cond_destroy(&m_cond);
}

bool ThreadPool::init(const int thread_count) {
    pthread_mutex_lock(&m_mutex);

    if(m_is_init) {
        put_error("Thread pool already initialized");
        pthread_mutex_unlock(&m_mutex);
        return false;
    }

    // Create threads
    for(int i=0; i<thread_count; ++i) {
        pthread_t pthr = 0;
        pthread_create(&pthr, 0, ThreadPool::thread_process, this);
        m_thrs.push_back(pthr);
    }

    m_is_init = true;

    pthread_mutex_unlock(&m_mutex);
    return true;
}

void ThreadPool::destroy() {
    pthread_mutex_lock(&m_mutex);

    m_is_closing = true;
    pthread_cond_broadcast(&m_cond);

    pthread_mutex_unlock(&m_mutex);

    // Wait for threads destroied
    for(deque<pthread_t>::const_iterator iter=m_thrs.begin();
            iter!=m_thrs.end(); ++iter) {
        pthread_join(*iter, 0);
    }
}
bool ThreadPool::add_event(const ThreadPool::event ent)
{

    if(!m_is_init) {
        put_error("Thread pool dosen't initialized");
        pthread_mutex_unlock(&m_mutex);
        return false;
    } else if(m_is_closing) {
        put_error("Thread pool is closing");
        pthread_mutex_unlock(&m_mutex);
        return false;
    }

    // Push event
    m_events.push_back(ent);
    // Notice threads
    if(m_events.size() == 1) {
        pthread_cond_signal(&m_cond);
    } else {
        pthread_cond_broadcast(&m_cond);
    }

    pthread_mutex_unlock(&m_mutex);
    return true;
}
bool ThreadPool::add_event(CALLBACK_FN callback, void *args) {
    pthread_mutex_lock(&m_mutex);

    if(!callback) {
        put_error("Callback function can't be null");
        return false;
    }

    if(!m_is_init) {
        put_error("Thread pool dosen't initialized");
        pthread_mutex_unlock(&m_mutex);
        return false;
    } else if(m_is_closing) {
        put_error("Thread pool is closing");
        pthread_mutex_unlock(&m_mutex);
        return false;
    }

    event ent;
    ent.callback = callback;
    ent.args = args;

    // Push event
    m_events.push_back(ent);
    // Notice threads
    if(m_events.size() == 1) {
        pthread_cond_signal(&m_cond);
    } else {
        pthread_cond_broadcast(&m_cond);
    }

    pthread_mutex_unlock(&m_mutex);
    return true;
}

void ThreadPool::put_error(const string err) {
    cout << "ThreadPool:" << err << endl;
}  

void* ThreadPool::thread_process(void *m_this) {
    ThreadPool *ptp = static_cast<ThreadPool*>(m_this);

    while(1) {
        pthread_mutex_lock(&ptp->m_mutex);

        // Wait for notice
        while(ptp->m_events.size()==0 && !ptp->m_is_closing) {
            pthread_cond_wait(&ptp->m_cond, &ptp->m_mutex);
        }

        if(ptp->m_is_closing && ptp->m_events.size()==0) {
            pthread_mutex_unlock(&ptp->m_mutex);
        }

        // Get event
        ThreadPool::event ent = ptp->m_events[0];
        ptp->m_events.pop_front();

        pthread_mutex_unlock(&ptp->m_mutex);

        // Process event
        ent.callback(ent.args);
    }

    pthread_mutex_unlock(&ptp->m_mutex);
    return 0;
}

