#ifndef DX1220_MESSAGER_HPP
#define DX1220_MESSAGER_HPP

#include <map>
#include <queue>
#include <string>
#include <memory>

#include "message.hpp"

enum SEND_FAILURE_CODES
{
	BAD_SEND = -1,
	SEND_SUCCESS = 200 // HTTP Success Code | why not
};

using Account = std::queue<std::shared_ptr<msg_base>>;

class Messager
{
	std::map<std::string,Account> _records;

public:
	static Messager& GetInstance();
	
	void Register(const std::string& address, void* addressOwner);
	int SendMessage(const std::string& address, std::shared_ptr<msg_base> msg);
    int SendMessageInstant(const std::string& address, std::shared_ptr<msg_base> msg);
	void HandleMessages();
	bool FetchMessages(const std::string& address, Account& record);
};

#endif
