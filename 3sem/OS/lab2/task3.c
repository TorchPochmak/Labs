#include <stdio.h>
#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>


//#define SYNC
#define N 5
#define LEFT (i % N)
#define RIGHT ((i + 1) % N)

int ids[N];
#ifdef SYNC
sem_t semaphore;
#endif
sem_t forks[N];

void* run_philosopher(void* data)
{
	int i = *((int*) data);
	
	while(1)
	{
		printf("Философ %d думает\n", i);
        #ifdef SYNC
		sem_wait(&semaphore);
        #endif
        sem_wait(&forks[LEFT]);
		printf("Философ %d взял левую вилку\n", i);
        sem_wait(&forks[RIGHT]);
		printf("Философ %d взял правую вилку \n", i);
		printf("Философ %d хавает\n", i);
		printf("Философ %d положил левую вилку \n", i);
		printf("Философ %d положил правую вилку \n", i);
        sem_post(&forks[RIGHT]);
        sem_post(&forks[LEFT]);
        #ifdef SYNC
		sem_post(&semaphore);
        #endif
	}
	return NULL;
}

int main()
{
	pthread_t philosophers[N];
	#ifdef SYNC
	sem_init(&semaphore, 0, N - 1);
	#endif
	for (int i = 0; i < N; ++i)
	{
        sem_init(&forks[i], 0, 1);
	}
	for (int i = 0; i < N; ++i)
	{
		ids[i] = i;
		pthread_create(&philosophers[i], NULL, run_philosopher, (void*) &(ids[i]));
	}
	for (int i = 0; i < N; ++i)
	{
		pthread_join(philosophers[i], NULL);
	}
	for (int i = 0; i < N; ++i)
	{
		sem_destroy(&forks[i]);
	}
    #ifdef SYNC
    sem_destroy(&semaphore);
    #endif
}