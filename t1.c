#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define N 5  // 哲学家数量

sem_t forks[N];
sem_t mutex;

void *philosopher(void *arg) {
    int id = *(int *)arg;
    int left = id;
    int right = (id + 1) % N;

    while (1) {
        sem_wait(&mutex);  // 获取锁
        sem_wait(&forks[left]);  // 获取左边的叉子
        printf("Philosopher %d is getting a left fork %d.\n", id, left);
        sem_wait(&forks[right]);  // 获取右边的叉子
        printf("Philosopher %d is getting a right fork %d.\n", id, right);
        printf("Philosopher %d is eating.\n", id);
        usleep(1000*1000);
        sem_post(&forks[left]);  // 放回左边的叉子
        printf("Philosopher %d is putting back the left fork %d.\n", id, left);
        sem_post(&forks[right]);  // 放回右边的叉子
        printf("Philosopher %d is putting back the right fork %d.\n", id, right);
        sem_post(&mutex);  // 释放锁
        printf("Philosopher %d is thinking.\n", id);
        usleep(1000*1000);
    }
}

int main() {
    pthread_t tid[N];
    int ids[N];
    int i;

    sem_init(&mutex, 0, 1);  // 初始化互斥锁
    for ( i = 0; i < N; i++) {
        sem_init(&forks[i], 0, 1);  // 初始化叉子信号量
        ids[i] = i;
    }
    
    for ( i = 0; i < N; i++) {
        pthread_create(&tid[i], NULL, philosopher, &ids[i]);  // 创建哲学家线程
    }

    for ( i = 0; i < N; i++) {
        pthread_join(tid[i], NULL);  // 等待所有哲学家线程结束
    }

    sem_destroy(&mutex);  // 销毁互斥锁
    for ( i = 0; i < N; i++) {
        sem_destroy(&forks[i]);  // 销毁叉子信号量
    }

    return 0;
}
