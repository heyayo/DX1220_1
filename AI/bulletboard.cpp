#include "bulletboard.hpp"

BulletBoard& BulletBoard::GetInstance()
{
	static BulletBoard instance;
	return instance;
}
