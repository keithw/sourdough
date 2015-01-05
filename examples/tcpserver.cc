#include <cstdlib>

#include "socket.hh"

int main( void )
{
  TCPSocket listening_socket;

  listening_socket.bind( Address( "::0", "8080" ) );

  listening_socket.listen();

  while ( true ) {
    TCPSocket client = listening_socket.accept();

    client.write( "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n<h1>Hello Stanford!" );
  }

  return EXIT_SUCCESS;
}
