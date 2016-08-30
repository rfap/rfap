#include "server.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <stdlib.h>

/* ---- accept connections ---- */

struct rfapd_server_acceptor
{
	struct rfap_evhandler_acceptor base;

	struct rfapd_server *server;
	int tls_enabled;
};

struct rfapd_server_acceptor_cleaner
{
	struct rfapd_server_cleaner base;
	struct rfapd_server_acceptor *acceptor;
};

static void rfapd_server_ready_to_accept(struct rfap_evhandler_acceptor *_acceptor);
static void rfapd_server_clean_acceptor(struct rfapd_server_cleaner *_cleaner);
static int rfapd_server_accept(struct rfapd_server *server, int fd,
		int tls_enabled);

/* ---- process connection of initial state ---- */

struct rfapd_server_stream_processor
{
	struct rfap_evhandler_channel base;

	struct rfapd_server *server;
};

struct rfapd_server_stream_processor_cleaner
{
	struct rfapd_server_cleaner base;
	struct rfapd_server_stream_processor *acceptor;
};

/* ---- accept connections ---- */

static void rfapd_server_ready_to_accept(struct rfap_evhandler_acceptor *_acceptor)
{
	struct rfapd_server_acceptor *acceptor =
			(struct rfapd_server_acceptor *) _acceptor;

	struct sockaddr sockaddr;
	socklen_t socklen;

	int fd = acceptor->base.base.fd;

	socklen = sizeof(sockaddr);
	int connfd = accept(fd, &sockaddr, &socklen);

	if (connfd == -1) {
		switch (errno) {
			case EAGAIN: case EINTR: case ECONNABORTED:
				goto err;

			case EMFILE: case ENFILE: case ENOBUFS: case ENOMEM:
				// TODO: pause the acceptor
				goto err;

			default:
				goto self_remove;
		}
		goto err;
	}

	// TODO
self_remove:
	acceptor->base.base.evman->remove(acceptor->base.base.evman,
			(struct rfap_evhandler *) acceptor);
err:
	return;
}

static void rfapd_server_clean_acceptor(struct rfapd_server_cleaner *_cleaner)
{
	struct rfapd_server_acceptor_cleaner *cleaner =
			(struct rfapd_server_acceptor_cleaner *) _cleaner;

	cleaner->acceptor->base.base.evman->remove(
			(struct rfap_evman *) cleaner->acceptor,
			(struct rfap_evhandler *) cleaner->acceptor);
}

static int rfapd_server_accept(struct rfapd_server *server, int fd,
		int tls_enabled)
{
	struct rfapd_server_acceptor *acceptor;
	struct rfapd_server_acceptor_cleaner *cleaner;

	acceptor = malloc(sizeof(struct rfapd_server_acceptor));
	cleaner = malloc(sizeof(struct rfapd_server_acceptor_cleaner));

	if (!acceptor) {
		goto clean_nothing;
	}

	if (!cleaner) {
		goto clean_acceptor;
	}

	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
		goto clean_cleaner;
	}

	acceptor->base.base.fd = fd;
	acceptor->base.ready_to_accept = rfapd_server_ready_to_accept;
	acceptor->server = server;
	acceptor->tls_enabled = tls_enabled;

	server->evman->add_acceptor(server->evman,
			(struct rfap_evhandler_acceptor *) acceptor);

	cleaner->base.server = server;
	cleaner->base.prev = NULL;
	cleaner->base.next = server->cleaner;
	cleaner->base.clean = rfapd_server_clean_acceptor;
	cleaner->acceptor = acceptor;
	server->cleaner->prev = (struct rfapd_server_cleaner *) cleaner;
	server->cleaner = (struct rfapd_server_cleaner *) cleaner;

	return 0;

clean_cleaner:
	free(cleaner);
	cleaner = NULL;

clean_acceptor:
	free(acceptor);
	acceptor = NULL;

clean_nothing:
	return -1;
}

/* ---- process connection of initial state ---- */


/* ---- server initialzier ---- */

void rfapd_server_init(struct rfapd_server *server, struct rfap_evman *evman)
{
	server->evman = evman;
	server->accept = rfapd_server_accept;
	server->cleaner = NULL;
}
