

#pragma once
#include "ICommand.h"

namespace Commands
{
    class NewCommand2 :
        public virtual ICommand
    {
    public:
        NewCommand2(__int64 timestamp, std::wstring str1, std::wstring str2, std::wstring str3, __int64 int64_1);
        ~NewCommand2();
        virtual std::shared_ptr<std::vector<char>> Encode();
        virtual std::shared_ptr<ICommand> Decode(std::shared_ptr<std::vector<char>> &data);
        virtual std::wstring Name();
        virtual std::wstring Description();
        virtual short Code() { return code; }

    private:

        std::wstring m_str1;
        std::wstring m_str2;
        std::wstring m_str3;
        __int64 m_int64_1;
        __int64 m_timestamp; // Always use a 64bit so the message doesn't change
        std::shared_ptr<std::vector<char>> m_internalvector;
        bool hasEncoded;
        short code;
    };

}
