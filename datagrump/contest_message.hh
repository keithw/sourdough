#ifndef CONTEST_MESSAGE_HH
#define CONTEST_MESSAGE_HH

#include <string>
#include <cstdint>

struct ContestMessage
{
  struct Header {
    uint64_t sequence_number;
    uint64_t send_timestamp;

    uint64_t ack_sequence_number;
    uint64_t ack_send_timestamp;
    uint64_t ack_recv_timestamp;
    uint64_t ack_payload_length;

    /* Header for new message */
    Header( const uint64_t s_sequence_number );

    /* Parse header from wire */
    Header( const std::string & str );

    /* Make wire representation of header */
    std::string to_string() const;
  } header;

  std::string payload;

  /* New message */
  ContestMessage( const uint64_t s_sequence_number,
		  const std::string & s_payload );

  /* Parse incoming datagram from wire */
  ContestMessage( const std::string & str );

  /* Fill in the send_timestamp for an outgoing datagram */
  void set_send_timestamp();

  /* Make wire representation of datagram */
  std::string to_string() const;

  /* Transform into an ack of the ContestMessage */
  void transform_into_ack( const uint64_t sequence_number,
			   const uint64_t recv_timestamp );

  /* Is this message an ack? */
  bool is_ack() const;
};

#endif /* CONTEST_MESSAGE_HH */
