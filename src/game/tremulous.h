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

/*
 * ALIEN weapons
 *
 * _REPEAT  - time in msec until the weapon can be used again
 * _DMG     - amount of damage the weapon does
 *
 */

#ifndef RAND_MAX
#define RAND_MAX 32768
#endif

#define ABUILDER_BUILD_REPEAT       100
#define ABUILDER_CLAW_DMG           15
#define ABUILDER_CLAW_RANGE         64.0f
#define ABUILDER_CLAW_WIDTH         4.0f
#define ABUILDER_CLAW_REPEAT        500
#define ABUILDER_CLAW_K_SCALE       1.0f
#define ABUILDER_BASE_DELAY         1000
#define ABUILDER_ADV_DELAY          500
#define ABUILDER_BLOB_DMG           4
#define ABUILDER_BLOB_REPEAT        1000
#define ABUILDER_BLOB_SPEED         800.0f
#define ABUILDER_BLOB_SPEED_MOD     0.5f
#define ABUILDER_BLOB_TIME          5000

#define LEVEL0_BITE_DMG             48
#define LEVEL0_BITE_RANGE           64.0f
#define LEVEL0_BITE_WIDTH           6.0f
#define LEVEL0_BITE_REPEAT          500
#define LEVEL0_BITE_K_SCALE         1.0f

#define LEVEL1_CLAW_DMG             25
#define LEVEL1_CLAW_RANGE           96.0f
#define LEVEL1_CLAW_WIDTH           10.0f
#define LEVEL1_CLAW_REPEAT          600
#define LEVEL1_CLAW_U_REPEAT        500
#define LEVEL1_CLAW_K_SCALE         1.0f
#define LEVEL1_CLAW_U_K_SCALE       1.0f
#define LEVEL1_GRAB_RANGE           64.0f
#define LEVEL1_GRAB_TIME            300
#define LEVEL1_GRAB_U_TIME          450
#define LEVEL1_PCLOUD_DMG           4
#define LEVEL1_PCLOUD_RANGE         200.0f
#define LEVEL1_PCLOUD_REPEAT        2000
#define LEVEL1_PCLOUD_TIME          10000

#define LEVEL2_CLAW_DMG             30
#define LEVEL2_CLAW_RANGE           96.0f
#define LEVEL2_CLAW_WIDTH           12.0f
#define LEVEL2_CLAW_REPEAT          500
#define LEVEL2_CLAW_K_SCALE         1.0f
#define LEVEL2_CLAW_U_REPEAT        400
#define LEVEL2_CLAW_U_K_SCALE       1.0f
#define LEVEL2_AREAZAP_DMG          60
#define LEVEL2_AREAZAP_RANGE        200.0f
#define LEVEL2_AREAZAP_WIDTH        15.0f
#define LEVEL2_AREAZAP_REPEAT       1500
#define LEVEL2_AREAZAP_TIME         1000
#define LEVEL2_AREAZAP_MAX_TARGETS  3
#define LEVEL2_WALLJUMP_MAXSPEED    1000.0f

#define LEVEL3_CLAW_DMG             60
#define LEVEL3_CLAW_RANGE           96.0f
#define LEVEL3_CLAW_WIDTH           16.0f
#define LEVEL3_CLAW_REPEAT          700
#define LEVEL3_CLAW_K_SCALE         1.0f
#define LEVEL3_CLAW_U_REPEAT        600
#define LEVEL3_CLAW_U_K_SCALE       1.0f
#define LEVEL3_POUNCE_DMG           75
#define LEVEL3_POUNCE_RANGE         72.0f
#define LEVEL3_POUNCE_WIDTH         16.0f
#define LEVEL3_POUNCE_SPEED         700
#define LEVEL3_POUNCE_UPG_SPEED     800
#define LEVEL3_POUNCE_SPEED_MOD     0.75f
#define LEVEL3_POUNCE_CHARGE_TIME   700
#define LEVEL3_POUNCE_TIME          400
#define LEVEL3_BOUNCEBALL_DMG       70
#define LEVEL3_BOUNCEBALL_REPEAT    1000
#define LEVEL3_BOUNCEBALL_SPEED     1000.0f

#define LEVEL4_CLAW_DMG             50
#define LEVEL4_CLAW_RANGE           128.0f
#define LEVEL4_CLAW_WIDTH           20.0f
#define LEVEL4_CLAW_REPEAT          750
#define LEVEL4_CLAW_K_SCALE         1.0f
#define LEVEL4_REGEN_RANGE          200.0f
#define LEVEL4_REGEN_MOD            2.0f
#define LEVEL4_CHARGE_SPEED         2.0f
#define LEVEL4_CHARGE_TIME          3000
#define LEVEL4_CHARGE_CHARGE_TIME   1500
#define LEVEL4_MIN_CHARGE_TIME      750
#define LEVEL4_CHARGE_CHARGE_RATIO  (LEVEL4_CHARGE_TIME/LEVEL4_CHARGE_CHARGE_TIME)
#define LEVEL4_CHARGE_REPEAT        1000
#define LEVEL4_CHARGE_DMG           110



/*
 * ALIEN classes
 *
 * _SPEED   - fraction of Q3A run speed the class can move
 * _REGEN   - health per second regained
 *
 */

#define ABUILDER_SPEED              0.8f
#define ABUILDER_VALUE              200
#define ABUILDER_HEALTH             50
#define ABUILDER_REGEN              2
#define ABUILDER_COST               0

#define ABUILDER_UPG_SPEED          1.0f
#define ABUILDER_UPG_VALUE          250
#define ABUILDER_UPG_HEALTH         75
#define ABUILDER_UPG_REGEN          3
#define ABUILDER_UPG_COST           0

#define LEVEL0_SPEED                1.3f
#define LEVEL0_VALUE                175
#define LEVEL0_HEALTH               25
#define LEVEL0_REGEN                1
#define LEVEL0_COST                 0

#define LEVEL1_SPEED                1.25f
#define LEVEL1_VALUE                225
#define LEVEL1_HEALTH               75
#define LEVEL1_REGEN                2
#define LEVEL1_COST                 1

#define LEVEL1_UPG_SPEED            1.25f
#define LEVEL1_UPG_VALUE            275
#define LEVEL1_UPG_HEALTH           100
#define LEVEL1_UPG_REGEN            3
#define LEVEL1_UPG_COST             1

#define LEVEL2_SPEED                1.2f
#define LEVEL2_VALUE                350
#define LEVEL2_HEALTH               150
#define LEVEL2_REGEN                4
#define LEVEL2_COST                 1

#define LEVEL2_UPG_SPEED            1.2f
#define LEVEL2_UPG_VALUE            450
#define LEVEL2_UPG_HEALTH           175
#define LEVEL2_UPG_REGEN            5
#define LEVEL2_UPG_COST             1

#define LEVEL3_SPEED                1.1f
#define LEVEL3_VALUE                500
#define LEVEL3_HEALTH               200
#define LEVEL3_REGEN                6
#define LEVEL3_COST                 1

#define LEVEL3_UPG_SPEED            1.1f
#define LEVEL3_UPG_VALUE            600
#define LEVEL3_UPG_HEALTH           250
#define LEVEL3_UPG_REGEN            7
#define LEVEL3_UPG_COST             1

#define LEVEL4_SPEED                1.2f
#define LEVEL4_VALUE                800
#define LEVEL4_HEALTH               400
#define LEVEL4_REGEN                7
#define LEVEL4_COST                 2



/*
 * ALIEN buildables
 *
 * _BP            - build points required for this buildable
 * _BT            - build time required for this buildable
 * _REGEN         - the amount of health per second regained
 * _SPLASHDAMGE   - the amount of damage caused by this buildable when melting
 * _SPLASHRADIUS  - the radius around which it does this damage
 *
 * CREEP_BASESIZE - the maximum distance a buildable can be from an egg/overmind
 *
 */

#define CREEP_BASESIZE              700
#define CREEP_TIMEOUT               1000
#define CREEP_MODIFIER              0.5f
#define CREEP_ARMOUR_MODIFIER       0.75f
#define CREEP_SCALEDOWN_TIME        3000

#define ASPAWN_BP                   10
#define ASPAWN_BT                   1
#define ASPAWN_HEALTH               1000
#define ASPAWN_REGEN                8
#define ASPAWN_SPLASHDAMAGE         50
#define ASPAWN_SPLASHRADIUS         50
#define ASPAWN_CREEPSIZE            120
#define ASPAWN_VALUE                150

#define BARRICADE_BP                10
#define BARRICADE_BT                1
#define BARRICADE_HEALTH            1500
#define BARRICADE_REGEN             14
#define BARRICADE_SPLASHDAMAGE      50
#define BARRICADE_SPLASHRADIUS      50
#define BARRICADE_CREEPSIZE         120

#define BOOSTER_BP                  12
#define BOOSTER_BT                  1
#define BOOSTER_HEALTH              500
#define BOOSTER_REGEN               8
#define BOOSTER_SPLASHDAMAGE        50
#define BOOSTER_SPLASHRADIUS        50
#define BOOSTER_CREEPSIZE           120
#define BOOSTER_INTERVAL            30000 //time in msec between uses (per player)
#define BOOSTER_REGEN_MOD           2.0f
#define BOOST_TIME                  30000

#define ACIDTUBE_BP                 8
#define ACIDTUBE_BT                 1
#define ACIDTUBE_HEALTH             700
#define ACIDTUBE_REGEN              20
#define ACIDTUBE_SPLASHDAMAGE       20
#define ACIDTUBE_SPLASHRADIUS       300
#define ACIDTUBE_CREEPSIZE          120
#define ACIDTUBE_RANGE              600.0f
#define ACIDTUBE_REPEAT             3000
#define ACIDTUBE_K_SCALE            1.0f

#define HIVE_BP                     12
#define HIVE_BT                     1
#define HIVE_HEALTH                 800
#define HIVE_REGEN                  10
#define HIVE_SPLASHDAMAGE           30
#define HIVE_SPLASHRADIUS           200
#define HIVE_CREEPSIZE              120
#define HIVE_RANGE                  400.0f
#define HIVE_REPEAT                 5000
#define HIVE_K_SCALE                1.0f
#define HIVE_DMG                    50
#define HIVE_SPEED                  240.0f
#define HIVE_DIR_CHANGE_PERIOD      500

#define TRAPPER_BP                  8
#define TRAPPER_BT                  1
#define TRAPPER_HEALTH              300
#define TRAPPER_REGEN               6
#define TRAPPER_SPLASHDAMAGE        15
#define TRAPPER_SPLASHRADIUS        100
#define TRAPPER_CREEPSIZE           30
#define TRAPPER_RANGE               400
#define TRAPPER_REPEAT              1000
#define TRAPPER_K_SCALE             1.0f
#define LOCKBLOB_SPEED              650.0f
#define LOCKBLOB_LOCKTIME           5000
#define LOCKBLOB_DOT                0.85f // max angle = acos( LOCKBLOB_DOT )
#define LOCKBLOB_K_SCALE            1.0f

#define OVERMIND_BP                 0
#define OVERMIND_BT                 1
#define OVERMIND_HEALTH             2000
#define OVERMIND_REGEN              6
#define OVERMIND_SPLASHDAMAGE       15
#define OVERMIND_SPLASHRADIUS       300
#define OVERMIND_CREEPSIZE          120
#define OVERMIND_ATTACK_RANGE       150.0f
#define OVERMIND_ATTACK_REPEAT      1000
#define OVERMIND_VALUE              300

#define HOVEL_BP                    0
#define HOVEL_BT                    1
#define HOVEL_HEALTH                1500
#define HOVEL_REGEN                 20
#define HOVEL_SPLASHDAMAGE          20
#define HOVEL_SPLASHRADIUS          200
#define HOVEL_CREEPSIZE             120



/*
 * ALIEN misc
 *
 * ALIENSENSE_RANGE - the distance alien sense is useful for
 *
 */

#define ALIENSENSE_RANGE            1000.0f

#define ALIEN_POISON_TIME           5000
#define ALIEN_POISON_DMG            5
#define ALIEN_POISON_DIVIDER        (1.0f/1.32f) //about 1.0/(time`th root of damage)

#define ALIEN_SPAWN_REPEAT_TIME     1000 //10000

#define ALIEN_REGEN_DAMAGE_TIME     2000 //msec since damage that regen starts again

/*
 * HUMAN weapons
 *
 * _REPEAT  - time between firings
 * _RELOAD  - time needed to reload
 * _PRICE   - amount in credits weapon costs
 *
 */

#define UNLIMITED_AMMO		    0

#define BLASTER_REPEAT              500 //600
#define BLASTER_K_SCALE             1.0f
#define BLASTER_SPREAD              200
#define BLASTER_SPEED               1400 //disabled
#define BLASTER_DMG                 10 //9

#define RIFLE_CLIPSIZE              30
#define RIFLE_MAXCLIPS              5
#define RIFLE_REPEAT                90 //90
#define RIFLE_K_SCALE               1.0f
#define RIFLE_RELOAD                2000
#define RIFLE_PRICE                 0
#define RIFLE_SPREAD                200
#define RIFLE_DMG                   6 //5

#define MEDGUN_PRICE                0
#define MEDGUN_AMMO                 500
#define MEDGUN_REPEAT               40
#define MEDGUN_HEALRATE             2

#define PAINSAW_PRICE               100
#define PAINSAW_REPEAT              50
#define PAINSAW_K_SCALE             1.0f
#define PAINSAW_DAMAGE              15
#define PAINSAW_RANGE               60.0f

#define GRENADE_PRICE               500
#define GRENADE_REPEAT              0
#define GRENADE_K_SCALE             1.0f
#define GRENADE_DAMAGE              10000
#define GRENADE_RANGE               200.0f
#define GRENADE_SPEED               400.0f

#define GRENADE_SHRAPNEL_COUNT      10
#define GRENADE_SHRAPNEL_LIFETIME   2000
#define GRENADE_SHRAPNEL_MAG        1000
#define GRENADE_SHRAPNEL_DAMAGE     100

#define SHOTGUN_PRICE               150
#define SHOTGUN_SHELLS              15
#define SHOTGUN_PELLETS             8 //used to sync server and client side
#define SHOTGUN_MAXCLIPS            6
#define SHOTGUN_REPEAT              500
#define SHOTGUN_K_SCALE             1.0f
#define SHOTGUN_RELOAD              1000
#define SHOTGUN_SPREAD              500
#define SHOTGUN_DMG                 6

#define LASGUN_PRICE                250
#define LASGUN_AMMO                 250
#define LASGUN_REPEAT               200
#define LASGUN_K_SCALE              1.0f
#define LASGUN_RELOAD               2000
#define LASGUN_DAMAGE               13

#define MDRIVER_PRICE               350
#define MDRIVER_CLIPSIZE            7 //5
#define MDRIVER_MAXCLIPS            6 //4
#define MDRIVER_DMG                 75 //38
#define MDRIVER_REPEAT              1000
#define MDRIVER_K_SCALE             1.0f
#define MDRIVER_RELOAD              2000

#define CHAINGUN_PRICE              400
#define CHAINGUN_BULLETS            300
#define CHAINGUN_REPEAT             90
#define CHAINGUN_K_SCALE            2.0f //1.0f
#define CHAINGUN_SPREAD             1000 //1000
#define CHAINGUN_DMG                7 //6

#define MINIGUN_PRICE               500
#define MINIGUN_BULLETS             400
#define MINIGUN_REPEAT              70
#define MINIGUN_K_SCALE             2.0f //1.0f
#define MINIGUN_SPREAD              500 //1000
#define MINIGUN_DMG                 8 //6

#define PRIFLE_PRICE                400
#define PRIFLE_CLIPS                75
#define PRIFLE_MAXCLIPS             5
#define PRIFLE_REPEAT               100
#define PRIFLE_K_SCALE              1.0f
#define PRIFLE_RELOAD               2000
#define PRIFLE_DMG                  9
#define PRIFLE_SPEED                750

#define FLAMER_PRICE                450
#define FLAMER_GAS                  200
#define FLAMER_REPEAT               200
#define FLAMER_K_SCALE              1.0f
#define FLAMER_DMG                  20
#define FLAMER_RADIUS               50
#define FLAMER_LIFETIME             800.0f
#define FLAMER_SPEED                200.0f
#define FLAMER_LAG                  0.65f  //the amount of player velocity that is added to the fireball

#define LCANNON_PRICE               600
#define LCANNON_AMMO                200
#define LCANNON_REPEAT              500
#define LCANNON_K_SCALE             1.0f
#define LCANNON_CHARGEREPEAT        1000
#define LCANNON_RELOAD              1500
#define LCANNON_DAMAGE              300
#define LCANNON_RADIUS              300
#define LCANNON_SECONDARY_DAMAGE    25
#define LCANNON_SECONDARY_RADIUS    150
#define LCANNON_SPEED               200
#define LCANNON_CHARGE_TIME         1750
#define LCANNON_TOTAL_CHARGE        200
#define LCANNON_MIN_CHARGE          25

#define HBUILD_PRICE                0
#define HBUILD_REPEAT               300
#define HBUILD_DELAY                1000
#define HBUILD_HEALRATE             18

#define HBUILD2_PRICE               0
#define HBUILD2_REPEAT              150 //1000
#define HBUILD2_DELAY               500



/*
 * HUMAN upgrades
 */

#define LIGHTARMOUR_PRICE           70
#define LIGHTARMOUR_POISON_PROTECTION 1

#define HELMET_PRICE                90
#define HELMET_RANGE                1000.0f
#define HELMET_POISON_PROTECTION    2 

#define MEDKIT_PRICE                0

#define BATTPACK_PRICE              100
#define BATTPACK_MODIFIER           1.5f //modifier for extra energy storage available

#define JETPACK_PRICE               120
#define JETPACK_FLOAT_SPEED         128.0f //up movement speed
#define JETPACK_SINK_SPEED          192.0f //down movement speed
#define JETPACK_DISABLE_TIME        1000 //time to disable the jetpack when player damaged
#define JETPACK_DISABLE_CHANCE      0.3f

#define BSUIT_PRICE                 400
#define BSUIT_POISON_PROTECTION     4

#define MGCLIP_PRICE                0

#define CGAMMO_PRICE                0

#define GAS_PRICE                   0

#define MEDKIT_POISON_IMMUNITY_TIME 0
#define MEDKIT_STARTUP_TIME         4000
#define MEDKIT_STARTUP_SPEED        5


/*
 * HUMAN buildables
 *
 * _BP            - build points required for this buildable
 * _BT            - build time required for this buildable
 * _SPLASHDAMGE   - the amount of damage caused by this buildable when it blows up
 * _SPLASHRADIUS  - the radius around which it does this damage
 *
 * REACTOR_BASESIZE - the maximum distance a buildable can be from an reactor
 * REPEATER_BASESIZE - the maximum distance a buildable can be from a repeater
 *
 */

#define REACTOR_BASESIZE            1000
#define REPEATER_BASESIZE           500
#define HUMAN_DETONATION_DELAY      100 //5000

#define HSPAWN_BP                   10
#define HSPAWN_BT                   10000
#define HSPAWN_HEALTH               310
#define HSPAWN_SPLASHDAMAGE         50
#define HSPAWN_SPLASHRADIUS         100
#define HSPAWN_VALUE                1

#define MEDISTAT_BP                 8
#define MEDISTAT_BT                 10000
#define MEDISTAT_HEALTH             190
#define MEDISTAT_SPLASHDAMAGE       50
#define MEDISTAT_SPLASHRADIUS       100

#define MGTURRET_BP                 8
#define MGTURRET_BT                 10000
#define MGTURRET_HEALTH             200
#define MGTURRET_SPLASHDAMAGE       100
#define MGTURRET_SPLASHRADIUS       100
#define MGTURRET_ANGULARSPEED       8  //degrees/think ~= 200deg/sec
#define MGTURRET_ACCURACYTOLERANCE  MGTURRET_ANGULARSPEED / 1.5f //angular difference for turret to fire
#define MGTURRET_VERTICALCAP        30  // +/- maximum pitch
#define MGTURRET_REPEAT             200
#define MGTURRET_K_SCALE            1.0f
#define MGTURRET_RANGE              350.0f
#define MGTURRET_SPREAD             200
#define MGTURRET_DMG                5 //4
#define MGTURRET_DCC_ANGULARSPEED       10
#define MGTURRET_DCC_ACCURACYTOLERANCE  MGTURRET_DCC_ANGULARSPEED / 1.5f
#define MGTURRET_GRAB_ANGULARSPEED      3
#define MGTURRET_GRAB_ACCURACYTOLERANCE MGTURRET_GRAB_ANGULARSPEED / 1.5f

#define TESLAGEN_BP                 10
#define TESLAGEN_BT                 15000
#define TESLAGEN_HEALTH             220
#define TESLAGEN_SPLASHDAMAGE       50
#define TESLAGEN_SPLASHRADIUS       100
#define TESLAGEN_REPEAT             200
#define TESLAGEN_K_SCALE            2.0f
#define TESLAGEN_RANGE              300 //250
#define TESLAGEN_DMG                16

#define DC_BP                       8
#define DC_BT                       10000
#define DC_HEALTH                   190
#define DC_SPLASHDAMAGE             50
#define DC_SPLASHRADIUS             100

#define ARMOURY_BP                  10
#define ARMOURY_BT                  10000
#define ARMOURY_HEALTH              280
#define ARMOURY_SPLASHDAMAGE        50
#define ARMOURY_SPLASHRADIUS        100

#define REACTOR_BP                  0
#define REACTOR_BT                  20000
#define REACTOR_HEALTH              930
#define REACTOR_SPLASHDAMAGE        200
#define REACTOR_SPLASHRADIUS        300
#define REACTOR_ATTACK_RANGE        200.0f //100.0f
#define REACTOR_ATTACK_REPEAT       1000
#define REACTOR_ATTACK_DAMAGE       100
#define REACTOR_VALUE               2

#define REPEATER_BP                 0
#define REPEATER_BT                 10000
#define REPEATER_HEALTH             500 //250
#define REPEATER_SPLASHDAMAGE       300 //50
#define REPEATER_SPLASHRADIUS       500 //100
#define REPEATER_INACTIVE_TIME      90000 //not in use

/*
 * HUMAN misc
 */

#define HUMAN_SPRINT_MODIFIER       1.2f
#define HUMAN_JOG_MODIFIER          1.0f
#define HUMAN_BACK_MODIFIER         0.8f
#define HUMAN_SIDE_MODIFIER         0.9f

#define STAMINA_STOP_RESTORE        50
#define STAMINA_WALK_RESTORE        30
#define STAMINA_SPRINT_TAKE         8
#define STAMINA_LARMOUR_TAKE        4

#define HUMAN_SPAWN_REPEAT_TIME     1000 //10000

/*
 * Misc
 */

#define MIN_FALL_DISTANCE           60.0f //the fall distance at which fall damage kicks in
#define MAX_FALL_DISTANCE           150.0f //the fall distance at which maximum damage is dealt
#define AVG_FALL_DISTANCE           ((MIN_FALL_DISTANCE+MAX_FALL_DISTANCE)/2.0f)

#define HUMAN_MAXED                 1500   //a human with a strong selection of weapons/upgrades
#define HUMAN_MAX_CREDITS           9999 //2000
#define ALIEN_MAX_KILLS             9
#define ALIEN_MAX_SINGLE_KILLS      3

#define FREEKILL_PERIOD             120000 //msec
#define FREEKILL_ALIEN              1
#define FREEKILL_HUMAN              LEVEL0_VALUE

#define DEFAULT_ALIEN_BUILDPOINTS   "500"
#define DEFAULT_ALIEN_STAGE2_THRESH "0"
#define DEFAULT_ALIEN_STAGE3_THRESH "0"
#define DEFAULT_ALIEN_MAX_STAGE     "2"
#define DEFAULT_HUMAN_BUILDPOINTS   "180"
#define DEFAULT_HUMAN_STAGE2_THRESH "30"
#define DEFAULT_HUMAN_STAGE3_THRESH "60"
#define DEFAULT_HUMAN_MAX_STAGE     "2"

#define DAMAGE_FRACTION_FOR_KILL    0.5f //how much damage players (versus structures) need to
                                         //do to increment the stage kill counters

// g_suddenDeathMode settings
#define SDMODE_BP                   0 
#define SDMODE_NO_BUILD             1
#define SDMODE_SELECTIVE            2

// !buy functions
#define AMMO_COST                   400
#define BUBBLES_COST                500
#define DOUBLEAMMO_COST             2000
#define DOUBLEDMG_COST              3000
#define HEALTH_COST                 600
#define HYPERSPEED_COST             1500
#define REGEN_COST                  800
#define DOUBLEREGEN_COST            1500
