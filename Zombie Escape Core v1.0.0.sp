

/*
 * =============================================================================
 * Prison Break (C)2012 파천지교.  All rights reserved.
 * =============================================================================
 */
 
#define Index_Respawn		22

/*
 * =============================================================================
 * I N C L U D E
 * =============================================================================
 */
 
#include<sdktools>
#include<sdkhooks>

#include<sourcemod>

#include<ze_define>
#include<ze_stock>
#include<ze_hook>
#include<ze_round>


/*
 * =============================================================================
 * B A S E  F U N C T I O N
 * =============================================================================
 */
 
public OnPluginStart()
{
  RegConsoleCmd("kill", Escape_Blocking);
  RegConsoleCmd("explode", Escape_Blocking);
  RegConsoleCmd("jointeam", Escape_Blocking);
  RegConsoleCmd("spectate", Escape_Blocking);
  
  HookEvent("player_spawn", Event_Spawn, EventHookMode_Pre);
  HookEvent("player_death", Event_Death, EventHookMode_Pre);

  HookEvent("player_team", Event_Player, EventHookMode_Pre);
  HookEvent("player_connect", Event_Player, EventHookMode_Pre);
  HookEvent("player_disconnect", Event_Player, EventHookMode_Pre);
  HookEvent("player_changename", Event_Player, EventHookMode_Pre);
  
  Escape_Noblock = FindSendPropOffs("CBaseEntity", "m_CollisionGroup");
  
  StartPrepSDKCall(SDKCall_Player);
  PrepSDKCall_SetVirtual(Index_Respawn);
  Timer_Respawn = EndPrepSDKCall();
}

public OnClientPutInServer(Client)
{
  if(Escape_Round) CreateTimer(0.1, Escape_PreSetSpectate, Client);
  
  SDKHook(Client, SDKHook_OnTakeDamage, OnTakeDamageHook);
  SDKHook(Client, SDKHook_WeaponSwitch, OnWeaponSwitch);
}