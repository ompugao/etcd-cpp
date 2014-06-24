//
// sync_client.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

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
    picojson::object obj = client.Get(key);

    for (picojson::object::const_iterator i = obj.begin(); i != obj.end(); ++i) {
        std::cout << i->first << "  " << i->second << std::endl;
    }
    return 0;
}


