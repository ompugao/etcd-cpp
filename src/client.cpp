//#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include <sstream>
#include "etcd-cpp.h"

namespace etcd_cpp {

using boost::asio::ip::tcp;
Client::Client(std::string host, int port) {
    _host = host;
    _port = port;
    // Get a list of endpoints corresponding to the server name.
    tcp::resolver resolver(_io_service);
    tcp::resolver::query query(host, boost::lexical_cast<std::string>(port));
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

    // Try each endpoint until we successfully establish a connection.
    _socket.reset(new tcp::socket(_io_service));
    //boost::asio::detail::socket_ops::connect(socket, endpoint_iterator);
    _socket->connect(*endpoint_iterator);
}

Client::~Client(){
    _socket.reset();
}

 bool Client::Get(std::string& key, picojson::value& val) {
    // Form the request. We specify the "Connection: close" header so that the
    // server will close the socket after transmitting the response. This will
    // allow us to treat all data up until the EOF as the content.
    boost::asio::streambuf request;
    std::ostream request_stream(&request);
    request_stream << "GET " << key << " HTTP/1.0\r\n";
    request_stream << "Host: " << _host << "\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Connection: close\r\n\r\n";

    // Send the request.
    boost::asio::write(*_socket.get(), request);
    boost::asio::streambuf response;
    _CheckResponse(response);
    std::stringstream jsonss;
    if (response.size() > 0) {
        jsonss << &response;
    }
    // Read until EOF, writing data to output as we go.
    boost::system::error_code error;
    while (boost::asio::read(*_socket.get(), response,
                             boost::asio::transfer_at_least(1), error)) {
        //std::cerr << "real response!" << std::endl;
        //std::cerr << &response << std::endl;
        jsonss << &response;
    }
    if (error != boost::asio::error::eof) {
        throw boost::system::system_error(error);
    }
    std::string jsonstring = jsonss.str();
    return _ParseString(jsonstring, val);
}

void Client::_CheckResponse(boost::asio::streambuf& response) {
    // Read the response status line. The response streambuf will automatically
    // grow to accommodate the entire line. The growth may be limited by passing
    // a maximum size to the streambuf constructor.
    boost::asio::read_until(*_socket.get(), response, "\r\n");

    // Check that response is OK.
    std::istream response_stream(&response);
    std::string http_version;
    response_stream >> http_version;
    unsigned int status_code;
    response_stream >> status_code;
    std::string status_message;
    std::getline(response_stream, status_message);
    if (!response_stream || http_version.substr(0, 5) != "HTTP/")
    {
        std::cerr << "Invalid response\n";
        throw EtcdCppException("Invalid response");
    }
    if (status_code != 200)
    {
        std::stringstream ss;
        ss <<"Response returned with status code " << status_code;
        throw EtcdCppException(ss.str());
    }

    // Read the response headers, which are terminated by a blank line.
    boost::asio::read_until(*_socket.get(), response, "\r\n\r\n");

    // Process the response headers.
    std::string header;
    while (std::getline(response_stream, header) && header != "\r") {
        //std::cerr << header << "\n";
    }
    //std::cerr << "\n";
}

bool Client::_ParseString(std::string& jsonstring, picojson::value& val) {
    picojson::value v;
    std::string err;
    picojson::parse(v, jsonstring.begin(), jsonstring.end(), &err);
    if (!err.empty()) {
        throw EtcdCppException(err);
    }

    if (!v.is<picojson::object>()) {
        throw EtcdCppException("body is not json object");
    }

    picojson::object obj = v.get<picojson::object>();
    if (obj.find("errorCode") != obj.end()) {
        int errorcode = obj["errorCode"].get<double>();
        throw EtcdCppException(obj["message"].get<std::string>(),static_cast<EtcdCppErrorCode>(errorcode));
    }
    if (obj["node"].get<picojson::object>().find("dir") != obj["node"].get<picojson::object>().end()
            && obj["node"].get<picojson::object>()["dir"].get<bool>()) { //isdir?
        return false;
    }
    val = obj["node"].get<picojson::object>()["value"];
    return true;
}
} // namespace etcd_cpp
