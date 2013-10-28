//
//  PressureFrame.h
//  PresureTest
//
//  Created by dongshihao on 13-9-22.
//
//

#ifndef __PresureTest__PressureFrame__
#define __PresureTest__PressureFrame__

#include <vector>
#include <list>
#include <forward_list>
#include <map>
#include <algorithm>
#include <atomic>

#include <QDebug>

#include "ScriptCore.h"
#include "ThreadPool.h"
#include "TestCase.h"
#include "Timer.h"
#include "HttpServer.h"

class PressureTest
{
public:
    typedef std::function<void(int,double,int,int,int)> CallBackType;
    typedef std::function<void(int,int,double,double)> ResultCallBackType;
    
    PressureTest()
    {
//        regisiterScriptInitialize() ;
    }
    
    ///
    /// @brief Initialize Pressure Test FrameWork
    /// @param vuser :  number of max virtual user
    /// @param time  :  last time of max user
    /// @param secs_per_vuser : seconds of increse 1 vuser
    ///
    void InitializePressureTest( int vuser , int time , int secs_per_vuser ) ;

    ///
    /// @brief Start Pressure Test
    ///        return immediately
    ///
    void StartAsync() ;
    
    ///
    /// @brief Set Test Case list
    /// 
    inline void SetTestCase( const std::vector<TestCaseInfo>& test_case_list )
    {
        m_case_list = test_case_list;
    }
    
    ///
    /// @brief Wait for task end
    ///
    void WaitForTaskCompleted() ;
    
    ///
    /// @brief Regisiter function of Per Second
    ///
    void RegisiterPerSecondCallBack(CallBackType f ) ;
    
    ///
    /// @brief Regisiter function of Result
    ///
    void RegisiterResultCallBack( ResultCallBackType f );
    
protected:
    
    ///
    /// @brief Manager Thread
    ///
    void managerThread();
    
    ///
    /// @brief Count time thread
    ///
    void countTimeThread() ;
    
    ///
    /// @brief Working Thread
    /// @param thread_id : unique and it should be in range [0, vuser) 
    ///
    void workThread(const int thread_id ) ;
    
    ///
    /// @brief Analysis Thread
    ///        call "m_call_back" per second
    ///
    void analysisThread() ;
    
    ///
    /// @brief  Add current vuser
    ///
    inline void arrivedNotify() {
        ++m_current_rendevzous;
    }
    
    ///
    /// @brief  Control Rendevzous
    ///
    inline void controlThread()
    {
        while (!m_none_should_continue)
        {
            if( m_current_rendevzous >= getCurrentUser() )
            {
//                qDebug() << "Current Vuser : " << m_current_vuser << "\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                
                m_current_rendevzous = 0;
                m_cond.notify_all();
            }
        }
        m_cond.notify_all();
    }
    
    ///
    /// @brief Get Current Second
    ///
    inline double getCurrentSecond() const {
        return m_timer.ElapsedSeconds();
    }
    
    ///
    /// @brief Extend record
    ///
    inline void extendRecord(const size_t currentSecond)
    {
        size_t currentRecordSecond = m_record.size();
        
        /// Hasn't <currentSecond>'s elements
        if( currentRecordSecond < currentSecond + 1)
        {
            for( size_t i = currentRecordSecond ; i != currentSecond + 1 ; ++i )
            {
                m_record.emplace_back();
            }
        }
    }
    
    ///
    /// @brief Get should continue
    ///
    inline bool getShouldContinue(const size_t index)
    {
        std::lock_guard<std::mutex> lock( m_mutex_continue );
        return m_should_continue[index];
    }
    
    ///
    /// @brief Set should continue
    ///
    inline void setShouldContinue(const size_t index , const bool value)
    {
        std::lock_guard<std::mutex> lock( m_mutex_continue );
        m_should_continue[index] = value ;
    }
    
    inline int getCurrentUser() {
        std::lock_guard<std::mutex> lock( m_mutex_vuser );
        return m_current_vuser;
    }
    
    inline void setCurrentUser(const int value) {
        std::lock_guard<std::mutex> lock( m_mutex_vuser );
        m_current_vuser = value ;
    }
    
    inline int incCurrentUser(const int offest) {
        std::lock_guard<std::mutex> lock( m_mutex_vuser );
        m_current_vuser += offest;
        return m_current_vuser;
    }
    
    inline int decCurrentUser(const int offest) {
        std::lock_guard<std::mutex> lock( m_mutex_vuser );
        m_current_vuser -= offest;
        return m_current_vuser ;
    }
    
    ///
    /// @brief wait until after current_second
    ///
    inline void waitUntil(const int current_second)
    {
        /// Sleep
        while (getCurrentSecond() <= current_second) {
            continue ;
        }
    }
    
    inline void addTaskToPool( ThreadPool::TaskType f )
    {
        m_threadpool.AddTask(f);
    }
    
    ScriptMgr                      m_script;
    ThreadPool                     m_threadpool ;
    std::condition_variable        m_cond ;
    std::mutex                     m_mutex_record;
    std::mutex                     m_mutex_wait ;
    std::mutex                     m_mutex_continue ;
    std::mutex                     m_mutex_vuser ;
    std::mutex                     m_mutex_thread;
    std::atomic<int>               m_current_rendevzous ;
    std::vector<bool>              m_should_continue ;
    std::atomic<bool>              m_none_should_continue ;
    timer                          m_timer ;
    std::forward_list<std::thread> m_worklist;
    
    int              m_vuser ;
    int              m_time  ;
    int              m_secs_per_vuser ;
    std::atomic_int  m_current_vuser ;
    std::vector<std::vector<TestResultInfo>> m_record ;
    std::vector<TestCaseInfo>                m_case_list ;
    CallBackType                             m_call_back ;
    ResultCallBackType                       m_result_call_back;
};

#endif /* defined(__PresureTest__PressureFrame__) */
