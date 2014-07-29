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
class Node {
public:
    Node(picojson::object& obj) : _obj(obj)
    {
    }
    std::string key(){
        if (_obj.find("key") != _obj.end()) {
            return _obj["key"].get<std::string>();
        }
        throw EtcdCppException(GetErrorCodeString(EEC_EcodeKeyNotFound), EEC_EcodeKeyNotFound);
    }
    std::string value(){
        if (dir()) {
            throw EtcdCppException(GetErrorCodeString(EEC_EcodeNotFile), EEC_EcodeNotFile);
        }
        if (_obj.find("value") != _obj.end()) {
            return _obj["value"].get<std::string>();
        }
        return "";
    }
    bool dir(){
        if (_obj.find("dir") != _obj.end()
                && _obj["dir"].get<bool>()) {
            return true;
        }
        return false;
    }
    int createdIndex(){
        if (_obj.find("createdIndex") != _obj.end()) {
            return (int)_obj["createdIndex"].get<double>();
        }
        throw EtcdCppException("no createdIndex");
    }
    int modifiedIndex(){
        if (_obj.find("modifiedIndex") != _obj.end()) {
            return (int)_obj["modifiedIndex"].get<double>();
        }
        throw EtcdCppException("no modifiedIndex");
    }
    void children(std::vector<Node>& nodes) {
        if (!dir()) {
            throw EtcdCppException(GetErrorCodeString(EEC_EcodeNotDir), EEC_EcodeNotDir);
        }
        picojson::array nodesjson = _obj["nodes"].get<picojson::array>();
        for (picojson::array::iterator itr = nodesjson.begin(); itr != nodesjson.end(); itr++) {
            nodes.push_back(Node((*itr).get<picojson::object>()));
        }
    }
private:
    picojson::object _obj;
};
class Client {

public:
    Client(std::string host = std::string("localhost"), int port = 4001);
    virtual ~Client();
    Node Get(std::string& key, bool wait = false);
    Node Set(std::string& key, std::string& value);
private:
    void _ReconnectSocket();
    void _CheckResponseHeader(boost::asio::streambuf& response);
    void _ReadResponseBody(boost::asio::streambuf& response, std::iostream& stream);
    void _ParseString(std::string& jsonstring, picojson::object& obj);

    std::string _host;
    int _port;
    boost::asio::io_service _io_service;
    boost::shared_ptr<boost::asio::ip::tcp::socket> _socket;
    boost::asio::ip::tcp::resolver::iterator _endpoint_iterator;
};

} // namespace etcd_cpp

#endif /* end of include guard */

