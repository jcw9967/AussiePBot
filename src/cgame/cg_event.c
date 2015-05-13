/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2000-2006 Tim Angus

This file is part of Tremulous.

Tremulous is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Tremulous is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Tremulous; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

// cg_event.c -- handle entity events at snapshot or playerstate transitions


#include "cg_local.h"

/*
=============
CG_Obituary
=============
*/
static void CG_Obituary( entityState_t *ent )
{
  int           mod;
  int           target, attacker;
  int           x;
  char          *message;
  char          *message2;
  const char    *targetInfo;
  const char    *attackerInfo;
  char          targetName[ 32 ];
  char          attackerName[ 32 ];
  char          className[ 64 ];
  clientInfo_t  *ci;
  qboolean      teamKill = qfalse;

  target = ent->otherEntityNum;
  attacker = ent->otherEntityNum2;
  mod = ent->eventParm;

  if( target < 0 || target >= MAX_CLIENTS )
    CG_Error( "CG_Obituary: target out of range" );

  ci = &cgs.clientinfo[ target ];

  if( attacker < 0 || attacker >= MAX_CLIENTS )
  {
    attacker = ENTITYNUM_WORLD;
    attackerInfo = NULL;
  }
  else
  {
    attackerInfo = CG_ConfigString( CS_PLAYERS + attacker );
    if( ci && cgs.clientinfo[ attacker ].team == ci->team )
      teamKill = qtrue;
  }

  targetInfo = CG_ConfigString( CS_PLAYERS + target );

  if( !targetInfo )
    return;

  Q_strncpyz( targetName, Info_ValueForKey( targetInfo, "n" ), sizeof( targetName ) - 2 );
  strcat( targetName, S_COLOR_WHITE );

  message2 = "";

  // check for single client messages
  if( cg_killmsg.integer == 1 )
  {
    switch( mod )
    {
      case MOD_SUICIDE:
        x = rand() % 9;
        switch(x)
        {
          case 0:message = "suicides";break;
          case 1:message = "became a lemming...";break;
          case 2:message = "decided to end it all";break;
          case 3:message = "touched Mr. Death";break;
          case 4:message = "couldn't handle the pressure";break;
          case 5:message = "slash killed";break;
          case 6:message = "got depressed and killed himself";break;
          case 7:message = "went across the street, not down the road";break;
          case 8:message = "decided life wasn't worth living anymore";break;
        }break;
      case MOD_FALLING:
        x = rand() % 9;
        switch(x)
        {
          case 0:message = "fell fowl to gravity";break;
          case 1:message = "jumped off...";break;
          case 2:message = "yelled GERONIMO!!!";break;
          case 3:message = "did a cannonball in the wrong spot";break;
          case 4:message = "tried to miss the floor";break;
          case 5:message = "hoped to wake up before he hit the ground";break;
          case 6:message = "crash landed";break;
          case 7:message = "was unadjusted to the gravity around here";break;
          case 8:message = "kept his legs straight when he hit the ground";break;
        }break;
      case MOD_CRUSH:
        x = rand() % 6;
        switch(x)
        {
          case 0:message = "was squished";break;
          case 1:message = "got crushed between two walls";break;
          case 2:message = "couldn't handle the pressure";break;
          case 3:message = "can now fit in a folder";break;
          case 4:message = "lost some weight";break;
          case 5:message = "was in the wrong place at the wrong time";break;
        }break;
      case MOD_WATER:
        x = rand() % 6;
        switch(x)
        {
          case 0:message = "forgot to pack a snorkel";break;
          case 1:message = "needs scuba lessons";break;
          case 2:message = "hasn't grown gills yet";break;
          case 3:message = "is sleeping with the fishes";break;
          case 4:message = "found an incompatible atmosphere";break;
          case 5:message = "found Davy Jones' locker";break;
        }break;
      case MOD_SLIME:
        x = rand() % 6;
        switch(x)
        {
          case 0:message = "melted";break;
          case 1:message = "took a hot bath";break;
          case 2:message = "left his Hazmat suit at home";break;
          case 3:message = "thought acid only smelled bad";break;
          case 4:message = "couldn't find an Alkaline";break;
          case 5:message = "took a dip in the wrong pool";break;
        }break;
      case MOD_LAVA:
        x = rand() % 5;
        switch(x)
        {
          case 0:message = "does a back flip into the lava";break;
          case 1:message = "took a hot bath";break;
          case 2:message = "got roasted";break;
          case 3:message = "didn't feel the heat";break;
          case 4:message = "burnt to death";break;
        }break;
      case MOD_TARGET_LASER:
        message = "saw the light";
        break;
      case MOD_TRIGGER_HURT:
        message = "was in the wrong place";
        break;
      case MOD_HSPAWN:
        x = rand() % 2;
        switch(x)
        {
          case 0:message = "should have run further";break;
          case 1:message = "died in the enemies base";break;
        }break;
      case MOD_ASPAWN:
        x = rand() % 2;
        switch(x)
        {
          case 0:message = "shouldn't have trod in the acid";break;
          case 1:message = "shouldn't have killed that egg...!";break;
        }break;
      case MOD_MGTURRET:
        x = rand() % 4;
        switch(x)
        {
          case 0:message = "was gunned down by a turret";break;
          case 1:message = "was no match for a lil turret";break;
          case 2:message = "was too slow for the turrets";break;
          case 3:message = "wanted to check if turret ammo is limited";break;
        }break;
      case MOD_TESLAGEN:
        x = rand() % 5;
        switch(x)
        {
          case 0:message = "was zapped by a tesla generator";break;
          case 1:message = "was electrocuted so hard";break;
          case 2:message = "was fried by a tesla";break;
          case 3:message = "came to a shocking conclusion with a tesla";break;
          case 4:message = "felt all warm and fuzzy and dead";break;
        }break;
      case MOD_ATUBE:
        x = rand() % 3;
        switch(x)
        {
          case 0:message = "was melted by an acid tube";break;
          case 1:message = "was killed by a baby acid tube";break;
          case 2:message = "took an acid shower to remove his dead skin";break;
        }break;
      case MOD_OVERMIND:
        x = rand() % 8;
        switch(x)
        {
          case 0:message = "got too close to the Overmind";break;
          case 1:message = "was distracted by the Overmind's voluptuous curves";break;
          case 2:message = "was shooed away by the Overmind";break;
          case 3:message = "was eaten by the Overmind";break;
          case 4:message = "was fed to the Alien bots by the Overmind";break;
          case 5:message = "was mangled by the Overmind";break;
          case 6:message = "got spanked by the Overmind";break;
          case 7:message = "found out what sex was like, in Tremulous!";break;
        }break;
      case MOD_REACTOR:
        x = rand() % 5;
        switch(x)
        {
          case 0:message = "got too close to the reactor";break;
          case 1:message = "found where the humans get all their electricity";break;
          case 2:message = "got a major energy boost";break;
          case 3:message = "reacted bad to the reactor";break;
          case 4:message = "was made dizzy by the reactor's spinning and died";break;
        }break;
      case MOD_SLOWBLOB:
        x = rand() % 3;
        switch(x)
        {
          case 0:message = "should have visited a medical station";break;
          case 1:message = "got pwnt by a battlegranger";break;
          case 2:message = "found out grangers spitting isn't pleasurable";break;
        }break;
      case MOD_SWARM:
        x = rand() % 5;
        switch(x)
        {
          case 0:message = "was hunted down by the swarm";break;
          case 1:message = "blood was sucked by the mosquitoes";break;
          case 2:message = "poked the Hive";break;
          case 3:message = "should have sprinted when he heard the buzzing";break;
          case 4:message = "annoyed the insects";break;
        }break;
      default:
        message = NULL;
        break;
    }

    if( attacker == target )
    {
      switch( mod )
      {
      case MOD_FLAMER_SPLASH:
        x = rand() % 6;
        switch(x)
        {
          case 0:message = "toasted himself";break;
          case 1:message = "wasn't hot enough to suit himself";break;
          case 2:message = "wasn't prepared for high temperatures";break;
          case 3:message = "played with fire";break;
          case 4:message = "started the barbecue with the wrong match";break;
          case 5:message = "had a spark, but it got to his head";break;
        }break;
      case MOD_LCANNON_SPLASH:
        x = rand() % 5;
        switch(x)
        {
          case 0:message = "pointed the wrong end at the enemy";break;
          case 1:message = "saw the light";break;
          case 2:message = "over-SHOOPed";break;
          case 3:message = "made a little mistake with his superior weaponry";break;
          case 4:message = "made a messy discharge";break;
        }break;
      case MOD_GRENADE:
        x = rand() % 5;
        switch(x)
        {
          case 0:message = "tried to put the pin back in";break;
          case 1:message = "accidentally pulled the pin early";break;
          case 2:message = "failed to do a nadejump";break;
          case 3:message = "was holding a thermal detonator!";break;
          case 4:message = "just tried to become a bird, maybe he can now in the afterlife";break;
        }break;
      }
    }

    if( message )
    {
      CG_Printf( "%s %s.\n", targetName, message );
      return;
    }

    // check for double client messages
    if( !attackerInfo )
    {
      attacker = ENTITYNUM_WORLD;
      strcpy( attackerName, "noname" );
    }
    else
    {
      Q_strncpyz( attackerName, Info_ValueForKey( attackerInfo, "n" ), sizeof( attackerName ) - 2);
      strcat( attackerName, S_COLOR_WHITE );
      // check for kill messages about the current clientNum
      if( target == cg.snap->ps.clientNum )
        Q_strncpyz( cg.killerName, attackerName, sizeof( cg.killerName ) );
    }

    if( attacker != ENTITYNUM_WORLD )
    {
      switch( mod )
      {
        case MOD_PAINSAW:
          x = rand() % 4;
          switch(x)
          {
            case 0:message = "was sawn by";break;
            case 1:message = "was massacred by";message2 = "'s NGNGNGNGNGNGNG";break;
            case 2:message = "didn't get the ''pain'' in";message2 = "'s painsaw";break;
            case 3:message = "got buzzed by";message2 = "'s painsaw";break;
          }break;
        case MOD_BLASTER:
          x = rand() % 2;
          switch(x)
          {
            case 0:message = "was blasted by";break;
            case 1:message = "is raging because he got owned by";message2 = "'s blaster";break;
          }break;
        case MOD_MACHINEGUN:
          x = rand() % 5;
          switch(x)
          {
            case 0:message = "was machinegunned by";break;
            case 1:message = "ate";message2 = "'s clip";break;
            case 2:message = "met";message2 = "'s hail of bullets";break;
            case 3:message = "was caught in";message2 = "'s rifle's bullet stream";break;
            case 4:message = "was shot one by one by";message2 = "'s rifle";break;
          }break;
        case MOD_CHAINGUN:
          x = rand() % 7;
          switch(x)
          {
            case 0:message = "was chaingunned by";break;
            case 1:message = "was rambo'd by";message2 = "'s chaingun";break;
            case 2:message = "was turned into swiss cheese by";message2 = "'s chaingun";break;
            case 3:message = "was filled with holes by";message2 = "'s chaingun";break;
            case 4:message = "wasn't smart enough to run from";message2 = "'s chaingun";break;
            case 5:message = "was owned by";message2 = "'s random spread of bullets";break;
            case 6:message = "was owned by";message2 = "'s rain of bullets";break;
          }break;
        case MOD_MINIGUN:
          x = rand() % 7;
          switch(x)
          {
            case 0:message = "was minigunned by";break;
            case 1:message = "was rambo'd by";message2 = "'s minigun";break;
            case 2:message = "was turned into swiss cheese by";message2 = "'s minigun";break;
            case 3:message = "was filled with holes by";message2 = "'s minigun";break;
            case 4:message = "wasn't smart enough to run from";message2 = "'s minigun";break;
            case 5:message = "was owned by";message2 = "'s random spread of bullets";break;
            case 6:message = "was owned by";message2 = "'s rain of bullets";break;
          }break;
        case MOD_SHOTGUN:
          x = rand() % 4;
          switch(x)
          {
            case 0:message = "was gunned down by";break;
            case 1:message = "ate";message2 = "'s hot leads";break;
            case 2:message = "was torn apart by the force of";message2 = "'s shotgun";break;
            case 3:message = "ate";message2 = "'s pellets";break;
          }break;
        case MOD_PRIFLE:
          x = rand() % 5;
          switch(x)
          {
            case 0:message = "was pulse rifled by";break;
            case 1:message = "was disintegrated by";message2 = "'s Pulse Rifle";break;
            case 2:message = "lapped up";message2 = "'s orange stream";break;
            case 3:message = "didn't think";message2 = "'s pulse rifle will hurt him that much";break;
            case 4:message = "thought that";message2 = "might be tasty but he didn't get a chance to try him due to the pulse rifle";break;
          }break;
        case MOD_MDRIVER:
          x = rand() % 6;
          switch(x)
          {
            case 0:message = "was mass driven by";break;
            case 1:message = "was given a lesson in nuclear physics by";break;
            case 2:message = "was filled with";message2 = "'s radioactive waste";break;
            case 3:message = "was killed by a dose of";message2 = "'s radiation";break;
            case 4:message = "was sniped by";message2 = "'s ''Dretch Be Gone''";break;
            case 5:message = "was aimbotted by";break;
          }break;
        case MOD_LASGUN:
          x = rand() % 4;
          switch(x)
          {
            case 0:message = "was lasgunned by";break;
            case 1:message = "got irradiated by";message2 = "'s laser";break;
            case 2:message = "wasn't good enough to dodge";message2 = "'s Lasgun";break;
            case 3:message = "forgot to hide from";message2 = "'s Lasgun";break;
          }break;
        case MOD_FLAMER:
          x = rand() % 5;
          switch(x)
          {
            case 0:message = "was grilled by";message2 = "'s Flamer";break;
            case 1:message = "was grilled to perfection by";message2 = "'s Flamer";break;
            case 2:message = "was instantly tanned by";message2 = "'s Flamer";break;
            case 3:message = "became crispy by";message2 = "'s Flamer";break;
            case 4:message = "fulfilled";message2 = "'s dream of crispy bacon";break;
          }break;
        case MOD_FLAMER_SPLASH:
          x = rand() % 5;
          switch(x)
          {
            case 0:message = "was toasted by";message2 = "'s Flamer";break;
            case 1:message = "just sitted in the grill of";message2 = "and became a tasty meal";break;
            case 2:message = "was instantly tanned by";message2 = "'s Flamer";break;
            case 3:message = "became crispy by";message2 = "'s Flamer";break;
            case 4:message = "got a little too close to";message2 = "'s fire";break;
          }break;
        case MOD_LCANNON:
          x = rand() % 6;
          switch(x)
          {
            case 0:message = "felt the full force of";message2 = "'s lucifer cannon";break;
            case 1:message = "felt the power of";message2 = "'s sun";break;
            case 2:message = "saw";message2 = "'s pretty lights";break;
            case 3:message = "was disintegrated by";message2 = "s Lucifer Cannon";break;
            case 4:message = "was annihilated by";message2 = "'s superior weapons";break;
            case 5:message = "sucked the whole ball from the";message2 = "'s lucifer cannon";break;
          }break;
        case MOD_LCANNON_SPLASH:
          x = rand() % 3;
          switch(x)
          {
            case 0:message = "was caught in the fallout of";message2 = "'s lucifer cannon";break;
            case 1:message = "couldn't handle the power of";message2 = "'s luci";break;
            case 2:message = "was irradiated by";message2 = "'s fallout";break;
          }break;
        case MOD_GRENADE:
          x = rand() % 4;
          switch(x)
          {
            case 0:message = "couldn't escape";message2 = "'s grenade";break;
            case 1:message = "was blown to pieces by";message2 = "'s shiny nade";break;
            case 2:message = "was filled with";message2 = "'s nade shrapnel";break;
            case 3:message = "should have heard";message2 = "'s grenade bouncing";break;
          }break;

        case MOD_ABUILDER_CLAW:
          x = rand() % 6;
          switch(x)
          {
            case 0:message = "should leave";message2 = "'s buildings alone";break;
            case 1:message = "ignored the signs and fed";message2 = "'s Granger";break;
            case 2:message = "was brutally butchered by";message2 = "'s Granger";break;
            case 3:message = "failed to hunt";message2 = "'s Granger";break;
            case 4:message = "shouldn't have tried to face";message2 = "'s master creature";break;
            case 5:message = "was messing with";message2 = "'s base!";break;
          }break;
        case MOD_LEVEL0_BITE:
          x = rand() % 3;
          switch(x)
          {
            case 0:message = "was bitten by";break;
            case 1:message = "makes a nice snack for";message2 = "'s Dretch";break;
            case 2:message = "should blame";message2 = "'s dretch for his missing face";break;
          }break;
        case MOD_LEVEL1_CLAW:
          message = "was swiped by";
          Com_sprintf( className, 64, "'s %s",
              BG_FindHumanNameForClassNum( PCL_ALIEN_LEVEL1 ) );
          message2 = className;
          break;
        case MOD_LEVEL2_CLAW:
          message = "was clawed by";
          Com_sprintf( className, 64, "'s %s",
              BG_FindHumanNameForClassNum( PCL_ALIEN_LEVEL2 ) );
          message2 = className;
          break;
        case MOD_LEVEL2_ZAP:
          message = "was zapped by";
          Com_sprintf( className, 64, "'s %s",
              BG_FindHumanNameForClassNum( PCL_ALIEN_LEVEL2 ) );
          message2 = className;
          break;
        case MOD_LEVEL3_CLAW:
          message = "was chomped by";
          Com_sprintf( className, 64, "'s %s",
              BG_FindHumanNameForClassNum( PCL_ALIEN_LEVEL3 ) );
          message2 = className;
          break;
        case MOD_LEVEL3_POUNCE:
          message = "was pounced upon by";
          Com_sprintf( className, 64, "'s %s",
              BG_FindHumanNameForClassNum( PCL_ALIEN_LEVEL3 ) );
          message2 = className;
          break;
        case MOD_LEVEL3_BOUNCEBALL:
          message = "was sniped by";
          Com_sprintf( className, 64, "'s %s",
              BG_FindHumanNameForClassNum( PCL_ALIEN_LEVEL3 ) );
          message2 = className;
          break;
        case MOD_LEVEL4_CLAW:
          message = "was mauled by";
          Com_sprintf( className, 64, "'s %s",
              BG_FindHumanNameForClassNum( PCL_ALIEN_LEVEL4 ) );
          message2 = className;
          break;
        case MOD_LEVEL4_CHARGE:
          message = "should have gotten out of the way of";
          Com_sprintf( className, 64, "'s %s",
              BG_FindHumanNameForClassNum( PCL_ALIEN_LEVEL4 ) );
          message2 = className;
          break;

        case MOD_POISON:
          x = rand() % 2;
          switch(x)
          {
            case 0:message = "should have used a medkit against";message2 = "'s poison";break;
            case 1:message = "got poisoned from";message2 = "'s bite";break;
          }break;
        case MOD_LEVEL1_PCLOUD:
          message = "was gassed by";
          Com_sprintf( className, 64, "'s %s",
              BG_FindHumanNameForClassNum( PCL_ALIEN_LEVEL1 ) );
          message2 = className;
          break;


        case MOD_TELEFRAG:
          message = "tried to invade";
          message2 = "'s personal space";
          break;
        default:
          message = "was killed by";
          break;
      }

      if( message )
      {
        CG_Printf( "%s ^7%s %s%s^7%s\n",
          targetName, message,
          ( teamKill ) ? S_COLOR_RED "TEAMMATE " S_COLOR_WHITE : "",
          attackerName, message2 );
        if( teamKill && attacker == cg.clientNum )
        {
          CG_CenterPrint( va ( "You killed " S_COLOR_RED "TEAMMATE "
            S_COLOR_WHITE "%s", targetName ),
            SCREEN_HEIGHT * 0.30, BIGCHAR_WIDTH );
        }
        return;
      }
    }
    // we don't know what it was
    CG_Printf( "%s died.\n", targetName );
  }
}

//==========================================================================

/*
================
CG_PainEvent

Also called by playerstate transition
================
*/
void CG_PainEvent( centity_t *cent, int health )
{
  char  *snd;

  // don't do more than two pain sounds a second
  if( cg.time - cent->pe.painTime < 500 )
    return;

  if( health < 25 )
    snd = "*pain25_1.wav";
  else if( health < 50 )
    snd = "*pain50_1.wav";
  else if( health < 75 )
    snd = "*pain75_1.wav";
  else
    snd = "*pain100_1.wav";

  trap_S_StartSound( NULL, cent->currentState.number, CHAN_VOICE,
    CG_CustomSound( cent->currentState.number, snd ) );

  // save pain time for programitic twitch animation
  cent->pe.painTime = cg.time;
  cent->pe.painDirection ^= 1;
}

/*
==============
CG_EntityEvent

An entity has an event value
also called by CG_CheckPlayerstateEvents
==============
*/
#define DEBUGNAME(x) if(cg_debugEvents.integer){CG_Printf(x"\n");}
void CG_EntityEvent( centity_t *cent, vec3_t position )
{
  entityState_t *es;
  int           event;
  vec3_t        dir;
  const char    *s;
  int           clientNum;
  clientInfo_t  *ci;
  int           steptime;

  if( cg.snap->ps.persistant[ PERS_TEAM ] == TEAM_SPECTATOR )
    steptime = 200;
  else
    steptime = BG_FindSteptimeForClass( cg.snap->ps.stats[ STAT_PCLASS ] );

  es = &cent->currentState;
  event = es->event & ~EV_EVENT_BITS;

  if( cg_debugEvents.integer )
    CG_Printf( "ent:%3i  event:%3i ", es->number, event );

  if( !event )
  {
    DEBUGNAME("ZEROEVENT");
    return;
  }

  clientNum = es->clientNum;
  if( clientNum < 0 || clientNum >= MAX_CLIENTS )
    clientNum = 0;

  ci = &cgs.clientinfo[ clientNum ];

  switch( event )
  {
    //
    // movement generated events
    //
    case EV_FOOTSTEP:
      DEBUGNAME( "EV_FOOTSTEP" );
      if( cg_footsteps.integer && ci->footsteps != FOOTSTEP_NONE )
      {
        if( ci->footsteps == FOOTSTEP_CUSTOM )
          trap_S_StartSound( NULL, es->number, CHAN_BODY,
            ci->customFootsteps[ rand( ) & 3 ] );
        else
          trap_S_StartSound( NULL, es->number, CHAN_BODY,
            cgs.media.footsteps[ ci->footsteps ][ rand( ) & 3 ] );
      }
      break;

    case EV_FOOTSTEP_METAL:
      DEBUGNAME( "EV_FOOTSTEP_METAL" );
      if( cg_footsteps.integer && ci->footsteps != FOOTSTEP_NONE )
      {
        if( ci->footsteps == FOOTSTEP_CUSTOM )
          trap_S_StartSound( NULL, es->number, CHAN_BODY,
            ci->customMetalFootsteps[ rand( ) & 3 ] );
        else
          trap_S_StartSound( NULL, es->number, CHAN_BODY,
            cgs.media.footsteps[ FOOTSTEP_METAL ][ rand( ) & 3 ] );
      }
      break;

    case EV_FOOTSTEP_SQUELCH:
      DEBUGNAME( "EV_FOOTSTEP_SQUELCH" );
      if( cg_footsteps.integer && ci->footsteps != FOOTSTEP_NONE )
      {
        trap_S_StartSound( NULL, es->number, CHAN_BODY,
          cgs.media.footsteps[ FOOTSTEP_FLESH ][ rand( ) & 3 ] );
      }
      break;

    case EV_FOOTSPLASH:
      DEBUGNAME( "EV_FOOTSPLASH" );
      if( cg_footsteps.integer && ci->footsteps != FOOTSTEP_NONE )
      {
        trap_S_StartSound( NULL, es->number, CHAN_BODY,
          cgs.media.footsteps[ FOOTSTEP_SPLASH ][ rand( ) & 3 ] );
      }
      break;

    case EV_FOOTWADE:
      DEBUGNAME( "EV_FOOTWADE" );
      if( cg_footsteps.integer && ci->footsteps != FOOTSTEP_NONE )
      {
        trap_S_StartSound( NULL, es->number, CHAN_BODY,
          cgs.media.footsteps[ FOOTSTEP_SPLASH ][ rand( ) & 3 ] );
      }
      break;

    case EV_SWIM:
      DEBUGNAME( "EV_SWIM" );
      if( cg_footsteps.integer && ci->footsteps != FOOTSTEP_NONE )
      {
        trap_S_StartSound( NULL, es->number, CHAN_BODY,
          cgs.media.footsteps[ FOOTSTEP_SPLASH ][ rand( ) & 3 ] );
      }
      break;


    case EV_FALL_SHORT:
      DEBUGNAME( "EV_FALL_SHORT" );
      trap_S_StartSound( NULL, es->number, CHAN_AUTO, cgs.media.landSound );

      if( clientNum == cg.predictedPlayerState.clientNum )
      {
        // smooth landing z changes
        cg.landChange = -8;
        cg.landTime = cg.time;
      }
      break;

    case EV_FALL_MEDIUM:
      DEBUGNAME( "EV_FALL_MEDIUM" );
      // use normal pain sound
      trap_S_StartSound( NULL, es->number, CHAN_VOICE, CG_CustomSound( es->number, "*pain100_1.wav" ) );

      if( clientNum == cg.predictedPlayerState.clientNum )
      {
        // smooth landing z changes
        cg.landChange = -16;
        cg.landTime = cg.time;
      }
      break;

    case EV_FALL_FAR:
      DEBUGNAME( "EV_FALL_FAR" );
      trap_S_StartSound (NULL, es->number, CHAN_AUTO, CG_CustomSound( es->number, "*fall1.wav" ) );
      cent->pe.painTime = cg.time;  // don't play a pain sound right after this

      if( clientNum == cg.predictedPlayerState.clientNum )
      {
        // smooth landing z changes
        cg.landChange = -24;
        cg.landTime = cg.time;
      }
      break;

    case EV_FALLING:
      DEBUGNAME( "EV_FALLING" );
      trap_S_StartSound( NULL, es->number, CHAN_AUTO, CG_CustomSound( es->number, "*falling1.wav" ) );
      break;

    case EV_STEP_4:
    case EV_STEP_8:
    case EV_STEP_12:
    case EV_STEP_16:    // smooth out step up transitions
    case EV_STEPDN_4:
    case EV_STEPDN_8:
    case EV_STEPDN_12:
    case EV_STEPDN_16:    // smooth out step down transitions
      DEBUGNAME( "EV_STEP" );
      {
        float  oldStep;
        int    delta;
        int    step;

        if( clientNum != cg.predictedPlayerState.clientNum )
          break;

        // if we are interpolating, we don't need to smooth steps
        if( cg.demoPlayback || ( cg.snap->ps.pm_flags & PMF_FOLLOW ) ||
            cg_nopredict.integer || cg_synchronousClients.integer )
          break;

        // check for stepping up before a previous step is completed
        delta = cg.time - cg.stepTime;

        if( delta < steptime )
          oldStep = cg.stepChange * ( steptime - delta ) / steptime;
        else
          oldStep = 0;

        // add this amount
        if( event >= EV_STEPDN_4 )
        {
          step = 4 * ( event - EV_STEPDN_4 + 1 );
          cg.stepChange = oldStep - step;
        }
        else
        {
          step = 4 * ( event - EV_STEP_4 + 1 );
          cg.stepChange = oldStep + step;
        }

        if( cg.stepChange > MAX_STEP_CHANGE )
          cg.stepChange = MAX_STEP_CHANGE;
        else if( cg.stepChange < -MAX_STEP_CHANGE )
          cg.stepChange = -MAX_STEP_CHANGE;

        cg.stepTime = cg.time;
        break;
      }

    case EV_JUMP:
      DEBUGNAME( "EV_JUMP" );
      trap_S_StartSound( NULL, es->number, CHAN_VOICE, CG_CustomSound( es->number, "*jump1.wav" ) );

      if( BG_ClassHasAbility( cg.predictedPlayerState.stats[ STAT_PCLASS ], SCA_WALLJUMPER ) )
      {
        vec3_t  surfNormal, refNormal = { 0.0f, 0.0f, 1.0f };
        vec3_t  rotAxis;

        if( clientNum != cg.predictedPlayerState.clientNum )
          break;

        //set surfNormal
        VectorCopy( cg.predictedPlayerState.grapplePoint, surfNormal );

        //if we are moving from one surface to another smooth the transition
        if( !VectorCompare( surfNormal, cg.lastNormal ) && surfNormal[ 2 ] != 1.0f )
        {
          CrossProduct( refNormal, surfNormal, rotAxis );
          VectorNormalize( rotAxis );

          //add the op
          CG_addSmoothOp( rotAxis, 15.0f, 1.0f );
        }

        //copy the current normal to the lastNormal
        VectorCopy( surfNormal, cg.lastNormal );
      }

      break;

    case EV_LEV1_GRAB:
      DEBUGNAME( "EV_LEV1_GRAB" );
      trap_S_StartSound( NULL, es->number, CHAN_VOICE, cgs.media.alienL1Grab );
      break;

    case EV_LEV4_CHARGE_PREPARE:
      DEBUGNAME( "EV_LEV4_CHARGE_PREPARE" );
      trap_S_StartSound( NULL, es->number, CHAN_VOICE, cgs.media.alienL4ChargePrepare );
      break;

    case EV_LEV4_CHARGE_START:
      DEBUGNAME( "EV_LEV4_CHARGE_START" );
      //FIXME: stop cgs.media.alienL4ChargePrepare playing here
      trap_S_StartSound( NULL, es->number, CHAN_VOICE, cgs.media.alienL4ChargeStart );
      break;

    case EV_TAUNT:
      DEBUGNAME( "EV_TAUNT" );
      if( !cg_noTaunt.integer )
        trap_S_StartSound( NULL, es->number, CHAN_VOICE, CG_CustomSound( es->number, "*taunt.wav" ) );
      break;

    case EV_WATER_TOUCH:
      DEBUGNAME( "EV_WATER_TOUCH" );
      trap_S_StartSound( NULL, es->number, CHAN_AUTO, cgs.media.watrInSound );
      break;

    case EV_WATER_LEAVE:
      DEBUGNAME( "EV_WATER_LEAVE" );
      trap_S_StartSound( NULL, es->number, CHAN_AUTO, cgs.media.watrOutSound );
      break;

    case EV_WATER_UNDER:
      DEBUGNAME( "EV_WATER_UNDER" );
      trap_S_StartSound( NULL, es->number, CHAN_AUTO, cgs.media.watrUnSound );
      break;

    case EV_WATER_CLEAR:
      DEBUGNAME( "EV_WATER_CLEAR" );
      trap_S_StartSound( NULL, es->number, CHAN_AUTO, CG_CustomSound( es->number, "*gasp.wav" ) );
      break;

    //
    // weapon events
    //
    case EV_NOAMMO:
      DEBUGNAME( "EV_NOAMMO" );
      {
      }
      break;

    case EV_CHANGE_WEAPON:
      DEBUGNAME( "EV_CHANGE_WEAPON" );
      trap_S_StartSound( NULL, es->number, CHAN_AUTO, cgs.media.selectSound );
      break;

    case EV_FIRE_WEAPON:
      DEBUGNAME( "EV_FIRE_WEAPON" );
      CG_FireWeapon( cent, WPM_PRIMARY );
      break;

    case EV_FIRE_WEAPON2:
      DEBUGNAME( "EV_FIRE_WEAPON2" );
      CG_FireWeapon( cent, WPM_SECONDARY );
      break;

    case EV_FIRE_WEAPON3:
      DEBUGNAME( "EV_FIRE_WEAPON3" );
      CG_FireWeapon( cent, WPM_TERTIARY );
      break;

    //=================================================================

    //
    // other events
    //
    case EV_PLAYER_TELEPORT_IN:
      DEBUGNAME( "EV_PLAYER_TELEPORT_IN" );
      //deprecated
      break;

    case EV_PLAYER_TELEPORT_OUT:
      DEBUGNAME( "EV_PLAYER_TELEPORT_OUT" );
      CG_PlayerDisconnect( position );
      break;

    case EV_BUILD_CONSTRUCT:
      DEBUGNAME( "EV_BUILD_CONSTRUCT" );
      //do something useful here
      break;

    case EV_BUILD_DESTROY:
      DEBUGNAME( "EV_BUILD_DESTROY" );
      //do something useful here
      break;

    case EV_RPTUSE_SOUND:
      DEBUGNAME( "EV_RPTUSE_SOUND" );
      trap_S_StartSound( NULL, es->number, CHAN_AUTO, cgs.media.repeaterUseSound );
      break;

    case EV_GRENADE_BOUNCE:
      DEBUGNAME( "EV_GRENADE_BOUNCE" );
      if( rand( ) & 1 )
        trap_S_StartSound( NULL, es->number, CHAN_AUTO, cgs.media.hardBounceSound1 );
      else
        trap_S_StartSound( NULL, es->number, CHAN_AUTO, cgs.media.hardBounceSound2 );
      break;

    //
    // missile impacts
    //
    case EV_MISSILE_HIT:
      DEBUGNAME( "EV_MISSILE_HIT" );
      ByteToDir( es->eventParm, dir );
      CG_MissileHitPlayer( es->weapon, es->generic1, position, dir, es->otherEntityNum );
      break;

    case EV_MISSILE_MISS:
      DEBUGNAME( "EV_MISSILE_MISS" );
      ByteToDir( es->eventParm, dir );
      CG_MissileHitWall( es->weapon, es->generic1, 0, position, dir, IMPACTSOUND_DEFAULT );
      break;

    case EV_MISSILE_MISS_METAL:
      DEBUGNAME( "EV_MISSILE_MISS_METAL" );
      ByteToDir( es->eventParm, dir );
      CG_MissileHitWall( es->weapon, es->generic1, 0, position, dir, IMPACTSOUND_METAL );
      break;

    case EV_HUMAN_BUILDABLE_EXPLOSION:
      DEBUGNAME( "EV_HUMAN_BUILDABLE_EXPLOSION" );
      ByteToDir( es->eventParm, dir );
      CG_HumanBuildableExplosion( position, dir );
      break;

    case EV_ALIEN_BUILDABLE_EXPLOSION:
      DEBUGNAME( "EV_ALIEN_BUILDABLE_EXPLOSION" );
      ByteToDir( es->eventParm, dir );
      CG_AlienBuildableExplosion( position, dir );
      break;

    case EV_TESLATRAIL:
      DEBUGNAME( "EV_TESLATRAIL" );
      cent->currentState.weapon = WP_TESLAGEN;
      {
        centity_t *source = &cg_entities[ es->generic1 ];
        centity_t *target = &cg_entities[ es->clientNum ];
        vec3_t    sourceOffset = { 0.0f, 0.0f, 28.0f };

        if( !CG_IsTrailSystemValid( &source->muzzleTS ) )
        {
          source->muzzleTS = CG_SpawnNewTrailSystem( cgs.media.teslaZapTS );

          if( CG_IsTrailSystemValid( &source->muzzleTS ) )
          {
            CG_SetAttachmentCent( &source->muzzleTS->frontAttachment, source );
            CG_SetAttachmentCent( &source->muzzleTS->backAttachment, target );
            CG_AttachToCent( &source->muzzleTS->frontAttachment );
            CG_AttachToCent( &source->muzzleTS->backAttachment );
            CG_SetAttachmentOffset( &source->muzzleTS->frontAttachment, sourceOffset );

            source->muzzleTSDeathTime = cg.time + cg_teslaTrailTime.integer;
          }
        }
      }
      break;

    case EV_BULLET_HIT_WALL:
      DEBUGNAME( "EV_BULLET_HIT_WALL" );
      ByteToDir( es->eventParm, dir );
      CG_Bullet( es->pos.trBase, es->otherEntityNum, dir, qfalse, ENTITYNUM_WORLD );
      break;

    case EV_BULLET_HIT_FLESH:
      DEBUGNAME( "EV_BULLET_HIT_FLESH" );
      CG_Bullet( es->pos.trBase, es->otherEntityNum, dir, qtrue, es->eventParm );
      break;

    case EV_SHOTGUN:
      DEBUGNAME( "EV_SHOTGUN" );
      CG_ShotgunFire( es );
      break;

    case EV_GENERAL_SOUND:
      DEBUGNAME( "EV_GENERAL_SOUND" );
      if( cgs.gameSounds[ es->eventParm ] )
        trap_S_StartSound( NULL, es->number, CHAN_VOICE, cgs.gameSounds[ es->eventParm ] );
      else
      {
        s = CG_ConfigString( CS_SOUNDS + es->eventParm );
        trap_S_StartSound( NULL, es->number, CHAN_VOICE, CG_CustomSound( es->number, s ) );
      }
      break;

    case EV_GLOBAL_SOUND: // play from the player's head so it never diminishes
      DEBUGNAME( "EV_GLOBAL_SOUND" );
      if( cgs.gameSounds[ es->eventParm ] )
        trap_S_StartSound( NULL, cg.snap->ps.clientNum, CHAN_AUTO, cgs.gameSounds[ es->eventParm ] );
      else
      {
        s = CG_ConfigString( CS_SOUNDS + es->eventParm );
        trap_S_StartSound( NULL, cg.snap->ps.clientNum, CHAN_AUTO, CG_CustomSound( es->number, s ) );
      }
      break;

    case EV_PAIN:
      // local player sounds are triggered in CG_CheckLocalSounds,
      // so ignore events on the player
      DEBUGNAME( "EV_PAIN" );
      if( cent->currentState.number != cg.snap->ps.clientNum )
        CG_PainEvent( cent, es->eventParm );
      break;

    case EV_DEATH1:
    case EV_DEATH2:
    case EV_DEATH3:
      DEBUGNAME( "EV_DEATHx" );
      trap_S_StartSound( NULL, es->number, CHAN_VOICE,
          CG_CustomSound( es->number, va( "*death%i.wav", event - EV_DEATH1 + 1 ) ) );
      break;

    case EV_OBITUARY:
      DEBUGNAME( "EV_OBITUARY" );
      CG_Obituary( es );
      break;

    case EV_GIB_PLAYER:
      DEBUGNAME( "EV_GIB_PLAYER" );
      // no gibbing
      break;

    case EV_STOPLOOPINGSOUND:
      DEBUGNAME( "EV_STOPLOOPINGSOUND" );
      trap_S_StopLoopingSound( es->number );
      es->loopSound = 0;
      break;

    case EV_DEBUG_LINE:
      DEBUGNAME( "EV_DEBUG_LINE" );
      CG_Beam( cent );
      break;

    case EV_BUILD_DELAY:
      DEBUGNAME( "EV_BUILD_DELAY" );
      if( clientNum == cg.predictedPlayerState.clientNum )
      {
        trap_S_StartLocalSound( cgs.media.buildableRepairedSound, CHAN_LOCAL_SOUND );
        cg.lastBuildAttempt = cg.time;
      }
      break;

    case EV_BUILD_REPAIR:
      DEBUGNAME( "EV_BUILD_REPAIR" );
      trap_S_StartSound( NULL, es->number, CHAN_AUTO, cgs.media.buildableRepairSound );
      break;

    case EV_BUILD_REPAIRED:
      DEBUGNAME( "EV_BUILD_REPAIRED" );
      trap_S_StartSound( NULL, es->number, CHAN_AUTO, cgs.media.buildableRepairedSound );
      break;

    case EV_OVERMIND_ATTACK:
      DEBUGNAME( "EV_OVERMIND_ATTACK" );
      if( cg.predictedPlayerState.stats[ STAT_PTEAM ] == PTE_ALIENS )
      {
        trap_S_StartLocalSound( cgs.media.alienOvermindAttack, CHAN_ANNOUNCER );
        CG_CenterPrint( "The Overmind is under attack!", 200, GIANTCHAR_WIDTH * 4 );
      }
      break;

    case EV_OVERMIND_DYING:
      DEBUGNAME( "EV_OVERMIND_DYING" );
      if( cg.predictedPlayerState.stats[ STAT_PTEAM ] == PTE_ALIENS )
      {
        trap_S_StartLocalSound( cgs.media.alienOvermindDying, CHAN_ANNOUNCER );
        CG_CenterPrint( "The Overmind is dying!", 200, GIANTCHAR_WIDTH * 4 );
      }
      break;

    case EV_DCC_ATTACK:
      DEBUGNAME( "EV_DCC_ATTACK" );
      if( cg.predictedPlayerState.stats[ STAT_PTEAM ] == PTE_HUMANS )
      {
        //trap_S_StartLocalSound( cgs.media.humanDCCAttack, CHAN_ANNOUNCER );
        CG_CenterPrint( "Our base is under attack!", 200, GIANTCHAR_WIDTH * 4 );
      }
      break;

    case EV_OVERMIND_SPAWNS:
      DEBUGNAME( "EV_OVERMIND_SPAWNS" );
      if( cg.predictedPlayerState.stats[ STAT_PTEAM ] == PTE_ALIENS )
      {
        trap_S_StartLocalSound( cgs.media.alienOvermindSpawns, CHAN_ANNOUNCER );
        CG_CenterPrint( "The Overmind needs spawns!", 200, GIANTCHAR_WIDTH * 4 );
      }
      break;

    case EV_ALIEN_EVOLVE:
      DEBUGNAME( "EV_ALIEN_EVOLVE" );
      trap_S_StartSound( NULL, es->number, CHAN_BODY, cgs.media.alienEvolveSound );
      {
        particleSystem_t *ps = CG_SpawnNewParticleSystem( cgs.media.alienEvolvePS );

        if( CG_IsParticleSystemValid( &ps ) )
        {
          CG_SetAttachmentCent( &ps->attachment, cent );
          CG_AttachToCent( &ps->attachment );
        }
      }

      if( es->number == cg.clientNum )
      {
        CG_ResetPainBlend( );
        cg.spawnTime = cg.time;
      }
      break;

    case EV_ALIEN_EVOLVE_FAILED:
      DEBUGNAME( "EV_ALIEN_EVOLVE_FAILED" );
      if( clientNum == cg.predictedPlayerState.clientNum )
      {
        //FIXME: change to "negative" sound
        trap_S_StartLocalSound( cgs.media.buildableRepairedSound, CHAN_LOCAL_SOUND );
        cg.lastEvolveAttempt = cg.time;
      }
      break;

    case EV_ALIEN_ACIDTUBE:
      DEBUGNAME( "EV_ALIEN_ACIDTUBE" );
      {
        particleSystem_t *ps = CG_SpawnNewParticleSystem( cgs.media.alienAcidTubePS );

        if( CG_IsParticleSystemValid( &ps ) )
        {
          CG_SetAttachmentCent( &ps->attachment, cent );
          ByteToDir( es->eventParm, dir );
          CG_SetParticleSystemNormal( ps, dir );
          CG_AttachToCent( &ps->attachment );
        }
      }
      break;

    case EV_MEDKIT_USED:
      DEBUGNAME( "EV_MEDKIT_USED" );
      trap_S_StartSound( NULL, es->number, CHAN_AUTO, cgs.media.medkitUseSound );
      break;

    case EV_PLAYER_RESPAWN:
      DEBUGNAME( "EV_PLAYER_RESPAWN" );
      if( es->number == cg.clientNum )
        cg.spawnTime = cg.time;
      break;

    default:
      DEBUGNAME( "UNKNOWN" );
      CG_Error( "Unknown event: %i", event );
      break;
  }
}


/*
==============
CG_CheckEvents

==============
*/
void CG_CheckEvents( centity_t *cent )
{
  entity_event_t event;
  entity_event_t oldEvent = EV_NONE;

  // check for event-only entities
  if( cent->currentState.eType > ET_EVENTS )
  {
    event = cent->currentState.eType - ET_EVENTS;

    if( cent->previousEvent )
      return; // already fired

    cent->previousEvent = 1;

    cent->currentState.event = cent->currentState.eType - ET_EVENTS;
    
    // Move the pointer to the entity that the
    // event was originally attached to
    if( cent->currentState.eFlags & EF_PLAYER_EVENT )
    {
      cent = &cg_entities[ cent->currentState.otherEntityNum ];
      oldEvent = cent->currentState.event;
      cent->currentState.event = event;
    }
  }
  else
  {
    // check for events riding with another entity
    if( cent->currentState.event == cent->previousEvent )
      return;

    cent->previousEvent = cent->currentState.event;
    if( ( cent->currentState.event & ~EV_EVENT_BITS ) == 0 )
      return;
  }

  // calculate the position at exactly the frame time
  BG_EvaluateTrajectory( &cent->currentState.pos, cg.snap->serverTime, cent->lerpOrigin );
  CG_SetEntitySoundPosition( cent );

  CG_EntityEvent( cent, cent->lerpOrigin );
  
  // If this was a reattached spilled event, restore the original event
  if( oldEvent != EV_NONE )
    cent->currentState.event = oldEvent;
}

