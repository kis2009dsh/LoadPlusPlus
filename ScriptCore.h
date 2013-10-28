//
//  ScriptCore.h
//  PresureTest
//
//  Created by dongshihao on 13-9-18.
//
//

#ifndef __PresureTest__ScriptCore__
#define __PresureTest__ScriptCore__

#include <iostream>
#include <memory>
#include <algorithm>
#include <functional>
#include <vector>
#include <map>
#include <string>

class ScriptMgr
{
public:
    typedef void( * RegisterFuncType )(...);

    inline 
    void RegisterFunction( const std::string& name , RegisterFuncType f )
    {
        m_map.insert( std::make_pair(name , f) );
    }
    
    template<typename ...Args>
    inline
    void CallFunction( const std::string& name , Args&&... args ) throw (std::exception)
    {
        auto funcIter = m_map.find(name);
        
        if( funcIter == m_map.end() )
        {
            throw std::out_of_range("Function out of range");
        }
        
        RegisterFuncType pFunc = NULL;
        
        if( (pFunc = funcIter->second) == NULL )
        {
            throw std::out_of_range( (funcIter->first + " not register").c_str() );
        }
        
        pFunc( std::forward<Args>(args)... );
    }
    
    inline
    void ClearRegisterFunction()
    {
        m_map.clear();
    }
    
private:
    std::map<std::string , RegisterFuncType > m_map ;
};

#endif /* defined(__PresureTest__ScriptCore__) */
