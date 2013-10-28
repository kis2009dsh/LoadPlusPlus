//
//  ThreadPool.h
//  rapidjson_test
//
//  Created by dongshihao on 13-9-12.
//  Copyright (c) 2013年 dongshihao. All rights reserved.
//

#ifndef __rapidjson_test__ThreadPool__
#define __rapidjson_test__ThreadPool__

#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
#include <map>
#include <vector>
#include <list>
#include <mutex>
#include <memory>
#include <condition_variable>

#include <cassert>

//class semaphore
//{
//public:
//    semaphore(int value = 1): m_count{value}, m_wakeups{0} {}
//    
//    void Wait();
//    
//    void Signal() ;
//    
//private:
//    int m_count;
//    int m_wakeups;
//    std::mutex m_mutex;
//    std::condition_variable m_condition;
//};

class ThreadPool
{
public:
    
    typedef std::function<void(void)> TaskType ;
    
public:
    
    ThreadPool() : m_running(false) 
    {
        /// Nothing
    }
    
    ~ThreadPool()
    {
        if( m_running.load() )
            Stop() ;
    }
    
public:
    
    ///
    ///  Create Thread Pool
    ///  @param thread_num    number of thread
    ///  @brief Call runInThread 
    ///
    void Start( size_t thread_mum ) ;
    
    ///
    ///  Add Task to Pool
    ///  @param task
    ///  @brief if Pool.empty() then Run it directly
    ///
    void AddTask( TaskType task ) ;
    
    ///
    ///  Stop All Thread in Pool
    ///
    void Stop() ;
    
    ///
    /// Non Busy 
    ///
    inline
    size_t PendingTask() const
    {
        return m_task.size() ;
    }
    
private:
    
    void runInThread(size_t thread_id) ;
    
//    TaskType getTask() ;
    
    std::atomic<bool>        m_running;
    std::mutex               m_mutex ;
    std::mutex               m_mutex_wait;
    std::mutex               m_mutex_print;
    std::mutex               m_mutex_busy;
    std::condition_variable  m_cond  ;
    std::list<std::thread>   m_thread;
    std::queue<TaskType>     m_task ;
};

//class ThreadImpl
//{
//public:
//    
//    typedef std::function<void(void)> TaskType;
//    
//    ThreadImpl() :m_thread(NULL), m_finish(false)
//    {
//        ////
//    }
//    
//    ~ThreadImpl()
//    {
//        if( m_thread != NULL )
//            delete m_thread;
//    }
//    
//private:
//    
//    ThreadImpl(const ThreadImpl& );
//    
//    ThreadImpl& operator = (const ThreadImpl&) ;
//    
//public:
//    
//    ///
//    /// @brief Set m_finish to be true and wake up main loop thread
//    ///
//    void Terminate();
//    
//    ///
//    /// @brief Join this thread
//    ///        call Terminate() first
//    ///
//    void Join();
//    
//    ///
//    /// @brief Get load task number
//    ///
//    size_t GetLoad() ;
//    
//    ///
//    /// @brief Append a task to queue and wake up main loop thread
//    ///
//    void AppendTask( ThreadImpl::TaskType task ) ;
//    
//    ///
//    /// @brief Start Main Loop Thread
//    ///
//    void Start() ;
//    
//protected:
//    
//    ///
//    /// @brief Main Thread in Background
//    ///
//    void runInThread() ;
//    
//protected:
//    
//    std::thread           * m_thread ;
//    std::condition_variable m_cond_incoming_task;
//    std::atomic<bool>       m_finish ;
//    std::mutex              m_mutex_cond;
//    std::mutex              m_mutex_task ;
//    std::queue<TaskType>    m_task_queue ;
//};
//
//struct MinLoadStrategy
//{
//    
//};
//
//template<typename Strategy>
//class ThreadPool
//{
//public:
//    
//    typedef typename ThreadImpl::TaskType TaskType ;
//    
//    ThreadPool( const int nThread )
//    {
//        m_pool.reserve(nThread , NULL);
//        
//        for(int i = 0 ; i < nThread ; ++i)
//        {
//            m_pool[i] = std::make_shared<ThreadImpl>();
//        }
//    }
//    
//    ~ThreadPool()
//    {
//        ////
//    }
//    
//public:
//    
//    ///
//    /// @brief Start All Main Loop
//    ///
//    void Start() ;
//    
//    ///
//    /// @brief Add Task
//    ///
//    void AddTask() ;
//    
//    ///
//    /// @brief Load
//    ///
//    size_t GetLoad() ;
//
//    
//    
//private:
//    
//    Strategy      m_strategy;
//    std::mutex    m_mutex_task;
//    std::vector<std::shared_ptr<ThreadImpl>> m_pool;
//};


#endif /* defined(__rapidjson_test__ThreadPool__) */
