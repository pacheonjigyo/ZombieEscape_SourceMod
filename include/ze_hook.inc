

/*
 * =============================================================================
 * Zombie Escape (C)2012 파천지교.  All rights reserved.
 * =============================================================================
 */
 

/*
 * =============================================================================
 * S T A T I C  D E F I N E
 * =============================================================================
 */

static String:GameDescription[32] = "ZM - Escape";


/*
 * =============================================================================
 * H O O K  E V E N T
 * =============================================================================
 */
 
public Action:Event_Spawn(Handle:Event, const String:Name[], bool:Broadcast)
{
  decl Client, Random;
  Client = GetClientOfUserId(GetEventInt(Event, "userid"));

  if(IsStillAlive(Client))
  {
    Escape_SelectClass(Client);
    
    CreateTimer(0.1, ColorRemove, Client);
    CreateTimer(0.1, WeaponRemove, Client);
    
    if(StrEqual(Escape_Type[Client], "생존자"))
    {
      Random = GetRandomInt(0, Index_Survivor - 1);
      SetEntityModel(Client, Escape_Survivor[Random]);
    }
    else if(StrEqual(Escape_Type[Client], "관전자"))
    {
      CreateTimer(0.1, Escape_PreSetSpectate, Client);
    }

    SetEntData(Client, Escape_Noblock, 2, 4, true);
    SetEntProp(Client, Prop_Data, "m_takedamage", 0, 1);
    
    Rendering(Client, FX:FxDistort, Render:RENDER_TRANSADD);
    SetEntPropFloat(Client, Prop_Data, "m_flLaggedMovementValue", 1.0);
    
    CreateTimer(1.0, Protection, Client, TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
  }
}

public Escape_SelectClass(Client)
{
	new Handle:Click = CreateMenu(Escape_ActiveClass);

	AddMenuItem(Click, "0", "돌격 보병");
	AddMenuItem(Click, "1", "저격 소총");
	AddMenuItem(Click, "2", "분대 지원");
	AddMenuItem(Click, "3", "중화기병");

	DisplayMenu(Click, Client, MENU_TIME_FOREVER);
	
	PrintToChat(Client, "%s \x07FFFFFFESC - 병과 선택", Hostname);
}

public Escape_ActiveClass(Handle:Menu, MenuAction:Menu_Action, Client, Select)
{
	if(Menu_Action == MenuAction_Select)
	{
    if(Select == 0)
    {
      GivePlayerItem(Client, "weapon_smg1");
      GivePlayerItem(Client, "weapon_shotgun");
      
      PrintToChat(Client, "%s \x07FFFFFF돌격 보병 - 무기 지급", Hostname);
    }
    else if(Select == 1)
    {
      GivePlayerItem(Client, "weapon_357");
      GivePlayerItem(Client, "weapon_crossbow");
      
      PrintToChat(Client, "%s \x07FFFFFF저격 소총 - 무기 지급", Hostname);
    }
    else if(Select == 2)
    {
      GivePlayerItem(Client, "weapon_physcannon");
      GivePlayerItem(Client, "weapon_ar2");
      GivePlayerItem(Client, "weapon_shotgun");
      
      PrintToChat(Client, "%s \x07FFFFFF분대 지원 - 무기 지급", Hostname);
    }
    else if(Select == 3)
    {
      GivePlayerItem(Client, "weapon_ar2");
      GivePlayerItem(Client, "weapon_rpg");
      
      PrintToChat(Client, "%s \x07FFFFFF중화기병 - 무기 지급", Hostname);
    }
	}
}

public Action:Protection(Handle:Timer, any:Client)
{
  if(IsStillAlive(Client))
  {
    if(Escape_Timer == -1)
    {
      SetEntProp(Client, Prop_Data, "m_takedamage", 2, 1);
      
      return Plugin_Stop;
    }
  }
  
  return Plugin_Handled;
}

public Action:WeaponRemove(Handle:Timer, any:Client)
{
  new Weapon_Max = 20;
  new Weapon_Offset = FindSendPropOffs("CHL2MP_Player", "m_hMyWeapons");

  for(new X, Y = Weapon_Max; X < Y; X += 4)
  {
    new Weapon_Name;
    Weapon_Name = GetEntDataEnt2(Client, Weapon_Offset + X);
    
    if(Weapon_Name > 0)
    {
      RemovePlayerItem(Client, Weapon_Name);
      RemoveEdict(Weapon_Name);
    }
  }

  if(IsStillAlive(Client))
  {
    GivePlayerItem(Client, "weapon_crowbar");
    GivePlayerItem(Client, "weapon_pistol");
    GivePlayerItem(Client, "weapon_frag");
    GivePlayerItem(Client, "weapon_slam");
    
    decl Ammo;
    Ammo = FindDataMapOffs(Client, "m_iAmmo");
    
    SetEntData(Client, Ammo + 44, 10, 4, true);
    SetEntData(Client, Ammo + 40, 10, 4, true);
    SetEntData(Client, Ammo + 32, 25, 4, true);
    SetEntData(Client, Ammo + 28, 120, true);
    SetEntData(Client, Ammo + 24, 100, 4, true);
    SetEntData(Client, Ammo + 20, 100, 4, true);
    SetEntData(Client, Ammo + 16, 600, 4, true);
    SetEntData(Client, Ammo + 12, 480, 4, true);
    SetEntData(Client, Ammo + 4, 900, 4, true);
    
    SetEntData(Client, Ammo + 36, 0, 4, true);
    SetEntData(Client, Ammo + 8, 0, 4, true);
  }
}

public Action:ColorRemove(Handle:Timer, any:Attacker)
{
  SetEntityRenderMode(Attacker, RENDER_TRANSCOLOR);
  SetEntityRenderColor(Attacker, 255, 255, 255, 255);
}

public Action:Event_Death(Handle:Event, const String:Name[], bool:Broadcast)
{
  decl Client;
  decl Random;
  
  Client = GetClientOfUserId(GetEventInt(Event, "userid"));

  if(Escape_Round) CreateTimer(0.5, Escape_PreSetSpectate, Client);

  if(StrEqual(Escape_Type[Client], "숙주 감염체") || StrEqual(Escape_Type[Client], "일반 감염체"))
  {
    Random = GetRandomInt(22, 24);
    Sound(Escape_Sound[Random], _, Client, true);
  }
  
  return Plugin_Handled;
}

public Action:OnTakeDamageHook(Client, &Attacker, &inflictor, &Float:damage, &damagetype)
{
  if(Client != Attacker && Attacker != 0)
  {
    decl Random;
    decl String:AttackerWeapon[32];

    if(StrEqual(Escape_Type[Client], "생존자"))
    {
      if(StrEqual(Escape_Type[Attacker], "숙주 감염체") || StrEqual(Escape_Type[Attacker], "일반 감염체"))
      {
        GetClientWeapon(Attacker, AttackerWeapon, 32);
        
        if(StrEqual(AttackerWeapon, "weapon_crowbar") && damagetype & DMG_CLUB)
        {
          SetEntProp(Client, Prop_Send, "m_iTeamNum", 2);
          Escape_SetZombie(Client, false);
          
          damage = 0.0;

          Random = GetRandomInt(1, 5);
          Sound(Escape_Sound[Random], _, Client, true);
          Sound("npc/antlion_guard/shove1.wav", _, Client, true);
    
          return Plugin_Changed;
        }
      }
      
      Random = GetRandomInt(2, 6);
      Sound(Escape_Sound[Random], _, Client, true);
    }
    else if(StrEqual(Escape_Type[Client], "숙주 감염체") || StrEqual(Escape_Type[Client], "일반 감염체"))
    {
      if(StrEqual(Escape_Type[Attacker], "생존자"))
      {
        GetClientWeapon(Attacker, AttackerWeapon, 32);

        Random = GetRandomInt(13, 18);
        Sound(Escape_Sound[Random], 1.0, Client, true);

        Random = GetRandomInt(19, 21);
        Sound(Escape_Sound[Random], 1.0, Client, true);
        
        if(damagetype & DMG_CLUB)
        {
          decl Float:AttackerPosition[3], Float:ClientPosition[3], Float:Vector[3];

          GetClientEyePosition(Attacker, AttackerPosition);
          GetClientEyePosition(Client, ClientPosition);

          MakeVectorFromPoints(AttackerPosition, ClientPosition, Vector);
          NormalizeVector(Vector, Vector);
          
          ScaleVector(Vector, 1000.0);

          TeleportEntity(Client, NULL_VECTOR, NULL_VECTOR, Vector);

          damage = 1000.0;

          return Plugin_Changed;
        }
        else
        {
          decl Float:AttackerPosition[3], Float:ClientPosition[3], Float:Vector[3];

          GetClientEyePosition(Attacker, AttackerPosition);
          GetClientEyePosition(Client, ClientPosition);

          MakeVectorFromPoints(AttackerPosition, ClientPosition, Vector);
          NormalizeVector(Vector, Vector);
          
          for(new X; X < Index_Weapon; X++)
          {
            if(StrEqual(AttackerWeapon, Escape_Weapon[X][0]))
            {
              ScaleVector(Vector, StringToFloat(Escape_Weapon[X][1]));
            }
          }
          
          TeleportEntity(Client, NULL_VECTOR, NULL_VECTOR, Vector);

          return Plugin_Changed;
        }
      }
    }
  }
  
  return Plugin_Continue;
}

public Action:OnWeaponSwitch(Client, Weapon)
{
  decl String:WeaponClass[32];
  GetEdictClassname(Weapon, WeaponClass, sizeof(WeaponClass));

  if(StrEqual(Escape_Type[Client], "숙주 감염체") || StrEqual(Escape_Type[Client], "일반 감염체"))
  {
    if(!StrEqual(WeaponClass, "weapon_crowbar")) return Plugin_Handled;
  }

  return Plugin_Continue;
}


/*
 * =============================================================================
 * A R G U M E N T S
 * =============================================================================
 */
 

public Action:Event_Player(Handle:Event, const String:Name[], bool:Broadcast)
{
	SetEventBroadcast(Event, true);

	return Plugin_Continue;
}

public Action:OnGetGameDescription(String:Description[64])
{
	strcopy(Description, 64, GameDescription);
	if(Escape_Finish) return Plugin_Changed;
  
	return Plugin_Handled;
}


/*
 * =============================================================================
 * K E Y  B U F F E R S 
 * =============================================================================
 */
 
public Action:OnPlayerRunCmd(Client, &buttons, &impulse, Float:vel[3], Float:angles[3], &weapon)
{
  if(impulse == 51) return Plugin_Handled;
  
  SetEntProp(Client, Prop_Send, "m_iHideHUD", ( 1<<3 ) | ( 1<<8 )); 
  
  if(IsStillAlive(Client))
  {
		if(buttons & IN_SPEED)
		{
      buttons &= ~IN_SPEED;
		}
  }
  
  /*
		else if(buttons & IN_USE)
		{
			if(!PrethinkBuffer[Client])
			{
				//CommandUse(Client);
				PrethinkBuffer[Client] = true;
			}
		}
		else
		{
			PrethinkBuffer[Client] = false;
		}
  }
  */
  
  return Plugin_Continue;
}

public CommandUse(Client)
{
  /*
  decl Entity;
  Entity = GetClientAimTarget(Client, false);
  
  decl Float:Dist, Float:ClientOrigin[3], Float:EntityOrigin[3];
  
  if(StrEqual(Prison_Type[Client], "간수"))
  {
    Entity = GetClientAimTarget(Client, false);

    if(IsStillAlive(Entity) && Prison_Cuff[Entity])
    {
      GetEntPropVector(Client, Prop_Send, "m_vecOrigin", ClientOrigin);
      GetEntPropVector(Entity, Prop_Send, "m_vecOrigin", EntityOrigin);
      
      if(Dist <= 150.0 && Prison_Timer == -1)
      {
        CreateTimer(0.5, Prison_PreSetSpectate, Entity);
        
        Sound("plats/tram_hit1.wav", _, Entity, true);
      }
    }
  }
  */
}

public Action:Escape_PreSetSpectate(Handle:Timer, any:Client)
{
  if(IsStillConnect(Client))
  {
    if(GetClientTeam(Client) != 1) ChangeClientTeam(Client, 1);
    if(!StrEqual(Escape_Type[Client], "관전자")) Escape_Type[Client] = "없음";
  }
}

public Action:Escape_Blocking(Client, Args)
{
  return Plugin_Handled;
}

public Escape_SetParent()
{
  new Count = GetClientCount();
  
  for(new X; X <= Count / 8; X++)
  {
    new MaxSelect = 0;
    new Selects[MaxClients+1];

    for(new Client = 1, Y = MaxClients; Client <= Y; Client++)
    {
      if(IsStillAlive(Client))
      {
        if(StrEqual(Escape_Type[Client], "생존자"))
        {
          MaxSelect++;
          Selects[MaxSelect] = Client;
        }
      }
    }

    if(MaxSelect > 0)
    {
      new Client = Selects[GetRandomInt(1, MaxSelect)];

      if(IsStillAlive(Client))
      {
        ChangeClientTeam(Client, 1);
        ChangeClientTeam(Client, 2);
      
        SDKCall(Timer_Respawn, Client);
        
        Escape_SetZombie(Client, true);
      }
    }
	}
}

public Escape_SetZombie(Client, bool:Parent)
{
  decl Random;
  
  if(Parent)
  {
    Escape_Type[Client] = "숙주 감염체";
    
    SetEntProp(Client, Prop_Data, "m_iHealth", 10000);
    SetEntPropFloat(Client, Prop_Data, "m_flLaggedMovementValue", 1.4);
    
    Random = GetRandomInt(0, Index_Parent - 1);
    SetEntityModel(Client, Escape_Parent[Random]);
  }
  else
  {
    Escape_Type[Client] = "일반 감염체";
    
    SetEntProp(Client, Prop_Data, "m_iHealth", 2500);
    SetEntPropFloat(Client, Prop_Data, "m_flLaggedMovementValue", 1.2);

    Random = GetRandomInt(0, Index_Zombie - 1);
    SetEntityModel(Client, Escape_Zombie[Random]);
  }
  
  Shake(Client, 1.0, 1.0);
  
  Random = GetRandomInt(8, 12);
  Sound(Escape_Sound[Random], _, Client, true);
  
  CreateTimer(0.1, WeaponRemove, Client);
}