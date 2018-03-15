using System.Collections;
using System.Collections.Generic;
using UnityEngine.Networking;
using UnityEngine;

public class PlayerController : NetworkBehaviour
{

	// Use this for initialization
	void Start () 
	{
		
	}
	
	// Update is called once per frame
	void Update ()
	{
	    if (!isLocalPlayer)
	    {
	        return;
	    }
	    var x = Input.GetAxis("Horizontal") * Time.deltaTime * 150.0f;
	    var z = Input.GetAxis("Vertical") * Time.deltaTime * 3.0f;

        transform.Rotate(0f, x, 0f);
        transform.Translate(0f, 0f, z);
	}
}
