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
    etcd_cpp::Node node = client.Get(key);
    //check 
    if (!node.dir()) {
        std::cout << node.value() << std::endl;
    } else {
        std::vector<etcd_cpp::Node> nodes;
        node.children(nodes);
        for(std::vector<etcd_cpp::Node>::iterator itr = nodes.begin(); itr != nodes.end(); itr++) {
            if (!itr->dir()) {
                std::cout << itr->key() << " " << itr->value() << std::endl;
            }
        }
    }

    std::string value = "testvalue2";
    etcd_cpp::Node res = client.Set(key,value);
    node = client.Get(key);
    //check 
    if (!node.dir()) {
        std::cout << node.value() << std::endl;
    } else {
        std::vector<etcd_cpp::Node> nodes;
        node.children(nodes);
        for(std::vector<etcd_cpp::Node>::iterator itr = nodes.begin(); itr != nodes.end(); itr++) {
            if (!itr->dir()) {
                std::cout << itr->key() << " " << itr->value() << std::endl;
            }
        }
    }
    value = "testvalue1";
    res = client.Set(key,value);
    return 0;
}


