#ifndef DX1220_BULLETBOARD_HPP
#define DX1220_BULLETBOARD_HPP

#include "FSMs/statemachine.hpp"

#include <string> // string
#include <vector> // vector
#include <set>
#include <memory> // shared_ptr
#include <algorithm> // find

template<typename T,
    template<typename>
	typename Container>
struct BoardAccount
{
	Container<T> posts;
	void(*Handler)(Container<T>&); // Function To Handle Posts | not used
};

class BulletBoard
{
public:
	static BulletBoard& GetInstance();
	
	BoardAccount<StateMachine*,std::vector> AI_Death_Queue; // Places AIs on deathrow to prevent state access when state machine has already been deleted
	BoardAccount<std::pair<StateMachine*,Entity*>,std::vector> AI_Birth_Queue; // Queues AIs to be born to prevent growing or shrinking std::vectors during loop (iterator invalidation)
	BoardAccount<Entity*,std::set> Bird_Prey_Reservation; // List of Bunnies already tracked by a bird | Prevents corpse theft and necrophilia
	BoardAccount<std::pair<Entity*,Entity*>,std::set> Bunny_Mate_Reservation; // List of Bunnies already with a mate | Prevents polygamy
};

#endif
