//
//  HttpServer.h
//  rapidjson_test
//
//  Created by dongshihao on 13-8-21.
//  Copyright (c) 2013年 dongshihao. All rights reserved.
//

#ifndef __rapidjson_test__HttpServer__
#define __rapidjson_test__HttpServer__

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <functional>
#include <memory>
#include <mutex>

#include <curl/curl.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum {
    GET = 0 ,
    POST = 1
} HttpRequestMethod;

///
/// @brief  Prefix Declaration
///
class HttpConnection ;

///
/// @brief  Exception
///
class HttpConnectionException
{
public:
    HttpConnectionException(const char * what) : m_what(what) { }
    ~HttpConnectionException() {}
    
    const char * what() const {
        return m_what.c_str();
    }
    
private:
    std::string m_what ;
};

///
/// @brief Http Request
///
class HttpRequest
{
public:
    typedef std::string url_type ;
    typedef std::multimap<std::string,std::string> headermap_type ;
    
public:
    HttpRequest() : m_url("") , m_method(GET) ,  m_http_body("") , m_header_map()
    { }
    
    HttpRequest( const url_type& url , HttpRequestMethod method , const std::string& body = "" )
    : m_url(url) , m_method(method) , m_http_body(body) , m_header_map()
    { }
    
    ~HttpRequest() { }
    
    ///
    /// @method SetURL
    /// @abstract Sets the URL of the receiver.
    /// @param URL The new URL for the receiver.
    ///
    inline
    void SetURL( const std::string& url ) {
        m_url = url ;
    }
    
    ///
    /// @abstract  Move Semantic
    ///
    inline
    void SetURL( std::string&& url ) {
        m_url = std::move(url);
    }
    
    ///
    /// @method SetMethod
    /// @abstract Set the method of request
    /// @param requestMethod to set
    ///
    inline
    void SetMethod( HttpRequestMethod requestMethod ) {
        m_method = requestMethod;
    }
    
    ///
    /// @method SetHttpFields
    /// @abstract Set Http Header Fields
    /// @param    map of http header
    ///
    inline
    void SetHttpFields( const headermap_type& header ) {
        m_header_map = header ;
    }
    
    ///
    /// @abstract Move Semantic
    ///
    inline
    void SetHttpFields( headermap_type&& header ) {
        m_header_map = std::move(header);
    }
    
    ///
    /// @method   SetValueForKey
    /// @abstract Set the value of key
    /// @param    key && value
    /// 
    inline
    void SetValueForKey( const std::string& key , const std::string& mapped ) {
        m_header_map.insert(std::make_pair(key, mapped));
    }
    
    ///
    /// @abstract Move Semantic
    ///
    void SetValueForKey( std::string&& key , std::string&& mapped ) {
        m_header_map.insert(std::move(std::make_pair(std::move(key), std::move(mapped))));
    }

    ///
    /// @method   SetItem
    /// @abstract Set the item 
    /// @param    value
    ///
    void SetItem( const std::string& item ) {
        
        auto pos = item.find_first_of(':');
        
        SetValueForKey(item.substr(0,pos) , item.substr( pos + 2 , item.length() - pos - 2 ) );
        
    }
    
    ///
    /// @abstract  Move Semantic
    ///
    void SetItem( std::string&& item ) {
        
        auto pos = item.find_first_of(':');
        
        SetValueForKey(std::move(item.substr(0,pos)), std::move(item.substr(pos + 2 , item.length() - pos - 2)));
        
    }
    
    ///
    /// @brief Friend Class ; To get private member variables ;
    ///
    friend class HttpConnection ;
    
private:
    url_type           m_url ;
    HttpRequestMethod  m_method ;
    std::string        m_http_body ;
    headermap_type     m_header_map;
};

///
/// @brief HttpResponse 
///
class HttpResponse
{
public:
    typedef int retcode_type ;
    typedef std::multimap<std::string,std::string> headermap_type ;
public:
    HttpResponse() {}
    ~HttpResponse() {}
    
    ///
    /// @method GetHttpFields
    /// @abstract Get All http fields
    /// @return   reference deadline must before HttpResponse !!!
    /// 
    inline
    const headermap_type& GetHttpFields() const {
        return m_header_map;
    }
    
    ///
    /// @method GetValue
    /// @abstract  Get value from key
    /// @return    reference deadline must before HttpResponse !!!
    inline
    const std::string& GetValue( const std::string& key ) const {
        return m_header_map.find(key)->second;
    }
    
    ///
    /// @method  GetStatusCode
    /// @abstract Get the status code of response
    /// @return   status code in libcurl
    ///
    inline
    retcode_type GetStatusCode() const {
        return m_status_code ;
    }
    
    ///
    /// @method GetTotalTime()
    /// @return Total Time for a request
    ///
    inline
    double GetTotalTime() const {
        return m_total_time;
    }
    
    ///
    /// @method GetConnectionTime()
    /// @return Total Time for a request
    ///
    inline
    double GetConnectionTime() const {
        return m_conn_time;
    }
    
    ///
    /// @method GetNameLookUpTime()
    /// @return Total Time for a request
    ///
    inline
    double GetNameLookUpTime() const {
        return m_namelookup_time;
    }
    
    ///
    /// @brief Friend Class ;  To set private member variables
    ///
    friend class HttpConnection ;
    
private:
    headermap_type m_header_map ;
    retcode_type   m_status_code ;
    double         m_total_time ;
    double         m_conn_time ;
    double         m_namelookup_time ;
    
};

///
/// @brief  Http  Connection
///
class HttpConnection
{
public:
    
    ///
    /// @method SendSyncHttpRequest
    /// @abstract   Sync  Send Http request
    /// @return The content of the URL resulting from performing the load, or "" if the load failed.
    ///
    static
    std::string SendSyncHttpRequest( const HttpRequest& httpRequest , HttpResponse& httpResponse , std::string& error , bool verbose = false) throw (HttpConnectionException);
    
    ///
    /// @method SendAsyncHttpRequest
    /// @abstract   Async  Send Http request , Multi thread
    /// @return The content of the URL resulting from performing the load, or "" if the load failed.
    ///
    static
    std::string SendAsyncHttpRequest(std::shared_ptr<const HttpRequest> pRequest , std::shared_ptr<HttpResponse> pResponse , std::shared_ptr<std::string> pError) throw (HttpConnectionException);
    
    ///
    /// @method SendSyncHttpRequest
    /// @abstract   Sync  Send Http request
    /// @return The content of the URL resulting from performing the load, or "" if the load failed.
    ///
    static
    std::string SendHttpRequest( const HttpRequest& httpRequest , HttpResponse& httpResponse , std::string& error , bool verbose = false) throw (HttpConnectionException);
    
    ///
    /// @method SendSyncHttpRequest
    /// @abstract   Sync  Send Http request
    /// @return The content of the URL resulting from performing the load, or "" if the load failed.
    ///
    static
    void SendSyncHttpRequestWithoutBody( const HttpRequest& httpRequest , HttpResponse& httpResponse , std::string& error , bool verbose = false) throw (HttpConnectionException);
    
    ///
    /// @method SendAsyncHttpRequest
    /// @abstract   Async  Send Http request , Multi thread
    /// @return The content of the URL resulting from performing the load, or "" if the load failed.
    ///
    static
    void SendAsyncHttpRequestWithoutBody(std::shared_ptr<const HttpRequest> pRequest , std::shared_ptr<HttpResponse> pResponse , std::shared_ptr<std::string> pError) throw (HttpConnectionException);
    
    ///
    /// @method SendSyncHttpRequest
    /// @abstract   Sync  Send Http request
    /// @return The content of the URL resulting from performing the load, or "" if the load failed.
    ///
    static
    void SendHttpRequestWithoutBody( const HttpRequest& httpRequest , HttpResponse& httpResponse , std::string& error , bool verbose = false) throw (HttpConnectionException);
    
private:
    
    ///
    /// @brief Sync Send Http Request Without Verify
    ///
    static
    std::string sendSyncHttpRequestWithoutVerify(const HttpRequest& httpRequest , HttpResponse& httpResponse , std::string& error , bool verbose);
    
    ///
    /// @method SendAsyncHttpRequest
    /// @abstract   Async  Send Http request , Multi thread
    /// @return The content of the URL resulting from performing the load, or "" if the load failed.
    ///
    static
    std::string sendAsyncHttpRequestWithoutVerify(std::shared_ptr<const HttpRequest> pRequest , std::shared_ptr<HttpResponse> pResponse , std::shared_ptr<std::string> pError) ;
    
    ///
    /// @brief Sync Send Http Request Without Verify
    ///
    static
    std::string sendHttpRequestWithoutVerify(const HttpRequest& httpRequest , HttpResponse& httpResponse , std::string& error , bool verbose);
    
    ///
    /// @brief Sync Send Http Request Without Verify
    ///
    static
    void sendSyncHttpRequestWithoutBodyWithoutVerify(const HttpRequest& httpRequest , HttpResponse& httpResponse , std::string& error , bool verbose);
    
    ///
    /// @method SendAsyncHttpRequest
    /// @abstract   Async  Send Http request , Multi thread
    /// @return The content of the URL resulting from performing the load, or "" if the load failed.
    ///
    static
    void sendAsyncHttpRequestWithoutBodyWithoutVerify(std::shared_ptr<const HttpRequest> pRequest , std::shared_ptr<HttpResponse> pResponse , std::shared_ptr<std::string> pError) ;
    
    ///
    /// @brief Sync Send Http Request Without Verify
    ///
    static
    void sendHttpRequestWithoutBodyWithoutVerify(const HttpRequest& httpRequest , HttpResponse& httpResponse , std::string& error , bool verbose);
    
    ///
    /// @brief Solve Header of Response
    ///
    static
    void solveHttpFields(std::string& header , HttpResponse& httpResponse , std::string& error);
    
    ///
    /// @brief Add Header to option
    ///   Must use with @cleanHeader
    static
    void addHeader(CURL * curl , const HttpRequest& httpRequest , curl_slist * chunk );
    
    ///
    /// @brief Clean Chunk
    ///   Must user with @addHeader
    static
    void cleanHeader(curl_slist * chunk);
    
    ///
    /// @brief  Mutex in Send Method
    ///
    static
    std::mutex g_send_mutex ;
};

#endif /* defined(__rapidjson_test__HttpServer__) */
