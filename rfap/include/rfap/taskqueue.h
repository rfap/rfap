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
	int (*add)(struct rfap_task *tasks, int n);
	int (*close)(struct rfap_task *task);

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

void rfap_taskqueue_work(struct rfap_taskqueue *tq);
