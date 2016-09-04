#include "evman_kqueue.h"

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <unistd.h>

#include <stdlib.h>

struct rfap_evman_kqueue
{
	struct rfap_evman base;

	int kq;
	int update_signal_pipe[2];
	struct rfap_evhandler update_handler;
};

static void rfap_evhandler_acceptor_kqueue_run(struct rfap_task *_acceptor)
{
	struct rfap_evhandler_acceptor *acceptor =
		(struct rfap_evhandler_acceptor *) _acceptor;
	acceptor->ready_to_accept(acceptor);
}

static void rfap_evman_kqueue_wait(struct rfap_taskqueue *_evman)
{
	struct rfap_evman_kqueue *evman;
	evman = (struct rfap_evman_kqueue *) _evman;
}

static int rfap_evman_kqueue_remove(struct rfap_evman *_evman,
		struct rfap_evhandler *evhandler)
{
	struct rfap_evman_kqueue *evman;
	evman = (struct rfap_evman_kqueue *) _evman;

	// TODO

	return 0;

err:
	return -1;
}

static int rfap_evman_kqueue_add_acceptor(
		struct rfap_evman *_evman,
		struct rfap_evhandler_acceptor *acceptor)
{
	struct rfap_evman_kqueue *evman;

	evman = (struct rfap_evman_kqueue *) _evman;

	acceptor->base.evman = _evman;
	acceptor->base.base.run = rfap_evhandler_acceptor_kqueue_run;

	// TODO

	return 0;

err:
	return -1;
}

static int rfap_evman_kqueue_register_update_signal(
		struct rfap_evman_kqueue *evman)
{
	struct kevent chlist;
	int r;

	evman->update_handler.evman = (struct rfap_evman *) evman;
	evman->update_handler.fd = evman->update_signal_pipe[0];
	evman->update_handler.base.run = /*TODO*/ NULL;
	evman->update_handler.base.next = NULL;

	EV_SET(
			&chlist,
			evman->update_signal_pipe[0],
			EVFILT_READ,
			EV_ADD | EV_ENABLE,
			0,
			0,
			&evman->update_handler);

	r = kevent(evman->kq, &chlist, 1, NULL, 0, NULL);

	if (r != 1) {
		return -1;
	} else {
		return 0;
	}
}

int rfap_evman_kqueue_init(struct rfap_evman *_evman)
{
	struct rfap_evman_kqueue *evman;

	rfap_taskqueue_init(
			(struct rfap_taskqueue *) evman,
			rfap_evman_kqueue_wait);

	evman = (struct rfap_evman_kqueue *) _evman;

	evman->base.remove = rfap_evman_kqueue_remove;
	evman->base.add_acceptor = rfap_evman_kqueue_add_acceptor;
	evman->kq = kqueue();

	if (evman->kq == -1) {
		goto clean_taskqueue;
	}

	if (pipe(evman->update_signal_pipe) == -1) {
		goto clean_kqueue;
	}

	if (rfap_evman_kqueue_register_update_signal(evman) == -1) {
		goto clean_kqueue;
	}

	return 0;

clean_kqueue:
	close(evman->kq);

clean_taskqueue:
	evman->base.base.close((struct rfap_taskqueue *) evman);

err:
		return -1;
}
