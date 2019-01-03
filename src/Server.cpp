//
//  Server.cpp
//  main
//
//  Created by Yuzhou Ge on 11/17/18.
//
#include "Server.h"


/*
 * This Awesome split function is Cited from PA5 skelton code: parser.hpp
 * Aurther: Sanjay Mudhav
 */
inline std::vector<std::string> Split(const std::string& str, bool isTab)
{
    char delim = '\t';
    if (!isTab)
    {
        delim = ',';
    }

    std::vector<std::string> retVal;
    
    size_t start = 0;
    size_t delimLoc = str.find_first_of(delim, start);
    while (delimLoc != std::string::npos)
    {
        retVal.emplace_back(str.substr(start, delimLoc - start));
        
        start = delimLoc + 1;
        delimLoc = str.find_first_of(delim, start);
    }
    
    retVal.emplace_back(str.substr(start));
    return retVal;
}

Server::Server(std::string& address) : mAddress(address)
{
}

//Function for loading the movie data file
bool Server::LoadFile(std::string& fileName)
{
    std::cout << "========File Loading========" << std::endl;
    std::ifstream ifile(fileName);
    
    std::string line;
    getline(ifile, line); //ignore the first line
    line.clear();
   
    while(getline(ifile, line))
    {
        std::vector<std::string> elements = Split(line, true);
        if (elements[1] != "movie")
        {
        }
        else {
            mData[elements[0]] = elements;
            mData1[elements[2]].push_back(elements);
        }
        elements.clear();
    }
    std::cout << "========File Loaded========" << std::endl;
    return true;
}


//main function to handle different requests
// The jason code referenced from official documentation here:
// https://github.com/Microsoft/cpprestsdk/wiki/JSON
void Server::respond(const http_request& request, const status_code& status)
{
    //return json objects
    json::value resp;
    
    auto paths = http::uri::split_path(http::uri::decode(request.relative_uri().path()));
    
    //reponse by id
    if (paths[0] == "movie" && paths[1] == "id")
    {
        
        if (mData.count(paths[2]) == 0) {
            resp["Error"] = json::value::string("No movie with that ID found");
            request.reply(status, resp);
            return;
        }
        
        std::vector<std::string> movieVec = mData[paths[2]];
        std::vector<std::string> genres = Split(movieVec[8], false);
        
        
        resp["Genres"] = json::value::array();
        for (int i = 0; i < genres.size(); ++i)
        {
            resp["Genres"][i] = json::value::string(genres[i]);
        }
        
        resp["ID"] = json::value::string(movieVec[0]);
        
        if (movieVec[7] == "\\N") {
            resp["Runtime"] = json::value::number(0);
        }
        else
        {
            resp["Runtime"] = json::value::number(std::stoi(movieVec[7]));
        }
        resp["Title"] = json::value::string(movieVec[2]);
        std::string constructURL = "https://www.imdb.com/title/" + paths[2] + "/";
        resp["URL"] = json::value::string(constructURL);
        
        if (movieVec[5] == "\\N")
        {
            resp["Year"] = json::value::number(0);
        }
        else {
            resp["Year"] = json::value::number(std::stoi(movieVec[5]));
        }
        request.reply(status, resp);
    }
    
    //response by movie name [could be multiple json objects]
    else if (paths[0] == "movie" && paths[1] == "name")
    {
    
        if (mData1.find(paths[2]) == mData1.end())
        {
            resp["Error"] = json::value::string("No movie by that name found");
            request.reply(status, resp);
            return;
        }

        json::value resp1 = json::value::array();
        std::vector<std::vector<std::string>> movieVecs = mData1[paths[2]];

        for (unsigned i = 0; i < movieVecs.size(); ++i)
        {
            
            //push in the genre list
            std::vector<std::string> genres = Split(movieVecs[i][8], false);
            resp["Genres"] = json::value::array();
            for (int i = 0; i < genres.size(); ++i)
            {
                resp["Genres"][i] = json::value::string(genres[i]);
            }
            resp["ID"] = json::value::string(movieVecs[i][0]);
            
            //handler wrong format
            if (movieVecs[i][7] == "\\N") {
                resp["Runtime"] = json::value::number(0);
            }
            else
            {
                resp["Runtime"] = json::value::number(std::stoi(movieVecs[i][7]));
            }
            resp["Title"] = json::value::string(movieVecs[i][2]);
            std::string constructURL = "https://www.imdb.com/title/" + movieVecs[i][0] + "/";
            resp["URL"] = json::value::string(constructURL);
            
            
            //handle wrong format
            if (movieVecs[i][5] == "\\N")
            {
                resp["Year"] = json::value::number(0);
            }
            else
            {
                resp["Year"] = json::value::number(std::stoi(movieVecs[i][5]));
            }
            resp1[i] = resp;
        }
        request.reply(status, resp1);
    }
}

bool Server::Start()
{
    /*The usage of http_listener cited from here: https://microsoft.github.io/cpprestsdk/classweb_1_1http_1_1experimental_1_1listener_1_1http__listener.html#ad28fa2bf42f757c3942b3b1f84681aa9
     AND
     This example linked from Wiki of official document
     https://mariusbancila.ro/blog/2013/08/19/full-fledged-client-server-example-with-cpprest-sdk-110/
    */
    std::cout << "========Server Starts Listening========"  << std::endl;
    http_listener listener(mAddress);
    
    listener.open().wait();
    listener.support(methods::GET, [this](http_request req){
            this->respond(req, status_codes::OK);
            return;
    });

    //keep listing on the port, non-stop
    while(true)
    {
    }

    return true;
}



