#include "evman_epoll.h"

#include <sys/epoll.h>

#include <stdlib.h>

struct rfap_evman_epoll
{
	struct rfap_evman base;

	int epfd;
};

static void rfap_evman_epoll_wait(struct rfap_evman *_evman, int timeout)
{
	struct rfap_evman_epoll *evman;
	evman = (struct rfap_evman_epoll *) _evman;

	struct epoll_event events[64];
	int evnum = 64;

	while (evnum == 64) {
		int i;

		evnum = epoll_wait(evman->epfd, events, 64, timeout);

		if (evnum == -1) {
			abort();  // TODO
		}

		for (i = 0; i != evnum; ++i) {
			struct rfap_evhandler *handler;
			handler = events[i].data.ptr;

			switch (handler->type) {
				case RFAP_EVHANDLER_ACCEPTABLE: {
					struct rfap_evhandler_acceptor *acceptor =
						(struct rfap_evhandler_acceptor *) handler;
					acceptor->ready_to_accept(acceptor);
					break;
				}

			default:
				abort();
			}
		}
	}
}

int rfap_evman_epoll_remove(struct rfap_evman *_evman,
		struct rfap_evhandler *evhandler)
{
	struct rfap_evman_epoll *evman;
	evman = (struct rfap_evman_epoll *) _evman;

	if (epoll_ctl(evman->epfd, EPOLL_CTL_DEL, evhandler->fd, NULL) == -1) {
		goto err;
	}

	return 0;

err:
	return -1;
}

static int rfap_evman_epoll_add_acceptor(struct rfap_evman *_evman,
		struct rfap_evhandler_acceptor *acceptor)
{
	struct rfap_evman_epoll *evman;
	struct epoll_event epev;
	evman = (struct rfap_evman_epoll *) _evman;

	acceptor->base.evman = _evman;

	epev.events = EPOLLIN;
	epev.data.ptr = acceptor;

	if (epoll_ctl(evman->epfd, EPOLL_CTL_ADD, acceptor->base.fd, &epev)
			== -1) {
		goto err;
	}

	return 0;

err:
	return -1;
}

void rfap_evman_epoll_init(struct rfap_evman *_evman)
{
	struct rfap_evman_epoll *evman;
	evman = (struct rfap_evman_epoll *) _evman;

	evman->base.wait = rfap_evman_epoll_wait;
	evman->base.remove = rfap_evman_epoll_remove;
	evman->base.add_acceptor = rfap_evman_epoll_add_acceptor;
	evman->epfd = epoll_create1(0);
}

// vim: ts=4 sw=4
