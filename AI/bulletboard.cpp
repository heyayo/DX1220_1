#include "bulletboard.hpp"

BulletBoard& BulletBoard::GetInstance()
{
	static BulletBoard instance;
	return instance;
}

void BulletBoard::registerAccount(const std::string& account_name, void (* handler_function)(std::vector<StateMachine*>&))
{
	if (_accounts.count(account_name)) return;
	
	// TODO Fix when Expanding Account Struct
	_accounts.insert({account_name,{{},handler_function}});
}
