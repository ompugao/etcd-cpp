#ifndef __ETCD_CPP_EXCEPTION__
#define __ETCD_CPP_EXCEPTION__

namespace etcd_cpp {
enum EtcdCppErrorCode {
    EEC_Failed=0,
    EEC_EcodeKeyNotFound       = 100,
    EEC_EcodeTestFailed        = 101,
    EEC_EcodeNotFile           = 102,
    EEC_EcodeNoMorePeer        = 103,
    EEC_EcodeNotDir            = 104,
    EEC_EcodeNodeExist         = 105,
    EEC_EcodeKeyIsPreserved    = 106,
    EEC_EcodeRootROnly         = 107,
    EEC_EcodeValueRequired     = 200,
    EEC_EcodePrevValueRequired = 201,
    EEC_EcodeTTLNaN            = 202,
    EEC_EcodeIndexNaN          = 203,
    EEC_EcodeRaftInternal      = 300,
    EEC_EcodeLeaderElect       = 301,
    EEC_EcodeWatcherCleared    = 400,
    EEC_EcodeEventIndexCleared = 401,
};

inline const char* GetErrorCodeString(EtcdCppErrorCode error)
{
    switch(error) {
    case EEC_Failed:                 return "Failed";
    case EEC_EcodeKeyNotFound:       return "Key Not Found";
    case EEC_EcodeTestFailed:        return "Test Failed";
    case EEC_EcodeNotFile:           return "Not A File";
    case EEC_EcodeNoMorePeer:        return "Reached the max number of peers in the cluster";
    case EEC_EcodeNotDir:            return "Not A Directory";
    case EEC_EcodeNodeExist:         return "Already exists";
    case EEC_EcodeKeyIsPreserved:    return "The prefix of given key is a keyword in etcd";
    case EEC_EcodeRootROnly:         return "Root is read only";
    case EEC_EcodeValueRequired:     return "Value is Required in POST form";
    case EEC_EcodePrevValueRequired: return "PrevValue is Required in POST form";
    case EEC_EcodeTTLNaN:            return "The given TTL in POST form is not a number";
    case EEC_EcodeIndexNaN:          return "The given index in POST form is not a number";
    case EEC_EcodeRaftInternal:      return "Raft Internal Error";
    case EEC_EcodeLeaderElect:       return "During Leader Election";
    case EEC_EcodeWatcherCleared:    return "watcher is cleared due to etcd recovery";
    case EEC_EcodeEventIndexCleared: return "The event in requested index is outdated and cleared";
    }
    // should throw an exception?
    return boost::lexical_cast<std::string>(error).c_str();
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
