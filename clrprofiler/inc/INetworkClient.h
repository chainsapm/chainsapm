#pragma once
#ifndef INETCLIENT
#define INETCLIENT

#pragma once
#include "Commands.h"

class Cprofilermain;

#define _SECOND ((__int64) 10000000)
#define _MINUTE (60 * _SECOND)
#define _HOUR   (60 * _MINUTE)
#define _DAY    (24 * _HOUR)

class INetworkClient
{
public:
	virtual ~INetworkClient() { };
	// Start the network client when we're ready.
	virtual void Start() = 0;
	// Shutdown the socket and stop send and recv.
	virtual void Shutdown() = 0;
	
	// Send a single command to the buffer to be processed.
	virtual HRESULT SendCommand(Commands::ICommand* packet) = 0;
	// Send a list of commands
	virtual HRESULT SendCommands(std::vector<Commands::ICommand*> &packet) = 0;
	
	// Recv single command from the buffer
	virtual std::shared_ptr<Commands::ICommand> ReceiveCommand() = 0;
	// Recv a list of commands
	virtual std::vector<std::shared_ptr<Commands::ICommand>> ReceiveCommands() = 0;
};

#endif // !NETCLIENT
