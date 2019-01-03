//
//  Server.h
//  pa7
//
//  Created by Yuzhou Ge on 11/17/18.
//

#ifndef Server_h
#define Server_h
#include <string>
#include <unordered_map>
#include <cpprest/http_listener.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <thread>
#include <chrono>
#include <vector>
#include <ctime>
#include <fstream>

using namespace web::http::experimental::listener;
using namespace web::http;
using namespace web;

class Server
{
public:
    Server();
    Server(std::string& listeningAddress);
    bool LoadFile(std::string& fileName);
    bool Start();
    void printData();
private:
    void respond(const http_request& request, const status_code& status);
    std::unordered_map<std::string, std::vector<std::string>> mData;
    std::unordered_map<std::string, std::vector<std::vector<std::string>>> mData1;
    std::string mAddress;
};

#endif /* Server_h */
