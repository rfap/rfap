#include <rfap/config.h>
#include <rfap/types.h>
#include <rfap/taskqueue.h>

struct rfap_evhandler
{
	struct rfap_task base;

	struct rfap_evman *evman;
	int fd;
};

struct rfap_evhandler_acceptor
{
	struct rfap_evhandler base;

	void (*ready_to_accept)(struct rfap_evhandler_acceptor *acceptor);
};

struct rfap_evhandler_channel
{
	struct rfap_evhandler base;

	void (*ready_to_read)(struct rfap_evhandler_channel *channel);
	void (*ready_to_write)(struct rfap_evhandler_channel *channel);
};

struct rfap_evman
{
	struct rfap_taskqueue base;

	int (*remove)(struct rfap_evman *evman,
			struct rfap_evhandler *evhandler);
	int (*add_acceptor)(struct rfap_evman *evman,
			struct rfap_evhandler_acceptor *acceptor);
};
