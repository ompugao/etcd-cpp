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
    _ReconnectSocket();
}
void Client::_ReconnectSocket() {
    // Get a list of endpoints corresponding to the server name.
    tcp::resolver resolver(_io_service);
    tcp::resolver::query query(_host, boost::lexical_cast<std::string>(_port));
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::resolver::iterator end;
    // Try each endpoint until we successfully establish a connection.
    _socket.reset(new tcp::socket(_io_service));
    boost::system::error_code error = boost::asio::error::host_not_found;
    while (error && endpoint_iterator != end) {
        _socket->close();
        _socket->connect(*endpoint_iterator++, error);
    }
    if (error) {
        throw boost::system::system_error(error);
    }
}

Client::~Client(){
    _socket.reset();
}

Node Client::Get(std::string& key, bool wait) {
    // Form the request. We specify the "Connection: close" header so that the
    // server will close the socket after transmitting the response. This will
    // allow us to treat all data up until the EOF as the content.
    boost::asio::streambuf request;
    std::ostream request_stream(&request);
    request_stream << "GET /v2/keys" << key;
    if (wait) {
        request_stream << "?wait=true";
    }
    request_stream << " HTTP/1.0\r\n";
    request_stream << "Host: " << _host << "\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Connection: Close\r\n\r\n";

    // Send the request.
    boost::asio::streambuf response;
    std::stringstream jsonss;
    _ReconnectSocket();
    boost::asio::write(*_socket.get(), request);
    _CheckResponseHeader(response);
    _ReadResponseBody(response, jsonss);
    std::string jsonstring = jsonss.str();
    //std::cout << jsonstring << std::endl;
    picojson::object obj;
    _ParseString(jsonstring, obj);
    return Node(obj["node"].get<picojson::object>());
}

Node Client::Set(std::string& key, std::string& value) {
    boost::asio::streambuf request;
    std::ostream request_stream(&request);
    request_stream << "PUT /v2/keys" << key << "?value=" << value << " HTTP/1.0\r\n";
    request_stream << "Host: " << _host << "\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Connection: Close\r\n\r\n";

    // Send the request.
    boost::asio::streambuf response;
    std::stringstream jsonss;
    _ReconnectSocket();
    boost::asio::write(*_socket.get(), request);
    _CheckResponseHeader(response);
    _ReadResponseBody(response, jsonss);

    std::string jsonstring = jsonss.str();
    //std::cout << jsonstring << std::endl;
    picojson::object obj;
    _ParseString(jsonstring, obj);
    return Node(obj["node"].get<picojson::object>());
}

void Client::_CheckResponseHeader(boost::asio::streambuf& response) {
    boost::system::error_code error;
    boost::asio::read_until(*_socket.get(), response, "\r\n", error);
    if (error && error != boost::asio::error::eof) {
        throw EtcdCppException("no response");
        return;
    }
    // Check that response is OK.
    std::istream response_stream(&response);
    std::string http_version;
    response_stream >> http_version;
    unsigned int status_code;
    response_stream >> status_code;
    std::string status_message;
    std::getline(response_stream, status_message);
    if (!response_stream && http_version.substr(0, 5) != "HTTP/")
    {
        std::cerr << "Invalid response\n";
        throw EtcdCppException("Invalid response");
    }
    /*
     * error is checked inside _ParseString using the response["errorCode"]
     */
    //if (status_code != 200)
    //{
    //EtcdCppErrorCode errorcode = static_cast<EtcdCppErrorCode>(status_code);
    //throw EtcdCppException("returned status_code is not 200",errorcode);
    //}

    // Read the response headers, which are terminated by a blank line.
    boost::asio::read_until(*_socket.get(), response, "\r\n\r\n");

    // Process the response headers.
    std::string header;
    while (std::getline(response_stream, header) && header != "\r") {
        // just ignore them for now
        //std::cerr << header << "\n";
    }
    //std::cerr << "\n";
}

void Client::_ReadResponseBody(boost::asio::streambuf& response, std::iostream& stream) {
    if (response.size() > 0) {
        stream << &response;
    }
    // Read until EOF, writing data to output as we go.
    boost::system::error_code error;
    while (boost::asio::read(*_socket.get(), response,
                             boost::asio::transfer_at_least(1), error)) {
        stream << &response;
    }
    if (error != boost::asio::error::eof) {
        throw boost::system::system_error(error);
    }
}

void Client::_ParseString(std::string& jsonstring, picojson::object& obj) {
    picojson::value v;
    std::string err;
    picojson::parse(v, jsonstring.begin(), jsonstring.end(), &err);
    if (!err.empty()) {
        throw EtcdCppException(err);
    }

    if (!v.is<picojson::object>()) {
        throw EtcdCppException("body is not json object");
    }

    obj = v.get<picojson::object>();
    if (obj.find("errorCode") != obj.end()) {
        int errorcode = obj["errorCode"].get<double>();
        throw EtcdCppException(obj["message"].get<std::string>(),static_cast<EtcdCppErrorCode>(errorcode));
    }
}
} // namespace etcd_cpp
