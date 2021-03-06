#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "BitStream.h"
#include <iostream>
#include <thread>         // std::thread
#include <chrono>
#include <map>
#include "Player.h"
#include <vector>
#include <algorithm>

static int SERVER_PORT = 65000;
static int CLIENT_PORT = 65001;
const static int MAX_CONNECTIONS = 4;

RakNet::RakPeerInterface* g_rakPeerInterface = nullptr;

bool isServer = false;
bool isRunning = true;
unsigned short g_totalPlayers = 0;

enum
{
	ID_THEGAME_LOBBY = ID_USER_PACKET_ENUM,
	ID_THEGAME_ACTION,
	ID_ADD_PLAYER,
	ID_PLAYER_ALREADY_EXISTS,
	ID_PLAYER_CREATED_SUCCESSFULLY,
	ID_CHECK_PLAYER_NAME,
	ID_PLAYER_NAME_AVAILABLE,
	ID_PLAYER_READY,
	ID_BEGIN_GAME,
	ID_GET_STATS,
	ID_PRINT_MESSAGE,
	ID_HEAL_PLAYER,
	ID_ATTACK_PLAYER,
	ID_GET_PLAYER_ID,
	ID_QUIT,
};

RakNet::SystemAddress g_serverAddress;
RakNet::SystemAddress g_clientAddress;
std::vector<Player> players;

//Joining Variables
bool hasJoined = false;
bool nameAvailable = false;
bool createdPlayers[MAX_CONNECTIONS];
int readyPlayers = 0;

//game variables
bool isPlaying = false;
bool nameFound = false;
int playerID = -1;

int currentPlayerTurnID = -1;

enum NetworkStates
{
	NS_Decision = 0,
	NS_CreateSocket,
	NS_PendingConnection,
	NS_Connected,
	NS_Running,
	NS_Join,
	NS_Lobby,
	NS_InGame,
};

NetworkStates g_networkState = NS_Decision;



void OnIncomingConnection(RakNet::Packet* packet)
{
	if (!isServer)
	{
		assert(0);
	}
	g_totalPlayers++;
	unsigned short numConnections = g_rakPeerInterface->NumberOfConnections();
	std::cout << "Total player(s): " << g_totalPlayers << "\nNum connection(s): " << numConnections << std::endl;
	RakNet::BitStream bs;
	bs.Write(RakNet::MessageID(ID_GET_PLAYER_ID));
	int ID(g_totalPlayers-1);
	bs.Write(ID);
	g_rakPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
}

void OnConnectionAccepted(RakNet::Packet* packet)
{
	if (isServer)
	{
		//server should never request connections, only clients do
		assert(0);
	}
	//we have successfully connected, go to lobby	
	g_networkState = NS_Lobby;
	g_serverAddress = packet->systemAddress;
	g_clientAddress = g_rakPeerInterface->GetExternalID(packet->systemAddress);
}

Player::AttackType ConverToAttackType(const std::string& str)
{
	if (str.compare("warrior") == 0)
		return Player::Warrior;
	if (str.compare("mage") == 0)
		return Player::Mage;
	if (str.compare("ranger") == 0)
		return Player::Ranger;
}

std::string ConvertAttackTypeToString(const Player::AttackType& type)
{
	if (type == Player::Warrior)
		return "warrior";
	if (type == Player::Mage)
		return "mage";
	if (type == Player::Ranger)
		return "ranger";
}

void InputHandler()
{
	while (isRunning)
	{
		std::string userInput;
		if (g_networkState == NS_Decision)
		{
			std::cout << "Press (s) for server, (c) for client" << std::endl;
			std::cin >> userInput;
			isServer = userInput.at(0) == 's';
			g_networkState = NS_CreateSocket;
		}
		else if (g_networkState == NS_Lobby)
		{
			if (!hasJoined) //if the user has already joined the lobby, don't display the welcome message more than once
			{
				std::cout << "If you would like to play this game, enter your name " << std::endl;
				std::cout << "if you want to quit, type /quit. " << std::endl;
				hasJoined = true;
			}

			while (true)
			{
				std::cin >> userInput;
				if (!nameAvailable)
				{
					if (userInput.compare("/quit") == 0)
					{
						//heartbreaking
						assert(0);
					}
					RakNet::BitStream bs;
					bs.Write((RakNet::MessageID)ID_CHECK_PLAYER_NAME);
					RakNet::SystemAddress sysAddress(g_clientAddress);
					bs.Write(sysAddress);
					RakNet::RakString str(userInput.data());
					bs.Write(str);
					g_rakPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, g_serverAddress, false);
				}
				std::this_thread::sleep_for(std::chrono::microseconds(125000)); //making the thread sleep a little so that the client can catch up to the server
				if (nameAvailable)
				{
					break;
				}
			}

			RakNet::BitStream bs;
			bs.Write((RakNet::MessageID)ID_ADD_PLAYER);
			RakNet::SystemAddress sysAddress(g_clientAddress);
			bs.Write(sysAddress);
			RakNet::RakString str(userInput.data());
			bs.Write(str);
			std::cout << "Now enter your attack type: Either warrior, mage, or ranger." << std::endl;
			std::string playerType;
			while (true)
			{
				std::cin >> playerType;
				if (playerType.compare("warrior") == 0 || playerType.compare("mage") == 0 || playerType.compare("ranger") == 0)
				{
					break;
				}
				std::cout << "That isn't a correct type. Please enter a correct type. (warrior, mage, ranger)" << std::endl;
				std::cout << "You entered: " << playerType << std::endl;
			}
			Player::AttackType plyrType(ConverToAttackType(playerType));
			bs.Write(plyrType);
			bs.Write(int(playerID));
			g_rakPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, g_serverAddress, false);
			g_networkState = NS_InGame;
			hasJoined = false;
		}
		else if (g_networkState == NS_InGame)
		{
			std::string input;
			if (!hasJoined)
			{
				std::cout << "Welcome to the game. Until everyone is ready, you're welcome to wait here.\n When ready. type '/ready'\n\nType /help for available actions." << std::endl;
				hasJoined = true;
			}
			getline(std::cin, input);
			if (input.compare("/ready") == 0)
			{
				RakNet::BitStream bs;
				bs.Write(RakNet::MessageID(ID_PLAYER_READY));
				g_rakPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, g_serverAddress, false);
			}
			else if (input.compare("/help") == 0)
			{
				printf("You have the following actions available to you:\n'/stats' view players stats\n");
			}
			else if (input.compare("/quit") == 0)
			{
				
			}
			if (isPlaying)
			{
				if (input.compare("/help") == 0)
				{
					printf("You have the following actions available to you:\n'/quit' - quits the game\n'/heal' - heal yourself\n'/attack [player name]' - attack the specified player\n'/stats' view players stats\n");
				}
				else if (input.compare("/stats") == 0)
				{
					RakNet::BitStream bs;
					bs.Write(RakNet::MessageID(ID_GET_STATS));
					bs.Write(RakNet::SystemAddress(g_clientAddress));
					g_rakPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, g_serverAddress, false);
				}
				else if(input.compare("/quit") == 0)
				{
					RakNet::BitStream bs;
					bs.Write(RakNet::MessageID(ID_QUIT));
					g_rakPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, g_serverAddress, false);
				}
				else if (input.compare("/heal") == 0)
				{
					RakNet::BitStream bs;
					bs.Write(RakNet::MessageID(ID_HEAL_PLAYER));
					bs.Write(int(playerID));
					g_rakPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, g_serverAddress, false);
				}
				else if (input.substr(0, 7).compare("/attack") == 0) //checking the first part of the string to see if the player does want to attack
				{
					std::string playerToAttack = input.substr(input.find_first_of(' ') + 1, input.length());
					
					std::cout << "Attack: " << playerToAttack << std::endl;
					RakNet::BitStream bs;
					bs.Write(RakNet::MessageID(ID_ATTACK_PLAYER));
					int player(playerID);
					bs.Write(player);
					RakNet::RakString otherPlayer(playerToAttack.data());
					bs.Write(otherPlayer);
					g_rakPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, g_serverAddress, false);
				}
			}
		}
		std::this_thread::sleep_for(std::chrono::microseconds(100));
	}
}

unsigned char GetPacketIdentifier(RakNet::Packet* packet)
{
	if (packet == nullptr)
		return 255;

	if ((unsigned char)packet->data[0] == ID_TIMESTAMP)
	{
		RakAssert(packet->length > sizeof(RakNet::MessageID) + sizeof(RakNet::Time));
		return (unsigned char)packet->data[sizeof(RakNet::MessageID) + sizeof(RakNet::Time)];
	}
	return (unsigned char)packet->data[0];
}

bool HandleLowLevelPacket(RakNet::Packet* packet)
{
	bool isHandled = true;
	unsigned char packetIdentifier = GetPacketIdentifier(packet);
	switch (packetIdentifier)
	{
		case ID_DISCONNECTION_NOTIFICATION:
			// Connection lost normally
			printf("ID_DISCONNECTION_NOTIFICATION\n");
			break;
		case ID_ALREADY_CONNECTED:
			// Connection lost normally
			printf("ID_ALREADY_CONNECTED with guid %" PRINTF_64_BIT_MODIFIER "u\n", packet->guid);
			break;
		case ID_INCOMPATIBLE_PROTOCOL_VERSION:
			printf("ID_INCOMPATIBLE_PROTOCOL_VERSION\n");
			break;
		case ID_REMOTE_DISCONNECTION_NOTIFICATION: // Server telling the clients of another client disconnecting gracefully.  You can manually broadcast this in a peer to peer enviroment if you want.
			printf("ID_REMOTE_DISCONNECTION_NOTIFICATION\n");
			break;
		case ID_REMOTE_CONNECTION_LOST: // Server telling the clients of another client disconnecting forcefully.  You can manually broadcast this in a peer to peer enviroment if you want.
			printf("ID_REMOTE_CONNECTION_LOST\n");
			break;
		case ID_NEW_INCOMING_CONNECTION:
		case ID_REMOTE_NEW_INCOMING_CONNECTION: // Server telling the clients of another client connecting.  You can manually broadcast this in a peer to peer enviroment if you want.
			printf("ID_REMOTE_NEW_INCOMING_CONNECTION\n");
			OnIncomingConnection(packet);
			break;
		case ID_CONNECTION_BANNED: // Banned from this server
			printf("We are banned from this server.\n");
			break;
		case ID_CONNECTION_ATTEMPT_FAILED:
			printf("Connection attempt failed\n");
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			// Sorry, the server is full.  I don't do anything here but
			// A real app should tell the user
			printf("ID_NO_FREE_INCOMING_CONNECTIONS\n");
			break;

		case ID_INVALID_PASSWORD:
			printf("ID_INVALID_PASSWORD\n");
			break;

		case ID_CONNECTION_LOST:
			// Couldn't deliver a reliable packet - i.e. the other system was abnormally
			// terminated
			printf("ID_CONNECTION_LOST\n");
			break;

		case ID_CONNECTION_REQUEST_ACCEPTED:
			// This tells the client they have connected
			printf("ID_CONNECTION_REQUEST_ACCEPTED to %s with GUID %s\n", packet->systemAddress.ToString(true), packet->guid.ToString());
			printf("My external address is %s\n", g_rakPeerInterface->GetExternalID(packet->systemAddress).ToString(true));
			OnConnectionAccepted(packet);
			break;
		case ID_CONNECTED_PING:
		case ID_UNCONNECTED_PING:
			printf("Ping from %s\n", packet->systemAddress.ToString(true));
			break;
		default:
			isHandled = false;
			break;
	}
	return isHandled;
}



void PacketHandler()
{
	while (isRunning)
	{
		for (RakNet::Packet* packet = g_rakPeerInterface->Receive(); packet != nullptr; g_rakPeerInterface->DeallocatePacket(packet) , packet = g_rakPeerInterface->Receive())
		{
			// We got a packet, get the identifier with our handy function
			if (!HandleLowLevelPacket(packet))
			{
				unsigned char packetIdentifier = GetPacketIdentifier(packet);
				switch (packetIdentifier)
				{
					case ID_THEGAME_LOBBY:
						{
							RakNet::BitStream myBitStream(packet->data, packet->length, false); // The false is for efficiency so we don't make a copy of the passed data
							RakNet::MessageID messageID;
							myBitStream.Read(messageID);
							std::string userName;
							myBitStream.Read(userName);
							std::cout << userName << " is ready to play!!! " << std::endl;
						}
						break;
					case ID_ADD_PLAYER:
						{
							RakNet::BitStream myBitStream(packet->data, packet->length, false);
							RakNet::MessageID messageID;
							myBitStream.Read(messageID);
							RakNet::SystemAddress sysAddress;
							myBitStream.Read(sysAddress);
							RakNet::RakString str;
							myBitStream.Read(str);
							Player::AttackType playerType;
							myBitStream.Read(playerType);
							int ID;
							myBitStream.Read(ID);
							auto temp = new Player(std::string(str), playerType, ID);
							temp->SetAddress(sysAddress);
							players.push_back(*temp);
							std::cout << "Added player: " << temp->GetName() << ". Type is: " << ConvertAttackTypeToString(temp->GetType()) << ". Player's length is: " << players.size() << std::endl;
							RakNet::BitStream bs;							
							break;
						}
					case ID_PLAYER_ALREADY_EXISTS:
						printf("Player with the name that already exists, please enter new name.\n ");
						break;
					case ID_PLAYER_CREATED_SUCCESSFULLY:
						{
							printf("Created player Successfully!\n");
							createdPlayers[g_rakPeerInterface->GetIndexFromSystemAddress(g_clientAddress)] = true; //setting client local variable to true
							std::cout << "Type ready to move onto the game." << std::endl;
						}
						break;
					case ID_CHECK_PLAYER_NAME:
						{
							RakNet::BitStream myBitStream(packet->data, packet->length, false);
							RakNet::MessageID messageID;
							myBitStream.Read(messageID);
							RakNet::SystemAddress sysAddress;
							myBitStream.Read(sysAddress);
							RakNet::RakString str;
							myBitStream.Read(str);

							auto it = find_if(players.begin(), players.end(), [&str](const Player& obj) { return obj.GetName() == std::string(str); });

							if (it != players.end()) //if a player is trying to create a new character, but is using an existing name
							{
								RakNet::BitStream bs; //send a message to the player saying that the player already exists
								bs.Write((RakNet::MessageID)ID_PLAYER_ALREADY_EXISTS);
								g_rakPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, sysAddress, false);
							}
							else
							{
								RakNet::BitStream bs; //send a message to the player saying that the player name is available
								bs.Write((RakNet::MessageID)ID_PLAYER_NAME_AVAILABLE);
								bs.Write(bool(true));
								g_rakPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, sysAddress, false);
								std::cout << "player checking name" << std::endl;
							}
							break;
						}
					case ID_PLAYER_READY:
						{
							++readyPlayers;
							std::cout << "packet recieved. Number of player: " << readyPlayers << " number of  connections: " << g_rakPeerInterface->NumberOfConnections() << std::endl;
							if (readyPlayers == g_rakPeerInterface->NumberOfConnections())
							{
								++currentPlayerTurnID; //increment the currentplayerid from -1 to 0 to start the game
								RakNet::BitStream bs;
								bs.Write(RakNet::MessageID(ID_BEGIN_GAME));
								std::string name = "The game has begun! it is " + players[currentPlayerTurnID].GetName() + "'s turn!\n";
								RakNet::RakString str(name.data());
								bs.Write(str);
								for each (Player player in players)
								{
									g_rakPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, player.GetAddress(), false);
								}
								
							}
							else
							{
								RakNet::BitStream bs;
								bs.Write(RakNet::MessageID(ID_PRINT_MESSAGE));
								int numCon = g_rakPeerInterface->NumberOfConnections();
								std::string name = std::to_string(readyPlayers) + "/" + std::to_string(numCon) + " players ready.";
								RakNet::RakString str(name.data());
								bs.Write(str);
								for each (Player player in players)
								{
									g_rakPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, player.GetAddress(), false);
								}
							}
						}
						break;
					case ID_PLAYER_NAME_AVAILABLE:
						{
							std::cout << "Player Name Available! " << std::endl;
							nameAvailable = true;
							break;
						}
					case ID_BEGIN_GAME:
						{
							RakNet::BitStream myBitStream(packet->data, packet->length, false);
							RakNet::MessageID m_ID;
							myBitStream.Read(m_ID);
							RakNet::RakString str;
							myBitStream.Read(str);
							std::cout << str;
							isPlaying = true;
						}
						break;
					case ID_GET_STATS:
						{
							RakNet::BitStream myBitStream(packet->data, packet->length, false);
							RakNet::MessageID m_ID;
							myBitStream.Read(m_ID);
							RakNet::SystemAddress address;
							myBitStream.Read(address);
							RakNet::BitStream bs;
							bs.Write(RakNet::MessageID(ID_PRINT_MESSAGE));
							RakNet::RakString str;
							str += ("----CURRENT PLAYER STATS----\n");
							for each (Player player in players)
							{
								RakNet::RakString thestring(player.GetPlayerStatsAsRakString());
								str += thestring;
							}
							str += ("----------------------------");
							bs.Write(str);
							g_rakPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, address, false);
						}
						break;
					case ID_PRINT_MESSAGE:
						{
							RakNet::BitStream myBitStream(packet->data, packet->length, false);
							RakNet::MessageID messageID;
							myBitStream.Read(messageID);
							RakNet::RakString str;
							myBitStream.Read(str);
							std::cout << str << std::endl;
						}
						break;
					case ID_HEAL_PLAYER:
						{
						RakNet::BitStream myBitStream(packet->data, packet->length, false);
						RakNet::MessageID messageID;
						myBitStream.Read(messageID);
						int _playerID;
						myBitStream.Read(_playerID);
						if (_playerID == currentPlayerTurnID)
						{
							RakNet::SystemAddress addr(packet->systemAddress);
							auto it = find_if(players.begin(), players.end(), [&addr](const Player& obj) { return obj.GetAddress() == addr; });

							if (it != players.end()) //if the player was found, heal them
							{
								std::cout << "Player found. Name is: " << it->GetName() << std::endl;
								it->Heal();

								RakNet::BitStream bs;
								bs.Write(RakNet::MessageID(ID_PRINT_MESSAGE));
								RakNet::RakString str;
								str += "Healed for ";
								str += std::to_string(it->healAmount).data();
								str += ". ";
								str += "Current health is: ";
								str += std::to_string(it->GetHealth()).data();
								bs.Write(str);
								g_rakPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
								currentPlayerTurnID = (currentPlayerTurnID + 1) % g_rakPeerInterface->NumberOfConnections();
								RakNet::BitStream messageBs;
								messageBs.Write(RakNet::MessageID(ID_PRINT_MESSAGE));
								messageBs.Write(RakNet::RakString("It is now %s's turn!", players[currentPlayerTurnID].GetName().data()));
								g_rakPeerInterface->Send(&messageBs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
							}
						}
						else
						{
							RakNet::BitStream bs;
							bs.Write(RakNet::MessageID(ID_PRINT_MESSAGE));
							bs.Write(RakNet::RakString("Command failed. It is not your turn!"));
							g_rakPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
						}

													
						}
						break;
					case ID_ATTACK_PLAYER:
						{
							RakNet::BitStream myBitStream(packet->data, packet->length, false);
							RakNet::MessageID messageID;
							myBitStream.Read(messageID);
							int attackingPlayerID;
							myBitStream.Read(attackingPlayerID);
							RakNet::RakString playerBeingAttacked;
							myBitStream.Read(playerBeingAttacked);
							
							if (attackingPlayerID == currentPlayerTurnID && players[attackingPlayerID].GetName().data() != std::string(playerBeingAttacked)) //player didn't survive the attack
							{

								auto it = find_if(players.begin(), players.end(), [&playerBeingAttacked](const Player& obj){ return obj.GetName() == std::string(playerBeingAttacked); });								

								if (it != players.end()) //if the player was found, attack
								{
									it->RemoveHealth(players[attackingPlayerID].GetAttackPower());

									if (it->GetHealth() <= 0) //player died, remove from game and notify other players
									{
										RakNet::BitStream bs;
										bs.Write(RakNet::MessageID(ID_PRINT_MESSAGE));
										bs.Write(RakNet::RakString("%s has died! They were killed by %s", it->GetName().data(), players[attackingPlayerID].GetName().data()));
										g_rakPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);

										it->isAlive = false;
										g_rakPeerInterface->CloseConnection(it->GetAddress(), true, 0); //kick player from channel
										if (it == players.begin())
										{
											players.erase(players.begin());
										}
										else
										{
											players.erase(it--);											
										}
										
										int itr = -1;
										for each (Player player in players) //reassign player id's
										{
											++itr;
											RakNet::BitStream bs;
											bs.Write(RakNet::MessageID(ID_GET_PLAYER_ID));
											bs.Write(int(itr));//set client id
											players[itr].id = itr; //set local id
											g_rakPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, player.GetAddress(), false);
										}
										if (players.size() > 1)
										{
											currentPlayerTurnID = (currentPlayerTurnID + 1) % g_rakPeerInterface->NumberOfConnections();
											RakNet::BitStream messageBs;
											messageBs.Write(RakNet::MessageID(ID_PRINT_MESSAGE));
											messageBs.Write(RakNet::RakString("It is now %s's turn!", players[currentPlayerTurnID].GetName().data()));
											g_rakPeerInterface->Send(&messageBs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
											
										}
										else
										{
											RakNet::BitStream messageBs;
											messageBs.Write(RakNet::MessageID(ID_PRINT_MESSAGE));
											messageBs.Write(RakNet::RakString("You've won! Congratulations!"));
											g_rakPeerInterface->Send(&messageBs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
											g_rakPeerInterface->CloseConnection(players[0].GetAddress(), true, 0); //kick player from channel
										}
									}
									else //player survived the attack
									{
										RakNet::BitStream bs;
										bs.Write(RakNet::MessageID(ID_PRINT_MESSAGE));
										bs.Write(RakNet::RakString("%s was attacked by %s. Their health is now: %i", it->GetName().data(), players[attackingPlayerID].GetName().data(), it->GetHealth()));
										g_rakPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
										currentPlayerTurnID = (currentPlayerTurnID + 1) % g_rakPeerInterface->NumberOfConnections();
										RakNet::BitStream messageBs;
										messageBs.Write(RakNet::MessageID(ID_PRINT_MESSAGE));
										messageBs.Write(RakNet::RakString("It is now %s's turn!", players[currentPlayerTurnID].GetName().data()));
										g_rakPeerInterface->Send(&messageBs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
									}
									
								}								
								else //couldnt find player
								{
									RakNet::BitStream bs;
									bs.Write(RakNet::MessageID(ID_PRINT_MESSAGE));
									RakNet::RakString str("Player not found.Please try again");
									bs.Write(str);
									g_rakPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
								}
							}
							else if (players[attackingPlayerID].GetName().data() == std::string(playerBeingAttacked) && attackingPlayerID == currentPlayerTurnID) //letting the player know they can't attack themselves
							{
								RakNet::BitStream bs;
								bs.Write(RakNet::MessageID(ID_PRINT_MESSAGE));
								bs.Write(RakNet::RakString("You can't attack yourself!"));
								g_rakPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
							}
							else //not your turn
							{
								RakNet::BitStream bs;
								bs.Write(RakNet::MessageID(ID_PRINT_MESSAGE));
								bs.Write(RakNet::RakString("Command failed. It is not your turn!"));
								g_rakPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
							}
							
						}
						break;
					case ID_GET_PLAYER_ID:
						{
							RakNet::BitStream myBitStream(packet->data, packet->length, false);
							RakNet::MessageID m_ID;
							myBitStream.Read(m_ID);
							int ID;
							myBitStream.Read(ID);
							playerID = ID;
							//printf("My ID is: %i\n", playerID);
						}
						break;
					case ID_QUIT:
						{
						RakNet::BitStream myBitStream(packet->data, packet->length, false);
						g_rakPeerInterface->CloseConnection(packet->systemAddress, true, 0);
						}
						break;
					default:
						// It's a client, so just show the message
						printf("%p\n", packet->data);
						break;
				}
			}
		}
		std::this_thread::sleep_for(std::chrono::microseconds(100));
	}//while isRunning
}

int main()
{
	g_rakPeerInterface = RakNet::RakPeerInterface::GetInstance();

	std::thread inputHandler(InputHandler);
	std::thread packetHandler(PacketHandler);

	Player players[MAX_CONNECTIONS]; //creating an array of players so that we can control them

	while (isRunning)
	{
		if (g_networkState == NS_CreateSocket)
		{
			if (isServer)
			{
				//opening up the server socket
				RakNet::SocketDescriptor socketDescriptors[1];
				socketDescriptors[0].port = SERVER_PORT;
				socketDescriptors[0].socketFamily = AF_INET; // Test out IPV4
				bool isSuccess = g_rakPeerInterface->Startup(MAX_CONNECTIONS, socketDescriptors, 1) == RakNet::RAKNET_STARTED;
				assert(isSuccess);
				g_rakPeerInterface->SetMaximumIncomingConnections(MAX_CONNECTIONS);
				g_networkState = NS_PendingConnection;
				std::cout << "Server waiting on connections.." << std::endl;
			}
			else
			{
				//creating a socket for communication
				RakNet::SocketDescriptor socketDescriptor(CLIENT_PORT, nullptr);
				socketDescriptor.socketFamily = AF_INET;

				while (RakNet::IRNS2_Berkley::IsPortInUse(socketDescriptor.port, socketDescriptor.hostAddress, socketDescriptor.socketFamily, SOCK_DGRAM) == true)
					socketDescriptor.port++;

				g_rakPeerInterface->Startup(8, &socketDescriptor, 1);

				//client connection
				//127.0.0.1 is localhost aka yourself
				RakNet::ConnectionAttemptResult car = g_rakPeerInterface->Connect("127.0.0.1", SERVER_PORT, nullptr, 0);
				RakAssert(car == RakNet::CONNECTION_ATTEMPT_STARTED);
				std::cout << "client attempted connection..waiting for response" << std::endl;
				g_networkState = NS_PendingConnection;
			}
		}
	}

	inputHandler.join();
	packetHandler.join();
	return 0;
}
