/*
 ============================================================================
 Name        : Scothorn_Producer_consumer.c
 Author      : Justin Scothorn
 Version     : 1.0
 Copyright   : I made this with help from: http://pages.cs.wisc.edu/~remzi/Classes/537/Fall2008/Notes/threads-semaphores.txt
 	 	 	 : I followed the tutorial and changed the code to fit the needs of our project requirements.
 Description : Producer Consumer (bounded buffer) problem in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

#define BUFFER_SIZE 5
int buffer[BUFFER_SIZE];// BUFFER_SIZE can be any size

int fill = 0;// used in the put and get functions
int use = 0; // used in the put and get functions

// semaphores
sem_t empty;
sem_t full;
sem_t lock;

// places a value into the buffer
void put(int value){
	buffer[fill] = value;
	fill = (fill + 1) % BUFFER_SIZE;
}

//gets a value from the buffer
int get(){
	int tmp = buffer[use];
	//buffer[use] = 0;// not sure if this is needed, to remove the value already there
	use = (use + 1) % BUFFER_SIZE;
	return tmp;
}

// loops from 0 through the BUFFER_SIZE and uses semaphores to handle the critical section of producing
// a value for the buffer, this prevents more than one thread accessing the buffer at any one time
void *producer(void *arg){
	int i;
	for(i = 0; i < BUFFER_SIZE; i++){
		sem_wait(&empty);// wait until empty
		sem_wait(&lock);// wait until buffer is safe
		put(i);//critical section
		printf("Producer 0x%hx produced %d\n", (int)pthread_self(), i);// the format specifier %hx truncates some of the beginning hex numbers for readability
		sem_post(&lock);// release buffer
		sem_post(&full);// let other threads know the buffer is full
	}
	return 0;
}

// loops through the BUFFER_SIZE and uses semaphores to handle the critical section of consuming
// a value in the buffer, this prevents more than one thread accessing the buffer at any one time
void *consumer(void *arg){
	int i;
	for(i = 0; i < BUFFER_SIZE; i++){
		sem_wait(&full);// waits until bufer is full
		sem_wait(&lock);// waits until buffer is safe
		int tmp = get();//critical section
		printf("Consumer 0x%hx consumed %d\n", (int)pthread_self() , tmp);// the format specifier %hx truncates some of the beginning hex numbers for readability
		sem_post(&lock);// release buffer
		sem_post(&empty);// let other threads know the buffer is empty
	}
	return 0;
}

// initializes the semaphores used and creates the threads, then joins the threads
// together until the producer/consumer functions return, (the buffer size is looped through)
// and exits with success.
int main(int argc, char *argv[]) {

	sem_init(&empty, 0, BUFFER_SIZE);
	sem_init(&full, 0, 0);
	sem_init(&lock, 0, 1);

	pthread_t pid1, cid1, pid2, cid2;
	pthread_create(&pid1, NULL, producer, NULL);
	pthread_create(&cid1, NULL, consumer, NULL);
	pthread_create(&pid2, NULL, producer, NULL);
	pthread_create(&cid2, NULL, consumer, NULL);

	pthread_join(pid1, NULL);
	pthread_join(cid1, NULL);
	pthread_join(pid2, NULL);
	pthread_join(cid2, NULL);

	return EXIT_SUCCESS;
}
