#include <rfap/evman.h>

struct rfap_evman *rfap_evman_epoll_alloc();
int rfap_evman_epoll_init(struct rfap_evman *evman);
