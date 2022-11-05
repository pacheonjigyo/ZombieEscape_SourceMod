

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
 
static Escape_Minute;
static Escape_Minutelimit;

static Escape_Second;
static Escape_Secondlimit;

static Escape_Winner;

//static bool:Prison_Rebel;


/*
 * =============================================================================
 * S Y S T E M  F U N C T I O N
 * =============================================================================
 */
 
public Action:Escape_Start(Handle:Timer)
{
  if(GetClientCount() > 0)
  {
    CreateTimer(1.0, Escape_Reset, _, TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
  }
  else
  {
    CreateTimer(5.0, Escape_Start, _, TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
  }
  
  return Plugin_Stop;
}

public Action:Escape_Check(Handle:Timer)
{
  if(GetClientCount() == 0)
  {
    CreateTimer(5.0, Escape_Start, _, TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
  
    return Plugin_Stop;
  }
  
  if(Escape_Round)
  {
    new Survivor, Parasite;
    new String:Health[33][128];
    
    for(new Client = 1; Client <= MaxClients; Client++)
    {
      if(IsStillConnect(Client))
      {
        if(IsPlayerAlive(Client))
        {
          //if(Escape_Timer == -1) SetEntData(Client, Escape_Noblock, 5, 4, true);
          
          if(StrEqual(Escape_Type[Client], "생존자"))
          {
            Survivor++;
            
            for(new X; X < 6; X++)
            {
              if(StringToInt(Escape_Health[X][1]) <= GetClientHealth(Client))
              {
                Health[Client] = Escape_Health[X][0];
              }
            }
              
            if(Escape_Secondlimit >= 10)
            {
              SetHudTextParams(0.02, 0.79, 1.0, 255, 255, 255, 255);
              ShowHudText(Client, -1, "\n%s\n\n생존 시간 0%d : %d", Health[Client], Escape_Minutelimit, Escape_Secondlimit);
            }
            else
            {
              SetHudTextParams(0.02, 0.79, 1.0, 255, 255, 255, 255);
              ShowHudText(Client, -1, "\n%s\n\n생존 시간 0%d : 0%d", Health[Client], Escape_Minutelimit, Escape_Secondlimit);
            }
          }
          else if(StrEqual(Escape_Type[Client], "숙주 감염체") || StrEqual(Escape_Type[Client], "일반 감염체"))
          {
            Parasite++;
            
            SetHudTextParams(0.02, 0.79, 1.0, 255, 255, 255, 255);
            ShowHudText(Client, -1, "\n생존력 %d%", GetClientHealth(Client));
          }
        }
        
        if(Escape_Minute >= 0)
        {
          if(Escape_Second >= 10)
          {
            SetHudTextParams(0.02, 0.79, 1.0, 255, 255, 255, 255);
            ShowHudText(Client, -1, "남은 시간 0%d : %d", Escape_Minute, Escape_Second);
          }
          else
          {
            SetHudTextParams(0.02, 0.79, 1.0, 255, 255, 255, 255);
            if(Escape_Minute == 0) SetHudTextParams(0.02, 0.79, 1.0, 255, 0, 0, 255);
            ShowHudText(Client, -1, "남은 시간 0%d : 0%d", Escape_Minute, Escape_Second);
          }
        }
        else
        {
          if(StrEqual(Escape_Type[Client], "숙주 감염체") || StrEqual(Escape_Type[Client], "일반 감염체")) ForcePlayerSuicide(Client);
        }
      }
    }
    
    Escape_Action();
    
    if(Parasite == 0 && Survivor == 0)
    {
      Escape_Round = false;
      Escape_Winner = 0;
    }
    else if(Parasite == 0)
    {
      Escape_Round = false;
      Escape_Winner = 1;
    }
    else if(Survivor == 0)
    {
      Escape_Round = false;
      Escape_Winner = 2;
    }
  }
  else
  {
    Escape_Victory();
    
    return Plugin_Stop;
  }
  
  return Plugin_Handled;
}

public Escape_Victory()
{
  PrintToChatAll("%s \x07FFFFFF라운드가 끝났습니다.", Hostname);
  
  switch(Escape_Winner)
  {
    case 0:
    {
      //
    }
    
    case 1:
    {
      SetHudTextParams(-1.0, -1.0, 5.0, 255, 255, 255, 255);
      ShowHudTextAll("생존자가 승리하였습니다.");
    }
    
    case 2:
    {
      SetHudTextParams(-1.0, -1.0, 5.0, 255, 255, 255, 255);
      ShowHudTextAll("감염체가 승리하였습니다.");
    }
  }
  
  CreateTimer(5.0, Escape_Bridge);
}

public Action:Escape_Bridge(Handle:Timer)
{
  CreateTimer(1.0, Escape_Reset, _, TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
}

public Action:Escape_Reset(Handle:Timer)
{
  if(!Escape_Round)
  {
    for(new Client = 1; Client <= MaxClients; Client++)
    {
      if(IsStillConnect(Client))
      {
        if(IsPlayerAlive(Client)) ChangeClientTeam(Client, 1);
        
        Escape_Type[Client] = "생존자";
        ChangeClientTeam(Client, 3);
      }
    }
    
    Escape_Timer = 30;
    
    Escape_Round = true;
    //Prison_Rebel = false;
    
    ServerCommand("mp_restartgame 1");
  }
  else
  {
    if(Escape_Timer > 0)
    {
      Escape_Timer--;
      
      if(Escape_Timer <= 30)
      {
        SetHudTextParams(-1.0, -1.0, 1.0, 255, 255, 255, 255);
            
        if(Escape_Timer >= 10)
        {
          ShowHudTextAll("첫번째 감염이 %d 초 후에 발생합니다.", Escape_Timer);
        }
        else
        {
          ShowHudTextAll("첫번째 감염이 0%d 초 후에 발생합니다.", Escape_Timer);
        }
      }
    }
    else
    {
      if(Escape_Timer == 0)
      {
        SetHudTextParams(-1.0, -1.0, 1.0, 255, 255, 255, 255);
        ShowHudTextAll("숙주 감염체가 감지되었습니다.", Escape_Timer);
          
        Escape_SetParent();
        
        Escape_Timer = -1;
        
        Escape_Minute = 7;
        Escape_Minutelimit = 0;
        
        Escape_Second = 0;
        Escape_Secondlimit = 0;
        
        CreateTimer(1.0, Escape_Check, _, TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
        
        Sound("ambient/alarms/klaxon1.wav");
        
        return Plugin_Stop;
      }
    }
  }
  
  return Plugin_Handled;
}

public Escape_Action()
{
	if(Escape_Second > 0)
	{
    Escape_Second--;
  }
	else
	{
		Escape_Second = 59;
		Escape_Minute--;
	}

	if(Escape_Second > 59)
	{
		Escape_Minute++;
		Escape_Second -= 59;
	}
	
	if(Escape_Secondlimit < 59)
	{
    Escape_Secondlimit++;
  }
	else
	{
		Escape_Secondlimit = 0;
		Escape_Minutelimit++;
	}

	if(Escape_Secondlimit < 0)
	{
		Escape_Minutelimit--;
		Escape_Secondlimit += 1;
	}
}