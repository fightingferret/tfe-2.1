#include <sys/types.h>
#include <stdio.h>
#include <syslog.h>
#include "define.h"
#include "struct.h"


int   disarm          ( char_data*, char_data* );

/*
 *   FIGHT SUB_ROUTINES
 */


void check_killer( char_data* ch, char_data* victim )
{
  if( ch->species != NULL || !is_enemy( victim, ch ) ) 
    return;
    
  add_enemy( victim, ch );
}


bool is_entangled( char_data* ch, const char* verb )
{
  if( !is_set( ch->affected_by, AFF_ENTANGLED ) )
    return FALSE;

  send( ch, "You are too entangled to %s anything.\n\r", verb );

  return TRUE;
}


/*
 *   SUPPORT SUBROUTINES
 */


char_data* get_victim( char_data* ch, char* argument, char* msg )
{
  char_data*    victim;
  char_data*  opponent;
  
  opponent = ch->fighting;

  if( *argument == '\0' ) 
    {
      if( opponent == NULL ) 
	return NULL;
      return opponent;
    }

  if( ( victim = one_character( ch, argument, msg, ch->array ) ) == NULL )
    {
      send( ch, "They aren't here.\n\r" );
      return NULL;
    }

  if( opponent != NULL && opponent != victim )
    {
      send( ch, "You are already fighting someone else.\n\r" );
      return NULL;
    }
  
  return victim;
}


/*
 *   DO_KILL FUNCTION
 */
    

void do_kill( char_data* ch, char* argument )
{
  char_data*    victim;
  char_data*  opponent;

  if( ( victim = one_character( ch, argument, "kill",
    ch->array ) ) == NULL )
    return;

  if( victim == ch ) {
    send( ch, "Typing quit is easier.\n\r" );
    return;
    }

  if( ch->mount != NULL && ch->Wearing( WEAR_HELD_R ) == NULL ) {
    send( ch, "You can't attack without a weapon while mounted.\n\r" );
    return;
    }

  if( ch->fighting == victim ) {
    send( ch, "You are already attacking %s!\n\r", victim );
    return;
    }

  if( !can_kill( ch, victim ) )
    return;

  check_killer( ch, victim );
  ch->fighting = victim;
  react_attack( ch, victim );

  remove_bit( &ch->status, STAT_WIMPY );
  leave_shadows( ch );

  fight_round( ch );
}


/*
 *   DISARM
 */


void do_disarm( char_data* ch, char* )
{
  char_data*    victim;
  obj_data*        obj;
  int          percent;

  if( is_confused_pet( ch ) )
    return;

  if( ch->pcdata != NULL 
    && ch->get_skill( SKILL_DISARM ) == UNLEARNT ) {
    send( ch, "You don't know how to disarm opponents.\n\r" );
    return;
    }

  if( ( victim = ch->fighting ) == NULL ) {
    send( "You aren't fighting anyone.\n\r", ch );
    return;
    }

  if( ch->Wearing( WEAR_HELD_R ) == NULL ) {
    send( ch, "You must wield a weapon to disarm.\n\r" );
    return;
    }

  if( ( obj = victim->Wearing( WEAR_HELD_R ) ) == NULL ) {
    send( "Your opponent is not wielding a weapon.\n\r", ch );
    return;
    }

  set_delay( ch, disarm( ch, victim ) );
  ch->improve_skill( SKILL_DISARM );
  
  return;
}


int disarm( char_data* ch, char_data* victim )
{
  
  obj_data* obj;

  if( ( obj = victim->Wearing( WEAR_HELD_R ) ) == NULL )
    return 0;
  
  if( is_set( obj->pIndexData->extra_flags, OFLAG_NO_DISARM ) ||
      !obj->droppable() )
    { 
      send( ch, "The weapon %s is wielding is impossible to disarm.\n\r",
	    victim->Name( ch ) );
      return 20;
    }

  if( ( (ch->get_skill( SKILL_DISARM )/3) + number_range( 0, 20 ) ) <  
      victim->shdata->dexterity )
    {
      send( ch, "Your disarm attempt failed.\n\r");
      return 20;
    }
  
  send( victim, "+++ %s disarms you! +++\n\r", ch );
  send( ch, "You disarm %s!\n\r", victim );
  send( *victim->array, "%s disarms %s!\n\r", ch, victim );
  
  if( ( obj = object( obj->From( obj->selected ) ) ) != NULL )
    {
      set_owner( obj, NULL, victim );
      obj->To( victim->array );
    }

  return 32;
}


/*
 *   KICK, PUNCH, BITE ROUTINES
 */


void do_punch( char_data* ch, char* argument )
{
  char_data*     victim;

  if(  is_confused_pet( ch )
    || !is_humanoid( ch )
    || is_mounted( ch )
    || is_entangled( ch, "punch" ) ) 
    return;

  
  if( ( victim = get_victim( ch, argument, "Punch" ) )
      == NULL )
    return;

  if( victim == ch ) {
    send( ch, "Punching yourself is pointless.\n\r" );
    return;
    }

  if( ch->get_skill( SKILL_PUNCH ) == UNLEARNT )
    {
      send( ch, "You are untrained in the art of punching.\n\r" );
      return;
    }

  if( !can_kill( ch, victim ) )
    return;

  check_killer( ch, victim );
  ch->fighting = victim;
  react_attack( ch, victim );

  remove_bit( &ch->status, STAT_WIMPY );
  leave_shadows( ch );

  ch->improve_skill( SKILL_PUNCH );
  
  attack( ch, victim, "punch", NULL, roll_dice( 1,4 ), 0 );

  set_delay( ch, 20 );

  return;
}


void do_bite( char_data* ch, char* argument )
{
  char_data*     victim;

  if(  is_confused_pet( ch )
    || is_mounted( ch )
    || is_entangled( ch, "bite" ) ) 
    return;

  if( argument[0] == '\0' )
    if( (victim = ch->fighting) == NULL )
      {
	send( ch, "Who do you want to bite?" );
	return;
      }
  else
    if( ( victim = get_victim( ch, argument, "Bite" ) ) == NULL )
      return;

  if( victim == ch ) 
    {
      send( ch, "Biting yourself is pointless.\n\r" );
      return;
    }

  if(  ch->shdata->race != RACE_LIZARD
    && ch->shdata->race != RACE_TROLL
    && ch->shdata->race != RACE_GOBLIN )
    {
      send( ch, "Your teeth are not long or sharp enough for biting to be an\
effective\n\rattack.\n\r" );
      return;
    }
  
  if( !can_kill( ch, victim ) )
    return;

  check_killer( ch, victim );
  react_attack( ch, victim );
  ch->fighting = victim;

  remove_bit( &ch->status, STAT_WIMPY );
  leave_shadows( ch );

  attack( ch, victim, "bite", NULL, roll_dice( 2, 4 ), 0 );
  set_delay( ch, 20 );

  return;
}


/*
 *   SPIN KICK ROUTINE
 */



void do_spin_kick( char_data* ch, char* arg)
{
  char_data*       rch;
  char_data*  rch_next;
  bool           found  = FALSE;

  if( is_confused_pet( ch ) || is_mounted( ch )
    || !is_humanoid( ch ) )
    return;

  
  if( ch->get_skill( SKILL_SPIN_KICK ) == UNLEARNT )
    {
      send( "You are untrained in that skill.\n\r", ch );
      return;
    }
  
  
  for( int i = 0; i < ch->array->size; i++ )
    {
      if( ( rch = character( ch->array->list[i] ) ) != NULL
	  && ( rch->fighting == ch || ch->fighting == rch ) )
	{
	  if( !found )
	    {
	      found = TRUE;
	      send( ch, "You leap in the air, spinning rapidly.\n\r" );
	      send_seen( ch,
			 "%s leaps into the air, becoming a deadly blur.", 
			 ch );
	    }
	  attack( ch, rch, "spin kick", NULL, 
		  roll_dice( 5, 10 + ch->get_skill( SKILL_SPIN_KICK ) ), 0);
	}
    }
  
  if( !found )
    {
      send( "You are not fighting anyone!\n\r", ch );
      return;
    }

  remove_bit( &ch->status, STAT_WIMPY );

  ch->improve_skill( SKILL_SPIN_KICK );
 
  set_delay( ch, 25 );

  return;
}


void do_melee( char_data* ch, char* arg )
{
  char_data*       rch;
  char_data*  rch_next;
  bool           found  = FALSE;
  
  if( is_confused_pet( ch ) || is_mounted( ch )
    || !is_humanoid( ch ) )
    return;

  if( ch->get_skill( SKILL_MELEE ) == UNLEARNT )
    {
      send( ch, "You are untrained in that skill.\n\r" );
      return;
    }
  


  for( int i = 0; i < ch->array->size; i++ )
    {
      if( ( rch = character( ch->array->list[i] ) ) != NULL
	  && ( rch->fighting == ch || ch->fighting == rch ) )
	{
	  if( !found )
	    {
	      found = TRUE;
	      send( ch, "You spin about, striking at all your foes!!\n\r" );
	      send_seen( ch, "%s spins, striking out at all %s foes!!\n\r",  
			 ch, ch->Him_Her( ) ); 
	    }
	  attack( ch, rch, "MELEE", NULL, 
		  roll_dice( 5, 8+ch->shdata->skill[SKILL_MELEE] ), 0 );
	}
    }
  
  if( !found )
    {
      send( "You are not fighting anyone!\n\r", ch );
      return;
    }

  remove_bit( &ch->status, STAT_WIMPY );

  ch->improve_skill( SKILL_MELEE );

  set_delay( ch, 30 );
  
  return;
}







