#include <rfap/taskqueue.h>

#include <stdlib.h>

#ifdef CONFIG_TASKQUEUE_PTHREAD

static struct rfap_task *rfap_taskqueue_wait_pthread(struct rfap_taskqueue *tq)
{
	struct rfap_task *task;

	if (pthread_mutex_lock(&tq->mutex)) {
		goto err;
	}

	tq->waiting++;

	if (tq->waiting == 1) {
		if (pthread_mutex_unlock(&tq->mutex)) {
			goto err;
		}
	} else {
		if (pthread_cond_wait(&tq->cond, &tq->mutex)) {
			goto err;
		}
	}

	while (!tq->head) {
		tq->wait_unsafely(tq);
	}

	task = tq->head;
	tq->head = task->next;

	if (pthread_mutex_lock(&tq->mutex)) {
		goto err;
	}

	tq->waiting--;

	if (pthread_cond_signal(&tq->cond)) {
		goto err;
	}

	if (pthread_mutex_unlock(&tq->mutex)) {
		goto err;
	}

	return task;

err:
	abort();
	return NULL;
}

#endif

int rfap_taskqueue_init(
		struct rfap_taskqueue *tq,
	   	void (*wait_unsafely)(struct rfap_taskqueue *tq))
{
	tq->wait_unsafely = wait_unsafely;

	tq->waiting = 0;

#ifdef CONFIG_TASKQUEUE_PTHREAD

	tq->wait = rfap_taskqueue_wait_pthread;

	if (pthread_mutex_init(&tq->mutex, NULL)) {
		goto err;
	}

	if (pthread_cond_init(&tq->cond, NULL)) {
		goto err;
	}

#endif

	tq->closed = 0;

	return 0;

err:
	return -1;
}

void rfap_taskqueue_add(struct rfap_taskqueue *tq, struct rfap_task *task)
{
	task->next = tq->head;
	tq->head = task;
}

void rfap_taskqueue_work(struct rfap_taskqueue *tq)
{
	struct rfap_task *task;

	for(;;) {
		task = tq->wait(tq);

		if (!task) {
			break;
		}

		task->run(task);
	}
}
