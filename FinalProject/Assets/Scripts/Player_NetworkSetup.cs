using UnityEngine;
using System.Collections;
using UnityEngine.Networking;
using UnityEngine.UI;

public class Player_NetworkSetup : NetworkBehaviour
{
    public int team { get; set; }

    [SerializeField] Camera FPSCharacterCam;
    [SerializeField] AudioListener audioListener;


    // Use this for initialization
    public override void OnStartLocalPlayer()
    {
        GameObject.Find("Scene Camera").SetActive(false);
        /*GetComponent<CharacterController>().enabled = true;
        GetComponent<UnityStandardAssets.Characters.FirstPerson.FirstPersonController>().enabled = true;
        GetComponent<Player_SyncPosition>().enabled = true;
        GetComponent<Player_SyncRotation>().enabled = true;
        FPSCharacterCam.enabled = true;
        audioListener.enabled = true;*/
        
            Cursor.visible = true;
        /*Cursor.visible = false;
        Cursor.lockState = CursorLockMode.Locked;*/

        FPSCharacterCam.enabled = true;

        Renderer[] rens = GetComponentsInChildren<Renderer>();
        foreach (Renderer ren in rens)
        {
            ren.enabled = false;
        }
    }

    [Command]
    public void CmdStartCTF()
    {

    }

    void Start()
    {
    }

}
