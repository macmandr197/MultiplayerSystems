using UnityEngine;
using System.Collections;
using UnityEngine.Networking;

public class Player_NetworkSetup : NetworkBehaviour
{

    [SerializeField] Camera FPSCharacterCam;
    [SerializeField] AudioListener audioListener;

    // Use this for initialization
    public override void OnStartLocalPlayer()
    {
        GameObject.Find("Scene Camera").SetActive(false);
        //GetComponent<CharacterController>().enabled = true;
        GetComponent<UnityStandardAssets.Characters.FirstPerson.FirstPersonController>().enabled = true;
        FPSCharacterCam.enabled = true;
        //audioListener.enabled = true;

        Renderer[] rens = GetComponentsInChildren<Renderer>();
        foreach (Renderer ren in rens)
        {
            ren.enabled = false;
        }

        //GetComponent<NetworkAnimator>().SetParameterAutoSend(0, true);
    }

    public override void PreStartClient()
    {
        //GetComponent<NetworkAnimator>().SetParameterAutoSend(0, true);
    }

}
