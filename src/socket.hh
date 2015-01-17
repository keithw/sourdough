#ifndef SOCKET_HH
#define SOCKET_HH

#include <functional>

#include "address.hh"
#include "file_descriptor.hh"

/* class for network sockets (UDP, TCP, etc.) */
class Socket : public FileDescriptor
{
private:
  /* get the local or peer address the socket is connected to */
  Address get_address( const std::string & name_of_function,
		       const std::function<int(int, sockaddr *, socklen_t *)> & function ) const;

protected:
  /* default constructor */
  Socket( const int domain, const int type );

  /* construct from file descriptor */
  Socket( FileDescriptor && s_fd, const int domain, const int type );

public:
  /* bind socket to a specified local address (usually to listen/accept) */
  void bind( const Address & address );

  /* connect socket to a specified peer address */
  void connect( const Address & address );

  /* accessors */
  Address local_address( void ) const;
  Address peer_address( void ) const;
};

/* UDP socket */
class UDPSocket : public Socket
{
public:
  UDPSocket() : Socket( AF_INET6, SOCK_DGRAM ) {}

  /* receive datagram and where it came from */
  std::pair<Address, std::string> recvfrom( void );

  /* send datagram to specified address */
  void sendto( const Address & peer, const std::string & buffer );

  /* send datagram to connected address */
  void send( const std::string & buffer );
};

/* TCP socket */
class TCPSocket : public Socket
{
public:
  TCPSocket() : Socket( AF_INET6, SOCK_STREAM ) {}
  TCPSocket( FileDescriptor && fd ) : Socket( std::move( fd ), AF_INET6, SOCK_STREAM ) {}

  /* mark the socket as listening for incoming connections */
  void listen( const int backlog = 16 );

  /* accept a new incoming connection */
  TCPSocket accept( void );

  /* allow local address to be reused sooner, at the cost of some robustness */
  void set_reuseaddr( void );
};

#endif /* SOCKET_HH */
