/* simple TCP listener/server to demonstrate sourdough starter classes */
/* Keith Winstein <keithw@cs.stanford.edu>, January 2015 */

#include <thread>
#include <iostream>

#include "socket.hh"
#include "util.hh"

using namespace std;

int main( int argc, char *argv[] )
{
  /* check the command-line arguments */
  if ( argc < 1 ) { /* for sticklers */
    abort();
  }

  if ( argc != 2 ) {
    cerr << "Usage: " << argv[ 0 ] << " PORT" << endl;
    return EXIT_FAILURE;
  }

  /* create a TCP socket */
  TCPSocket listening_socket;

  /* it's ok to reuse the server's address as soon as the program quits
     (this helps debugging, at the slight cost to robustness) */
  listening_socket.set_reuseaddr();

  /* "bind" the socket to the user-specified local port number */
  listening_socket.bind( Address( "::0", argv[ 1 ] ) );

  /* mark the socket as listening for incoming connections */
  listening_socket.listen();
  cerr << "Listening on local address: " << listening_socket.local_address().to_string() << endl;

  /* Wait for clients to connect */
  while ( true ) {

    /* This line does a lot. It waits for a client to connect
       ("listening_socket.accept()"). When that returns a new socket,
       it starts a thread to handle that client and passes in the
       result of accept() as the "client" parameter to the handler. */

    thread client_handler( [] ( TCPSocket client ) {
	cerr << "New connection from " << client.peer_address().to_string() << endl;

	/* Print every line that the client sends */
	while ( true ) {
	  const string chunk = client.read();
	  if ( client.eof() ) { break; }
	  cerr << "Got " << chunk.size() << " bytes from "
	       << client.peer_address().to_string() << ": " << chunk;
	  client.write( "Received " + to_string( chunk.size() ) + " bytes from you.\n" );
	}

	cerr << client.peer_address().to_string() << " closed the connection." << endl; 
      }, listening_socket.accept() );

    /* Let the client handler continue to run without having
       to keep track of it. The main thread can go back to accepting
       new incoming connections. */

    client_handler.detach();
  }

  return EXIT_SUCCESS;
}
