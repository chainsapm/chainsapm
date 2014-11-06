
#ifndef NETCLIENT
#define NETCLIENT
#pragma once

#include "profilermain.h"
class Cprofilermain;

enum NetworkCommands
{
	PING_SERVER,// 0x1
	PING_AGGREGATOR, 
	PING_AGENT,
	SEND_MODULENAME,
	SEND_CLASSNAME,
	SEND_FUNCTIONNAME
	
};


class networkclient
{
public:
	networkclient();
	~networkclient();
};




#endif // !NETCLIENT
