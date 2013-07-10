/* This daemon program pipes into the TFE program.  It sits and spins, checking
   periodically if there is any new info coming in.  It takes in addresses, 
   converts them to IP format, and tries to get the host name.  If it get the
   host name, it sends it back to TFE, otherwise it sends back the IP.  Then 
   it sits and spins again */
#include <arpa/telnet.h>
#include <ctype.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include "signal.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <syslog.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <unistd.h>

#include "machine.h"


void    delay           ( void );
void    update          ( void );


char  read_buffer  [ 160 ];
int     read_pntr  = 0;
int          idle_new  = 0;

int   input;
int   output;


int main( int, char **argv )
{
  /* ignore signals from the sigpipe */
  signal( SIGPIPE, SIG_IGN );

  // set the input and output strings to what they are sent from TFE.
  input   = atoi( argv[1] );
  output  = atoi( argv[2] );

  fprintf( stdout, "[Daemon] Starting\n" );

  fcntl( input, F_SETFL, O_NDELAY );

  for( ; ; ) {
    delay( );
    update( );
    }
    
  return 0;
}

/* Tells the system to check null sets for new information for 100000 ticks. */
void delay( )
{
  struct timeval time;

  time.tv_usec = 100000;
  time.tv_sec  = 0;

  if( select( 0, NULL, NULL, NULL, &time ) < 0 ) {
    fprintf( stderr, "[BUG] Delay: error in select.\n" );
    exit( 1 );
    }

  return;
}


void update( )
{
  char               buf  [ 80 ];
  int               addr;
  struct hostent*   from;
  int              nRead;
  int                  i;

  #define sa  (int)( sizeof( struct in_addr ) )


  /* If the read pointer is less the the size of a in_addr, then read in
     some more stuff.  If you read in some stuff, move the read pointer
     a little */
  if( read_pntr < sa
    && ( nRead = read( input, &read_buffer[read_pntr], 10 ) ) > 0 )
    read_pntr += nRead;

 
  /* If the read pointer is still smaller then a in_addr, and the link
     has idled for too long, tell tfe "Alive?".  If tfe is gone, exit
     the daemon. Reset the idle. Go back to main. */
  if( read_pntr < sa ) {
    if( ++idle_new > 10000 ) {
      if( write( output, "Alive?", 7 ) < 0 ) {
        fprintf( stderr, "[Daemon] Exiting\n" );
        exit( 1 );
        }
      idle_new = 0;
      }
    return;
    }

  /* If the read pointer finally got a in_addr, open up the link */

  // Copy the address to addr, and format it.
  memcpy( &addr, read_buffer, sizeof( int ) );
  addr = ntohl( addr );

  // Put the IP in the buffer.
  sprintf( buf, "%d.%d.%d.%d",
    ( addr >> 24 ) & 0xFF, ( addr >> 16 ) & 0xFF,
    ( addr >>  8 ) & 0xFF, ( addr       ) & 0xFF );

  //Get the host address, and put it in the buffer
  from = gethostbyaddr( read_buffer, sa, AF_INET );
 
  /* If an address was returned, send it back to tfe. Otherwise, send the
     IP address */

  if( from != NULL ) 
    write( output, from->h_name, 1+strlen( from->h_name ) );
  else
    write( output, buf, 1+strlen( buf ) );
  
  /* reset the read buffer to everything from the size of an in_addr to the
     end of the read buffer */
  for( i = sa; i < read_pntr; i++ )
    read_buffer[i-sa] = read_buffer[i];

  /* put the read pointer back where it belongs. */
  read_pntr -= sa; 

  #undef sa

  return;
}




