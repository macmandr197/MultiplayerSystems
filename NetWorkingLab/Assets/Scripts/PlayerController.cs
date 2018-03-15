using System.Collections;
using System.Collections.Generic;
using UnityEngine.Networking;
using UnityEngine;
using UnityEngine.Experimental.Rendering;

public class PlayerController : NetworkBehaviour
{

    public GameObject bulletPrefab;
    public Transform bulletSpawn;

	// Use this for initialization
	public override void OnStartLocalPlayer ()
	{
	    GetComponent<MeshRenderer>().material.color = Color.blue;


	}

    [Command]
    void CmdFire()
    {
        var bullet = (GameObject) Instantiate(bulletPrefab, bulletSpawn.position, bulletSpawn.rotation);

        bullet.GetComponent<Rigidbody>().velocity = bullet.transform.forward * 6;

        NetworkServer.Spawn(bullet);

        Destroy(bullet, 2.0f);
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

	    if (Input.GetKeyDown(KeyCode.Space))
	    {
	        CmdFire();
	    }
	}
}
