#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

#define NUM_PHILOSOPHERS 5 // 哲学家的个数
#define NUM_CHOPSTICKS 5   // 筷子的个数

// 定义信号量和临界区变量
sem_t chopstick[NUM_CHOPSTICKS];
sem_t waiter;
pthread_mutex_t mutex;

// 生产函数（模拟就餐）
void eat(void) {
    sleep(rand() % 3);  // 模拟进餐的时间
}

// 哲学家线程函数
void *philosopher(void *arg) {
    int id = *(int*)arg;
    int left_chopstick = id;
    int right_chopstick = (id + 1) % NUM_CHOPSTICKS;

    while (1) {
        printf("哲学家 %d 在思考...\n", id);

        sem_wait(&waiter);  // 服务员上锁

        // 尝试获取左边的筷子
        sem_wait(&chopstick[left_chopstick]);
        printf("哲学家 %d 拿起了左边的筷子。\n", id);

        // 尝试获取右边的筷子
        if (sem_trywait(&chopstick[right_chopstick]) != 0) {
            sem_post(&chopstick[left_chopstick]);  // 放下左边的筷子
            printf("哲学家 %d 没有抢到右边的筷子，放下了左边的筷子。\n", id);
        } else {
            printf("哲学家 %d 拿起了右边的筷子。\n", id);

            sem_post(&waiter);  // 服务员解锁

            // 进餐
            pthread_mutex_lock(&mutex);
            printf("哲学家 %d 开始就餐...\n", id);
            eat();
            printf("哲学家 %d 就餐结束。\n", id);
            pthread_mutex_unlock(&mutex);

            sem_post(&chopstick[left_chopstick]);   // 放下左边的筷子
            sem_post(&chopstick[right_chopstick]);  // 放下右边的筷子
        }
    }
}

// 服务员线程函数
void *waitress(void *arg) {
    while (1) {
        usleep(100000);  // 服务员每 0.1s 尝试分配筷子

        int i;
        for (i = 0; i < NUM_CHOPSTICKS; i++) {
            if (sem_trywait(&chopstick[i]) == 0) {
                int left_philosopher = i;
                int right_philosopher = (i + NUM_PHILOSOPHERS - 1) % NUM_PHILOSOPHERS;
                if (sem_trywait(&chopstick[right_philosopher]) == 0) {
                    printf("服务员给哲学家 %d 和 %d 分配了筷子。\n", left_philosopher, right_philosopher);
                    sem_post(&waiter);  // 服务员解锁
                    break;
                } else {
                    sem_post(&chopstick[i]);  // 放下已拿起的左边筷子
                }
            }
        }
    }
}

int main() {
    int i;
    pthread_t phil_tid[NUM_PHILOSOPHERS];
    pthread_t wait_tid;

    // 初始化信号量和临界区变量
    sem_init(&waiter, 0, 1);
    for (i = 0; i < NUM_CHOPSTICKS; i++) {
        sem_init(&chopstick[i], 0, 1);
    }
    pthread_mutex_init(&mutex, NULL);

    // 创建哲学家线程和服务员线程
    for (i = 0; i < NUM_PHILOSOPHERS; i++) {
        int *id = malloc(sizeof(int));
        *id = i;
        pthread_create(&phil_tid[i], NULL, philosopher, id);
    }
    pthread_create(&wait_tid, NULL, waitress, NULL);

    // 等待线程结束
    for (i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(phil_tid[i], NULL);
    }
    pthread_join(wait_tid, NULL);

    // 销毁信号量和临界区变量
    sem_destroy(&waiter);
    for (i = 0; i < NUM_CHOPSTICKS; i++) {
        sem_destroy(&chopstick[i]);
    }
    pthread_mutex_destroy(&mutex);

    return 0;
}
