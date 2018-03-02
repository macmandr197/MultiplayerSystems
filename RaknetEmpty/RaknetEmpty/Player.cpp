#include "Player.h"


Player::Player()
{
	name = "notset";
}

Player::Player(std::string playerName)
{
	name = playerName;
}
void Player::AttackPlayer(Player other) const
{
	if (other.health > 0)
	{
		other.health--;
		std::cout << other.name << "'s health is:" << other.health;
	}
}

std::string Player::GetName() const
{
	return name;
}

void Player::SetName(std::string val)
{
	name = val;
}

void Player::SetAddress(RakNet::SystemAddress val)
{
	address = val;
}

RakNet::SystemAddress Player::GetAddress() const
{
	return address;
}


