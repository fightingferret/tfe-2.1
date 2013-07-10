#include "ctype.h"
#include "sys/types.h"
#include "stdio.h"
#include "stdlib.h"
#include "define.h"
#include "struct.h"

bool   can_study   ( char_data*, int );


const char* skill_cat_name [ MAX_SKILL_CAT ] = { "weapon", "language",
  "spell", "physical", "trade" };


/*
 *   CURRENTLY UNUSED
 */


char* abil_name( int i )
{
  if( i <  0 ) return " (unknown) ";
  if( i <  1 ) return "(appalling)";
  if( i <  5 ) return "(dreadful) ";
  if( i < 10 ) return "  (awful)  ";
  if( i < 20 ) return "   (bad)   ";
  if( i < 30 ) return "  (poor)   ";
  if( i < 50 ) return "  (fair)   ";
  if( i < 60 ) return " (capable) ";
  if( i < 70 ) return "  (good)   ";
  if( i < 80 ) return "(very good)";
  if( i < 90 ) return " (superb)  ";
               return " (wicked)  ";
}


/*
 *   ABILITY ROUTINE
 */


void do_skills( char_data* ch, char* )
{
  send( ch, "For information on how to get a skill list type help\
 abilities.\n\r" );

  return;
}

/* Outputs the current players skills */

void do_abilities( char_data* ch, char* argument )
{
  char        buf  [ MAX_INPUT_LENGTH ];
  char        arg  [ MAX_INPUT_LENGTH ];
  int        i, j;
  int    ps1, ps2;
  int        cost;
  int      length;
  int        clss;
  bool      found  = FALSE;

  if( not_player( ch ) )
    return;

  /* No arguement given to abil. 
   * Output the help
   */
  if( *argument == '\0' ) {
    send( ch, "Syntax: abil <cat> (optional class)\n\r" );
    send( ch, "Where <cat> is one of Phy, Wea, Spe, or Lan.\n\r" );
    send( ch, "See help abilities for more detail.\n\r" );
    return;
    }

  argument = one_argument( argument, arg );

  /* What class is the player? */
  clss = ch->pcdata->clss;

  /* If not looking for the skills available
   * for a specific class then print current players skills
   */
  if( *argument == '\0' ) {
    length = strlen( arg );
    for( j = 0; j < MAX_SKILL_CAT; j++ ) {
      if( !strncasecmp( skill_cat_name[j], arg, length ) ) {
       for( i = 0; i < MAX_SKILL; i++ ) {
          if( skill_table[i].category != j 
            || ( cost = skill_table[i].prac_cost[clss] ) == -1
            || skill_table[i].level[clss] > ch->shdata->level )
            continue;       
          if( !found ) {
            page_underlined( ch,
              "Skill             Level   Cost   Pracs    Prerequistes\n\r" );
            found = TRUE;
	    }
          if( ch->shdata->skill[i] == 0 ) 
            sprintf( arg, " unk" );
          else
            sprintf( arg, "%d ", ch->shdata->skill[i] );
          sprintf( buf, "%-18s%4s", skill_table[i].name, arg );
          ps1 = skill_table[i].pre_skill[0];
          ps2 = skill_table[i].pre_skill[1];
          if( ch->shdata->skill[i] != 10 )
            sprintf( buf+strlen(buf), "%8d%6d%6s",
              ch->shdata->level*cost*cost, cost, "" );
          else
            sprintf( buf+strlen(buf), "%20s", "" );
          if( ps1 != SKILL_NONE )
            sprintf( buf+strlen(buf), "%s [%d]", skill_table[ps1].name, 
              skill_table[i].pre_level[0] );
          if( ps2 != SKILL_NONE ) 
            sprintf( buf+strlen(buf), " & %s [%d]", skill_table[ps2].name, 
              skill_table[i].pre_level[1] );
          sprintf( buf+strlen(buf), "\n\r" );
          page( ch, buf );
          }
        if( !found ) 
          send( ch, "You have no %s skills.\n\r", skill_cat_name[j] );
        return;
        }
      }
    send( ch, "Unknown skill category.\n\r" );
    return;
    }

  /* Something was after abil <type> in the command line.
   * Looking for skills for a specific class...
   * i.e. abil <type> <class>
   */

  length = strlen( argument ); 

  /* Select the correct table for that class. */

  for( clss = 0; clss < MAX_CLSS; clss++ ) 
    if( !strncasecmp( argument, clss_table[clss].name, length ) ) 
      break;

  if( clss == MAX_CLSS ) {
    send( ch, "Unknown class.\n\r" );
    return;
    }

  /* Get the correct skill tables for that type.  
   * spell, physical, weapons, language.
   */

  length = strlen( arg );
  for( j = 0; j < MAX_SKILL_CAT; j++ ) {
    if( !strncasecmp ( skill_cat_name[j], arg, length ) ) {
      for( i = 0; i < MAX_SKILL; i++ ) {
        if( skill_table[i].category != j 
          || ( cost = skill_table[i].prac_cost[clss] ) == -1 )
          continue;   
        if( !found ) {
          page_underlined( ch,
            "Skill               Pracs  Level     Prerequistes\n\r" );
          found = TRUE;
	  }
        if( skill_table[i].level[clss] >= LEVEL_BUILDER ) {
          sprintf( buf, "%-20s<Unfinished>%5s", skill_table[i].name, "" );
	  }
        else {
          sprintf( buf, "%-18s%5d%7d%7s", skill_table[i].name, cost,
            skill_table[i].level[clss], "" );
	  }
 
        ps1 = skill_table[i].pre_skill[0];
        ps2 = skill_table[i].pre_skill[1];

        if( ps1 != SKILL_NONE )
          sprintf( buf+strlen(buf), "%s [%d]", skill_table[ps1].name, 
            skill_table[i].pre_level[0] );
        if( ps2 != SKILL_NONE ) 
          sprintf( buf+strlen(buf), " & %s [%d]", skill_table[ps2].name, 
            skill_table[i].pre_level[1] );
        sprintf( buf+strlen(buf), "\n\r" );
        page( ch, buf );
        }
      if( !found ) 
        send( ch, "The %s class has no %s skills.\n\r",
          clss_table[clss].name, skill_cat_name[j] ); 
      return;
      }
    }

  send( ch, "Unknown skill category.\n\r" );
  return;
}


/*
 *   PRACTICE ROUTINES
 */


void do_practice( char_data* ch, char* argument )
{ 
  char                 buf  [ MAX_STRING_LENGTH ];
  char                 arg  [ MAX_STRING_LENGTH ];
  mob_data*        trainer  = NULL;
  trainer_data*   pTrainer  = NULL;
  int                    i;
  int                pracs;

  /* So that mobs, NPC's etc cannot attempt to practise things. */

  if( ch->species != NULL ) {
    send( ch, "Only players can practice skills.\n\r" );
    return;
    }
  
  /* Dunno what ch->array points to,
   * but this determines that there is someone in the room that can
   * train people.
   */  

  for( i = 0; ; i++ ) {
    if( i == *ch->array ) {
      send( ch, "There is no one here who can train you.\n\r" );
      return;
      }
    /* Find the first available trainer in the room
     * that the player can see and that can train that player
     */
    if( ( trainer = mob( ch->array->list[i] ) ) != NULL
      && ( pTrainer = trainer->pTrainer ) != NULL
      && trainer->Seen( ch ) )
      break;
    }

  *buf = '\0';

  /* Display the skills that can be taught to this player. */

  if( *argument == '\0' ) {
    for( i = 0; i < MAX_SKILL; i++ ) {
      if( !is_set( pTrainer->skill, i )
        || skill_table[i].prac_cost[ch->pcdata->clss] == -1 ) 
        continue;

      if( *buf == '\0' ) {
        sprintf( buf, "Practice Pnts: %d    Copper Pieces: %d\n\r\n\r",
          ch->pcdata->practice, get_money( ch ) );
        sprintf( buf+strlen( buf ),
          "Skill             Level       Cost       Prac\n\r" );
        sprintf( buf+strlen( buf ),
          "-----             -----       ----       ----\n\r" );
        }

      pracs = skill_table[i].prac_cost[ch->pcdata->clss];
      if( ch->shdata->skill[i] == 0 )
        sprintf( arg, "%s", "unk" );
      else
        sprintf( arg, "%d ", ch->shdata->skill[i] );
   
      sprintf( buf+strlen( buf ), "%-15s%7s",
        skill_table[i].name, arg );

      if( ch->shdata->skill[i] == 10 ) 
        strcat( buf, "\n\r" ); 
      else
        sprintf( buf+strlen( buf ), "%12d%10d\n\r",
          ch->shdata->level*pracs*pracs, pracs );
      }
    if( *buf == '\0' ) 
      process_tell( trainer, ch, "Sorry, there is nothing I can teach you" );
    else
      send( ch, buf );
    return;
    }

  /* Can you still buy that skill up more? */
  
  for( i = 0; i < MAX_SKILL; i++ ) {
    if( !is_set( pTrainer->skill, i )
      || skill_table[i].prac_cost[ch->pcdata->clss] == -1 
      || !fmatches( argument, skill_table[i].name ) )
      continue;

    if( ch->shdata->skill[i] >= 7 ) {
      fsend( ch, "You know %s as well as you can be taught it.  It\
 can only be improved by practice now.\n\r", skill_table[i].name );
      return;
      }   

    pracs = skill_table[i].prac_cost[ch->pcdata->clss];

    /* Can you learn this skill? */

    if( !can_study( ch, i ) )
      return;

    /* Do you have the practise points needed? */

    if( ch->pcdata->practice < pracs ) {
      fsend( ch, "%s tries to explain %s to you, but after a short time\
 tells you, you need to go out and practice your current skills more before\
 you can learn new ones.\n\r", trainer, skill_table[i].name ); 
      return;
      }

    /* Take the money and check for enough money. */

    sprintf( buf, "You hand %s", trainer->descr->name );
    if( !remove_coins( ch, pracs*pracs*ch->shdata->level, buf ) ) {
      process_tell( trainer, ch, "You are unable to afford my services." );
      ch->reply = trainer;
      return;
      }

    /* Take the practise points. */

    ch->pcdata->practice -= pracs;
    ch->shdata->skill[i]++;
    send( ch, "%s teaches you %s%s.\n\r",
      trainer, skill_table[i].name,
      ch->shdata->skill[i] == 0 ? "" : ", improving your ability at it" );
    send_seen( ch, "%s practices %s.\n\r", ch, skill_table[i].name );
    return;
    }

   /* Monkey tried to practise something that the trainer
    * does not know or player spelled it wrong. 
    */

  process_tell( trainer, ch, "I know of no such skill." );
}

/* Check all the prerequisites for the skill that the player is trying to 
 * learn.  The skill is passed in i. 
 */ 

bool can_study( char_data* ch, int i )
{
  char           tmp  [ TWO_LINES ];
  skill_type*  skill  = &skill_table[i];
  int             n1  = skill->pre_skill[0];
  int             n2  = skill->pre_skill[1];
  skill_type*    ps1;
  skill_type*    ps2;

  /* Do the prereq check on the skill. */
 
  ps1 = ( n1 == SKILL_NONE ? (skill_type *) NULL : &skill_table[n1] );
  ps2 = ( n2 == SKILL_NONE ? (skill_type *) NULL : &skill_table[n2] );

  /* Don't have the prereq. */

  if( ( ps1 != NULL && ch->shdata->skill[n1]
    < skill->pre_level[0] ) || ( ps2 != NULL
    && ch->shdata->skill[n2] < skill->pre_level[1] ) ) {
    sprintf( tmp, "Before you are prepared to study %s you need to know ",
      skill->name );
    if( ps2 == NULL ) 
      sprintf( tmp+strlen( tmp ), "%s at level %d.",
        ps1->name, skill->pre_level[0] );
    else
      sprintf( tmp+strlen( tmp ),
        "%s at level %d and %s at level %d.",
        ps1->name, skill->pre_level[0], ps2->name, skill->pre_level[1] );
    fsend( ch, tmp );
    return FALSE;
    }

  /* Do you meet the minimum level for that skill? */

  if( ch->shdata->level < skill->level[ ch->pcdata->clss ] ) {
    send( ch, "To study %s you must be at least level %d.\n\r",
      skill->name, skill->level[ ch->pcdata->clss ] );
    return FALSE;
    }  

  return TRUE;
}


/*
 *   PRACTICE POINTS
 */


/* Dunno -- come back to this later after it is used. */

int total_pracs( char_data* ch )
{
  int            clss  = ch->pcdata->clss;
  int           total  = 0;
  int            cost  = 0;
  int           skill;
  register int      i;

  for( i = 0; i < MAX_SKILL; i++ ) 
    if( ( skill = ch->shdata->skill[i] ) != 0 
      && ( cost = skill_table[i].prac_cost[clss] ) != -1 ) 
      total += cost*skill;

  total -= 20*skill_table[LANG_PRIMAL].prac_cost[clss];
  total += ch->pcdata->practice;

  return total;
}    


/* Does the calcs to see if you have improved at the skill you are using.
 * Uses total pracs to date, expected pracs to date, a prac_timer and
 * weights Intellegence and Wisdom equally.  Also takes into account
 * your level.  If the skill is already at 10 then award a practise point.
 */

void char_data :: improve_skill( int i )
{
  int j, total, norm;
 
 /* i is a valid skill, the players prac_timer has counted out, and the
  * player has the skill at least at 1. 
  */
  
  if( species != NULL || pcdata->prac_timer > 0
    || i < 0 || i > MAX_SKILL || shdata->skill[i] == 0 )
    return;

  /* Weapon, language or a skill that is being considered
   * for an improve.  If the timer just now counted below 0  
   * or the timer IS 0 then return.  If the timer is 0 then
   * set it to a random number between   -20 and -2.
   */

  if( i < SPELL_FIRST || i >= WEAPON_UNARMED ) {
    if( pcdata->prac_timer < 0 && ++pcdata->prac_timer != -1 ) 
      return;
    if( pcdata->prac_timer == 0 ) {
      pcdata->prac_timer = number_range( -20, -2 );
      return;
      }
    }

  /* nums for calculating if you get an improve.
   * norm --> dunno --> find expected pracs.
   * total --> total number of pracs that a player has (received/used?)
   * up to now.  
   */

  norm  = expected_pracs( this );
  total = total_pracs( this );
  j     = max( 0, 2*(total-norm) );

  /* reset the prac timer.*/   

  pcdata->prac_timer = 5;

  /* Using random numbers between 0 and 30+the above j,
   * comparisons to Intelligence and Wisdom are made.  Must be less
   * than both.  
   * Random between 0 and 200 must be greater than shdata->level...Dunno
   */ 

  if( number_range( 0, 30+j ) < Intelligence( )
    && number_range( 0, 30+j ) < Wisdom( )
    && number_range( 0, 200 ) > shdata->level ) {
    /* if the skill is already at 10 give a prac point. */
    if( shdata->skill[i] == 10 || number_range( 0, 2 ) != 0 ) {
      pcdata->practice++;
      send( this, "\n\r** You gain a practice point. **\n\r\n\r" );
      }
    else {
      /* otherwise improve. */
      shdata->skill[i]++;
      send( this, "\n\r** You improve at %s. **\n\r\n\r",
        skill_table[i].name );
      }
    }
  
  return;
}


