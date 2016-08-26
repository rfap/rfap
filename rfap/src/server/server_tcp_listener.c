#include <rfap/server.h>

#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <unistd.h>

struct rfapd_tcp_handler {
	struct rfapd_handler base;

#ifdef FEATURE_TLS
	bool tls_enabled;
#endif
};

static void rfapd_tcp_accept(struct rfapd_handler *handler)
{
	int fd;
	struct sockaddr address;
	socklen_t address_len;

	if (!handler->server) {
		fd = accept(handler->fd, &address, &address_len);
		close(fd);
		return;
	}

	// TODO
}

struct rfapd_handler *rfapd_alloc_tcp_handler()
{
	struct rfapd_handler *handler = malloc(sizeof (struct rfapd_tcp_handler));
#ifndef NDEBUG
	handler->type = rfapd_handler_tcp;
#endif

	return handler;
}

int rfapd_init_tcp_handler(
		struct rfapd_handler *handler,
		int fd,
		int backlog,
		bool tls_enabled)
{
#ifndef FEATURE_TLS
	assert(!tls_enabled);
#endif
	assert(handler->type == rfapd_handler_tcp);

	struct rfapd_tcp_handler *tcp_handler = (struct rfapd_tcp_handler *) handler;

	if (listen(fd, backlog)) {
		return -1;
	}

	handler->on_read = rfapd_tcp_accept;
	handler->on_write = NULL;

	handler->server = NULL;

#ifdef FEATURE_TLS
	tcp_handler->tls_enabled = tls_enabled;
#endif

	return 0;
}
