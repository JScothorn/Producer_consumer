/*
 ============================================================================
 Name        : Scothorn_Producer_consumer.c
 Author      : Justin Scothorn
 Version     : 1.0
 Copyright   : I made this with help from: http://pages.cs.wisc.edu/~remzi/Classes/537/Fall2008/Notes/threads-semaphores.txt
 Description : Producer Consumer (bounded buffer) problem in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

#define MAX 1000
int buffer[MAX];// made to be size 1000, can be any size though
int fill = 0;
int use = 0;
int loops;

sem_t empty;
sem_t full;
sem_t lock;

void put(int value){
	buffer[fill] = value;
	fill = (fill + 1) % loops;
}

int get(){
	int tmp = buffer[use];
	use = (use + 1) % loops;
	return tmp;
}

void *producer(void *arg){
	int i;

	for(i = 0; i < loops; i++){
		sem_wait(&empty);
		sem_wait(&lock);
		put(i);//critical section
		printf("Producer 0x%hx produced %d\n", (int)pthread_self(), i);// the format specifier %hx truncates some of the beginning hex numbers for readability
		sem_post(&lock);
		sem_post(&full);
	}
	return 0;
}

void *consumer(void *arg){
	int i;
	for(i = 0; i < loops; i++){
		sem_wait(&full);
		sem_wait(&lock);
		int tmp = get();//critical section
		printf("Consumer 0x%hx consumed %d\n", (int)pthread_self() , tmp);// the format specifier %hx truncates some of the beginning hex numbers for readability
		sem_post(&lock);
		sem_post(&empty);
	}
	return 0;
}

int main(int argc, char *argv[]) {

	loops = atoi(argv[1]);
	sem_init(&empty, 0, loops);
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
