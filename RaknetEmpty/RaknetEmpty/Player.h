#include <iostream>
#include <string>
#include <RakNetTypes.h>

class Player
{
public:
	Player();
	Player(std::string playerName);
private:
	std::string name;
	RakNet::SystemAddress address;
	int health = 10;
	int attackPower = 1;

public:
	void AttackPlayer(Player other) const;
	std::string GetName() const;
	void SetName(std::string val);
	void SetAddress(RakNet::SystemAddress val);
	RakNet::SystemAddress GetAddress() const;
	

};
