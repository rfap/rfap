#include "common.h"

struct rfapd_server;
struct rfapd_handler;

#ifndef NDEBUG
enum rfapd_handler_type {
	rfapd_handler_tcp,
	rfapd_handler_udp,
	rfapd_handler_tcp_conn
};
#endif

struct rfapd_handler {
#ifndef NDEBUG
	enum rfapd_handler_type type;
#endif
	struct rfapd_server *server;
	struct rfapd_handler *prev, *next;

	int fd;

	void (*on_read)(struct rfapd_handler *handler);
	void (*on_write)(struct rfapd_handler *handler);
};

struct rfapd_server {
	struct rfapd_handler *handlers;

	struct rfap_message_channel *(*on_creating_message_channel)(struct rfapd_server *server);

	struct rfap_block_receiver *(*on_creating_block_receiver)(struct rfapd_server *server);
	struct rfap_block_sender *(*on_creating_block_sender)(struct rfapd_server *server);

	struct rfap_stream_receiver *(*on_creating_stream_receiver)(struct rfapd_server *server);
	struct rfap_stream_sender *(*on_creating_stream_sender)(struct rfapd_server *server);
};
