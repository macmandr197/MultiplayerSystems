using System.Collections;
using System.Collections.Generic;
using UnityEngine.Networking;
using UnityEngine.UI;
using UnityEngine;

public class NetworkManager_Custom : NetworkManager
{
    public void StartupHost()
    {
        SetPort();
        singleton.StartHost();
    }

    public void JoinGame()
    {
        SetIPAddress();
        SetPort();

        singleton.StartClient();
    }

    void SetIPAddress()
    {
        string ipAddress = GameObject.Find("InputFieldIPAddress").transform.Find("Text").GetComponent<Text>().text;
        singleton.networkAddress = ipAddress;
    }

    void SetPort()
    {
        singleton.networkPort = 7777;
    }

    void OnLevelWasLoaded(int level)
    {
        if (level == 0)
        {
            SetupMenuSceneButtons();
        }
        else
        {
            SetupOtherSceneButtons();
        }
    }

    void SetupMenuSceneButtons()
    {
        GameObject.Find("ButtonStartHost").GetComponent<Button>().onClick.RemoveAllListeners();
        GameObject.Find("ButtonStartHost").GetComponent<Button>().onClick.AddListener(StartupHost);

        GameObject.Find("ButtonJoinGame").GetComponent<Button>().onClick.RemoveAllListeners();
        GameObject.Find("ButtonJoinGame").GetComponent<Button>().onClick.AddListener(JoinGame);
    }

    void SetupOtherSceneButtons()
    {
        GameObject.Find("ButtonDisconnect").GetComponent<Button>().onClick.RemoveAllListeners();
        GameObject.Find("ButtonDisconnect").GetComponent<Button>().onClick.AddListener(singleton.StopHost);
    }
}
