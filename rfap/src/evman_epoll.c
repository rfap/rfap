#include "evman_epoll.h"

#include <sys/epoll.h>

#include <stdlib.h>

struct rfap_evman_epoll
{
	struct rfap_evman base;

	int epfd;
};

static void rfap_evhandler_acceptor_epoll_run(struct rfap_task *_acceptor)
{
	struct rfap_evhandler_acceptor *acceptor =
		(struct rfap_evhandler_acceptor *) _acceptor;
	acceptor->ready_to_accept(acceptor);
}

static void rfap_evman_epoll_wait(struct rfap_taskqueue *_evman)
{
	struct rfap_evman_epoll *evman;
	evman = (struct rfap_evman_epoll *) _evman;

	struct epoll_event events[64];
	int evnum = 64;

	while (evnum == 64) {
		int i;

		evnum = epoll_wait(evman->epfd, events, 64, 0);

		if (evnum == -1) {
			abort();  // TODO
		}

		for (i = 0; i != evnum; ++i) {
			struct rfap_evhandler *handler;
			handler = events[i].data.ptr;

			rfap_taskqueue_add(
					(struct rfap_taskqueue *) evman,
					(struct rfap_task *) handler);
		}
	}
}

static int rfap_evman_epoll_remove(struct rfap_evman *_evman,
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

static int rfap_evman_epoll_add_acceptor(
		struct rfap_evman *_evman,
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

	acceptor->base.base.run = rfap_evhandler_acceptor_epoll_run;

	return 0;

err:
	return -1;
}

int rfap_evman_epoll_init(struct rfap_evman *_evman)
{
	struct rfap_evman_epoll *evman;

	rfap_taskqueue_init(
			(struct rfap_taskqueue *) evman,
			rfap_evman_epoll_wait);

	evman = (struct rfap_evman_epoll *) _evman;

	evman->base.remove = rfap_evman_epoll_remove;
	evman->base.add_acceptor = rfap_evman_epoll_add_acceptor;
	evman->epfd = epoll_create1(0);

	if (evman->epfd == -1) {
		goto clean_taskqueue;
	}

	return 0;

clean_taskqueue:
	evman->base.base.close((struct rfap_taskqueue *) evman);

err:
	return -1;
}
