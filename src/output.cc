#include "ctype.h"
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "define.h"
#include "struct.h"


const char* He_She          [] = { "it",  "he",  "she" };
const char* Him_Her         [] = { "it",  "him", "her" };
const char* His_Her         [] = { "its", "his", "her" };


const char* scroll_line[] =
{
   "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\
=-=-=-=-\n\r",
   ">>>>==--------====--<>--====--------==<<<<\n\r",
   "+=-------------------------------------------------------------------\
-------=+\n\r"
};


/*
 *   LOW LEVEL ROUTINES
 */


void send( link_data* link, const char* message )
{
  text_data* text;

  if( link != NULL ) {
    text = new text_data( message );
    append( link->send, text );
    if( link->snoop_by != NULL )
      send( link->snoop_by, message );
    }

  return;
}


/*
 *   ACCEPT MESSAGE?
 */


bool Player_Data :: Accept_Msg( char_data* ch )
{
  if( msg_type == MSG_STANDARD )
    return TRUE;

  int level = level_setting( &pcdata->mess_settings, msg_type );

  if( level == 3 )
    return TRUE;

  if( level == 0 ) 
    return FALSE;

  if( Befriended( ch ) )
    return TRUE;

  if( level == 1 )
    return FALSE;

  return is_same_group( this, ch );
}


bool Mob_Data :: Accept_Msg( char_data* )
{
  return FALSE;
}


/* 
 *   PAGER ROUTINES
 */


/* This function takes a character and a text string as input.  It parses
   the string creating a new text_data item that is appended to the 
   end of of the character's link->paged list for output */
void page( char_data* ch, const char* txt )
{
  char          tmp  [ MAX_STRING_LENGTH ];
  text_data*   send;
  link_data*   link;
  char*        line;

  /* Set link to point at the characters link.  If the character doesn't
     exist, or the link doesn't exist, return. */
  if( ch == NULL || ( link = ch->link ) == NULL )
    return;

  
  /* For loop parses through the inputed string, until end of line or end
     of string.  If it hits end of string, it puts an end of line character
     before the end of string.  If it hits end of line, it skips the 
     carriage return and line feed, and continues parsing the string. */
  for( ; ; ) {
    /* Copy txt to line (which points to the front of the tmp buffer, until
       end of string or new line. */
    for( line = tmp; *txt != '\n' && *txt != '\0'; )
      *(line++) = *(txt++);

    /* If hit end of string, put a newline and carriage return at the end
       of line */
    if( *txt != '\0' ) {
      *(line++) = '\n';
      *(line++) = '\r';
      }

    /* Terminate the line string */
    *line     = '\0';

    /* If the text pointer is at the end of line, skip the newline
       and carriage return. */ 
    if( *txt == '\n' )
      if( *(++txt) == '\r' )
        txt++;
    
    /* If the tmp buffer doesn't exist, then don't send it to the character */
    if( *tmp == '\0' )
      break;

    /* Create a new text_data item with tmp as the string, and append it to 
       the link->paged list (for output) */
    send = new text_data( tmp );
    append( link->paged, send );
    } 

  return;
}


void fpage( char_data* ch, const char *text )
{
  char buf [ MAX_STRING_LENGTH ];

  if( ch == NULL || ch->link == NULL )
    return;

  format( buf, text );
  strcat( buf, "\n\r" );
  corrupt( buf, MAX_STRING_LENGTH, "Fpage" );

  page( ch, buf );
}    


void clear_pager( char_data* ch )
{
  if( ch->link == NULL || ch->link->paged == NULL )
    return;

  delete_list( ch->link->paged );

  return;
}


void next_page( link_data* link )
{
  text_data*   next;
  text_data*   send;
  char*      letter;
  int         lines  = 0;

  if( link == NULL || ( send = link->paged ) == NULL )
    return;

  for( ; send->next != NULL; send = send->next ) { 
    for( letter = send->message.text; *letter != '\0'; letter++ )
      if( *letter == '\r' )
        lines++;
    if( lines > link->character->pcdata->lines-6 )
      break;
    }

  next        = send->next;
  send->next  = link->send;
  link->send  = link->paged;
  link->paged = next;

  return;
}


void page_centered( char_data* ch, const char* text )
{
  char  tmp1  [ TWO_LINES ];
  char  tmp2  [ TWO_LINES ];

  if( strlen( text ) > 80 ) {
    bug( "Page_Centered: Text > 80 Characters." );
    bug( text );
    return;
    }

  sprintf( tmp1, "%%%ds%%s\n\r", 40-strlen( text )/2 );
  sprintf( tmp2, tmp1, "", text );

  page( ch, tmp2 );

  return;
}


void page_underlined( char_data* ch, const char* text )
{
  char  tmp  [ TWO_LINES ];
  int     i;

  page( ch, text );

  for( i = 0; i < strlen( text ); i++ )
    if( text[i] == ' ' || text[i] == '\n' || text[i] == '\r' )
      tmp[i] = text[i];
    else
      tmp[i] = '-';

  tmp[i] = '\0';

  page( ch, tmp );
}  


/*
 *   SEND_TO_AREA
 */


void send_to_area( const char* string, area_data* area )
{
  char_data *rch;
  room_data *room;

  for( room = area->room_first; room != NULL; room = room->next )   
    for( int i = 0; i < room->contents; i++ )    
      if( ( rch = character( room->contents[i] ) ) != NULL
        && rch->link != NULL ) 
        send( rch, string );

  return;
}


/*
 *   FORMATTED SEND
 */


void fsend( char_data* ch, const char *text )
{
  char buf [ MAX_STRING_LENGTH ];

  if( ch == NULL || ch->link == NULL )
    return;

  format( buf, text );
  strcat( buf, "\n\r" );
  corrupt( buf, MAX_STRING_LENGTH, "Fsend" );

  send( ch->link, buf );

  return;
}    


void send_centered( char_data* ch, const char* text )
{
  char  tmp1  [ TWO_LINES ];
  char  tmp2  [ TWO_LINES ];

  if( strlen( text ) > 80 ) {
    bug( "Send_Centered: Text > 80 Characters." );
    bug( text );
    return;
    }

  sprintf( tmp1, "%%%ds%%s\n\r", 40-strlen( text )/2 );
  sprintf( tmp2, tmp1, "", text );

  send( tmp2, ch );

  return;
}


void send_underlined( char_data* ch, const char* text )
{
  char  tmp  [ TWO_LINES ];
  int     i;

  send( text, ch );

  for( i = 0; i < strlen( text ); i++ )
    if( text[i] == ' ' || text[i] == '\n' || text[i] == '\r' )
      tmp[i] = text[i];
    else
      tmp[i] = '-';

  tmp[i] = '\0';

  send( tmp, ch );

  return;
}  


