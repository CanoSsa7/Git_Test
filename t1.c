#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define N 5  // ��ѧ������

sem_t forks[N];
sem_t mutex;

void *philosopher(void *arg) {
    int id = *(int *)arg;
    int left = id;
    int right = (id + 1) % N;

    while (1) {
        sem_wait(&mutex);  // ��ȡ��
        sem_wait(&forks[left]);  // ��ȡ��ߵĲ���
        printf("Philosopher %d is getting a left fork %d.\n", id, left);
        sem_wait(&forks[right]);  // ��ȡ�ұߵĲ���
        printf("Philosopher %d is getting a right fork %d.\n", id, right);
        printf("Philosopher %d is eating.\n", id);
        usleep(1000*1000);
        sem_post(&forks[left]);  // �Ż���ߵĲ���
        printf("Philosopher %d is putting back the left fork %d.\n", id, left);
        sem_post(&forks[right]);  // �Ż��ұߵĲ���
        printf("Philosopher %d is putting back the right fork %d.\n", id, right);
        sem_post(&mutex);  // �ͷ���
        printf("Philosopher %d is thinking.\n", id);
        usleep(1000*1000);
    }
}

int main() {
    pthread_t tid[N];
    int ids[N];
    int i;

    sem_init(&mutex, 0, 1);  // ��ʼ��������
    for ( i = 0; i < N; i++) {
        sem_init(&forks[i], 0, 1);  // ��ʼ�������ź���
        ids[i] = i;
    }
    
    for ( i = 0; i < N; i++) {
        pthread_create(&tid[i], NULL, philosopher, &ids[i]);  // ������ѧ���߳�
    }

    for ( i = 0; i < N; i++) {
        pthread_join(tid[i], NULL);  // �ȴ�������ѧ���߳̽���
    }

    sem_destroy(&mutex);  // ���ٻ�����
    for ( i = 0; i < N; i++) {
        sem_destroy(&forks[i]);  // ���ٲ����ź���
    }

    return 0;
}
