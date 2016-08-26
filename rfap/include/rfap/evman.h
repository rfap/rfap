#include <rfap/types.h>

enum rfap_evhandler_type
{
	RFAP_EVHANDLER_ACCEPTABLE
};

struct rfap_evhandler
{
	struct rfap_evman *evman;

	enum rfap_evhandler_type type;
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
	void (*wait)(struct rfap_evman *evman, int timeout);
	int (*remove)(struct rfap_evman *evman,
			struct rfap_evhandler *evhandler);
	int (*add_acceptor)(struct rfap_evman *evman,
			struct rfap_evhandler_acceptor *acceptor);
};

// vim: ts=4 sw=4
