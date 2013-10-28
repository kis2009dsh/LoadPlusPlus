//
//  Algorithm.h
//  rapidjson_test
//
//  Created by dongshihao on 13-9-4.
//  Copyright (c) 2013年 dongshihao. All rights reserved.
//

#ifndef __rapidjson_test__Algorithm__
#define __rapidjson_test__Algorithm__

#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>

template<typename Tdst , typename Tsrc>
Tdst convert( Tsrc str )
{
    Tdst result ;
    std::stringstream convert;
    convert << str ;
    convert >> result ;
    return result;
}

///
/// [begin , dResult)
///
template<typename OutIt>
OutIt split( const std::string& inputStr , const char delim , OutIt dResult )
{
    typedef typename std::iterator_traits<OutIt>::value_type Out_type ;
    
    for(std::string::const_iterator i = inputStr.begin() ; i != inputStr.end() ; ++i )
    {
        if( delim == *i )
            ++dResult ;
        else
            *dResult += *i ;
    }
    
    return ++dResult;
}

template<typename tVal>
std::string to_string(tVal x) {
    return convert<std::string>(x);
}

#endif /* defined(__rapidjson_test__Algorithm__) */
