#include <rfap/evman.h>

#include <sys/types.h>
#include <sys/socket.h>

struct rfapd_server_cleaner
{
	void (*clean)(struct rfapd_server_cleaner *cleaner);

	struct rfapd_server *server;
	struct rfapd_server_cleaner *prev, *next;
};

struct rfapd_server
{
	struct rfap_evman *evman;

	int (*accept)(struct rfapd_server *server, int fd, int tls_enabled);
	void (*close)();

	struct rfapd_server_cleaner *cleaner;
};

void rfapd_server_init(struct rfapd_server *server, struct rfap_evman *evman);

// vim: ts=4 sw=4
