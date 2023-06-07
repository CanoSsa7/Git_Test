#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

#define NUM_PHILOSOPHERS 5 // ��ѧ�ҵĸ���
#define NUM_CHOPSTICKS 5   // ���ӵĸ���

// �����ź������ٽ�������
sem_t chopstick[NUM_CHOPSTICKS];
sem_t waiter;
pthread_mutex_t mutex;

// ����������ģ��Ͳͣ�
void eat(void) {
    sleep(rand() % 3);  // ģ����͵�ʱ��
}

// ��ѧ���̺߳���
void *philosopher(void *arg) {
    int id = *(int*)arg;
    int left_chopstick = id;
    int right_chopstick = (id + 1) % NUM_CHOPSTICKS;

    while (1) {
        printf("��ѧ�� %d ��˼��...\n", id);

        sem_wait(&waiter);  // ����Ա����

        // ���Ի�ȡ��ߵĿ���
        sem_wait(&chopstick[left_chopstick]);
        printf("��ѧ�� %d ��������ߵĿ��ӡ�\n", id);

        // ���Ի�ȡ�ұߵĿ���
        if (sem_trywait(&chopstick[right_chopstick]) != 0) {
            sem_post(&chopstick[left_chopstick]);  // ������ߵĿ���
            printf("��ѧ�� %d û�������ұߵĿ��ӣ���������ߵĿ��ӡ�\n", id);
        } else {
            printf("��ѧ�� %d �������ұߵĿ��ӡ�\n", id);

            sem_post(&waiter);  // ����Ա����

            // ����
            pthread_mutex_lock(&mutex);
            printf("��ѧ�� %d ��ʼ�Ͳ�...\n", id);
            eat();
            printf("��ѧ�� %d �Ͳͽ�����\n", id);
            pthread_mutex_unlock(&mutex);

            sem_post(&chopstick[left_chopstick]);   // ������ߵĿ���
            sem_post(&chopstick[right_chopstick]);  // �����ұߵĿ���
        }
    }
}

// ����Ա�̺߳���
void *waitress(void *arg) {
    while (1) {
        usleep(100000);  // ����Աÿ 0.1s ���Է������

        int i;
        for (i = 0; i < NUM_CHOPSTICKS; i++) {
            if (sem_trywait(&chopstick[i]) == 0) {
                int left_philosopher = i;
                int right_philosopher = (i + NUM_PHILOSOPHERS - 1) % NUM_PHILOSOPHERS;
                if (sem_trywait(&chopstick[right_philosopher]) == 0) {
                    printf("����Ա����ѧ�� %d �� %d �����˿��ӡ�\n", left_philosopher, right_philosopher);
                    sem_post(&waiter);  // ����Ա����
                    break;
                } else {
                    sem_post(&chopstick[i]);  // �������������߿���
                }
            }
        }
    }
}

int main() {
    int i;
    pthread_t phil_tid[NUM_PHILOSOPHERS];
    pthread_t wait_tid;

    // ��ʼ���ź������ٽ�������
    sem_init(&waiter, 0, 1);
    for (i = 0; i < NUM_CHOPSTICKS; i++) {
        sem_init(&chopstick[i], 0, 1);
    }
    pthread_mutex_init(&mutex, NULL);

    // ������ѧ���̺߳ͷ���Ա�߳�
    for (i = 0; i < NUM_PHILOSOPHERS; i++) {
        int *id = malloc(sizeof(int));
        *id = i;
        pthread_create(&phil_tid[i], NULL, philosopher, id);
    }
    pthread_create(&wait_tid, NULL, waitress, NULL);

    // �ȴ��߳̽���
    for (i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(phil_tid[i], NULL);
    }
    pthread_join(wait_tid, NULL);

    // �����ź������ٽ�������
    sem_destroy(&waiter);
    for (i = 0; i < NUM_CHOPSTICKS; i++) {
        sem_destroy(&chopstick[i]);
    }
    pthread_mutex_destroy(&mutex);

    return 0;
}
