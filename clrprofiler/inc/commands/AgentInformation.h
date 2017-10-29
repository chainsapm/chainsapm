   
 
 
/****************************************************************************************
*
* This code was generated by the Command Creator tool for the ChainsAPM project. 
* If manual changes are made to the code they could be lost.
* 
****************************************************************************************/ 
#pragma once
#include "ICommand.h"

namespace Commands  
{
    class AgentInformation :
        public virtual ICommand
    {


    public:
    
        AgentInformation(__int64 timestamp, std::wstring agentname, std::wstring machinename, std::wstring workingdirectory, std::wstring commandline, std::wstring processname, __int16 processpid, std::wstring netversion); 
        ~AgentInformation();
        virtual std::shared_ptr<std::vector<char>> Encode();
        virtual std::shared_ptr<ICommand> Decode(std::shared_ptr<std::vector<char>> &data);  
        virtual std::wstring Name();
        virtual std::wstring Description();
        virtual short Code() { return code; }
        std::wstring AgentName;
        std::wstring MachineName;
        std::wstring WorkingDirectory;
        std::wstring CommandLine;
        std::wstring ProcessName;
        __int16 ProcessPID;
        std::wstring NETVersion;
      private:
        __int64 timestamp; // Always use a 64bit so the message doesn't change
        std::shared_ptr<std::vector<char>> internalvector;
        bool hasEncoded;
        short code;
    };

}
