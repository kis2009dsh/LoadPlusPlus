//
//  CodeConvert.cpp
//  rapidjson_test
//
//  Created by dongshihao on 13-9-11.
//  Copyright (c) 2013年 dongshihao. All rights reserved.
//

#include "CodeConvert.h"

std::string GBKtoUTF8( std::string buffer )
{
    
    iconv_t  ic = iconv_open("UTF-8","GBK");
    
    char * in = new char [buffer.size()];
    std::strcpy(in , buffer.c_str());
    
    char * out = new char [buffer.size() * 4];
    
    char * inbuff = in ;
    char * outbuff = out ;
    
    size_t inlen = buffer.size();
    size_t outlen = 4 * inlen;
    
    auto ret = iconv( ic , &inbuff , (size_t *)&inlen , &outbuff , &outlen);
    
    if( ret == size_t(-1) )
    {
        switch (errno) {
            case E2BIG:
                std::cout << "There is not sufficient room at *outbuf." << std::endl;
                break;
                
            case EILSEQ:
                std::cout << "An invalid multibyte sequence has been encountered in the input." << std::endl;
                break;
                
            case EINVAL:
                std::cout << "An incomplete multibyte sequence has been encountered in the input." << std::endl;
            default:
                std::cout << "Unknown" << std::endl;
                break;
        }
    }
    
    std::string result( out );
    
    delete [] in;
    delete [] out;
    
    iconv_close(ic);
    
    return result;
}