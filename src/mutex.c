#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>

#include "read.h"

int rwlock_init(ReadWrite_Lock *rw){
    rw->reader = 0;

    if (pthread_mutex_init(&rw->reader_count, NULL) != 0)
        return -1;

    if (pthread_mutex_init(&rw->writer_count, NULL) != 0) {
        pthread_mutex_destroy(&rw->reader_count);
        return -1;
    }

    if (sem_init(&rw->resource, 0, 1) != 0) {
        pthread_mutex_destroy(&rw->reader_count);
        pthread_mutex_destroy(&rw->writer_count);
        return -1;
    }

    return 0;
}

void reader_enter(ReadWrite_Lock *lock){
    pthread_mutex_lock(&lock->writer_count);

    pthread_mutex_lock(&lock->reader_count);

    lock->reader++;

    if (lock->reader == 1) {
        sem_wait(&lock->resource);
    }

    pthread_mutex_unlock(&lock->reader_count);

    pthread_mutex_unlock(&lock->writer_count);
}

void reader_exit(ReadWrite_Lock *lock){
    pthread_mutex_lock(&lock->reader_count);

    lock->reader--;

    if (lock->reader == 0) {
        sem_post(&lock->resource);
    }

    pthread_mutex_unlock(&lock->reader_count);
}

void writer_enter(ReadWrite_Lock *lock){
    pthread_mutex_lock(&lock->writer_count);
    sem_wait(&lock->resource);
}

void writer_exit(ReadWrite_Lock *lock){
    sem_post(&lock->resource);
    pthread_mutex_unlock(&lock->writer_count);
}

void rwlock_destroy(ReadWrite_Lock *rw)
{
    pthread_mutex_destroy(&rw->reader_count);
    pthread_mutex_destroy(&rw->writer_count);
    sem_destroy(&rw->resource);
}