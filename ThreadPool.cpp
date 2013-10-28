//
//  ThreadPool.cpp
//  rapidjson_test
//
//  Created by dongshihao on 13-9-12.
//  Copyright (c) 2013年 dongshihao. All rights reserved.
//

#include "ThreadPool.h"

//#define QT
#ifdef QT
#include <QDebug>
#endif

//void semaphore::Wait()
//{
//    std::unique_lock<std::mutex> lock (m_mutex) ;
//    
//    if ( --m_count < 0)
//    { // count is not enough ?
//        m_condition.wait(lock, [&]()->bool{ return m_wakeups > 0;}); // suspend and wait ...
//        
//        --m_wakeups;  // ok, me wakeup !
//    }
//}
//
//void semaphore::Signal()
//{
//    std::lock_guard<std::mutex> lock (m_mutex) ;
//    
//    if( ++m_count <= 0)
//    { // have some thread suspended ?
//        ++m_wakeups;
//        
//        m_condition.notify_one(); // notify one !
//    }
//}
//
//
///////////////////////

void ThreadPool::Start(size_t thread_mum)
{
    assert(m_thread.empty());
    
    m_running.store(true);
    
    for(size_t i = 0 ; i < thread_mum ; ++i)
    {
        m_thread.push_back( std::thread( std::bind(&ThreadPool::runInThread , this, i) ) );
    }
}

void ThreadPool::AddTask(TaskType task)
{
    assert(!m_thread.empty());
    
    std::lock_guard<std::mutex> lock( m_mutex );
        
    m_task.push( std::move(task) );
    
#ifdef QT
    
    qDebug() << "Add Task\n";
    qDebug() << "Peding : " << PendingTask() << "\n";
    
#endif
    
//    m_cond.notify_all();
}

void ThreadPool::Stop()
{
    m_running.store(false);
//    m_cond.notify_all();
    
    for(auto& i : m_thread)
        i.join();
}

void ThreadPool::runInThread(const size_t thread_id)
{
    try {
        while (m_running.load())
        {
            
//            { /// Wait For WakeUp
//                std::unique_lock<std::mutex> u_lock( m_mutex_wait );
//                m_cond.wait(u_lock);
//            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
            TaskType task;
            
            { //// Get Task
                std::lock_guard<std::mutex> lock( m_mutex );
                
                if ( !m_task.empty() )
                {
#ifdef QT
                    qDebug() << "Wake Up and get valid task!\n";
#endif
                    task = m_task.front();
                    m_task.pop();
                }
            }
            
            /// Run Task
            if( task )
            {
#ifdef QT
                qDebug() << "thread : " << thread_id << " has been executed!\n";
#endif
                task();
            }
        }
        
#ifdef QT
        qDebug() << "thread : " << thread_id << " Exit!\n";
#endif
        
    } catch (const std::exception& e) {
        std::lock_guard<std::mutex> lock ( m_mutex_print );
        std::cout << "Thread Pool Exception : " << e.what() << std::endl;
    }
}

//ThreadPool::TaskType ThreadPool::getTask()
//{
//    { /// Wait For WakeUp
//        std::unique_lock<std::mutex> u_lock( m_mutex_wait );
//        m_cond.wait_for(u_lock , std::chrono::milliseconds(1000));
//    }
//    
//    std::lock_guard<std::mutex> lock( m_mutex );
//    
//    TaskType task;
//    if ( !m_task.empty() )
//    {
//#ifdef QT
//        qDebug() << "Wake Up and get valid task!\n";
//#endif
//        task = m_task.front();
//        m_task.pop();
//    }
//    return task;
//}


////////////////////////////////////////////////////////
//
//void ThreadImpl::Terminate()
//{
//    m_finish = true ;
//    
//    m_cond_incoming_task.notify_one();
//}
//
//void ThreadImpl::Join()
//{
//    /// Let thread no longer run
//    Terminate();
//    
//    if( m_thread != NULL )
//        m_thread->join();
//}
//
//size_t ThreadImpl::GetLoad()
//{
//    std::lock_guard<std::mutex> lock( m_mutex_task );
//    
//    return m_task_queue.size();
//}
//
//void ThreadImpl::Start()
//{
//    if( m_thread == NULL )
//    {
//        auto mainFunc = std::bind(&ThreadImpl::runInThread , this);
//        m_thread = new std::thread( mainFunc );
//    }
//}
//
//void ThreadImpl::AppendTask(ThreadImpl::TaskType task)
//{
//    { /// Put Task into Queue
//        std::lock_guard<std::mutex> lock( m_mutex_task );
//        m_task_queue.push(task);
//    }
//    
//    /// Wake Up Main thread
//    m_cond_incoming_task.notify_one();
//}
//
//void ThreadImpl::runInThread()
//{
//    while ( !m_finish )
//    {
//        if( !GetLoad() )
//        {
//            /// Wait for Task come or terminal
//            std::unique_lock<std::mutex> lock ( m_mutex_cond );
//            
//            m_cond_incoming_task.wait_for(lock , std::chrono::milliseconds(5000));
//        }
//        
//        if( m_finish )
//        {
//            break ;
//        }
//        
//        /// (5000 ms) or (Task come) or (Terminate) or (Unknown Reason)
//        
//        ThreadImpl::TaskType task ;
//        
//        { /// Get Task  thread-safely
//            std::lock_guard<std::mutex> lock( m_mutex_task );
//            
//            if( !m_task_queue.empty() )
//            {
//                task = m_task_queue.front();
//            
//                /// Pop this task
//                m_task_queue.pop();
//            }
//        }
//        
//        /// Do Task
//        if ( task )
//        {
//            task();
//        }
//    }
//}
//

/////////////////////////////////////////////////////////


