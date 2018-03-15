using System.Collections;
using System.Collections.Generic;
using UnityEngine.UI;
using UnityEngine.Networking;
using UnityEngine;

public class Health : NetworkBehaviour
{
    public RectTransform healthBar;

    public bool destroyOnDeath;

    public const int MaxHealth = 100;
    [SyncVar(hook = "OnChangedHealth")]
    public int currentHealth = MaxHealth;

    public void TakeDamage(int amount)
    {
        if (!isServer)
        {
            return;
        }
        currentHealth -= amount;
        if (currentHealth <= 0)
        {
            if (destroyOnDeath)
            {
                Destroy(gameObject);
            }
            else
            {
                currentHealth = MaxHealth;
                RpcRespawn();
            }
                
        }
    }

    void OnChangedHealth(int health)
    {
        healthBar.sizeDelta = new Vector2(health, healthBar.sizeDelta.y);
    }

    [ClientRpc]
    void RpcRespawn()
    {
        if (isLocalPlayer)
        {
            transform.position = Vector3.zero;
        }
    }
}
