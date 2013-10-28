//
//  TestCase.h
//  PresureTest
//
//  Created by dongshihao on 13-9-22.
//
//

#ifndef __PresureTest__TestCase__
#define __PresureTest__TestCase__

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <regex>

///
/// @brief Test Case
///
class TestCaseInfo;

///
/// @brief Test Result
///
class TestResultInfo;

///////////////////////////////////////////

class TestCaseInfo
{
public:
    TestCaseInfo()
    : m_should_record(false) ,
    m_rendezvous_before(false) ,
    m_expect_value(200) ,
    m_url("") ,
    m_post_data("")
    {
        //
    }
    
    TestCaseInfo(const std::string& url)
    : m_should_record(false) ,
    m_rendezvous_before(false) ,
    m_expect_value(200) ,
    m_url(url) ,
    m_post_data("")
    {
        //
    }
    
    ~TestCaseInfo() { }
    
public:
    inline void SetRecord(bool isRecord = true) {
        m_should_record = isRecord;
    }
    
    inline bool GetRecord() const {
        return m_should_record;
    }
    
    inline void SetRendezvous( bool isTrue = true) {
        m_rendezvous_before = isTrue ;
    }
    
    inline bool GetRendezvous() const {
        return m_rendezvous_before ;
    }
    
    inline void SetExpectValue(int value = 200) {
        m_expect_value = value;
    }
    
    inline int GetExpectValue() const {
        return m_expect_value;
    }
    
    inline void SetPost(const std::string& post){
        m_post_data = post ;
    }
    
    inline std::string GetPost() const {
        return m_post_data ;
    }
    
    inline void SetUrl( const std::string& url) {
        m_url = url ;
    }
    
    inline std::string GetUrl() const {
        return m_url;
    }
    
protected:
    
    bool        m_should_record;
    bool        m_rendezvous_before ;
    int         m_expect_value ;
    std::string m_url ;
    std::string m_post_data;
};

/////////////////////////////////////

class TestResultInfo
{
public:
    
    TestResultInfo() : m_success(false)
    {
        //
    }
    
    TestResultInfo(bool value , double current , double last)
    :m_success(value),
    m_current_time(current),
    m_last_time(last)
    {
        ///
    }
    
    ~TestResultInfo() { }
    
public:
    
    inline double GetCurrentTime() const {
        return m_current_time;
    }
    
    inline double GetResponseTime() const {
        return m_last_time ;
    }
    
    inline double GetSuccess() const {
        return m_success;
    }
    
protected:
    bool   m_success      ;
    double m_current_time ;
    double m_last_time    ;
};


#endif /* defined(__PresureTest__TestCase__) */
