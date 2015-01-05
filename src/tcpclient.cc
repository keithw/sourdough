#include <iostream>
#include <thread>

#include "socket.hh"
#include "util.hh"
#include "poller.hh"

using namespace std;
using namespace PollerShortNames;

int main( int argc, char *argv[] )
{
  /* check the command-line arguments */
  if ( argc < 1 ) { /* for sticklers */
    abort();
  }

  if ( argc != 3 ) {
    cerr << "Usage: " << argv[ 0 ] << " HOST PORT" << endl;
    return EXIT_FAILURE;
  }

  string host { argv[ 1 ] }, port { argv[ 2 ] };

  /* Look up the server's address */
  cerr << "Looking up " << host << ":" << port << endl;
  Address server( host, port );
  cerr << "Done. Found " << server.to_string() << endl;

  /* create a TCP socket */
  TCPSocket socket;

  /* connect to the server */
  cerr << "Connecting...";
  socket.connect( server );
  cerr << "done." << endl;

  /* now read and write from the server using an event-driven "poller" */
  Poller poller;

  /* first rule: if the socket has data ready (in the "In" direction),
     print it to the screen (cout) */
  poller.add_action( Action( socket, Direction::In,
			     [&] () {
			       cout << socket.read();

			       /* exit if the server closes the connection */
			       if ( socket.eof() ) {
				 return ResultType::Exit;
			       } else {
				 return ResultType::Continue;
			       }
			     } ) );

  /* second rule: if the keyboard has data ready (also in the "In" direction),
     write it to the server, plus a carriage return and newline */
  FileDescriptor keyboard( 0 );
  poller.add_action( Action( keyboard, Direction::In,
			     [&] () {
			       socket.write( keyboard.read() + "\r\n" );
			       return ResultType::Continue;
			     } ) );

  /* run these two rules forever until it's time to quit */
  while ( true ) {
    const auto ret = poller.poll( -1 );
    if ( ret.result == PollResult::Exit ) {
      return ret.exit_status;
    }
  }
}
