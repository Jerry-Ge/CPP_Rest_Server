#include "SrcMain.h"
#include <iostream>
#include <map>
#include <string>
#include <ctime>
#include "Server.h"

void ProcessCommandArgs(int argc, const char* argv[])
{
	// TODO: Implement
    std::string mAddress = "http://127.0.0.1:12345";
    Server mServer(mAddress);
    std::string fName = "title.basics.tsv";
    bool success = mServer.LoadFile(fName);
    
    if (success)
    {
        mServer.Start();
    }
}
