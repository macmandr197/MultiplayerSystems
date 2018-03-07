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
	if (type == Warrior)
	{
		attackPower = 2;
	}
	else if (type == Mage)
	{
		attackPower = 3;
	}
	else if (type == Ranger)
	{
		attackPower = 1;
	}
}

inline const char* ToString(Player::AttackType v)
{
	switch (v)
	{
	case Player::Warrior:   return "Warrior";
	case Player::Mage:   return "Mage";
	case Player::Ranger: return "Ranger";
	default:      return "[Unknown Attack Type]";
	}
}

void Player::AttackPlayer(Player other) const
{
	if (other.health > 0)
	{
		other.health--;
		std::cout << other.name << "'s health is:" << other.health;
	}
}

void Player::RemoveHealth(int dmg)
{
	health -= dmg;
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

int Player::GetAttackPower() const
{
	return attackPower;
}

int Player::GetHealth() const
{
	return health;
}

void Player::Heal()
{
	health += healAmount;
}

std::string Player::GetPlayerStats() const
{
	return ("Player: " + name + " | Type: " + ToString(currentType) + " | Attack Power: " + std::to_string(attackPower) + " | Current Health: " + std::to_string(health));
}

RakNet::RakString Player::GetPlayerStatsAsRakString() const //why the fuck do i need to do this
{
	RakNet::RakString ret;
	ret += "Player: ";
	ret += RakNet::RakString(name.data());
	ret += " | Type: ";
	ret += RakNet::RakString(ToString(currentType));
	ret += " | Attack Power: ";
	ret += RakNet::RakString(std::to_string(attackPower).data());
	ret += " | Current Health: ";
	ret += RakNet::RakString(std::to_string(health).data());
	ret += "\n";
	return ret;
}
