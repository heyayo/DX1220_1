#ifndef DX1220_BULLETBOARD_HPP
#define DX1220_BULLETBOARD_HPP

#include "FSMs/statemachine.hpp"

#include <map>
#include <string>
#include <vector>
#include <memory>

struct BoardAccount
{
	std::vector<Entity*> posts;
	void(*Handler)(std::vector<StateMachine*>&); // Function To Handle Posts
};

class BulletBoard
{
	std::map<std::string,BoardAccount> _accounts;
	
public:
	static BulletBoard& GetInstance();
	
	BoardAccount Entity_Death_Queue; // Places AIs on deathrow to prevent state access when state machine has already been deleted
	BoardAccount Bird_Prey_Reservation; // List of Bunnies already tracked by a bird | Prevents corpse theft and necrophilia
	BoardAccount Bunny_Mate_Reservation; // List of Bunnies already with a mate | Prevents polygamy
	
	void registerAccount(const std::string& account_name, void(*handler_function)(std::vector<StateMachine*>&));
};

#endif
