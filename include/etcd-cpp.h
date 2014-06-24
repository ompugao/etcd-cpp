#ifndef __ETCD_CPP_H__
#define __ETCD_CPP_H__

#ifdef _MSC_VER

#pragma warning(disable:4251) // needs to have dll-interface to be used by clients of class
#pragma warning(disable:4190) // C-linkage specified, but returns UDT 'boost::shared_ptr<T>' which is incompatible with C
#pragma warning(disable:4819) //The file contains a character that cannot be represented in the current code page (932). Save the file in Unicode format to prevent data loss using native typeof

#ifndef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __FUNCDNAME__
#endif

#else
#endif

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

#include "etcd-cpp/config.h"
#include "etcd-cpp-exception.h"
#include "picojson.h"

namespace etcd_cpp {
class Client {

public:
    Client(std::string host = std::string("localhost"), int port = 4001);
    virtual ~Client();
    ///< brief return true if it is value, return false if it is dir
    bool Get(std::string& key,picojson::value& val);
private:
    void _CheckResponse(boost::asio::streambuf& response);
    bool _ParseString(std::string& jsonstring, picojson::value& val);

    std::string _host;
    int _port;
    boost::asio::io_service _io_service;
    boost::shared_ptr<boost::asio::ip::tcp::socket> _socket;
};

} // namespace etcd_cpp

#endif /* end of include guard */

