#include "Player.h"


Player::Player()
{
	name = "notset";
	currentType = Warrior;
}

Player::Player(std::string playerName, AttackType type)
{
	name = playerName;
	currentType = type;
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

Player::AttackType Player::GetType() const
{
	return currentType;
}

void Player::SetType(AttackType type)
{
	currentType = type;
}



