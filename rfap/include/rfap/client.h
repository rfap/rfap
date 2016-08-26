#include "common.h"

struct rfap_client {
	rfap_message_channel *(*create_message_channel)();
	rfap_block_receiver *(*create_block_receiver)();
	rfap_block_sender *(*create_block_sender)();
	rfap_stream_receiver *(*create_stream_receiver)();
	rfap_stream_sender *(*create_stream_sender)();
};
