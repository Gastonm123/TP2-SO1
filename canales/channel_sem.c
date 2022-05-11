#include "channel.h"
#include <semaphore.h>

void chan_init(struct channel *c) {
  sem_init(&c->chan_closed, 0, 1);
  sem_init(&c->chan_open, 0, 0);
  sem_init(&c->chan_reading, 0, 0);
  sem_init(&c->chan_writing, 0, 0);
}

void chan_write(struct channel *c, int v) {
  sem_wait(&c->chan_closed);
  sem_post(&c->chan_open);
  sem_wait(&c->chan_reading);
  c->passed = v;
  sem_post(&c->chan_writing);
}

int chan_read(struct channel *c) {
  sem_wait(&c->chan_open);
  sem_post(&c->chan_reading);
  sem_wait(&c->chan_writing);
  int passed = c->passed;
  sem_post(&c->chan_closed);
  return passed;
}
