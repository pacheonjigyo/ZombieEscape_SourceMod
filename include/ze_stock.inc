

/*
 * =============================================================================
 * Zombie Escape (C)2012 파천지교.  All rights reserved.
 * =============================================================================
 */
 

/*
 * =============================================================================
 * B O O L  F U N C T I O N
 * =============================================================================
 */
 
public bool:IsStillAlive(Client)
{
	if(IsValidEntity(Client))
	{
		if(Client < GetMaxClients() +1)
		{
			if(Client != 0)
			{
				if(IsClientConnected(Client))
				{
					if(IsPlayerAlive(Client))
					{
						return true;
					}
				}
			}
		}
	}	return false;
}

public bool:IsStillConnect(Client)
{
	if(IsValidEntity(Client))
	{
		if(Client <= GetMaxClients())
		{
			if(Client != 0)
			{
				if(IsClientConnected(Client))
				{
					return true;
				}
			}
		}
	}	return false;
}


/*
 * =============================================================================
 * C A L L B A C K  S T O C K
 * =============================================================================
 */
 
stock Sound(const String:Path[], Float:Vol=1.0, Client=0, bool:Origin=false)
{
  if(Client == 0)
  {
    EmitSoundToAll(Path, SOUND_FROM_PLAYER, _, _, _, Vol);
  }
  else
  {
    if(Origin)
    {
      if(IsClientConnected(Client) && IsPlayerAlive(Client))
      {
        decl Float:ClientOrigin[3];
        GetClientAbsOrigin(Client, ClientOrigin);
        
        EmitSoundToAll(Path, SOUND_FROM_WORLD, _, _, _, Vol, _, _, ClientOrigin);
      }
    }
    else
    {
      EmitSoundToClient(Client, Path, SOUND_FROM_PLAYER, _, _, _, Vol);
    }
  }
}

stock SayText2(Client, Target, const String:Message[])
{ 
  decl Handle:Buffer;
  
  if(Target == 0)
  {
    Buffer = StartMessageAll("SayText2");
  }
  else
  {
    Buffer = StartMessageOne("SayText2", Target);
  }
  
  if(Buffer != INVALID_HANDLE)
  { 	
    BfWriteByte(Buffer, Client);
    BfWriteByte(Buffer, true);
    BfWriteString(Buffer, Message);
    EndMessage();
  }
}

stock Rendering(Client, FX:fx=FxNone, Render:render=Normal)
{
	SetEntProp(Client, Prop_Send, "m_nRenderFX", _:fx, 1);
	SetEntProp(Client, Prop_Send, "m_nRenderMode", _:render, 1);
}

stock Shake(Client, Float:Length, Float:Severity)
{
 decl Handle:View_Message;
 View_Message = StartMessageOne("Shake", Client);
 
 BfWriteByte(View_Message, 0);
 BfWriteFloat(View_Message, Severity);
 BfWriteFloat(View_Message, 10.0);
 BfWriteFloat(View_Message, Length);
 
 EndMessage();
}

stock ShowHudTextAll(const String:format[], any:...)
{
	decl String:buffer[192];
	
	for(new Client = 1; Client <= MaxClients; Client++)
	{
		if(IsStillConnect(Client))
		{
			SetGlobalTransTarget(Client);
			VFormat(buffer, sizeof(buffer), format, 2);
			
			ShowHudText(Client, -1, "%s", buffer);
		}
	}
}