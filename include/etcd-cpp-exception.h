#ifndef __ETCD_CPP_EXCEPTION__
#define __ETCD_CPP_EXCEPTION__

namespace etcd_cpp {
enum EtcdCppErrorCode {
    EEC_Failed=0,
    EEC_InvalidKey=1, ///< passed in input key are not valid
    EEC_Assert=4,
    EEC_Timeout=11, ///< process timed out
    EEC_HTTPClient=12, ///< HTTP client error
    EEC_HTTPServer=13, ///< HTTP server error
    EEC_UserAuthentication=14, ///< authentication failed
};

inline const char* GetErrorCodeString(EtcdCppErrorCode error)
{
    switch(error) {
    case EEC_Failed: return "Failed";
    case EEC_InvalidKey: return "InvalidKey";
    case EEC_Assert: return "Assert";
    case EEC_Timeout: return "Timeout";
    case EEC_HTTPClient: return "HTTPClient";
    case EEC_HTTPServer: return "HTTPServer";
    case EEC_UserAuthentication: return "UserAuthentication";
    }
    // should throw an exception?
    return "";
}

class ETCD_CPP_API EtcdCppException : public std::exception
{
public:
    EtcdCppException() : std::exception(), _s("unknown exception"), _error(EEC_Failed) {
    }
    EtcdCppException(const std::string& s, EtcdCppErrorCode error=EEC_Failed) : std::exception() {
        _error = error;
        _s = "etcd_cpp (";
        _s += GetErrorCodeString(_error);
        _s += "): ";
        _s += s;
    }
    virtual ~EtcdCppException() throw() {
    }
    char const* what() const throw() {
        return _s.c_str();
    }
    const std::string& message() const {
        return _s;
    }
    EtcdCppErrorCode GetCode() const {
        return _error;
    }
private:
    std::string _s;
    EtcdCppErrorCode _error;
};
    
} // namespace etcd_cpp
#endif /* end of include guard */
