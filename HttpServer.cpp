//
//  HttpServer.cpp
//  rapidjson_test
//
//  Created by dongshihao on 13-8-21.
//  Copyright (c) 2013年 dongshihao. All rights reserved.
//

#include "HttpServer.h"

#include <cassert>
#include <vector>
#include "Algorithm.h"

///////////////////////////////////////////////////////////////////////////////////////////////

static size_t WriteResponseCallbackFunction(void* buffer, size_t size, size_t nmemb, void* lpVoid) {
    
    if( NULL == lpVoid || NULL == buffer )
    {
        return -1 ;
    }
    
    std::string response = std::move(*(std::string *)lpVoid);
    
    response.append( (const char *)buffer , size * nmemb );
    
    *(std::string *)lpVoid = std::move(response);
    
    return size * nmemb;
}

static size_t WriteHeaderCallbackFunction( void * buffer, size_t size , size_t nmemb , void * lpVoid) {
    
    if( NULL == lpVoid || NULL == buffer )
    {
        return  -1;
    }
    
    std::string header = std::move(*(std::string *)lpVoid);
    
    header.append( (const char *)buffer , size * nmemb );
    
    *(std::string *)lpVoid = std::move(header);
    
    return size * nmemb ;
}

static size_t EmptyWriteResponseCallbackFunction(void* buffer, size_t size, size_t nmemb, void* lpVoid) {
    return size * nmemb ;
}

static size_t EmptyWriteHeaderCallbackFunction( void * buffer, size_t size , size_t nmemb , void * lpVoid) {
    return size * nmemb;
}

static void SolveHeaderTestFunction(const curl_slist * header )
{
    while (header) {
        std::cout << header->data << std::endl;
        header = header->next;
    }
}

std::mutex HttpConnection::g_send_mutex ;

std::string HttpConnection::SendSyncHttpRequest(const HttpRequest &httpRequest, HttpResponse &httpResponse, std::string &error , bool verbose) throw(HttpConnectionException)
{
    const char * pHttpPrefix = "http://";
    
    for(size_t i = 0 ; *(pHttpPrefix + i) != '\0' ; ++i )
    {
        if( i == httpRequest.m_url.length() || httpRequest.m_url[i] != *(pHttpPrefix + i) )
            throw HttpConnectionException("URL Format isn't begin with \"http://\"");
    }
    
    return sendSyncHttpRequestWithoutVerify(httpRequest, httpResponse, error , verbose);
}

std::string HttpConnection::SendAsyncHttpRequest(std::shared_ptr<const HttpRequest> pRequest, std::shared_ptr<HttpResponse> pResponse, std::shared_ptr<std::string> pError) throw (HttpConnectionException)
{
    const char * pHttpPrefix = "http://";
    
    for(size_t i = 0 ; *(pHttpPrefix + i) != '\0' ; ++i )
    {
        if( i == pRequest->m_url.length() || pRequest->m_url[i] != *(pHttpPrefix + i) )
            throw HttpConnectionException("URL Format isn't begin with \"http://\"");
    }
    
    return sendAsyncHttpRequestWithoutVerify(pRequest , pResponse , pError);
}

std::string HttpConnection::SendHttpRequest(const HttpRequest &httpRequest, HttpResponse &httpResponse, std::string &error,bool verbose) throw(HttpConnectionException)
{
    const char * pHttpPrefix = "http://";
    
    for(size_t i = 0 ; *(pHttpPrefix + i) != '\0' ; ++i )
    {
        if( i == httpRequest.m_url.length() || httpRequest.m_url[i] != *(pHttpPrefix + i) )
            throw HttpConnectionException("URL Format isn't begin with \"http://\"");
    }
    
    return sendHttpRequestWithoutVerify(httpRequest, httpResponse, error , verbose);
}

void HttpConnection::SendSyncHttpRequestWithoutBody(const HttpRequest &httpRequest, HttpResponse &httpResponse, std::string &error , bool verbose ) throw(HttpConnectionException)
{
    const char * pHttpPrefix = "http://";
    
    for(size_t i = 0 ; *(pHttpPrefix + i) != '\0' ; ++i )
    {
        if( i == httpRequest.m_url.length() || httpRequest.m_url[i] != *(pHttpPrefix + i) )
            throw HttpConnectionException("URL Format isn't begin with \"http://\"");
    }
    
    sendSyncHttpRequestWithoutBodyWithoutVerify(httpRequest, httpResponse, error, verbose);
}

void HttpConnection::SendAsyncHttpRequestWithoutBody(std::shared_ptr<const HttpRequest> pRequest, std::shared_ptr<HttpResponse> pResponse, std::shared_ptr<std::string> pError) throw(HttpConnectionException)
{
    const char * pHttpPrefix = "http://";
    
    for(size_t i = 0 ; *(pHttpPrefix + i) != '\0' ; ++i )
    {
        if( i == pRequest->m_url.length() || pRequest->m_url[i] != *(pHttpPrefix + i) )
            throw HttpConnectionException("URL Format isn't begin with \"http://\"");
    }
    
    sendAsyncHttpRequestWithoutBodyWithoutVerify(pRequest , pResponse , pError);
}

void HttpConnection::SendHttpRequestWithoutBody(const HttpRequest &httpRequest, HttpResponse &httpResponse, std::string &error , bool verbose) throw(HttpConnectionException)
{
    const char * pHttpPrefix = "http://";
    
    for(size_t i = 0 ; *(pHttpPrefix + i) != '\0' ; ++i )
    {
        if( i == httpRequest.m_url.length() || httpRequest.m_url[i] != *(pHttpPrefix + i) )
            throw HttpConnectionException("URL Format isn't begin with \"http://\"");
    }
    
    sendHttpRequestWithoutBodyWithoutVerify(httpRequest, httpResponse, error, verbose);
}

std::string HttpConnection::sendSyncHttpRequestWithoutVerify(const HttpRequest &httpRequest, HttpResponse &httpResponse, std::string &error , bool verbose)
{
    std::lock_guard<std::mutex> lock( g_send_mutex );
    
    std::shared_ptr<CURL> curl( curl_easy_init() , curl_easy_cleanup );
    
    if( NULL == curl.get() )
    {
        error = curl_easy_strerror(CURLE_FAILED_INIT);
        
        return std::move(std::string(""));
    }
    
    std::string response ;
    std::string header ;
    
//#ifdef DEBUG
//    curl_easy_setopt(curl.get() , CURLOPT_VERBOSE , 1L);
//#endif
    
    curl_easy_setopt(curl.get(), CURLOPT_URL , httpRequest.m_url.c_str() );
    curl_easy_setopt(curl.get(), CURLOPT_READFUNCTION , NULL );
    
    curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION , WriteResponseCallbackFunction );
    curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA , (void *)&response );
    
    curl_easy_setopt(curl.get(), CURLOPT_HEADERFUNCTION , WriteHeaderCallbackFunction );
    curl_easy_setopt(curl.get(), CURLOPT_HEADERDATA , (void *)&header );
    
    curl_easy_setopt(curl.get(), CURLOPT_NOSIGNAL , 1L );
    curl_easy_setopt(curl.get(), CURLOPT_CONNECTTIMEOUT, 5);
	curl_easy_setopt(curl.get(), CURLOPT_TIMEOUT, 5);
    
    if(verbose)
    {
        curl_easy_setopt(curl.get(), CURLOPT_VERBOSE , 1L);
    }
    
    if( httpRequest.m_method == HttpRequestMethod::POST )
    {
        curl_easy_setopt(curl.get(), CURLOPT_POST, 1);
        curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDS , httpRequest.m_http_body.c_str() );
    }
    
    /// Custom Header
    curl_slist * chunk = NULL ;
    addHeader(curl.get(), httpRequest , chunk);
    
    auto ret = curl_easy_perform(curl.get());
    
    cleanHeader(chunk);
    
    if( ret != CURLE_OK )
    {
        error = curl_easy_strerror(ret);
        return std::move(std::string(""));
    }
    
    curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE , &httpResponse.m_status_code );
    curl_easy_getinfo(curl.get(), CURLINFO_TOTAL_TIME , &httpResponse.m_total_time );
    curl_easy_getinfo(curl.get(), CURLINFO_CONNECT_TIME , &httpResponse.m_conn_time );
    curl_easy_getinfo(curl.get(), CURLINFO_NAMELOOKUP_TIME , &httpResponse.m_namelookup_time);
    
    solveHttpFields(header, httpResponse, error);
    
    return response ;
}

std::string HttpConnection::sendAsyncHttpRequestWithoutVerify(std::shared_ptr<const HttpRequest> pRequest, std::shared_ptr<HttpResponse> pResponse, std::shared_ptr<std::string> pError)
{
    std::shared_ptr<CURL> curl( curl_easy_init() , curl_easy_cleanup );
    
    assert(pRequest);
    assert(pResponse);
    assert(pError);
    
    if( NULL == curl.get() )
    {
        pError->assign(curl_easy_strerror(CURLE_FAILED_INIT));
        
        return std::move(std::string(""));
    }
    
    std::string response ;
    std::string header ;
    
//#ifdef DEBUG
//    curl_easy_setopt(curl.get() , CURLOPT_VERBOSE , 1L);
//#endif
    
    curl_easy_setopt(curl.get(), CURLOPT_URL , pRequest->m_url.c_str() );
    curl_easy_setopt(curl.get(), CURLOPT_READFUNCTION , NULL );
    
    curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION , WriteResponseCallbackFunction );
    curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA , (void *)&response );
    
    curl_easy_setopt(curl.get(), CURLOPT_HEADERFUNCTION , WriteHeaderCallbackFunction );
    curl_easy_setopt(curl.get(), CURLOPT_HEADERDATA , (void *)&header );
    
    curl_easy_setopt(curl.get(), CURLOPT_NOSIGNAL , 1L );
    curl_easy_setopt(curl.get(), CURLOPT_CONNECTTIMEOUT, 5);
	curl_easy_setopt(curl.get(), CURLOPT_TIMEOUT, 5);
    
    if( pRequest->m_method == HttpRequestMethod::POST )
    {
        curl_easy_setopt(curl.get(), CURLOPT_POST, 1);
        curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDS , pRequest->m_http_body.c_str() );
    }
    
    /// Custom Header
    curl_slist * chunk = NULL ;
    addHeader(curl.get(), *pRequest , chunk);
    
    auto ret = curl_easy_perform(curl.get());
    
    cleanHeader(chunk);
    
    if( ret != CURLE_OK )
    {
        pError->assign( curl_easy_strerror(ret) );
        return std::move(std::string(""));
    }
    
    curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE , std::addressof(pResponse->m_status_code) );
    curl_easy_getinfo(curl.get(), CURLINFO_TOTAL_TIME , std::addressof(pResponse->m_total_time) );
    curl_easy_getinfo(curl.get(), CURLINFO_CONNECT_TIME , std::addressof(pResponse->m_conn_time) );
    curl_easy_getinfo(curl.get(), CURLINFO_NAMELOOKUP_TIME , std::addressof(pResponse->m_namelookup_time));
    
    solveHttpFields(header, *pResponse , *pError);
    
    return response ;
}

std::string HttpConnection::sendHttpRequestWithoutVerify(const HttpRequest &httpRequest, HttpResponse &httpResponse, std::string &error, bool verbose)
{
    std::shared_ptr<CURL> curl( curl_easy_init() , curl_easy_cleanup );
    
    if( NULL == curl.get() )
    {
        error = curl_easy_strerror(CURLE_FAILED_INIT);
        
        return std::move(std::string(""));
    }
    
    std::string response ;
    std::string header ;
    
    //#ifdef DEBUG
    //    curl_easy_setopt(curl.get() , CURLOPT_VERBOSE , 1L);
    //#endif
    
    curl_easy_setopt(curl.get(), CURLOPT_URL , httpRequest.m_url.c_str() );
    curl_easy_setopt(curl.get(), CURLOPT_READFUNCTION , NULL );
    
    curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION , WriteResponseCallbackFunction );
    curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA , (void *)&response );
    
    curl_easy_setopt(curl.get(), CURLOPT_HEADERFUNCTION , WriteHeaderCallbackFunction );
    curl_easy_setopt(curl.get(), CURLOPT_HEADERDATA , (void *)&header );
    
    curl_easy_setopt(curl.get(), CURLOPT_NOSIGNAL , 1L );
    curl_easy_setopt(curl.get(), CURLOPT_CONNECTTIMEOUT, 5);
	curl_easy_setopt(curl.get(), CURLOPT_TIMEOUT, 5);
    
    if(verbose)
    {
        curl_easy_setopt(curl.get(), CURLOPT_VERBOSE , 1L);
    }
    
    if( httpRequest.m_method == HttpRequestMethod::POST )
    {
        curl_easy_setopt(curl.get(), CURLOPT_POST, 1);
        curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDS , httpRequest.m_http_body.c_str() );
    }
    
    /// Custom Header
    curl_slist * chunk = NULL ;
    addHeader(curl.get(), httpRequest , chunk);
    
    auto ret = curl_easy_perform(curl.get());
    
    cleanHeader(chunk);
    
    if( ret != CURLE_OK )
    {
        error = curl_easy_strerror(ret);
        return std::move(std::string(""));
    }
    
    curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE , &httpResponse.m_status_code );
    curl_easy_getinfo(curl.get(), CURLINFO_TOTAL_TIME , &httpResponse.m_total_time );
    curl_easy_getinfo(curl.get(), CURLINFO_CONNECT_TIME , &httpResponse.m_conn_time );
    curl_easy_getinfo(curl.get(), CURLINFO_NAMELOOKUP_TIME , &httpResponse.m_namelookup_time);
    
    solveHttpFields(header, httpResponse, error);
    
    return response ;
}

void HttpConnection::sendSyncHttpRequestWithoutBodyWithoutVerify(const HttpRequest &httpRequest, HttpResponse &httpResponse, std::string &error, bool verbose)
{
    std::lock_guard<std::mutex> lock( g_send_mutex );
    
    std::shared_ptr<CURL> curl( curl_easy_init() , curl_easy_cleanup );
    
    if( NULL == curl.get() )
    {
        error = curl_easy_strerror(CURLE_FAILED_INIT);
        
        return ;
    }
    
    //#ifdef DEBUG
    //    curl_easy_setopt(curl.get() , CURLOPT_VERBOSE , 1L);
    //#endif
    
    curl_easy_setopt(curl.get(), CURLOPT_URL , httpRequest.m_url.c_str() );
    curl_easy_setopt(curl.get(), CURLOPT_READFUNCTION , NULL );
    
//    curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION , WriteResponseCallbackFunction );
//    curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA , (void *)&response );
    curl_easy_setopt(curl.get() , CURLOPT_WRITEFUNCTION , EmptyWriteResponseCallbackFunction );
    curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA , NULL );
    
//    curl_easy_setopt(curl.get(), CURLOPT_HEADERFUNCTION , WriteHeaderCallbackFunction );
//    curl_easy_setopt(curl.get(), CURLOPT_HEADERDATA , (void *)&header );
    curl_easy_setopt(curl.get(), CURLOPT_HEADERFUNCTION , EmptyWriteHeaderCallbackFunction );
    curl_easy_setopt(curl.get(), CURLOPT_HEADERDATA , NULL );
    
    curl_easy_setopt(curl.get(), CURLOPT_NOSIGNAL , 1L );
    curl_easy_setopt(curl.get(), CURLOPT_CONNECTTIMEOUT, 5);
	curl_easy_setopt(curl.get(), CURLOPT_TIMEOUT, 5);
    
    if(verbose)
    {
        curl_easy_setopt(curl.get(), CURLOPT_VERBOSE , 1L);
    }
    
    if( httpRequest.m_method == HttpRequestMethod::POST )
    {
        curl_easy_setopt(curl.get(), CURLOPT_POST, 1);
        curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDS , httpRequest.m_http_body.c_str() );
    }
    
    /// Custom Header
    curl_slist * chunk = NULL ;
    addHeader(curl.get(), httpRequest , chunk);
    
    auto ret = curl_easy_perform(curl.get());
    
    cleanHeader(chunk);
    
    if( ret != CURLE_OK )
    {
        error = curl_easy_strerror(ret);
        return ;
    }
    
    curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE , &httpResponse.m_status_code );
    curl_easy_getinfo(curl.get(), CURLINFO_TOTAL_TIME , &httpResponse.m_total_time );
    curl_easy_getinfo(curl.get(), CURLINFO_CONNECT_TIME , &httpResponse.m_conn_time );
    curl_easy_getinfo(curl.get(), CURLINFO_NAMELOOKUP_TIME , &httpResponse.m_namelookup_time);
}

void HttpConnection::sendAsyncHttpRequestWithoutBodyWithoutVerify(std::shared_ptr<const HttpRequest> pRequest, std::shared_ptr<HttpResponse> pResponse, std::shared_ptr<std::string> pError)
{
    std::shared_ptr<CURL> curl( curl_easy_init() , curl_easy_cleanup );
    
    assert(pRequest);
    assert(pResponse);
    assert(pError);
    
    if( NULL == curl.get() )
    {
        pError->assign(curl_easy_strerror(CURLE_FAILED_INIT));
        
        return ;
    }
    
    //#ifdef DEBUG
    //    curl_easy_setopt(curl.get() , CURLOPT_VERBOSE , 1L);
    //#endif
    
    curl_easy_setopt(curl.get(), CURLOPT_URL , pRequest->m_url.c_str() );
    curl_easy_setopt(curl.get(), CURLOPT_READFUNCTION , NULL );
    
//    curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION , WriteResponseCallbackFunction );
//    curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA , (void *)&response );
    curl_easy_setopt(curl.get() , CURLOPT_WRITEFUNCTION , EmptyWriteResponseCallbackFunction );
    curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA , NULL );
    
//    curl_easy_setopt(curl.get(), CURLOPT_HEADERFUNCTION , WriteHeaderCallbackFunction );
//    curl_easy_setopt(curl.get(), CURLOPT_HEADERDATA , (void *)&header );
    curl_easy_setopt(curl.get(), CURLOPT_HEADERFUNCTION , EmptyWriteHeaderCallbackFunction );
    curl_easy_setopt(curl.get(), CURLOPT_HEADERDATA , NULL );
    
    curl_easy_setopt(curl.get(), CURLOPT_NOSIGNAL , 1L );
    curl_easy_setopt(curl.get(), CURLOPT_CONNECTTIMEOUT, 5);
	curl_easy_setopt(curl.get(), CURLOPT_TIMEOUT, 5);
    
    if( pRequest->m_method == HttpRequestMethod::POST )
    {
        curl_easy_setopt(curl.get(), CURLOPT_POST, 1);
        curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDS , pRequest->m_http_body.c_str() );
    }
    
    /// Custom Header
    curl_slist * chunk = NULL ;
    addHeader(curl.get(), *pRequest , chunk);
    
    auto ret = curl_easy_perform(curl.get());
    
    cleanHeader(chunk);
    
    if( ret != CURLE_OK )
    {
        pError->assign( curl_easy_strerror(ret) );
        return ;
    }
    
    curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE , std::addressof(pResponse->m_status_code) );
    curl_easy_getinfo(curl.get(), CURLINFO_TOTAL_TIME , std::addressof(pResponse->m_total_time) );
    curl_easy_getinfo(curl.get(), CURLINFO_CONNECT_TIME , std::addressof(pResponse->m_conn_time) );
    curl_easy_getinfo(curl.get(), CURLINFO_NAMELOOKUP_TIME , std::addressof(pResponse->m_namelookup_time) );
}

void HttpConnection::sendHttpRequestWithoutBodyWithoutVerify(const HttpRequest &httpRequest, HttpResponse &httpResponse, std::string &error, bool verbose)
{
    std::shared_ptr<CURL> curl( curl_easy_init() , curl_easy_cleanup );
    
    if( NULL == curl.get() )
    {
        error = curl_easy_strerror(CURLE_FAILED_INIT);
        
        return ;
    }
    
    curl_easy_setopt(curl.get(), CURLOPT_URL , httpRequest.m_url.c_str() );
    curl_easy_setopt(curl.get(), CURLOPT_READFUNCTION , NULL );
    
    curl_easy_setopt(curl.get() , CURLOPT_WRITEFUNCTION , EmptyWriteResponseCallbackFunction );
    curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA , NULL );
    
    curl_easy_setopt(curl.get(), CURLOPT_HEADERFUNCTION , EmptyWriteHeaderCallbackFunction );
    curl_easy_setopt(curl.get(), CURLOPT_HEADERDATA , NULL );
    
    curl_easy_setopt(curl.get(), CURLOPT_NOSIGNAL , 1L );
    curl_easy_setopt(curl.get(), CURLOPT_CONNECTTIMEOUT, 5);
	curl_easy_setopt(curl.get(), CURLOPT_TIMEOUT, 5);
    
    if(verbose)
    {
        curl_easy_setopt(curl.get(), CURLOPT_VERBOSE , 1L);
    }
    
    if( httpRequest.m_method == HttpRequestMethod::POST )
    {
        curl_easy_setopt(curl.get(), CURLOPT_POST, 1);
        curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDS , httpRequest.m_http_body.c_str() );
    }
    
    /// Custom Header
    curl_slist * chunk = NULL ;
    addHeader(curl.get(), httpRequest , chunk);
    
    auto ret = curl_easy_perform(curl.get());
    
    cleanHeader(chunk);
    
    if( ret != CURLE_OK )
    {
        error = curl_easy_strerror(ret);
        return ;
    }
    
    curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE , &httpResponse.m_status_code );
    curl_easy_getinfo(curl.get(), CURLINFO_TOTAL_TIME , &httpResponse.m_total_time );
    curl_easy_getinfo(curl.get(), CURLINFO_CONNECT_TIME , &httpResponse.m_conn_time );
    curl_easy_getinfo(curl.get(), CURLINFO_NAMELOOKUP_TIME , &httpResponse.m_namelookup_time);
}

void HttpConnection::solveHttpFields(std::string& header, HttpResponse &httpResponse, std::string &error)
{
    std::reference_wrapper<HttpResponse::headermap_type> httpFields( httpResponse.m_header_map );

    header.erase(std::remove(header.begin(), header.end(), '\r'), header.end());
    
    std::vector<std::string> header_vec(32);
    
    auto end_iter = split(header, '\n', header_vec.begin() );
    
    for(auto curIter = header_vec.begin() ; curIter != end_iter ; ++curIter)
    {
        auto pos = curIter->find_first_of(':');
        
        if( pos != curIter->npos )
            httpFields.get().insert(std::make_pair(curIter->substr(0,pos), curIter->substr(pos + 2 , curIter->length() - pos - 2)));
    }
}

void HttpConnection::addHeader(CURL *curl, const HttpRequest &httpRequest , curl_slist * chunk )
{
    std::reference_wrapper<const HttpRequest::headermap_type> httpFields( httpRequest.m_header_map );
    
    if(!httpFields.get().empty())
    {
        for(const auto& head_item : httpFields.get())
        {
            chunk = curl_slist_append(chunk, (head_item.first + ": " + head_item.second).c_str() );
        }
        
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    }
}

void HttpConnection::cleanHeader(curl_slist *chunk)
{
    curl_slist_free_all(chunk);
}

