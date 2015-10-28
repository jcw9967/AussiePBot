/*
===========================================================================
This file is part of Tremulous.

The original works of vcxzet (lamebot3) were used a guide to create TremBot.
These works were built on more by Amine Haddad.
Finally these bots were fully fledged AI created by Jordan Wilson.

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

#ifndef RAND_MAX
#define RAND_MAX 32768
#endif

void G_BotReload( gentity_t *ent, int clientNum )
{
	ClientDisconnect( clientNum );
	G_BotAdd( ent->client->pers.netname, ent->botSkillLevel, clientNum );
	AP( "print \"Interfering bot reloaded\n\"" );
}

void G_DeleteBots( void )
{
	int i;
	gentity_t *bot;

	for( i = 0; i < level.maxclients; i++ )
	{
		bot = &g_entities[i];

		if( bot->r.svFlags & SVF_BOT )
		{
			ClientDisconnect( bot->client->ps.clientNum );
		}
	}
}

void G_BotAdd( char *name, int skill, int ignore )
{
	int i;
	int clientNum = -1;
	char userinfo[MAX_INFO_STRING];
	gentity_t *bot;
	int reservedSlots = trap_Cvar_VariableIntegerValue( "sv_privateclients" );

	// find what clientNum to use for bot
	for( i = 0; i < reservedSlots; i++ )
	{
		if( i == ignore )
		{
			continue;
		}

		if( !g_entities[i].inuse )
		{
			clientNum = i;
			break;
		}
	}

	if( clientNum == -1 )
	{
		trap_Printf( "No more slots for bots.\n" );
	}
	else
	{
		bot = &g_entities[clientNum];
		bot->inuse = qtrue;

		//default bot data
		bot->botEnemy = NULL;
		bot->botEnemyLastSeen = 0;

		if( skill > 360 )
		{
			skill = 360;
		}
		else if( skill < 1 )
		{
			skill = 1;
		}
		bot->botSkillLevel = skill;
		bot->pathChosen = qfalse;
		bot->nextNode = qfalse;
		bot->state = FIND_NEW_PATH;
		bot->blocked = qfalse;

		// register user information
		userinfo[0] = '\0';
		Info_SetValueForKey( userinfo, "name", name );
		Info_SetValueForKey( userinfo, "rate", "0" ); //25000
		Info_SetValueForKey( userinfo, "snaps", "0" );

		trap_SetUserinfo( clientNum, userinfo );

		// have it connect to the game as a normal client
		if( ClientConnect( clientNum, qtrue ) != NULL )
		{
			// won't let us join
			return;
		}

		bot->r.svFlags |= SVF_BOT;
		ClientBegin( clientNum );
		G_ChangeTeam( bot, PTE_ALIENS );
	}
}

void G_BotDel( int clientNum )
{
	gentity_t *bot = &g_entities[clientNum];

	if( bot->r.svFlags & SVF_BOT )
	{
		ClientDisconnect( clientNum );
	}
	else
	{
		trap_Printf( va( "'^7%s^7' is not a bot\n", bot->client->pers.netname ) );
	}
}

int distanceToTargetNode( gentity_t *self )
{
	int distance;
	int Ax, Ay, Az;
	int Bx, By, Bz;
	const int targetPathIndex = self->targetPathIndexes[self->targetPathIndex];

	Ax = level.paths[targetPathIndex].coord[0];
	Ay = level.paths[targetPathIndex].coord[1];
	Az = level.paths[targetPathIndex].coord[2];
	Bx = self->s.pos.trBase[0];
	By = self->s.pos.trBase[1];
	Bz = self->s.pos.trBase[2];

	distance = sqrt( ( Ax - Bx )*( Ax - Bx ) + ( Ay - By )*( Ay - By ) + ( Az - Bz )*( Az - Bz ) );

	return distance;
}

qboolean botAimAtPath( gentity_t *self )
{
	vec3_t dirToTarget, angleToTarget;
	vec3_t top = { 0 };
	int viewHeight;
	int i;

	BG_FindViewheightForClass( self->client->ps.stats[STAT_PCLASS], &viewHeight, NULL );
	top[2] = viewHeight;

	VectorAdd( self->s.pos.trBase, top, top );
	VectorSubtract( level.paths[self->targetPathIndexes[self->targetPathIndex]].coord, top, dirToTarget );
	VectorNormalize( dirToTarget );
	vectoangles( dirToTarget, angleToTarget );

	for( i = 0; i < 2; i++ )
	{
		self->client->ps.delta_angles[i] = ANGLE2SHORT( angleToTarget[i] );
	}

	return qtrue;
}

/**
 * Simplified version provided by Jordan Wilson
 */
void G_FrameAim( gentity_t *self )
{
	if( self->client->ps.stats[STAT_PCLASS] != PCL_NONE )
	{
		if( self->botEnemy )
		{
			botAimAtTarget( self, self->botEnemy );
		}
		else
		{
			botAimAtPath( self );
		}
	}
}

/**
 * Simplified and cleaned version provided by Jordan Wilson
 */
void G_FastThink( gentity_t *self )
{
	if( self->client->ps.stats[STAT_PCLASS] != PCL_NONE )
	{
		if( self->botEnemy )
		{
			//Make sure we can still attack our enemy
			if( !botTargetInRange( self, self->botEnemy ) )
			{
				//Dead enemy or we can't see them anymore
				self->botEnemy = NULL;
			}
			else
			{
				botShootIfTargetInRange( self, self->botEnemy );
				self->enemytime = level.time;
			}
		}
		else if( self->state == TARGET_PATH )
		{
			self->client->pers.cmd.buttons = 0;
			self->client->pers.cmd.upmove = 0;
			self->client->pers.cmd.rightmove = 0;

			if( ( self->isBlocked == qtrue || VectorLength( self->client->ps.velocity ) < 50.0f ) )
			{
				self->client->pers.cmd.buttons |= BUTTON_GESTURE;
				self->client->pers.cmd.rightmove = -100;

				if( self->client->time1000 >= 500 )
				{
					self->client->pers.cmd.rightmove = 100;
				}

				if( level.time - self->jumptime > 3000 )
				{
					self->client->pers.cmd.upmove = 20;
					if( level.time - self->jumptime > 4000 )
					{
						self->jumptime = level.time;
					}
				}
			}

			self->client->pers.cmd.forwardmove = 127;
			if( level.time - self->timeFoundPath > 10000 )
			{
				self->state = FIND_NEW_PATH;
				self->timeFoundPath = level.time;
			}
			//We're closing in on our node
			else if( distanceToTargetNode( self ) < 70 )
			{
				/**
				 * This whole section is new to deal with our BFS algorithm. We need to keep tabs of the index and size of the array.
				 */
				self->targetPathIndex++;
				self->targetPathSize--;
				//Do we have any more nodes or should we calculate more?
				if( self->targetPathSize > 0 )
				{
					self->state = TARGET_PATH;
					self->timeFoundPath = level.time;
				}
				else
				{
					self->state = FIND_NEXT_PATH;
				}
			}
		}
	}
}

void findNewPath( gentity_t *self )
{
	trace_t trace;
	int i;
	int distance;
	int Ax, Ay, Az, Bx, By, Bz;
	int closestPath;
	int closestPathDistance;
	qboolean pathFound = qfalse;

	for( i = 0; i < level.numPaths; i++ ) //find a nearby path that wasn't used before
	{
		trap_Trace( &trace, self->s.pos.trBase, NULL, NULL, level.paths[i].coord, self->s.number, MASK_SHOT );

		if( trace.fraction < 1.0 )
		{
			continue;
		}

		Ax = level.paths[i].coord[0];
		Ay = level.paths[i].coord[1];
		Az = level.paths[i].coord[2];
		Bx = self->s.pos.trBase[0];
		By = self->s.pos.trBase[1];
		Bz = self->s.pos.trBase[2];

		distance = sqrt( ( Ax - Bx )*( Ax - Bx ) + ( Ay - By )*( Ay - By ) + ( Az - Bz )*( Az - Bz ) );
		if( !pathFound || distance < closestPathDistance )
		{
			closestPath = i;
			closestPathDistance = distance;
			pathFound = qtrue;
		}
	}

	if( pathFound )
	{
		/**
		 * New implementation here to help make the BFS work.
		 */
		self->state = TARGET_PATH;
		self->targetPathSize = 1;
		self->targetPathIndex = 0;
		self->targetPathIndexes[self->targetPathIndex] = closestPath;
		self->lastPathID = closestPath;
		self->timeFoundPath = level.time;
		self->isBlocked = qfalse;
	}
	else
	{
		//We're lost
		self->state = FIND_NEW_PATH;
		self->client->pers.cmd.forwardmove = 0;
		self->client->pers.cmd.upmove = -1;
		self->client->pers.cmd.rightmove = 0;
		self->client->pers.cmd.buttons = 0;
		self->client->pers.cmd.buttons |= BUTTON_GESTURE;
	}
	return;
}

/**
 * This is a new function needed to reverse the flow of the path.
 */
void reversePath( gentity_t *self )
{
	int i;
	int swap;
	int size = self->targetPathSize;

	for( i = 0; i < --size; i++ )
	{
		swap = self->targetPathIndexes[i];
		self->targetPathIndexes[i] = self->targetPathIndexes[size];
		self->targetPathIndexes[size] = swap;
	}
}

/**
 * Completely new implementation.
 */
void breadthFirstSearch( int startNodeIndex, int goalNodeIndex, gentity_t *self )
{
	int firstIndex = 0;  //Keep track of the first index of the array instead of reallocating memory all the time.
	int nextFreeIndex = 1;  //Keep track of the next free index of the array so we don't have to search every time.
	int openSet[MAX_PATHS];  //Open set only starts with our first node; we add more as we expand through the graph.
	int cameFrom[MAX_PATHS];  //Keep track of where we came from on each node so we can reconstruct the path afterwards.
	int currentIndex;  //Used for the current index within the loop.
	int neighbourIndex;  //Used for the neighbour index within the loop.
	int i, j;  //Used for looping variables.
	qboolean alreadyInOpenSet;  //Check to see if the node is already in the open set.

	//Reset the arrays to 1200 so it doesn't get confused with node 0. Using -1 causes issues, and 1200 will never be reached.
	for( i = 0; i < MAX_PATHS; i++ )
	{
		cameFrom[i] = openSet[i] = self->targetPathIndexes[i] = 1000 + MAX_PATHS;
	}

	//Insert our first item into the open set.
	openSet[0] = startNodeIndex;

	//BFS Algorithm
	while( openSet[firstIndex] )
	{
		//Load our next node from the queue.
		currentIndex = openSet[firstIndex];

		//Have we found our end node?
		if( currentIndex == goalNodeIndex )
		{
			break;
		}

		//Iterate through all possible neighbours of our current node.
		for( i = 0; i < MAX_PATH_NEIGHBOURS; i++ )
		{
			//The current node index that we're working with.
			neighbourIndex = level.paths[currentIndex].neighbourID[i];

			//Make sure the neighbour node exists
			if( neighbourIndex != 1000 + MAX_PATHS )
			{
				alreadyInOpenSet = qfalse;

				//Check to see if node already exists in open set.
				for( j = 0; j < nextFreeIndex; j++ )
				{
					if( openSet[j] == neighbourIndex )
					{
						alreadyInOpenSet = qtrue;
						break;
					}
				}

				//If it's already in the open set, don't add it again.
				if( !alreadyInOpenSet )
				{
					//Add new node to the open set to explore.
					openSet[nextFreeIndex] = neighbourIndex;
					nextFreeIndex++;

					//Make note of where we came from.
					cameFrom[neighbourIndex] = currentIndex;
				}
			}
		}

		firstIndex++;
	}

	//Reconstruct path by starting from the goal node and working backwards.
	currentIndex = goalNodeIndex;
	self->targetPathSize = 1;
	self->targetPathIndexes[0] = currentIndex;
	for( i = 1; i < MAX_PATHS && currentIndex != startNodeIndex; i++ )
	{
		currentIndex = cameFrom[currentIndex];
		self->targetPathSize++;
		self->targetPathIndexes[i] = currentIndex;
	}

	//Do we have a path?
	if( self->targetPathSize > 0 )
	{
		self->targetPathIndex = 0;

		//Reverse the path because we're currently going from goal -> start.
		reversePath( self );
	}
	else
	{
		//Find a new path
		self->state = FIND_NEW_PATH;
	}
}

/**
 * If the reactor is moved, we still need to be able to redirect to it.
 * By using this, we can dynamically find the reactor instead of directing to a static node.
 */
void findReactorInWorld( gentity_t* reactor )
{
	int i;
	gentity_t *target;

	for( i = 0; i < level.num_entities; i++ )
	{
		target = &g_entities[i];

		if( target->s.modelindex == BA_H_REACTOR )
		{
			reactor = target;
			break;
		}
	}
}

/**
 * Partially new implementation
 */
void findNextPath( gentity_t *self )
{
	//We're currently at self->lastPathID
	int currentPathIndex = self->lastPathID;

	if( currentPathIndex >= 0 && currentPathIndex < level.numPaths )
	{
		if( level.paths[currentPathIndex].random )
		{
			int i;
			int nextFreeIndex = 0;
			int possibleNodes[MAX_PATH_NEIGHBOURS];
			int neighbourID;


			//Count neighbours
			for( i = 0; i < MAX_PATH_NEIGHBOURS; i++ )
			{
				neighbourID = level.paths[currentPathIndex].neighbourID[i];

				if( neighbourID != 1000 + MAX_PATHS )
				{
					possibleNodes[nextFreeIndex] = neighbourID;
					nextFreeIndex++;
				}
			}

			self->targetPathSize = 1;
			srand( trap_Milliseconds() );
			self->targetPathIndexes[0] = nextFreeIndex == 0 ? 1200 : possibleNodes[rand() % nextFreeIndex];
		}
		else
		{
			gentity_t *reactor = NULL;

			//find the reactor
			findReactorInWorld( reactor );

			if( reactor != NULL )
			{
				//try to get to that node
				breadthFirstSearch( currentPathIndex, reactor->nearestNodeIndex, self );
			}
			else
			{
				breadthFirstSearch( currentPathIndex, 0, self );
			}
		}
	}

	//We don't have any paths to join to. Let's find a new one.
	if( self->targetPathIndexes[0] == 1000 + MAX_PATHS )
	{
		self->state = FIND_NEW_PATH;
	}
	else
	{
		//We've found a path that we can travel along.
		self->state = TARGET_PATH;

		//Make note of the last path id we used.
		self->lastPathID = currentPathIndex;
		self->targetPathIndex = 0;

		//This is when we've found the path so we can use a timeout if we need to find a new path.
		self->timeFoundPath = level.time;
	}
}

/**
 * Partially new implementation.
 */
void pathFinding( gentity_t *self )
{
	switch( self->state )
	{
		case FIND_NEW_PATH:
			findNewPath( self );
			break;

		/**
		 * Use our BFS algorithm.
		 */
		case FIND_NEXT_PATH:
			findNextPath( self );
			break;

		/**
		 * We're still following the path we've been allocated.
		 */
		case TARGET_PATH:
			break;
	}
}

/**
 * Cleaned up.
 */
#define CLASSES_SIZE 7
void Bot_Evolve( gentity_t *self )
{
	if( self->client->ps.stats[STAT_PCLASS] == PCL_ALIEN_BUILDER0_UPG )
	{
		int randclass;
		int class = PCL_ALIEN_BUILDER0_UPG;
		vec3_t origin;

		srand( trap_Milliseconds() );
		randclass = rand() % g_ambush_stage.integer / 3;
		switch( randclass )
		{
			case 0:
				return;
				break;

			case 1:
				class = PCL_ALIEN_LEVEL0;
				break;

			case 2:
				class = PCL_ALIEN_LEVEL1;
				break;

			case 3:
				class = PCL_ALIEN_LEVEL1_UPG;
				break;

			case 4:
				class = PCL_ALIEN_LEVEL2;
				break;

			case 5:
				class = PCL_ALIEN_LEVEL2_UPG;
				break;

			case 6:
				class = PCL_ALIEN_LEVEL3;
				break;

			case 7:
				class = PCL_ALIEN_LEVEL3_UPG;
				break;

			default:
				class = PCL_ALIEN_LEVEL4;
				break;
		}

		if( level.overmindPresent
			&& G_RoomForClassChange( self, class, origin )
			&& !( self->client->ps.stats[STAT_STATE] & SS_WALLCLIMBING )
			&& !( self->client->ps.stats[STAT_STATE] & SS_WALLCLIMBINGCEILING ) )
		{
			int clientNum = self->client - level.clients;

			self->client->pers.evolveHealthFraction = (float)self->client->ps.stats[STAT_HEALTH] /
				(float)BG_FindHealthForClass( self->client->ps.stats[STAT_PCLASS] );

			if( self->client->pers.evolveHealthFraction < 0.0f )
			{
				self->client->pers.evolveHealthFraction = 0.0f;
			}
			else if( self->client->pers.evolveHealthFraction > 1.0f )
			{
				self->client->pers.evolveHealthFraction = 1.0f;
			}

			self->client->pers.classSelection = class;
			ClientUserinfoChanged( clientNum );
			VectorCopy( origin, self->s.pos.trBase );
			ClientSpawn( self, self, self->s.pos.trBase, self->s.apos.trBase );
			self->evolvetime = level.time;
			self->client->pers.cmd.buttons = 0;
			self->client->pers.cmd.upmove = 0;
			self->client->pers.cmd.rightmove = 0;
		}
	}
}

/**
 * Partially new implementation. Cleanup up.
 */
#define CLICKS_TO_STOP_CHASE 30
void G_BotThink( gentity_t *self )
{
	int tempEntityIndex;
	trace_t tr;
	vec3_t end;
	vec3_t mins, maxs;
	vec3_t forward, right, up;
	vec3_t muzzle;
	gentity_t *traceEnt;

	//Only perform if bot is spawned
	if( self->client->ps.stats[STAT_PCLASS] != PCL_NONE )
	{
		if( level.time - self->evolvetime > 500
			&& ( ROTACAK_ambush_rebuild_time_temp < level.time
				&& ( ( level.time - level.startTime ) >( g_ambush_secToStart.integer * 1000 ) ) ) )
		{
			Bot_Evolve( self );
		}

		if( self->botEnemy )
		{
			//Make sure we can still attack our enemy
			if( !botTargetInRange( self, self->botEnemy ) )
			{
				//dead enemy or we can't see them anymore
				self->botEnemy = NULL;
			}
		}
		else
		{
			//Try to find the nearest enemy
			if( level.time - self->searchtime > 2 * ( 360 - self->botSkillLevel ) )
			{
				self->searchtime = level.time;
				tempEntityIndex = botFindClosestEnemy( self, qfalse );
				if( tempEntityIndex >= 0 )
				{
					self->botEnemy = &g_entities[tempEntityIndex];
				}
			}
		}

		//We may have found them. Check again.
		if( self->botEnemy )
		{
			botShootIfTargetInRange( self, self->botEnemy );
			self->enemytime = level.time;

			// enable wallwalk if alien
			if( BG_ClassHasAbility( self->client->ps.stats[STAT_PCLASS], SCA_WALLCLIMBER ) )
			{
				self->client->pers.cmd.upmove = -1;
			}
			else if( self->client->ps.stats[STAT_PTEAM] == PTE_ALIENS
				&& level.time - self->jumptime > 3000
				&& self->client->ps.stats[STAT_PCLASS] != PCL_ALIEN_LEVEL3_UPG
				&& self->client->ps.stats[STAT_PCLASS] != PCL_ALIEN_LEVEL3 )
			{
				self->client->pers.cmd.upmove = 20;
				if( level.time - self->jumptime > 4000 )
				{
					self->jumptime = level.time;
				}
			}

			//We're facing a human player
			if( self->botEnemy->client )
			{
				int sideMovement = 100;

				self->client->pers.cmd.forwardmove = 127;

				//Dodge!
				if( self->client->time1000 >= 500 )
				{
					self->client->pers.cmd.rightmove = sideMovement;
				}
				else
				{
					self->client->pers.cmd.rightmove = -sideMovement;
				}
			}
		}
		//Keep travelling along our path. Check for blockages.
		else if( self->state == TARGET_PATH )
		{
			//Can we get to the next node or is it blocked?
			VectorSet( mins, -20, -20, -20 );
			VectorSet( maxs, 20, 20, 20 );
			AngleVectors( self->client->ps.viewangles, forward, right, up );
			CalcMuzzlePoint( self, forward, right, up, muzzle );
			VectorMA( muzzle, 20, forward, end );
			trap_Trace( &tr, muzzle, mins, maxs, end, self->s.number, MASK_SHOT );
			traceEnt = &g_entities[tr.entityNum];

			if( traceEnt->health > 0 )
			{
				self->isBlocked = qtrue;
			}
			else
			{
				self->isBlocked = qfalse;
			}
		}

		pathFinding( self ); //Roam the map.
	}
}

void G_BotSpectatorThink( gentity_t *self )
{
	if( g_bot.integer > 0
		&& !( self->client->ps.pm_flags & PMF_QUEUED )
		&& self->client->ps.stats[STAT_PCLASS] == PCL_NONE
		&& self->client->sess.sessionTeam == TEAM_SPECTATOR )
	{
		int clientNum = self->client->ps.clientNum;

		if( ROTACAK_ambush_rebuild_time_temp < level.time && ( ( level.time - level.startTime ) >( g_ambush_secToStart.integer * 1000 ) ) )
		{
			self->client->pers.classSelection = PCL_ALIEN_BUILDER0_UPG;
			self->client->ps.stats[STAT_PCLASS] = PCL_ALIEN_BUILDER0_UPG;
			G_PushSpawnQueue( &level.alienSpawnQueue, clientNum );
		}
	}
}

qboolean botAimAtTarget( gentity_t *self, gentity_t *target )
{
	int Ax, Ay, Az, Bx, By, Bz;
	vec3_t dirToTarget, angleToTarget;
	vec3_t top = { 0, 0, 0 };
	vec3_t forward, right, up;
	vec3_t muzzle, delta;
	float deltangle[2];
	float diffangle[2];
	int i;

	AngleVectors( self->client->ps.viewangles, forward, right, up );
	CalcMuzzlePoint( self, forward, right, up, muzzle );

	if( self->client->ps.stats[STAT_PCLASS] == PCL_ALIEN_LEVEL3 ||
		self->client->ps.stats[STAT_PCLASS] == PCL_ALIEN_LEVEL3_UPG )
	{
		Ax = target->s.pos.trBase[0];
		Ay = target->s.pos.trBase[1];
		Az = target->s.pos.trBase[2];
		Bx = self->s.pos.trBase[0];
		By = self->s.pos.trBase[1];
		Bz = self->s.pos.trBase[2];

		if( self->client->ps.stats[STAT_PCLASS] == PCL_ALIEN_LEVEL3 )
		{
			top[2] = sqrt( ( Ax - Bx )*( Ax - Bx ) + ( Ay - By )*( Ay - By ) + ( Az - Bz )*( Az - Bz ) ) / 3;
		}
		else
		{
			top[2] = sqrt( ( Ax - Bx )*( Ax - Bx ) + ( Ay - By )*( Ay - By ) + ( Az - Bz )*( Az - Bz ) ) / 5;
		}
	}

	VectorAdd( target->s.pos.trBase, top, top );
	VectorSubtract( top, muzzle, dirToTarget );
	VectorNormalize( dirToTarget );
	vectoangles( dirToTarget, angleToTarget );

	for( i = 0; i < 2; i++ )
	{
		deltangle[i] = SHORT2ANGLE( self->client->ps.delta_angles[i] );
		diffangle[i] = AngleSubtract( angleToTarget[i], deltangle[i] );

		if( diffangle[i] > self->botSkillLevel )
		{
			delta[i] = deltangle[i] + self->botSkillLevel;
		}
		else if( diffangle[i] < -self->botSkillLevel )
		{
			delta[i] = deltangle[i] - self->botSkillLevel;
		}
		else
		{
			delta[i] = angleToTarget[i];
		}

		self->client->ps.delta_angles[i] = ANGLE2SHORT( delta[i] );
	}

	return qtrue;
}

qboolean botTargetInRange( gentity_t *self, gentity_t *target )
{
	if( self && self->client && target && target->health > 0 )
	{
		vec3_t  forward, right, up;
		vec3_t  muzzle;

		AngleVectors( self->client->ps.viewangles, forward, right, up );
		CalcMuzzlePoint( self, forward, right, up, muzzle );

		if( !target->client && g_ambush_attackBuildables.integer == 0
			|| target->client->ps.stats[STAT_STATE] & SS_HOVELING
			|| self->client->ps.stats[STAT_PCLASS] != PCL_ALIEN_LEVEL3_UPG && self->client->ps.stats[STAT_PCLASS] != PCL_ALIEN_LEVEL3 && target->s.pos.trBase[2] - self->s.pos.trBase[2] > 150
			|| self->client->ps.stats[STAT_PCLASS] == PCL_ALIEN_LEVEL4 && Distance( self->s.pos.trBase, target->s.pos.trBase ) > g_level4_range.integer
			|| self->client->ps.stats[STAT_PCLASS] == PCL_ALIEN_LEVEL3_UPG && Distance( self->s.pos.trBase, target->s.pos.trBase ) > g_level3UPG_range.integer
			|| self->client->ps.stats[STAT_PCLASS] == PCL_ALIEN_LEVEL3 && Distance( self->s.pos.trBase, target->s.pos.trBase ) > g_level3_range.integer
			|| self->client->ps.stats[STAT_PCLASS] == PCL_ALIEN_LEVEL2_UPG && Distance( self->s.pos.trBase, target->s.pos.trBase ) > g_level2UPG_range.integer
			|| self->client->ps.stats[STAT_PCLASS] == PCL_ALIEN_LEVEL2 && Distance( self->s.pos.trBase, target->s.pos.trBase ) > g_level2_range.integer
			|| self->client->ps.stats[STAT_PCLASS] == PCL_ALIEN_LEVEL1_UPG && Distance( self->s.pos.trBase, target->s.pos.trBase ) > g_level1UPG_range.integer
			|| self->client->ps.stats[STAT_PCLASS] == PCL_ALIEN_LEVEL1 && Distance( self->s.pos.trBase, target->s.pos.trBase ) > g_level1_range.integer
			|| self->client->ps.stats[STAT_PCLASS] == PCL_ALIEN_LEVEL0 && Distance( self->s.pos.trBase, target->s.pos.trBase ) > g_level0_range.integer
			|| self->client->ps.stats[STAT_PCLASS] == PCL_ALIEN_BUILDER0_UPG && Distance( self->s.pos.trBase, target->s.pos.trBase ) > 200 )
		{
			return qfalse;
		}
		else
		{
			trace_t   trace;
			gentity_t *traceEnt;
			
			trap_Trace( &trace, muzzle, NULL, NULL, target->s.pos.trBase, self->s.number, MASK_SHOT );
			traceEnt = &g_entities[trace.entityNum];

			//check our target is in LOS
			if( traceEnt == target )
			{
				return qtrue;
			}
		}
	}

	return qfalse;
}

// really an int? what if it's too long? If it is, we are fuxed.
int botGetDistanceBetweenPlayer( gentity_t *self, gentity_t *player )
{
	return Distance( self->s.pos.trBase, player->s.pos.trBase );
}

int botFindClosestEnemy( gentity_t *self, qboolean includeTeam )
{
	int i;
	gentity_t *target;

	// check list for enemies
	for( i = 0; i < MAX_GENTITIES; i++ )
	{
		target = &g_entities[i];

		if( self != target
			&& botGetDistanceBetweenPlayer( self, target ) < g_ambush_range.integer
			&& botTargetInRange( self, target ) )
		{
			if( g_ambush_attackBuildables.integer == 1 )
			{
				//If target is player
				if( target->client )
				{
					if( target->client->ps.stats[STAT_PTEAM] != self->client->ps.stats[STAT_PTEAM] )
					{
						return i;
					}
				}
				//If target is structure
				else
				{
					if( self->client->ps.stats[STAT_PCLASS] == PCL_ALIEN_LEVEL0 )
					{
						if( target->s.modelindex == BA_H_MGTURRET || target->s.modelindex == BA_H_TESLAGEN )
						{
							return i;
						}
					}
					else
					{
						if( target->s.modelindex == BA_H_SPAWN
							|| target->s.modelindex == BA_H_MGTURRET
							|| target->s.modelindex == BA_H_TESLAGEN
							|| target->s.modelindex == BA_H_ARMOURY
							|| target->s.modelindex == BA_H_DCC
							|| target->s.modelindex == BA_H_MEDISTAT
							|| target->s.modelindex == BA_H_REACTOR
							|| target->s.modelindex == BA_H_REPEATER )
						{
							return i;
						}
					}
				}
			}
			else
			{
				if( target->client && target->client->ps.stats[STAT_PTEAM] != self->client->ps.stats[STAT_PTEAM] )
				{
					return i;
				}
			}
		}
	}

	return -1;
}

/**
 * Partially new implementation
 *
 * Switch from if-else ladder to switch-case; faster.
 */
qboolean botShootIfTargetInRange( gentity_t *self, gentity_t *target )
{
	if( botTargetInRange( self, target ) )
	{
		int chance;

		srand( trap_Milliseconds() );
		chance = (int)( ( (double)rand() / ( (double)(RAND_MAX)+(double)( 1 ) ) ) * 20 );
		self->client->pers.cmd.buttons = 0;

		switch( self->client->pers.classSelection )
		{
			case PCL_ALIEN_BUILDER0_UPG:
			{
				if( chance > 10 )
				{
					self->client->pers.cmd.buttons |= BUTTON_ATTACK2;
				}
				else
				{
					self->client->pers.cmd.buttons |= BUTTON_USE_HOLDABLE;
				}

				break;
			}

			case PCL_ALIEN_LEVEL1_UPG:
			{
				if( chance > 15 )
				{
					self->client->pers.cmd.buttons |= BUTTON_ATTACK2;
				}
				else
				{
					self->client->pers.cmd.buttons |= BUTTON_ATTACK;
				}

				break;
			}

			case PCL_ALIEN_LEVEL2_UPG:
			{
				if( Distance( self->s.pos.trBase, target->s.pos.trBase ) > LEVEL2_CLAW_RANGE )
				{
					self->client->pers.cmd.buttons |= BUTTON_ATTACK2;
				}
				else
				{
					self->client->pers.cmd.buttons |= BUTTON_ATTACK;
				}

				break;
			}

			case PCL_ALIEN_LEVEL3:
			{
				if( Distance( self->s.pos.trBase, target->s.pos.trBase ) > 150
					&& self->client->ps.stats[STAT_MISC] < LEVEL3_POUNCE_SPEED )
				{
					self->client->pers.cmd.buttons |= BUTTON_ATTACK2;
				}
				else
				{
					self->client->pers.cmd.buttons |= BUTTON_ATTACK;
				}

				break;
			}

			case PCL_ALIEN_LEVEL3_UPG:
			{
				if( self->client->ps.ammo[WP_ALEVEL3_UPG] > 0
					&& Distance( self->s.pos.trBase, target->s.pos.trBase ) > 150 )
				{
					self->client->pers.cmd.buttons |= BUTTON_USE_HOLDABLE;
				}
				else
				{
					if( Distance( self->s.pos.trBase, target->s.pos.trBase ) > 150
						&& self->client->ps.stats[STAT_MISC] < LEVEL3_POUNCE_UPG_SPEED )
					{
						self->client->pers.cmd.buttons |= BUTTON_ATTACK2;
					}
					else
					{
						self->client->pers.cmd.buttons |= BUTTON_ATTACK;
					}
				}

				break;
			}

			case PCL_ALIEN_LEVEL4:
			{
				if( Distance( self->s.pos.trBase, target->s.pos.trBase ) > LEVEL4_CLAW_RANGE )
				{
					self->client->pers.cmd.buttons |= BUTTON_ATTACK2;
				}
				else
				{
					self->client->pers.cmd.buttons |= BUTTON_ATTACK;
				}

				break;
			}

			default:
			{
				self->client->pers.cmd.buttons |= BUTTON_ATTACK;

				break;
			}
		}

		return qtrue;
	}
	else
	{
		self->botEnemy = NULL;
	}

	return qfalse;
}
