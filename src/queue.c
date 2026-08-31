#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "read.h"

int message_queue_init(Message_Queue *queue){
  queue->head=0;
  queue->tail=0;
  if(pthread_mutex_init(&queue->mutex,NULL)!=0){
    return -1;
  }
  if(sem_init(&queue->empty,0,50)!=0){
    pthread_mutex_destroy(&queue->mutex);
    return -1;
  }
  if(sem_init(&queue->full,0,0)!=0){
    sem_destroy(&queue->empty);
    pthread_mutex_destroy(&queue->mutex);
    return -1;
  }

  return 0;

};

void message_destroy(Message_Queue *queue){
  pthread_mutex_destroy(&queue->mutex);
  sem_destroy(&queue->full);
  sem_destroy(&queue->empty);

};

int message_queue_push(Message_Queue *queue,const Message *msg){
  /* 
   * Make the logic for adding an element to the queue only when the queue is avaliabale and not full 
   * Make the queue in such a way that it does not waste any memory and no extra memory is required .
   *
   */

};

int message_queue_pop(Message_Queue *queue,Message *msg){
  sem_wait(&queue->full);
  pthread_mutex_lock(&queue->mutex);
  *msg=queue->buffer[queue->head];
  queue->head=(queue->head +1)%50;
  pthread_mutex_unlock(&queue->mutex);
  sem_post(&queue->empty);
  return 0;
};






