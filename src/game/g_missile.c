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

#include "g_local.h"

#define MISSILE_PRESTEP_TIME  50

/*
================
G_BounceMissile

================
*/
void G_BounceMissile( gentity_t *ent, trace_t *trace )
{
  vec3_t  velocity;
  float dot;
  int   hitTime;

  // reflect the velocity on the trace plane
  hitTime = level.previousTime + ( level.time - level.previousTime ) * trace->fraction;
  BG_EvaluateTrajectoryDelta( &ent->s.pos, hitTime, velocity );
  dot = DotProduct( velocity, trace->plane.normal );
  VectorMA( velocity, -2 * dot, trace->plane.normal, ent->s.pos.trDelta );

  if( ent->s.eFlags & EF_BOUNCE_HALF )
  {
    VectorScale( ent->s.pos.trDelta, 0.65, ent->s.pos.trDelta );
    // check for stop
    if( trace->plane.normal[ 2 ] > 0.2 && VectorLength( ent->s.pos.trDelta ) < 40 )
    {
      G_SetOrigin( ent, trace->endpos );
      return;
    }
  }

  VectorAdd( ent->r.currentOrigin, trace->plane.normal, ent->r.currentOrigin );
  VectorCopy( ent->r.currentOrigin, ent->s.pos.trBase );
  ent->s.pos.trTime = level.time;
}


void G_ExplodeMissile( gentity_t *ent );

gentity_t *fire_shrapnel( gentity_t *self, vec3_t start, vec3_t dir )
{
  gentity_t *bolt;

  VectorNormalize (dir);

  bolt = G_Spawn();
  bolt->classname = "blaster";
  bolt->nextthink = level.time + GRENADE_SHRAPNEL_LIFETIME;
  bolt->think = G_ExplodeMissile;
  bolt->s.eType = ET_MISSILE;
  bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
  bolt->s.eFlags |= EF_BOUNCE|EF_NO_BOUNCE_SOUND;
  bolt->s.weapon = WP_BLASTER;
  bolt->s.generic1 = self->s.generic1; //weaponMode
  bolt->r.ownerNum = self->r.ownerNum;
  bolt->parent = self;
  bolt->damage = GRENADE_SHRAPNEL_DAMAGE;
  bolt->splashDamage = 0;
  bolt->splashRadius = 0;
  bolt->methodOfDeath = MOD_GRENADE;
  bolt->splashMethodOfDeath = MOD_GRENADE;
  bolt->clipmask = MASK_SHOT;
  bolt->target_ent = NULL;

  bolt->s.pos.trType = TR_LINEAR;
  bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;   // move a bit on the very first frame
  VectorCopy( start, bolt->s.pos.trBase );
  VectorScale( dir, BLASTER_SPEED, bolt->s.pos.trDelta );
  SnapVector( bolt->s.pos.trDelta );      // save net bandwidth

  VectorCopy( start, bolt->r.currentOrigin );

  return bolt;
}

void G_Explodefragnade( gentity_t *ent, vec3_t origin )
{
    vec3_t dir;
    long i;
    //int damage;
    float mag;
    //trace_t tr;
    //gentity_t *target;
    srand( trap_Milliseconds( ) );
    for(i = 0;i < GRENADE_SHRAPNEL_COUNT;i++)
    {
        gentity_t *shrapnel;
        dir[0] = (int)(( (double)rand() / ((double)(RAND_MAX)+(double)(1)) ) * GRENADE_SHRAPNEL_MAG) - (GRENADE_SHRAPNEL_MAG / 2);
        dir[1] = (int)(( (double)rand() / ((double)(RAND_MAX)+(double)(1)) ) * GRENADE_SHRAPNEL_MAG) - (GRENADE_SHRAPNEL_MAG / 2);
        dir[2] = (int)(( (double)rand() / ((double)(RAND_MAX)+(double)(1)) ) * GRENADE_SHRAPNEL_MAG) - (GRENADE_SHRAPNEL_MAG / 2);
        //VectorNormalize( dir );
        mag = abs(dir[0]) + abs(dir[1]) + abs(dir[2]);
        dir[0] = dir[0] / mag;
        dir[1] = dir[1] / mag;
        dir[2] = dir[2] / mag;
        //VectorMA( origin, GRENADE_SHRAPNEL_RANGE, dir, pos );
        //trap_Trace( &tr, origin, NULL, NULL, pos, ent->r.ownerNum, MASK_SHOT );
        //target = &g_entities[ tr.entityNum ];
        //damage = Distance(tr.endpos,origin) / GRENADE_SHRAPNEL_RANGE * GRENADE_SHRAPNEL_DAMAGE;
        shrapnel = fire_shrapnel(ent,origin,dir);
        //if(target->health > 0 && target->takedamage)
        //{
        //    G_Damage( target, ent, ent, dir, tr.endpos, damage, 0, MOD_GRENADE );
        //}
    }
}

/*
================
G_ExplodeMissile

Explode a missile without an impact
================
*/
void G_ExplodeMissile( gentity_t *ent )
{
  vec3_t    dir;
  vec3_t    origin;

  BG_EvaluateTrajectory( &ent->s.pos, level.time, origin );
  SnapVector( origin );
  G_SetOrigin( ent, origin );

  // we don't have a valid direction, so just point straight up
  dir[ 0 ] = dir[ 1 ] = 0;
  dir[ 2 ] = 1;

  ent->s.eType = ET_GENERAL;

  // tired... can't be fucked... hack
  if( ent->s.weapon != WP_LOCKBLOB_LAUNCHER &&
      ent->s.weapon != WP_FLAMER )
    G_AddEvent( ent, EV_MISSILE_MISS, DirToByte( dir ) );

  ent->freeAfterEvent = qtrue;
  if( ent->s.weapon == WP_GRENADE )
	G_Explodefragnade(ent,origin);
  // splash damage
  if( ent->splashDamage )
    G_RadiusDamage( ent->r.currentOrigin, ent->parent, ent->splashDamage,
                    ent->splashRadius, ent, ent->splashMethodOfDeath );

  trap_LinkEntity( ent );
}

void AHive_ReturnToHive( gentity_t *self );

/*
================
G_MissileImpact

================
*/
void G_MissileImpact( gentity_t *ent, trace_t *trace )
{
  gentity_t   *other, *attacker;
  qboolean    returnAfterDamage = qfalse;
  vec3_t      dir;

  other = &g_entities[ trace->entityNum ];
  attacker = &g_entities[ ent->r.ownerNum ];

  // check for bounce
  if( !other->takedamage &&
      ( ent->s.eFlags & ( EF_BOUNCE | EF_BOUNCE_HALF ) ) )
  {
    G_BounceMissile( ent, trace );

    //only play a sound if requested
    if( !( ent->s.eFlags & EF_NO_BOUNCE_SOUND ) )
      G_AddEvent( ent, EV_GRENADE_BOUNCE, 0 );

    return;
  }

  if( !strcmp( ent->classname, "grenade" ) )
  {
    //grenade doesn't explode on impact
    G_BounceMissile( ent, trace );

    //only play a sound if requested
    if( !( ent->s.eFlags & EF_NO_BOUNCE_SOUND ) )
      G_AddEvent( ent, EV_GRENADE_BOUNCE, 0 );

    return;
  }
  else if( !strcmp( ent->classname, "lockblob" ) )
  {
    if( other->client && other->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS )
    {
      other->client->ps.stats[ STAT_STATE ] |= SS_BLOBLOCKED;
      other->client->lastLockTime = level.time;
      AngleVectors( other->client->ps.viewangles, dir, NULL, NULL );
      other->client->ps.stats[ STAT_VIEWLOCK ] = DirToByte( dir );
    }
  }
  else if( !strcmp( ent->classname, "slowblob" ) )
  {
    if( other->client && other->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS )
    {
      other->client->ps.stats[ STAT_STATE ] |= SS_SLOWLOCKED;
      other->client->lastSlowTime = level.time;
      AngleVectors( other->client->ps.viewangles, dir, NULL, NULL );
      other->client->ps.stats[ STAT_VIEWLOCK ] = DirToByte( dir );
    }
  }
  else if( !strcmp( ent->classname, "hive" ) )
  {
    if( other->s.eType == ET_BUILDABLE && other->s.modelindex == BA_A_HIVE )
    {
      if( !ent->parent )
        G_Printf( S_COLOR_YELLOW "WARNING: hive entity has no parent in G_MissileImpact\n" );
      else
        ent->parent->active = qfalse;

      G_FreeEntity( ent );
      return;
    }
    else
    {
      //prevent collision with the client when returning
      ent->r.ownerNum = other->s.number;

      ent->think = AHive_ReturnToHive;
      ent->nextthink = level.time + FRAMETIME;

      //only damage humans
      if( other->client && other->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS )
        returnAfterDamage = qtrue;
      else
        return;
    }
  }

  // impact damage
  if( other->takedamage )
  {
    // FIXME: wrong damage direction?
    if( ent->damage )
    {
      vec3_t  velocity;

      BG_EvaluateTrajectoryDelta( &ent->s.pos, level.time, velocity );
      if( VectorLength( velocity ) == 0 )
        velocity[ 2 ] = 1;  // stepped on a grenade

      G_Damage( other, ent, attacker, velocity, ent->s.origin, ent->damage,
        0, ent->methodOfDeath );
    }
  }

  if( returnAfterDamage )
    return;

  // is it cheaper in bandwidth to just remove this ent and create a new
  // one, rather than changing the missile into the explosion?

  if( other->takedamage && other->client )
  {
    G_AddEvent( ent, EV_MISSILE_HIT, DirToByte( trace->plane.normal ) );
    ent->s.otherEntityNum = other->s.number;
  }
  else if( trace->surfaceFlags & SURF_METALSTEPS )
    G_AddEvent( ent, EV_MISSILE_MISS_METAL, DirToByte( trace->plane.normal ) );
  else
    G_AddEvent( ent, EV_MISSILE_MISS, DirToByte( trace->plane.normal ) );

  ent->freeAfterEvent = qtrue;

  // change over to a normal entity right at the point of impact
  ent->s.eType = ET_GENERAL;

  SnapVectorTowards( trace->endpos, ent->s.pos.trBase );  // save net bandwidth

  G_SetOrigin( ent, trace->endpos );

  // splash damage (doesn't apply to person directly hit)
  if( ent->splashDamage )
    G_RadiusDamage( trace->endpos, ent->parent, ent->splashDamage, ent->splashRadius,
                    other, ent->splashMethodOfDeath );

  trap_LinkEntity( ent );
}


/*
================
G_RunMissile

================
*/
void G_RunMissile( gentity_t *ent )
{
  vec3_t    origin;
  trace_t   tr;
  int     passent;

  // get current position
  BG_EvaluateTrajectory( &ent->s.pos, level.time, origin );

  // ignore interactions with the missile owner
  passent = ent->r.ownerNum;

  // trace a line from the previous position to the current position
  trap_Trace( &tr, ent->r.currentOrigin, ent->r.mins, ent->r.maxs, origin, passent, ent->clipmask );

  if( tr.startsolid || tr.allsolid )
  {
    // make sure the tr.entityNum is set to the entity we're stuck in
    trap_Trace( &tr, ent->r.currentOrigin, ent->r.mins, ent->r.maxs, ent->r.currentOrigin, passent, ent->clipmask );
    tr.fraction = 0;
  }
  else
    VectorCopy( tr.endpos, ent->r.currentOrigin );

  ent->r.contents = CONTENTS_SOLID; //trick trap_LinkEntity into...
  trap_LinkEntity( ent );
  ent->r.contents = 0; //...encoding bbox information

  if( tr.fraction != 1 )
  {
    // never explode or bounce on sky
    if( tr.surfaceFlags & SURF_NOIMPACT )
    {
      // If grapple, reset owner
      if( ent->parent && ent->parent->client && ent->parent->client->hook == ent )
        ent->parent->client->hook = NULL;

      G_FreeEntity( ent );
      return;
    }

    G_MissileImpact( ent, &tr );
    if( ent->s.eType != ET_MISSILE )
      return;   // exploded
  }

  // check think function after bouncing
  G_RunThink( ent );
}


//=============================================================================

/*
=================
fire_flamer

=================
*/
gentity_t *fire_flamer( gentity_t *self, vec3_t start, vec3_t dir )
{
  gentity_t *bolt;
  vec3_t    pvel;

  VectorNormalize (dir);

  bolt = G_Spawn();
  bolt->classname = "flame";
  bolt->nextthink = level.time + FLAMER_LIFETIME;
  bolt->think = G_ExplodeMissile;
  bolt->s.eType = ET_MISSILE;
  bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
  bolt->s.weapon = WP_FLAMER;
  bolt->s.generic1 = self->s.generic1; //weaponMode
  bolt->r.ownerNum = self->s.number;
  bolt->parent = self;
  bolt->damage = FLAMER_DMG;
  bolt->splashDamage = FLAMER_DMG;
  bolt->splashRadius = FLAMER_RADIUS;
  bolt->methodOfDeath = MOD_FLAMER;
  bolt->splashMethodOfDeath = MOD_FLAMER_SPLASH;
  bolt->clipmask = MASK_SHOT;
  bolt->target_ent = NULL;
  bolt->r.mins[ 0 ] = bolt->r.mins[ 1 ] = bolt->r.mins[ 2 ] = -15.0f;
  bolt->r.maxs[ 0 ] = bolt->r.maxs[ 1 ] = bolt->r.maxs[ 2 ] = 15.0f;

  bolt->s.pos.trType = TR_LINEAR;
  bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;   // move a bit on the very first frame
  VectorCopy( start, bolt->s.pos.trBase );
  VectorScale( self->client->ps.velocity, FLAMER_LAG, pvel );
  VectorMA( pvel, FLAMER_SPEED, dir, bolt->s.pos.trDelta );
  SnapVector( bolt->s.pos.trDelta );      // save net bandwidth

  VectorCopy( start, bolt->r.currentOrigin );

  return bolt;
}

//=============================================================================

/*
=================
fire_blaster

=================
*/
gentity_t *fire_blaster( gentity_t *self, vec3_t start, vec3_t dir )
{
  gentity_t *bolt;

  VectorNormalize (dir);

  bolt = G_Spawn();
  bolt->classname = "blaster";
  bolt->nextthink = level.time + 10000;
  bolt->think = G_ExplodeMissile;
  bolt->s.eType = ET_MISSILE;
  bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
  bolt->s.weapon = WP_BLASTER;
  bolt->s.generic1 = self->s.generic1; //weaponMode
  bolt->r.ownerNum = self->s.number;
  bolt->parent = self;
  bolt->damage = BLASTER_DMG;
  bolt->splashDamage = 0;
  bolt->splashRadius = 0;
  bolt->methodOfDeath = MOD_BLASTER;
  bolt->splashMethodOfDeath = MOD_BLASTER;
  bolt->clipmask = MASK_SHOT;
  bolt->target_ent = NULL;

  bolt->s.pos.trType = TR_LINEAR;
  bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;   // move a bit on the very first frame
  VectorCopy( start, bolt->s.pos.trBase );
  VectorScale( dir, BLASTER_SPEED, bolt->s.pos.trDelta );
  SnapVector( bolt->s.pos.trDelta );      // save net bandwidth

  VectorCopy( start, bolt->r.currentOrigin );

  return bolt;
}

//=============================================================================

/*
=================
fire_pulseRifle

=================
*/
gentity_t *fire_pulseRifle( gentity_t *self, vec3_t start, vec3_t dir )
{
  gentity_t *bolt;

  VectorNormalize (dir);

  bolt = G_Spawn();
  bolt->classname = "pulse";
  bolt->nextthink = level.time + 10000;
  bolt->think = G_ExplodeMissile;
  bolt->s.eType = ET_MISSILE;
  bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
  bolt->s.weapon = WP_PULSE_RIFLE;
  bolt->s.generic1 = self->s.generic1; //weaponMode
  bolt->r.ownerNum = self->s.number;
  bolt->parent = self;
  bolt->damage = PRIFLE_DMG;
  bolt->splashDamage = 0;
  bolt->splashRadius = 0;
  bolt->methodOfDeath = MOD_PRIFLE;
  bolt->splashMethodOfDeath = MOD_PRIFLE;
  bolt->clipmask = MASK_SHOT;
  bolt->target_ent = NULL;

  bolt->s.pos.trType = TR_LINEAR;
  bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;   // move a bit on the very first frame
  VectorCopy( start, bolt->s.pos.trBase );
  VectorScale( dir, PRIFLE_SPEED, bolt->s.pos.trDelta );
  SnapVector( bolt->s.pos.trDelta );      // save net bandwidth

  VectorCopy( start, bolt->r.currentOrigin );

  return bolt;
}

//=============================================================================

/*
=================
fire_luciferCannon

=================
*/
gentity_t *fire_luciferCannon( gentity_t *self, vec3_t start, vec3_t dir, int damage, int radius )
{
  gentity_t *bolt;
  int localDamage = (int)( ceil( ( (float)damage /
                                   (float)LCANNON_TOTAL_CHARGE ) * (float)LCANNON_DAMAGE ) );

  VectorNormalize( dir );

  bolt = G_Spawn( );
  bolt->classname = "lcannon";

  if( damage == LCANNON_TOTAL_CHARGE )
    bolt->nextthink = level.time;
  else
    bolt->nextthink = level.time + 10000;

  bolt->think = G_ExplodeMissile;
  bolt->s.eType = ET_MISSILE;
  bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
  bolt->s.weapon = WP_LUCIFER_CANNON;
  bolt->s.generic1 = self->s.generic1; //weaponMode
  bolt->r.ownerNum = self->s.number;
  bolt->parent = self;
  bolt->damage = localDamage;
  bolt->splashDamage = localDamage / 2;
  bolt->splashRadius = radius;
  bolt->methodOfDeath = MOD_LCANNON;
  bolt->splashMethodOfDeath = MOD_LCANNON_SPLASH;
  bolt->clipmask = MASK_SHOT;
  bolt->target_ent = NULL;

  bolt->s.pos.trType = TR_LINEAR;
  bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;   // move a bit on the very first frame
  VectorCopy( start, bolt->s.pos.trBase );
  VectorScale( dir, LCANNON_SPEED, bolt->s.pos.trDelta );
  SnapVector( bolt->s.pos.trDelta );      // save net bandwidth

  VectorCopy( start, bolt->r.currentOrigin );

  return bolt;
}

/*
=================
launch_grenade

=================
*/
gentity_t *launch_grenade( gentity_t *self, vec3_t start, vec3_t dir )
{
  gentity_t *bolt;

  VectorNormalize( dir );

  bolt = G_Spawn( );
  bolt->classname = "grenade";
  bolt->nextthink = level.time + 5000;
  bolt->think = G_ExplodeMissile;
  bolt->s.eType = ET_MISSILE;
  bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
  bolt->s.weapon = WP_GRENADE;
  bolt->s.eFlags = EF_BOUNCE_HALF;
  bolt->s.generic1 = WPM_PRIMARY; //weaponMode
  bolt->r.ownerNum = self->s.number;
  bolt->parent = self;
  bolt->damage = GRENADE_DAMAGE;
  bolt->splashDamage = GRENADE_DAMAGE;
  bolt->splashRadius = GRENADE_RANGE;
  bolt->methodOfDeath = MOD_GRENADE;
  bolt->splashMethodOfDeath = MOD_GRENADE;
  bolt->clipmask = MASK_SHOT;
  bolt->target_ent = NULL;
  bolt->r.mins[ 0 ] = bolt->r.mins[ 1 ] = bolt->r.mins[ 2 ] = -3.0f;
  bolt->r.maxs[ 0 ] = bolt->r.maxs[ 1 ] = bolt->r.maxs[ 2 ] = 3.0f;
  bolt->s.time = level.time;

  bolt->s.pos.trType = TR_GRAVITY;
  bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;   // move a bit on the very first frame
  VectorCopy( start, bolt->s.pos.trBase );
  VectorScale( dir, GRENADE_SPEED, bolt->s.pos.trDelta );
  SnapVector( bolt->s.pos.trDelta );      // save net bandwidth

  VectorCopy( start, bolt->r.currentOrigin );

  return bolt;
}
//=============================================================================

/*
================
AHive_ReturnToHive

Adjust the trajectory to point towards the hive
================
*/
void AHive_ReturnToHive( gentity_t *self )
{
  vec3_t  dir;
  trace_t tr;

  if( !self->parent )
  {
    G_Printf( S_COLOR_YELLOW "WARNING: AHive_ReturnToHive called with no self->parent\n" );
    return;
  }

  trap_UnlinkEntity( self->parent );
  trap_Trace( &tr, self->r.currentOrigin, self->r.mins, self->r.maxs,
              self->parent->r.currentOrigin, self->r.ownerNum, self->clipmask );
  trap_LinkEntity( self->parent );

  if( tr.fraction < 1.0f )
  {
    //if can't see hive then disperse
    VectorCopy( self->r.currentOrigin, self->s.pos.trBase );
    self->s.pos.trType = TR_STATIONARY;
    self->s.pos.trTime = level.time;

    self->think = G_ExplodeMissile;
    self->nextthink = level.time + 2000;
    self->parent->active = qfalse; //allow the parent to start again
  }
  else
  {
    VectorSubtract( self->parent->r.currentOrigin, self->r.currentOrigin, dir );
    VectorNormalize( dir );

    //change direction towards the hive
    VectorScale( dir, HIVE_SPEED, self->s.pos.trDelta );
    SnapVector( self->s.pos.trDelta );      // save net bandwidth
    VectorCopy( self->r.currentOrigin, self->s.pos.trBase );
    self->s.pos.trTime = level.time;

    self->think = G_ExplodeMissile;
    self->nextthink = level.time + 15000;
  }
}

/*
================
AHive_SearchAndDestroy

Adjust the trajectory to point towards the target
================
*/
void AHive_SearchAndDestroy( gentity_t *self )
{
  vec3_t dir;
  trace_t tr, tr2, tr3;
  gentity_t *traceEnt;

  if (self->TotalWP == 0 )
	{
		self->TotalWP++;
		VectorCopy(self->target_ent->r.currentOrigin, self->WPA[ 0 ]);
		VectorCopy(self->target_ent->r.currentOrigin, self->WPA[ self->TotalWP ]);
	}
  
  trap_Trace( &tr, self->r.currentOrigin, self->r.mins, self->r.maxs,
              self->target_ent->r.currentOrigin, self->r.ownerNum, self->clipmask );
  traceEnt = &g_entities[ tr.entityNum ];

  //if there is no LOS or the parent hive is too far away or the target is dead or notargeting, return
  if( Distance( self->r.currentOrigin, self->parent->r.currentOrigin ) > ( HIVE_RANGE * 30 ) ||
      self->target_ent->health <= 0 || self->target_ent->flags & FL_NOTARGET )
  {
    self->r.ownerNum = ENTITYNUM_WORLD;

    self->think = AHive_ReturnToHive;
    self->nextthink = level.time + FRAMETIME;
	    return;
  }
  else
  {
    if ( tr.entityNum == ENTITYNUM_WORLD || traceEnt->s.eType == ET_BUILDABLE)
	{
		trap_Trace( &tr2, self->WPA[ self->TotalWP - 1 ], self->r.mins, self->r.maxs,
			self->target_ent->r.currentOrigin, self->r.ownerNum, self->clipmask );
	    traceEnt = &g_entities[ tr2.entityNum ];
		if ( tr2.entityNum == ENTITYNUM_WORLD || traceEnt->s.eType == ET_BUILDABLE )
		{
			trap_Trace( &tr3, self->WPA[ self->TotalWP ], self->r.mins, self->r.maxs,
				self->target_ent->r.currentOrigin, self->r.ownerNum, self->clipmask ); // if the player turned around another corner while we were thinking, drop the target.
			  traceEnt = &g_entities[ tr3.entityNum ];
			if ( tr3.entityNum == ENTITYNUM_WORLD || traceEnt->s.eType == ET_BUILDABLE )
			{
				self->r.ownerNum = ENTITYNUM_WORLD;

				self->think = AHive_ReturnToHive;
				self->nextthink = level.time + FRAMETIME;
				//;//trap_SendServerCommand( -1, va( "print \"Lost Target\n\""));
				return;				// lost the track, get back home
			}
			else
			{
				if (self->TotalWP < MAX_WPA)
				{
					self->TotalWP++;
					//;//trap_SendServerCommand( -1, va( "print \"New Waypoint, %i\n\"", self->TotalWP - 1));
					VectorCopy(self->target_ent->r.currentOrigin, self->WPA[ self->TotalWP ]);
				}
				else
				{
					self->r.ownerNum = ENTITYNUM_WORLD;

					self->think = AHive_ReturnToHive;
					self->nextthink = level.time + FRAMETIME;
					//;//trap_SendServerCommand( -1, va( "print \"Out Of memory\n\""));
					return;				// out of memory, get back home
				}
			}
		}
		else
		{
			VectorCopy(self->target_ent->r.currentOrigin, self->WPA[ self->TotalWP ]); //update latest waypoint without creating a new one, if there is a LOS from the old one
		}		
	}
	else
	{
		if (self->TotalWP > 1)
		{
			self->TotalWP = 1;
			//;//trap_SendServerCommand( -1, va( "print \"Direct Line of Sight\n\""));
		}
		VectorCopy(self->target_ent->r.currentOrigin, self->WPA[ 0 ]);
		VectorCopy(self->target_ent->r.currentOrigin, self->WPA[ self->TotalWP ]);
		

	}
	
	if (Distance( self->r.currentOrigin, self->WPA[ 0 ]) < 64 && self->TotalWP > 1) // snap the swarm to the waypoint if it is close enough.
	{
	int i;
	VectorCopy(self->WPA[ 0 ], self->r.currentOrigin);
		for (i = 0; i <= MAX_WPA; i++)
		{
			VectorCopy(self->WPA[i+1], self->WPA[i]);
		}
		self->TotalWP--;
		if (self->TotalWP < 1) self->TotalWP = 1;
		VectorCopy(self->target_ent->r.currentOrigin, self->WPA[ self->TotalWP ]);
		//;//trap_SendServerCommand( -1, va( "print \"Waypoint Reached\n\""));
	}
	
	trap_Trace( &tr, self->r.currentOrigin, self->r.mins, self->r.maxs,
        self->WPA[ 0 ], self->r.ownerNum, self->clipmask );
	  traceEnt = &g_entities[ tr.entityNum ];
	if ( tr.entityNum == ENTITYNUM_WORLD || traceEnt->s.eType == ET_BUILDABLE )  // check for doors closing
	{
	
	    self->r.ownerNum = ENTITYNUM_WORLD;

		self->think = AHive_ReturnToHive;
		self->nextthink = level.time + FRAMETIME;
		//;//trap_SendServerCommand( -1, va( "print \"Invalid Waypoint\n\""));
		return;
	}
	
	VectorSubtract( self->WPA[ 0 ], self->r.currentOrigin, dir );
    VectorNormalize( dir );

    //change direction towards the player
    VectorScale( dir, HIVE_SPEED, self->s.pos.trDelta );
    SnapVector( self->s.pos.trDelta );      // save net bandwidth
    VectorCopy( self->r.currentOrigin, self->s.pos.trBase );
    self->s.pos.trTime = level.time;

    self->nextthink = level.time + HIVE_DIR_CHANGE_PERIOD/2;
  }
}

/*
=================
fire_hive
=================
*/
gentity_t *fire_hive( gentity_t *self, vec3_t start, vec3_t dir )
{
  gentity_t *bolt;

  VectorNormalize ( dir );

  bolt = G_Spawn( );
  bolt->classname = "hive";
  bolt->nextthink = level.time + HIVE_DIR_CHANGE_PERIOD;
  bolt->think = AHive_SearchAndDestroy;
  bolt->s.eType = ET_MISSILE;
  bolt->s.eFlags |= EF_BOUNCE|EF_NO_BOUNCE_SOUND;
  bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
  bolt->s.weapon = WP_HIVE;
  bolt->s.generic1 = WPM_PRIMARY; //weaponMode
  bolt->r.ownerNum = self->s.number;
  bolt->parent = self;
  bolt->damage = HIVE_DMG;
  bolt->splashDamage = 0;
  bolt->splashRadius = 0;
  bolt->methodOfDeath = MOD_SWARM;
  bolt->clipmask = MASK_SHOT;
  bolt->target_ent = self->target_ent;
  
  bolt->TotalWP = 0;
 
  if(self->targWP != -1)
  {
    if(self->targWP >= 0)
    {
		bolt->TotalWP++;
		VectorCopy(self->WPA[self->targWP], bolt->WPA[ 0 ]);
		VectorCopy(self->WPA[self->targWP], bolt->WPA[ bolt->TotalWP ]);
	    ;//trap_SendServerCommand( -1, va( "print \"Swarm : Waypoint Received, proceeding to target\n\""));	
	}
	else
	{
	 int TW;
		TW = abs(self->targWP) - 2;
		bolt->TotalWP += 2;
		VectorCopy(self->WPA[TW], bolt->WPA[ 0 ]);
		VectorCopy(self->WPA2[TW], bolt->WPA[ 1 ]);
		VectorCopy(self->WPA2[TW], bolt->WPA[ bolt->TotalWP ]);
	    ;//trap_SendServerCommand( -1, va( "print \"Swarm : 2 Waypoints received, proceeding to target\n\""));		
	}
  }

  bolt->s.pos.trType = TR_LINEAR;
  bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;   // move a bit on the very first frame
  VectorCopy( start, bolt->s.pos.trBase );
  VectorScale( dir, HIVE_SPEED, bolt->s.pos.trDelta );
  SnapVector( bolt->s.pos.trDelta );      // save net bandwidth
  VectorCopy( start, bolt->r.currentOrigin );

  return bolt;
}

//=============================================================================

/*
=================
fire_lockblob
=================
*/
gentity_t *fire_lockblob( gentity_t *self, vec3_t start, vec3_t dir )
{
  gentity_t *bolt;

  VectorNormalize ( dir );

  bolt = G_Spawn( );
  bolt->classname = "lockblob";
  bolt->nextthink = level.time + 15000;
  bolt->think = G_ExplodeMissile;
  bolt->s.eType = ET_MISSILE;
  bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
  bolt->s.weapon = WP_LOCKBLOB_LAUNCHER;
  bolt->s.generic1 = WPM_PRIMARY; //weaponMode
  bolt->r.ownerNum = self->s.number;
  bolt->parent = self;
  bolt->damage = 0;
  bolt->splashDamage = 0;
  bolt->splashRadius = 0;
  bolt->methodOfDeath = MOD_UNKNOWN; //doesn't do damage so will never kill
  bolt->clipmask = MASK_SHOT;
  bolt->target_ent = NULL;

  bolt->s.pos.trType = TR_LINEAR;
  bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;   // move a bit on the very first frame
  VectorCopy( start, bolt->s.pos.trBase );
  VectorScale( dir, 500, bolt->s.pos.trDelta );
  SnapVector( bolt->s.pos.trDelta );      // save net bandwidth
  VectorCopy( start, bolt->r.currentOrigin );

  return bolt;
}

/*
=================
fire_slowBlob
=================
*/
gentity_t *fire_slowBlob( gentity_t *self, vec3_t start, vec3_t dir )
{
  gentity_t *bolt;

  VectorNormalize ( dir );

  bolt = G_Spawn( );
  bolt->classname = "slowblob";
  bolt->nextthink = level.time + 15000;
  bolt->think = G_ExplodeMissile;
  bolt->s.eType = ET_MISSILE;
  bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
  bolt->s.weapon = WP_ABUILD2;
  bolt->s.generic1 = self->s.generic1; //weaponMode
  bolt->r.ownerNum = self->s.number;
  bolt->parent = self;
  bolt->damage = ABUILDER_BLOB_DMG;
  bolt->splashDamage = 0;
  bolt->splashRadius = 0;
  bolt->methodOfDeath = MOD_SLOWBLOB;
  bolt->splashMethodOfDeath = MOD_SLOWBLOB;
  bolt->clipmask = MASK_SHOT;
  bolt->target_ent = NULL;

  bolt->s.pos.trType = TR_GRAVITY;
  bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;   // move a bit on the very first frame
  VectorCopy( start, bolt->s.pos.trBase );
  VectorScale( dir, ABUILDER_BLOB_SPEED, bolt->s.pos.trDelta );
  SnapVector( bolt->s.pos.trDelta );      // save net bandwidth
  VectorCopy( start, bolt->r.currentOrigin );

  return bolt;
}

/*
=================
fire_paraLockBlob
=================
*/
gentity_t *fire_paraLockBlob( gentity_t *self, vec3_t start, vec3_t dir )
{
  gentity_t *bolt;

  VectorNormalize ( dir );

  bolt = G_Spawn( );
  bolt->classname = "lockblob";
  bolt->nextthink = level.time + 15000;
  bolt->think = G_ExplodeMissile;
  bolt->s.eType = ET_MISSILE;
  bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
  bolt->s.weapon = WP_LOCKBLOB_LAUNCHER;
  bolt->s.generic1 = self->s.generic1; //weaponMode
  bolt->r.ownerNum = self->s.number;
  bolt->parent = self;
  bolt->damage = 0;
  bolt->splashDamage = 0;
  bolt->splashRadius = 0;
  bolt->clipmask = MASK_SHOT;
  bolt->target_ent = NULL;

  bolt->s.pos.trType = TR_GRAVITY;
  bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;   // move a bit on the very first frame
  VectorCopy( start, bolt->s.pos.trBase );
  VectorScale( dir, LOCKBLOB_SPEED, bolt->s.pos.trDelta );
  SnapVector( bolt->s.pos.trDelta );      // save net bandwidth
  VectorCopy( start, bolt->r.currentOrigin );

  return bolt;
}

/*
=================
fire_bounceBall
=================
*/
gentity_t *fire_bounceBall( gentity_t *self, vec3_t start, vec3_t dir )
{
  gentity_t *bolt;

  VectorNormalize ( dir );

  bolt = G_Spawn( );
  bolt->classname = "bounceball";
  bolt->nextthink = level.time + 3000;
  bolt->think = G_ExplodeMissile;
  bolt->s.eType = ET_MISSILE;
  bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
  bolt->s.weapon = WP_ALEVEL3_UPG;
  bolt->s.generic1 = self->s.generic1; //weaponMode
  bolt->r.ownerNum = self->s.number;
  bolt->parent = self;
  bolt->damage = LEVEL3_BOUNCEBALL_DMG;
  bolt->splashDamage = 0;
  bolt->splashRadius = 0;
  bolt->methodOfDeath = MOD_LEVEL3_BOUNCEBALL;
  bolt->splashMethodOfDeath = MOD_LEVEL3_BOUNCEBALL;
  bolt->clipmask = MASK_SHOT;
  bolt->target_ent = NULL;

  bolt->s.pos.trType = TR_GRAVITY;
  bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME;   // move a bit on the very first frame
  VectorCopy( start, bolt->s.pos.trBase );
  VectorScale( dir, LEVEL3_BOUNCEBALL_SPEED, bolt->s.pos.trDelta );
  SnapVector( bolt->s.pos.trDelta );      // save net bandwidth
  VectorCopy( start, bolt->r.currentOrigin );
  /*bolt->s.eFlags |= EF_BOUNCE;*/

  return bolt;
}

