#include "messager.hpp"

Messager& Messager::GetInstance()
{
	static Messager instance;
	return instance;
}

void Messager::Register(const std::string& address)
{
	_records.insert({address,{}});
}

int Messager::SendMessage(const std::string& address, std::shared_ptr<msg_base> msg)
{
	if (!_records.count(address)) return INVALID_ADDRESS; // Invalid Address
	
	_records.at(address).push(msg);
	return SEND_SUCCESS;
}

bool Messager::FetchMessages(const std::string& address, Account& record)
{
	if (!_records.count(address)) return false;
	
	record = _records.at(address); // Return back their message records
	auto& a  = _records.at(address);
	while (!a.empty())
		a.pop();
	return true;
}
