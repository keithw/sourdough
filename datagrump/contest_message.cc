#include <stdexcept>

#include "contest_message.hh"
#include "timestamp.hh"

using namespace std;

/* helper to get the nth uint64_t field (in network byte order) */
uint64_t get_header_field( const size_t n, const string & str )
{
  if ( str.size() < (n + 1) * sizeof( uint64_t ) ) {
    throw runtime_error( "contest message too small to contain header" );
  }

  const uint64_t * const data_ptr
    = reinterpret_cast<const uint64_t *>( str.data() ) + n;

  return be64toh( *data_ptr );
}

/* Parse header from wire */
ContestMessage::Header::Header( const string & str )
  : sequence_number( get_header_field( 0, str ) ),
    send_timestamp( get_header_field( 1, str ) ),
    ack_sequence_number( get_header_field( 2, str ) ),
    ack_send_timestamp( get_header_field( 3, str ) ),
    ack_recv_timestamp( get_header_field( 4, str ) ),
    ack_payload_length( get_header_field( 5, str ) )
{}

/* Parse incoming message from wire */
ContestMessage::ContestMessage( const string & str )
  : header( str ),
    payload( str.begin() + sizeof( header ), str.end() )
{}

/* Fill in the send_timestamp for an outgoing message */
void ContestMessage::set_send_timestamp()
{
  header.send_timestamp = timestamp_ms();
}

/* helper to put a uint64_t field (in network byte order) */
string put_header_field( const uint64_t n )
{
  const uint64_t network_order = htobe64( n );
  return string( reinterpret_cast<const char *>( &network_order ),
		 sizeof( network_order ) );
}

/* Make wire representation of header */
string ContestMessage::Header::to_string() const
{
  return put_header_field( sequence_number )
    + put_header_field( send_timestamp )
    + put_header_field( ack_sequence_number )
    + put_header_field( ack_send_timestamp )
    + put_header_field( ack_recv_timestamp )
    + put_header_field( ack_payload_length );
}

/* Make wire representation of message */
string ContestMessage::to_string() const
{
  return header.to_string() + payload;
}

/* Transform into an ack of the ContestMessage */
void ContestMessage::transform_into_ack( const uint64_t sequence_number,
					 const uint64_t recv_timestamp )
{
  /* ack the old sequence number */
  header.ack_sequence_number = header.sequence_number;

  /* now assign a new sequence number for the outgoing ack */
  header.sequence_number = sequence_number;

  /* ack the other fields */
  header.ack_send_timestamp = header.send_timestamp;
  header.ack_recv_timestamp = recv_timestamp;
  header.ack_payload_length = payload.length();

  /* delete the payload */
  payload.clear();
}

/* New message */
ContestMessage::ContestMessage( const uint64_t s_sequence_number,
				const std::string & s_payload )
  : header( s_sequence_number ),
    payload( s_payload )
{}

/* Header for new message */
ContestMessage::Header::Header( const uint64_t s_sequence_number )
  : sequence_number( s_sequence_number ),
    send_timestamp( -1 ),
    ack_sequence_number( -1 ),
    ack_send_timestamp( -1 ),
    ack_recv_timestamp( -1 ),
    ack_payload_length( -1 )
{}

/* Is this message an ack? */
bool ContestMessage::is_ack() const
{
  return header.ack_sequence_number != uint64_t( -1 );
}
