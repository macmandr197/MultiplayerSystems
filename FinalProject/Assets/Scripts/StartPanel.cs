using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Networking;
using UnityEngine.UI;

public class StartPanel : NetworkBehaviour
{

    public Text connectionsText;
    public Text team1Text;
    public Text team2Text;

    public Button startButton;

    [SyncVar] public int numPlayers;
    [SyncVar(hook = "IncrementReadyPlayers")] public int numReadyPlayers;

	// Use this for initialization
	void Start () 
	{
		
	}

    public void IncrementReadyPlayers(int val)
    {
        numReadyPlayers += val;
    }

    [ClientRpc]
    public void RpcEnableStartButton()
    {
        startButton.interactable = true;
    }
	
	// Update is called once per frame
	void Update ()
	{
	    if (isServer)
	    {
	        numPlayers = NetworkServer.connections.Count;


	        if (numReadyPlayers == numPlayers && numPlayers > 1)
	        {
	            RpcEnableStartButton();
	        }
        }
	        connectionsText.text = "Total Connections: " + numPlayers + "/4";

	    
	}
}
