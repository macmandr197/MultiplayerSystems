using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Networking;

public class GameManager : NetworkBehaviour
{
    public List<Player_NetworkSetup> playerNetworkList = new List<Player_NetworkSetup>();
	// Use this for initialization
	void Start () 
	{
		
	}

    public void AddToList(Player_NetworkSetup pn)
    {
        playerNetworkList.Add(pn);
    }
	
	// Update is called once per frame
	void Update () 
	{
		
	}
}
