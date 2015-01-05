#include <sys/socket.h>

#include "socket.hh"
#include "util.hh"

using namespace std;

/* default constructor for socket of (subclassed) domain and type */
Socket::Socket( const int domain, const int type )
  : FileDescriptor( SystemCall( "socket", socket( domain, type, 0 ) ) )
{}

/* construct from file descriptor */
Socket::Socket( FileDescriptor && fd, const int domain, const int type )
  : FileDescriptor( move( fd ) )
{
  int actual_value;
  socklen_t len;

  /* verify domain */
  len = sizeof( actual_value );
  SystemCall( "getsockopt",
	      getsockopt( fd_num(), SOL_SOCKET, SO_DOMAIN, &actual_value, &len ) );
  if ( (len != sizeof( actual_value )) or (actual_value != domain) ) {
    throw runtime_error( "socket domain mismatch" );
  }

  /* verify type */
  len = sizeof( actual_value );
  SystemCall( "getsockopt",
	      getsockopt( fd_num(), SOL_SOCKET, SO_TYPE, &actual_value, &len ) );
  if ( (len != sizeof( actual_value )) or (actual_value != type) ) {
    throw runtime_error( "socket type mismatch" );
  }
}

/* get the local or peer address the socket is connected to */
Address Socket::get_address( const std::string & name_of_function,
			     const std::function<int(int, sockaddr *, socklen_t *)> & function ) const
{
  sockaddr_storage address;
  socklen_t size = sizeof( address );

  SystemCall( name_of_function, function( fd_num(),
					  reinterpret_cast<sockaddr *>( &address ),
					  &size ) );

  return Address( address, size );
}

Address Socket::local_address( void ) const
{
  return get_address( "getsockname", getsockname );
}

Address Socket::peer_address( void ) const
{
  return get_address( "getpeername", getpeername );
}

/* bind socket to a specified local address (usually to listen/accept) */
void Socket::bind( const Address & address )
{
  SystemCall( "bind", ::bind( fd_num(),
			      &address.to_sockaddr(),
			      address.size() ) );
}

/* connect socket to a specified peer address */
void Socket::connect( const Address & address )
{
  SystemCall( "connect", ::connect( fd_num(),
				    &address.to_sockaddr(),
				    address.size() ) );
}

/* receive datagram and where it came from */
pair<Address, string> UDPSocket::recvfrom( void )
{
  static const ssize_t RECEIVE_MTU = 65536;

  /* receive source address and payload */
  sockaddr_storage packet_remote_addr;
  char buffer[ RECEIVE_MTU ];

  socklen_t fromlen = sizeof( packet_remote_addr );

  ssize_t recv_len = SystemCall( "recvfrom",
				 ::recvfrom( fd_num(),
					     buffer,
					     sizeof( buffer ),
					     MSG_TRUNC,
					     reinterpret_cast<sockaddr *>( &packet_remote_addr ),
					     &fromlen ) );

  if ( recv_len > RECEIVE_MTU ) {
    throw runtime_error( "recvfrom (oversized datagram)" );
  }

  register_read();

  return make_pair( Address( packet_remote_addr, fromlen ),
		    string( buffer, recv_len ) );
}

/* send datagram to specified address */
void UDPSocket::sendto( const Address & destination, const string & payload )
{
    SystemCall( "sendto", ::sendto( fd_num(),
                                    payload.data(),
                                    payload.size(),
                                    0,
                                    &destination.to_sockaddr(),
                                    sizeof( destination.to_sockaddr() ) ) );

    register_write();
}

/* mark the socket as listening for incoming connections */
void TCPSocket::listen( const int backlog )
{
    SystemCall( "listen", ::listen( fd_num(), backlog ) );
}

/* accept a new incoming connection */
TCPSocket TCPSocket::accept( void )
{
  register_read();
  return TCPSocket( FileDescriptor( SystemCall( "accept", ::accept( fd_num(), nullptr, nullptr ) ) ) );
}

/* allow local address to be reused sooner, at the cost of some robustness */
void TCPSocket::set_reuseaddr( void )
{
  const int value = true;
  SystemCall( "setsockopt", setsockopt( fd_num(), SOL_SOCKET, SO_REUSEADDR, &value, sizeof( value ) ) );
}
