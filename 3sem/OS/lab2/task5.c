#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define N 3 // Максимальное количество людей в ванной комнате

int womenCount = 0;
int menCount = 0;
int indicator = 1;
pthread_mutex_t mutex;

#define sleep_time (rand() % 2 + 1)

void woman_leaves()
{
    pthread_mutex_lock(&mutex);
    womenCount--;
    if (womenCount == 0 && menCount == 0) 
    {
        indicator = 1;
    }
    printf("Woman left\n");
    pthread_mutex_unlock(&mutex);
}

void man_leaves()
{
    pthread_mutex_lock(&mutex);
    menCount--;
    if (menCount == 0 && womenCount == 0) 
    {
        indicator = 1;
    }
    printf("Man left\n");
    pthread_mutex_unlock(&mutex);
}

void* woman_wants_to_enter(void* arg) 
{
    pthread_mutex_lock(&mutex);
    //Ждем мужчин короче...
    while (indicator == 3) 
    {
        pthread_mutex_unlock(&mutex);
        sleep(sleep_time);
        pthread_mutex_lock(&mutex);
    }
    womenCount++;
    if (womenCount > 0 && menCount == 0) 
    {
        indicator = 2;
    }

    printf("Woman entered\n");
    pthread_mutex_unlock(&mutex);
    sleep(sleep_time); // ванная комната занята некоторое время

    woman_leaves();

    return NULL;
}

void* man_wants_to_enter(void* arg) 
{
    pthread_mutex_lock(&mutex);
    //Ждем дам короче...
    while (indicator == 2) 
    {
        pthread_mutex_unlock(&mutex);
        sleep(sleep_time);
        pthread_mutex_lock(&mutex);
    }
    menCount++;
    if (menCount > 0 && womenCount == 0) 
    {
        indicator = 3;
    }

    printf("Man entered\n");
    pthread_mutex_unlock(&mutex);
    sleep(sleep_time); // моемся...
    man_leaves();
    return NULL;
}

int main(int argc, char *argv[]) 
{

    if (argc < 2) {
        printf("<count>\n");
        return 1;
    }

    int maxCount = atoi(argv[1]);
    if(maxCount < 1 || maxCount > 100)
    {
        printf("Error: invalid input...");
        return 2;
    }

    int cnt = rand() % 5 + maxCount;
    printf("People count: %d\n", cnt);
    pthread_t threads[cnt];
    pthread_mutex_init(&mutex, NULL);

    // Создание потоков
    for (int i = 0; i < cnt; i++) {
        int is_man = rand() % 2;
        if(!is_man) 
            pthread_create(&threads[i], NULL, woman_wants_to_enter, NULL);
        else
            pthread_create(&threads[i], NULL, man_wants_to_enter, NULL);
    }

    // Ожидание завершения потоков
    for (int i = 0; i < cnt; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    return 0;
}