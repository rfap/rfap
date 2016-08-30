#include <rfap/config.h>
#include <rfap/types.h>

#ifdef CONFIG_TASKQUEUE_PTHREAD
#include <pthread.h>
#endif

struct rfap_task {
	void (*run)(struct rfap_task *task);
	struct rfap_task *next;
};

struct rfap_taskqueue {
	/* provider */
	void (*wait_unsafely)(struct rfap_taskqueue *tq);

	/* interface */
	struct rfap_task *(*wait)(struct rfap_taskqueue *tq);
	int (*close)(struct rfap_taskqueue *tq);

	/* private */
	struct rfap_task *head;
	int closed;

	int waiting;

#ifdef CONFIG_TASKQUEUE_PTHREAD
	pthread_mutex_t mutex;
	pthread_cond_t cond;
#endif
};

int rfap_taskqueue_init(
		struct rfap_taskqueue *tq,
		void (*wait_unsafely)(struct rfap_taskqueue *tq));

/* This method should only be called from tq->wait_unsafely */
void rfap_taskqueue_add(struct rfap_taskqueue *tq, struct rfap_task *task);

void rfap_taskqueue_work(struct rfap_taskqueue *tq);
