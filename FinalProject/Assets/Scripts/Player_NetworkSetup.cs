using UnityEngine;
using System.Collections;
using UnityEngine.Networking;
using UnityEngine.UI;
using UnityStandardAssets.Characters.FirstPerson;

public class Player_NetworkSetup : NetworkBehaviour
{
    public int team { get; set; }

    public GameManager gm;

    [SerializeField] Camera FPSCharacterCam;
    [SerializeField] AudioListener audioListener;
        

    public StartPanel sPanel;

    void OnEnable()
    {
        StartPanel.OnClicked += DestroyStartPanel;
    }

    // Use this for initialization
    public override void OnStartLocalPlayer()
    {
        GameObject.Find("Scene Camera").SetActive(false);

        Cursor.visible = true;

        GameObject.Find("StartPanel").GetComponent<StartPanel>().playerNetwork = this;

        FPSCharacterCam.enabled = true;

        Renderer[] rens = GetComponentsInChildren<Renderer>();
        foreach (Renderer ren in rens)
        {
            ren.enabled = false;
        }
    }

    void DestroyStartPanel()
    {
            CmdDestroyStartPanel();
    }

    [Command]
    public void CmdDestroyStartPanel()
    {
        RpcDestroyStartPanel();
    }

    [ClientRpc]
    public void RpcDestroyStartPanel()
    {
        DP();
    }

    void DP()
    {
        Destroy(GameObject.Find("StartPanel").gameObject);
        EnablePlayer();
    }

    public void EnablePlayer()
    {
        if (!isLocalPlayer)
        {
            return;
        }
        GetComponent<CharacterController>().enabled = true;
        GetComponent<FirstPersonController>().enabled = true;
        //GetComponent<FirstPersonController>().enabled = true;
        GetComponent<Player_SyncPosition>().enabled = true;
        GetComponent<Player_SyncRotation>().enabled = true;
        FPSCharacterCam.enabled = true;
        //audioListener.enabled = true;
        Cursor.visible = false;
        Cursor.lockState = CursorLockMode.Locked;
    }

}
