#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <boost/lexical_cast.hpp>
#include "etcd-cpp.h"


int main(int argc, char* argv[])
{
    if (argc != 4) {
        std::cout << "usage: " << argv[0] << " host port key" << std::endl;
        exit(1);
    }
    std::string host = std::string(argv[1]);
    int port = boost::lexical_cast<int>(argv[2]);
    std::string key = std::string(argv[3]);

    etcd_cpp::Client client(host, port);
    picojson::value val;
    if (client.Get(key,val)) {
        std::cout << val << std::endl;
    }
    return 0;
}


