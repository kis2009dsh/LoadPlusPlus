//
//  PressureFrame.cpp
//  PresureTest
//
//  Created by dongshihao on 13-9-22.
//
//

#include "PressureFrame.h"

#include <cassert>

/*
 * /!
 *    1. managerThread
 *    2. countTimeThread
 *    3. controlThread
 *    4. analysisThread
 * \!
 */
static const size_t InternalTaskNumber = 8 ;

void PressureTest::InitializePressureTest(int vuser, int time, int secs_per_vuser)
{
    m_vuser = vuser ;
    m_time  = time ;
    m_secs_per_vuser = secs_per_vuser;
    
    /// Create Flags
    m_should_continue.reserve(vuser);
    
    for(int i = 0 ; i < vuser ; ++i)
    {
        setShouldContinue(i, true);
    };
    
    m_none_should_continue = false ;
    
    /// Create Thread Pool
    m_threadpool.Start(m_vuser + InternalTaskNumber);
    
    setCurrentUser(0);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void PressureTest::StartAsync()
{
    ThreadPool::TaskType mgrFunc = std::bind(&PressureTest::managerThread , this);
    ThreadPool::TaskType cntFunc = std::bind(&PressureTest::countTimeThread , this);
    ThreadPool::TaskType ctrlFunc = std::bind(&PressureTest::controlThread , this);
    ThreadPool::TaskType analyFunc = std::bind(&PressureTest::analysisThread ,this);
    
    /// controll thread  
    /// Vitrual User 
    /// Count Time
    /// Analysis
    m_worklist.push_front(std::thread(ctrlFunc));
    m_worklist.push_front(std::thread(mgrFunc));
    m_worklist.push_front(std::thread(cntFunc));
    m_worklist.push_front(std::thread(analyFunc));
}

void PressureTest::RegisiterPerSecondCallBack(CallBackType f)
{
    m_call_back = f ;
}

void PressureTest::RegisiterResultCallBack(ResultCallBackType f)
{
    m_result_call_back = f ;
}

void PressureTest::WaitForTaskCompleted()
{
    for(auto& i : m_worklist)
        i.join();
    
    /// No Work Task
//    qDebug() << "No Work Task\n";
    
    while (m_threadpool.PendingTask()) {
        continue;
    }
    
    /// No Pending Task
//    qDebug() << "No Pending Task\n";
    m_threadpool.Stop();
}

void PressureTest::managerThread()
{
    setCurrentUser(1);
    
    /// Add first vuser
    ThreadPool::TaskType firstWorkFunc = std::bind(&PressureTest::workThread , this , 0);
    
//    m_threadpool.AddTask(firstWorkFunc);
    addTaskToPool(firstWorkFunc);
    
    for(int currentId = 1 ; currentId < m_vuser ; ++currentId)
    {
        // Add vuser
        ThreadPool::TaskType currentWorkFunc = std::bind(&PressureTest::workThread , this , currentId);
        
        /// Sleep 1 second
        std::this_thread::sleep_for(std::chrono::seconds(m_secs_per_vuser));
        
//        m_threadpool.AddTask(currentWorkFunc);
        addTaskToPool(currentWorkFunc);
        
        incCurrentUser(1);
    }
}

void PressureTest::countTimeThread()
{
    double total_time = (2 * m_vuser - 1) * m_secs_per_vuser + m_time;
    
    m_timer.Reset();
    
    while (true)
    {
        double currentTime = m_timer.ElapsedSeconds();
        
        for(int i = 0 ; i < m_vuser ; ++i)
        {
            double endTimeI = total_time - i * m_secs_per_vuser ;
            
            if( currentTime >= endTimeI )
            {
                setShouldContinue(i , false);
            }
        }
        
        /// End of time
        if (currentTime >= total_time)
        {
            qDebug() << "m_none_should_continue = true\n";
            m_none_should_continue = true ;
            break ;
        }
    }
}

void PressureTest::analysisThread()
{
    int current = 0;
    
    std::pair<int,int>       record(0 , 0) ;   /// ( success , error )
    std::pair<double,double> response_time_range  (std::numeric_limits<double>::max() , 0.0);
    std::pair<int,int>       concurrent_conn_range(std::numeric_limits<int>::max() , 0);
    std::pair<int,int>       response_time_range_second (1,1);
    std::pair<int,int>       concurrent_conn_range_second(1,1);
    
    double total_time = ( 2 * m_vuser - 1 ) * m_secs_per_vuser + m_time ;
    int    temp_current_user = 0 ;
    
    while ( (temp_current_user = getCurrentUser()) )
    {
        /// Sleep 1 second
        waitUntil(current);
        
        int concurrent_connections = 0;
        double average_response_time = 0.0;
        
        { /// Read from m_record
            std::lock_guard<std::mutex> lock( m_mutex_record );
            
            extendRecord(current);
            
            if( !m_record[current].empty() )
            {
                for(const auto& test_result : m_record[current])
                {
                    bool isSuccess = test_result.GetSuccess();
                    
                    concurrent_connections  += isSuccess ? 1 : 0;
                    average_response_time += isSuccess ? test_result.GetResponseTime() : 0.0 ;
                    
                    isSuccess ? ++record.first : ++record.second ;
                }
                
                if( concurrent_connections > 0 )
                    average_response_time /= concurrent_connections;
                
                std::less<double> comparePred ;
                
                /// Get (min,max) of concurrent connections
                if( comparePred(concurrent_connections , concurrent_conn_range.first) )
                {
                    concurrent_conn_range.first = concurrent_connections ;
                    concurrent_conn_range_second.first = current - 1;
                }
                
                if( !comparePred(concurrent_connections , concurrent_conn_range.second) )
                {
                    concurrent_conn_range.second = concurrent_connections ;
                    concurrent_conn_range_second.second = current - 1;
                }
                
                /// Get (min,max) of response time
                if( comparePred(average_response_time , response_time_range.first ) )
                {
                    response_time_range.first = average_response_time ;
                    response_time_range_second.first = current - 1;
                }
                
                if( !comparePred(average_response_time , response_time_range.second ) )
                {
                    response_time_range.second = average_response_time ;
                    response_time_range_second.second = current - 1;
                }

            }
        }
        
        if(m_call_back)
            m_call_back( concurrent_connections , average_response_time , record.first , record.second , temp_current_user );
        
        ++current;
    }
    
    if(m_result_call_back)
        m_result_call_back(concurrent_conn_range.first , concurrent_conn_range.second, response_time_range.first , response_time_range.second);
}

void PressureTest::workThread(const int thread_id)
{
    const std::vector<TestCaseInfo>& test_case_list = m_case_list;
    
    /// All Read Here
    int index = 0 ;
    
    while( true )
    {
        if( m_none_should_continue )
        {
            break ;
        }
        
        if( !getShouldContinue(thread_id) )
        {
            break;
        }
        
        /// Iteration
        for(const auto& test_case : test_case_list)
        {
            if (! getShouldContinue(thread_id) )
            {
                break;
            }
            
            /// Should Rendezvous
            if( test_case.GetRendezvous() )
            {
                /// Arrived Rendevzous
                arrivedNotify();
                
                //// Wait
                {
                    std::unique_lock<std::mutex> lock(m_mutex_wait);
                    m_cond.wait(lock);
                }
                
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            
            HttpRequestMethod method = ((test_case.GetPost() == "") ? (HttpRequestMethod::GET) : (HttpRequestMethod::POST));
            
            HttpRequest  request( test_case.GetUrl() , method , test_case.GetPost());
            
            //// User Agent ////
            request.SetItem("User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_8_2) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/28.0.1500.95 Safari/537.36");
            
            HttpResponse response ;
            std::string  error ;
            
            HttpConnection::SendHttpRequestWithoutBody(request, response, error);
            
            /// Should Record
            if( test_case.GetRecord() )
            {
                size_t currentSecond = getCurrentSecond();
                
                std::lock_guard<std::mutex> lock( m_mutex_record );
                
                extendRecord(currentSecond);
                
                m_record[currentSecond].emplace_back( response.GetStatusCode() == test_case.GetExpectValue() , currentSecond , response.GetTotalTime());
                
                if( response.GetStatusCode() != test_case.GetExpectValue())
                    qDebug() << "Error Code : " << response.GetStatusCode() << "\n";
            }
        }
    }
    
    /// End of Work
    decCurrentUser(1);
}

