#pragma once
#include "ICommand.h"

class SendString :
	public ICommand
{
public:
	SendString(std::wstring data);
	~SendString();
	virtual std::shared_ptr<std::vector<char>> Encode();
	virtual std::shared_ptr<ICommand> Decode(std::shared_ptr<std::vector<char>> &data);
	virtual std::wstring Name();
	virtual std::wstring Description();

private:
	std::wstring m_string;
	std::vector<char> m_internalvector;
	bool hasEncoded;
};

