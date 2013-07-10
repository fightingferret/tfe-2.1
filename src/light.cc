#include "math.h"
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include "define.h"
#include "struct.h"


/*
 *   SUNLIGHT
 */


const char* light_name( int i )
{
  const char* adj [] = { "Pitch Dark", "Extremely Dark", "Dark",
    "Dimly Lit", "Lit", "Well Lit", "Brightly Lit", "Blinding" };
  
  if( i >= 45 ) {i = 7;}
  else if( i >= 31  ) {i = 6;}
  else if( i >= 21  ) {i = 5;}
  else if( i >= 5   ) {i = 4;}
  else if( i >= 1   ) {i = 3;}
  else if( i >= -10 ) {i = 2;}
  else if( i >= -20 ) {i = 1;}
  else i = 0;
  
  return adj[i];
}


int sunlight( int time )
{
  time /= 60;
  
  if( time >= 20 ) {return 0;}
  else if( time >= 17 ) {return (4+(17-time));}
  else if( time >= 8  ) {return 21;}
  else if( time >= 5  ) {return (5+time);}
  else return 0;

}


/*
 *   ILLUMINATION
 */


int Thing_Data  :: Light( int )  { return 0; }
int Char_Data   :: Light( int )  
{
  if( species != NULL )
    return ( wearing.light + species->light );
  return wearing.light;
}


int Obj_Data :: Light( int n )
{
  int i  = pIndexData->light;
  
  i = ( ( i > 0 ) ? 1 : ( (i < 0) ? -1 : 0 ) );
  
  //i *= ( n == -1 ? number : n );

  return i; 
}   


int Room_Data :: Light( int ignore_me ) 
{
  int i = contents.light + ( is_set( &room_flags, RFLAG_LIT ) ? 10 : 0 );
  char_data* ch;
 
  if( !is_set( &room_flags, RFLAG_INDOORS ) 
      && !is_set( &room_flags, RFLAG_UNDERGROUND ) ) 
    {
      i += weather.sunlight;
      i = max( -10, i );
      i = min( 30, i );
    }
  else if( size > SIZE_GIANT )
    if( i > 3 ) { i -= 3; }
    else if( i < -3) { i += 3; }
  
  return i;
} 

void extinquish_light( char_data* ch )
{
  obj_data* obj;

  for( int i = 0; i < ch->wearing; i++ )
    {
      if( (obj = object( ch->wearing[i] ) ) != NULL 
	  && obj->pIndexData->item_type == ITEM_LIGHT
	  && !is_set( obj->pIndexData->extra_flags, OFLAG_WATER_PROOF ) )
	{
	  send( ch, "%s slips from your hands.\n\r", obj );
	  send_seen( ch, "%s slips from %s's hands.\n\r", obj, ch );
	  obj->value[2] /= 2;
	  obj->From();
	  obj->To( &ch->contents );
	}

    }
}
/*
 *   SPELLS
 */


bool spell_create_light( char_data* ch, char_data*, void*,
  int level, int )
{
  obj_data*     light;
  event_data*   event;
  int           cnt = 0;
  obj_data*       obj;

  if( null_caster( ch, SPELL_CREATE_LIGHT ) )
    return TRUE;
  
  light = create( get_obj_index( OBJ_BALL_OF_LIGHT ) );

  for( int i = 0; i < ch->wearing; i++ )
    if( (obj = object( ch->wearing[i] )) != NULL &&
	obj->pIndexData == light->pIndexData )
      cnt++;

  if( cnt >= 5 )
    {
      send( ch, "%s appears, but gets absorbed by the others.", light);
      delete light;
    }
  else
    {
      send( ch, "%s appears floating near you.\n\r", light );
      send( *ch->array, "%s appears floating near %s.\n\r", light, ch );
      
      light->position = WEAR_FLOATING;
      light->layer    = 0;
      light->To( &ch->wearing );
 
    }

  return TRUE;    
}


bool spell_continual_light( char_data *ch, char_data *victim, void*,
  int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_CONTINUAL_LIGHT,
    AFF_CONTINUAL_LIGHT );

  return TRUE;
}


