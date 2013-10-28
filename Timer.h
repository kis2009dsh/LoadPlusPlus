//
//  Timer.h
//  rapidjson_test
//
//  Created by dongshihao on 13-9-3.
//  Copyright (c) 2013年 dongshihao. All rights reserved.
//

#ifndef __rapidjson_test__Timer__
#define __rapidjson_test__Timer__

#include <iostream>
#include <chrono>

class timer {
public:
    timer();
    ~timer() ;
    
    void Reset() ;
    
    ///
    /// @brief return seconds last from Reset() or Constructor 
    ///
    double ElapsedSeconds() const ;
    
    ///
    /// @brief return Milliseconds last from Reset() or Constructor
    ///
    double ElapsedMilliseconds() const ;
    
private:
    std::chrono::system_clock::time_point m_Start ;
    
    inline
    void resetStartTime();

    inline
    std::chrono::system_clock::duration elapsedDuration() const ;
};

#endif /* defined(__rapidjson_test__Timer__) */
