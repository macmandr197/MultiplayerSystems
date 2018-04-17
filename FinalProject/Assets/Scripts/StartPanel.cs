using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Networking;
using UnityEngine.UI;

public class StartPanel : NetworkBehaviour
{

    public Player_NetworkSetup playerNetwork;

    public Text connectionsText;
    public Text readyText;

    private bool isReady = false;

    public Button readyButton;
    public Button startButton;

    [SyncVar] public int numPlayers;
    [SyncVar] public int numReadyPlayers;

    public delegate void ClickAction();

    public static event ClickAction OnClicked;

    //red is new Color(1f, 0.670f, 0.670f)
    //green is new Color(0.670f, 1f, 0.670f)
	// Use this for initialization
	void Start () 
	{
	    ChangeReadyButtonColor(new Color(1f, 0.670f, 0.670f));
	    readyButton.GetComponentInChildren<Text>().text = "Not Ready";
	    if (!isServer)
	    {
	        startButton.gameObject.SetActive(false);
	    }
	}

    void ChangeReadyButtonColor(Color color)
    {
        ColorBlock cb = readyButton.colors;
        cb.normalColor = color;
        cb.highlightedColor = color * 1.125f;
        cb.pressedColor = color * 0.75f;
        readyButton.colors = cb;
    }

    public void UpdateReadyButtonColor()
    {
        isReady = !isReady;
    }

    public void IncrementReadyPlayers(int val)
    {
        if (!isReady)
        {
            numReadyPlayers += 1;
        }
        else if (isReady)
        {
            numReadyPlayers -= 1;
        }
        
    }

    public void StartButtonOnClick()
    {
        if (OnClicked != null)
        {
            OnClicked();
        }
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
	    readyText.text = "Ready Players: " + numReadyPlayers + "/" + numPlayers;

	    if (isReady)
	    {
	        readyButton.GetComponentInChildren<Text>().text = "Ready";
	        ChangeReadyButtonColor(new Color(0.670f, 1f, 0.670f));
        }
	    else
	    {
	        readyButton.GetComponentInChildren<Text>().text = "Not Ready";
	        ChangeReadyButtonColor(new Color(1f, 0.670f, 0.670f));
        }
	}
}
