//
//  CodeConvert.h
//  rapidjson_test
//
//  Created by dongshihao on 13-9-11.
//  Copyright (c) 2013年 dongshihao. All rights reserved.
//

#ifndef __rapidjson_test__CodeConvert__
#define __rapidjson_test__CodeConvert__

#include <iostream>
#include <string>
#include <exception>
#include <memory>
#include <iconv.h>
#include <errno.h>

std::string GBKtoUTF8( std::string buffer );

///
/// @brief GBK
///
struct GBK
{
public:
    static const char * value() {
        return "GBK";
    }
};

///
/// @brief
///
struct UTF8
{
public:
    static const char * value() {
        return "UTF-8";
    }
};

class code_convert_exception : std::exception {
public:
    code_convert_exception() : m_buffer("") { } 
    code_convert_exception(const char * what_) : m_buffer(what_) { }
    
    virtual const char * what() {
        return m_buffer ;
    }
    
private:
    const char * m_buffer ;
};

///
/// @brief  Convert from "From" to "To"
///
class code_convert
{
public:
    template<typename fromCode , typename toCode >
    inline static
    std::string Convert(const std::string& input ) throw (code_convert_exception)
    {
        size_t inlen = input.length();
        size_t outlen = inlen * 4 ;
        
        std::unique_ptr<char[]> inbuffer ( new char [inlen + 1] );
        std::unique_ptr<char[]> outbuffer( new char [outlen + 1] );
        
        iconv_t ic = iconv_open( toCode::value() , fromCode::value() );
        
        std::char_traits<char>::copy(inbuffer.get() , input.c_str() , inlen);
        inbuffer[inlen] = '\0';
        
        char * in_ = inbuffer.get();
        char * out_ = outbuffer.get();
        
        size_t ret = iconv( ic , &in_ , &inlen , &out_ , &outlen );
        
        /// Fail
        if( ret == size_t(-1) )
        {
            throw code_convert_exception(getErrorMsg());
        }
        
        std::string result( outbuffer.get() );
        
        return std::move(result);
    }
    
private:
    
    static inline
    const char * getErrorMsg()
    {
        switch (errno) {
            case E2BIG:
                return "There is not sufficient room at *outbuf." ;
                break;
                
            case EILSEQ:
                return "An invalid multibyte sequence has been encountered in the input." ;
                break;
                
            case EINVAL:
                return "An incomplete multibyte sequence has been encountered in the input.";
            default:
                return "Unknown" ;
                break;
        }
    }
};


#endif /* defined(__rapidjson_test__CodeConvert__) */
