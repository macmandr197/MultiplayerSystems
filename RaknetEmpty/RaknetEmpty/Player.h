#include <iostream>
#include <string>
#include <RakNetTypes.h>
#include <RakString.h>

class Player
{
public:
	enum AttackType { Warrior, Mage, Ranger, };

private:
	AttackType currentType;

public:
	Player();

	Player(std::string playerName, AttackType type,int ID);

	int healAmount = 2;
	int id = -1;
	bool isAlive = true;

private:
	std::string name;
	RakNet::SystemAddress address;
	int health = 10;
	int attackPower = 1;

public:
	void AttackPlayer(Player other) const;

	void RemoveHealth(int dmg);

	std::string GetName() const;

	void SetName(std::string val);

	void SetAddress(RakNet::SystemAddress val);

	RakNet::SystemAddress GetAddress() const;

	AttackType GetType() const;

	void SetType(AttackType type);

	int GetAttackPower() const;

	int GetHealth() const;

	void Heal();

	std::string GetPlayerStats() const;

	RakNet::RakString GetPlayerStatsAsRakString() const;

	/*bool Player::operator==(const Player& p);*/
	
	bool  operator==(const Player & p) const
	{
	return ((p.GetName().data() == this->name) &&
	(p.GetAddress() == this->address) &&
	(p.GetAttackPower() == this->attackPower) &&
	(p.GetHealth() == this->health) &&
	(p.GetType() == this->currentType) &&
	(p.healAmount == this->healAmount) &&
	(p.id == this->id));
	}
};
