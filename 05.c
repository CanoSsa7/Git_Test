#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

#define msleep(x) usleep(x*1000)   // ����΢����ʱ����
#define PRODUCT_SPEED 1            // �����ٶ�
#define CONSUM_SPEED 2             // �����ٶ�
#define INIT_NUM 0                 // �ֿ�ԭ�в�Ʒ��
#define TOTAL_NUM 7                // �ֿ�����

sem_t p_sem,      // �������ź�������ʾ�������д������Ĳ�Ʒ����
      c_sem,      // �������ź�������ʾ�������д����ѵĲ�Ʒ����
      sh_sem;     // ������Դ�������ڻ�����ʲֿ�

int num = INIT_NUM;  // �ֿ���ԭ�еĲ�Ʒ����

// ����������ģ��������
void product(void) { 
    sleep(CONSUM_SPEED); 
}

// ��ֿ�����Ӳ�Ʒ
int add_to_lib() {
    num++;
    msleep(200);
    return num;
}

// ���Ѻ�����ģ�����ѣ�
void consum(void) { sleep(PRODUCT_SPEED); }

// �Ӳֿ���ȡ����Ʒ
int remove_from_lib() {
    num--;
    msleep(200);
    return num;
}

// �������̺߳���
void *productor(void *arg) {
    while(1){
        product();  // ����������ģ��������
        sleep(1);
        sem_wait(&p_sem);  // �ȴ��ֿ��п�λ��
        sem_wait(&sh_sem);  // ��ȡ�ֿ⹲����Դ��
        add_to_lib();  // ��ֿ�����Ӳ�Ʒ
        printf("producer is working now\n");  // ��ӡ���������ڹ�������Ϣ
        printf("size is %d\n",num);  // ��ӡ�ֿ��в�Ʒ����
        sem_post(&sh_sem);  // �ͷŲֿ⹲����Դ��
        sem_post(&c_sem);  // �����ź���֪ͨ���������²�Ʒ����
    }
}

// �������̺߳���
void *consumer(void *arg) {
    while(1){
        sem_wait(&c_sem);  // �ȴ��ֿ����в�Ʒ������
        sem_wait(&sh_sem);  // ��ȡ�ֿ⹲����Դ��
        remove_from_lib();  // �Ӳֿ���ȡ����Ʒ
        printf("consumer is working now\n");  // ��ӡ���������ڹ�������Ϣ
        printf("size is %d\n",num);  // ��ӡ�ֿ��в�Ʒ����
        sem_post(&sh_sem);  // �ͷŲֿ⹲����Դ��
        sem_post(&p_sem);  // �����ź���֪ͨ�������п�λ��
        sleep(10);  // ģ�������ߴ����Ʒ��ʱ��
        consum();  // ���Ѻ�����ģ�����ѣ�
    }
}

int main() {
    pthread_t tid_1, tid_2;

    // ��ʼ�������ź���
    sem_init(&p_sem, 0, TOTAL_NUM - INIT_NUM);  // �������ź�����ʼ��Ϊ�ֿ�������ȥ��ǰ��Ʒ����
    sem_init(&c_sem, 0, INIT_NUM);  // �������ź�����ʼ��Ϊ��ǰ��Ʒ����
    sem_init(&sh_sem, 0, 1);  // ������Դ���ĳ�ʼֵΪ 1����ʾ�ֿ�δ������

    // ���������ߺ��������߳�
    //�̱߳�ʶ�����߳��������á��̺߳�����ʼ��ַ(����ָ��)������
    pthread_create(&tid_1, NULL, productor, NULL);
    pthread_create(&tid_2, NULL, consumer, NULL);

    // �ȴ��߳̽���
    pthread_join(tid_1, NULL);
    pthread_join(tid_2, NULL);

    return 0;
}
