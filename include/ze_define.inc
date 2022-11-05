

/*
 * =============================================================================
 * Zombie Escape (C)2012 파천지교.  All rights reserved.
 * =============================================================================
 */
 

/*
 * =============================================================================
 * U S E R  S E T T I N G S
 * =============================================================================
 */
 
#define Hostname "\x07FF0000[ZM-Escape]" //채팅 출력 설정

#define Index_Parent 1 //숙주 감염체 스킨 수
#define Index_Zombie 1 //일반 감염체 스킨 수
#define Index_Survivor 1 //생존자 스킨 수

#define Index_Sound  25 //사운드 최대 수
#define Index_Weapon 9 //총기 최대 수
 
 
/*
 * =============================================================================
 * N E W  D E F I N E
 * =============================================================================
 */
 
new Escape_Timer;
 
new bool:Escape_Round = false;
new bool:Escape_Finish = false;

new Escape_Noblock;

//new String:Escape_Last[256];
new String:Escape_Type[33][256];

//숙주 감염체 스킨 배열:
new String:Escape_Parent[Index_Parent][128] =
{
  //스킨 경로:
  "models/zombie/fast.mdl"
};

//일반 감염체 스킨 배열:
new String:Escape_Zombie[Index_Zombie][128] =
{
  //스킨 경로:
  "models/zombie/classic.mdl"
};

//생존자 스킨 배열:
new String:Escape_Survivor[Index_Survivor][128] =
{
  //스킨 경로:
  "models/police.mdl"
};

//사운드 배열:
new String:Escape_Sound[Index_Sound][128] =
{
  //크랙션 소리:
  "ambient/alarms/klaxon1.wav",
  
  
  //생존자 맞을시 소리:
  "ambient/voices/citizen_beaten1.wav",
  "ambient/voices/citizen_beaten2.wav",
  "ambient/voices/citizen_beaten3.wav",
  "ambient/voices/citizen_beaten4.wav",
  "ambient/voices/citizen_beaten5.wav",
  
  "npc/antlion_guard/shove1.wav",
  
  
  //반란시 소리:
  "npc/combine_gunship/gunship_pain.wav",
  
  
  //좀비로 감염될시 소리:
  "npc/fast_zombie/fz_frenzy1.wav",
  "npc/fast_zombie/fz_scream1.wav",
  
  "npc/zombie_poison/pz_pain1.wav",
  "npc/zombie_poison/pz_pain2.wav",
  "npc/zombie_poison/pz_pain3.wav",
  
  
  //좀비 맞을시 소리:
  "npc/zombie/zombie_pain1.wav",
  "npc/zombie/zombie_pain2.wav",
  "npc/zombie/zombie_pain3.wav",
  "npc/zombie/zombie_pain4.wav",
  "npc/zombie/zombie_pain5.wav",
  "npc/zombie/zombie_pain6.wav",

  "npc/zombie/claw_strike1.wav",
  "npc/zombie/claw_strike2.wav",
  "npc/zombie/claw_strike3.wav",
  
  
  //좀비 죽을시 소리:
  "npc/zombie/zombie_die1.wav",
  "npc/zombie/zombie_die2.wav",
  "npc/zombie/zombie_die3.wav"
};

//무기 배열:
new String:Escape_Weapon[Index_Weapon][2][128] =
{
  //첫번째 배열 : 무기 이름
  //두번째 배열 : 넉백 크기
  
  {"weapon_pistol", "150.0"},
  {"weapon_357", "500.0"},
  
  {"weapon_smg1", "120.0"},
  {"weapon_ar2", "100.0"},
  
  {"weapon_shotgun", "350.0"},
  {"weapon_crossbow", "500.0"},
  
  {"weapon_frag", "250.0"},
  {"weapon_rpg", "750.0"},
  {"weapon_slam", "500.0"}
};

//상태 배열:
new String:Escape_Health[6][2][128] =
{
  //첫번째 배열 : 상태 표시
  //두번째 배열 : 해당 체력
  
  {"생명이 위험합니다.", "15"},
  {"중상을 입었습니다.", "40"},
  {"경상을 입었습니다.", "60"},
  {"가벼운 타박상입니다.", "75"},
  {"컨디션이 나쁩니다.", "85"},
  {"매우 건강합니다.", "90"}
};

new Handle:Timer_Respawn = INVALID_HANDLE;

/*
 * =============================================================================
 * B A S E  F U N C T I O N
 * =============================================================================
 */
 
public OnMapStart()
{
  CreateTimer(5.0, Escape_Start, _, TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);

  Escape_Round = false;
  Escape_Finish = true;
  
  for(new X; X < Index_Parent; X++) PrecacheModel(Escape_Parent[X], true);
  for(new X; X < Index_Zombie; X++) PrecacheModel(Escape_Zombie[X], true);
  for(new X; X < Index_Survivor; X++) PrecacheModel(Escape_Survivor[X], true);
  for(new X; X < Index_Sound; X++) PrecacheSound(Escape_Sound[X], true);
  
  //SetLightStyle(0, "a");
}

public OnMapEnd() Escape_Finish = false;


/*
 * =============================================================================
 * F X  D E F I N E
 * =============================================================================
 */
 
enum FX
{
	FxNone = 0,
	FxPulseFast,
	FxPulseSlowWide,
	FxPulseFastWide,
	FxFadeSlow,
	FxFadeFast,
	FxSolidSlow,
	FxSolidFast,
	FxStrobeSlow,
	FxStrobeFast,
	FxStrobeFaster,
	FxFlickerSlow,
	FxFlickerFast,
	FxNoDissipation,
	FxDistort,
	FxHologram,
	FxExplode,            
	FxGlowShell,          
	FxClampMinScale,      
	FxEnvRain,           
	FxEnvSnow,         
	FxSpotlight,     
	FxRagdoll,
	FxPulseFastWider,
};

enum Render
{
	Normal = 0, 		
	TransColor, 		
	TransTexture,		
	Glow,			
	TransAlpha,			
	TransAdd,			
	Environmental,		
	TransAddFrameBlend,	
	TransAlphaAdd,		
	WorldGlow,
	None,
};