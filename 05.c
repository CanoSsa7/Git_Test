#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

#define msleep(x) usleep(x*1000)   // 定义微秒延时函数
#define PRODUCT_SPEED 1            // 生产速度
#define CONSUM_SPEED 2             // 消费速度
#define INIT_NUM 0                 // 仓库原有产品数
#define TOTAL_NUM 7                // 仓库容量

sem_t p_sem,      // 生产者信号量，表示缓冲区中待生产的产品数量
      c_sem,      // 消费者信号量，表示缓冲区中待消费的产品数量
      sh_sem;     // 共享资源锁，用于互斥访问仓库

int num = INIT_NUM;  // 仓库中原有的产品数量

// 生产函数（模拟生产）
void product(void) { 
    sleep(CONSUM_SPEED); 
}

// 向仓库中添加产品
int add_to_lib() {
    num++;
    msleep(200);
    return num;
}

// 消费函数（模拟消费）
void consum(void) { sleep(PRODUCT_SPEED); }

// 从仓库中取出产品
int remove_from_lib() {
    num--;
    msleep(200);
    return num;
}

// 生产者线程函数
void *productor(void *arg) {
    while(1){
        product();  // 生产函数（模拟生产）
        sleep(1);
        sem_wait(&p_sem);  // 等待仓库有空位置
        sem_wait(&sh_sem);  // 获取仓库共享资源锁
        add_to_lib();  // 向仓库中添加产品
        printf("producer is working now\n");  // 打印生产者正在工作的信息
        printf("size is %d\n",num);  // 打印仓库中产品数量
        sem_post(&sh_sem);  // 释放仓库共享资源锁
        sem_post(&c_sem);  // 发送信号量通知消费者有新产品到达
    }
}

// 消费者线程函数
void *consumer(void *arg) {
    while(1){
        sem_wait(&c_sem);  // 等待仓库中有产品可消费
        sem_wait(&sh_sem);  // 获取仓库共享资源锁
        remove_from_lib();  // 从仓库中取出产品
        printf("consumer is working now\n");  // 打印消费者正在工作的信息
        printf("size is %d\n",num);  // 打印仓库中产品数量
        sem_post(&sh_sem);  // 释放仓库共享资源锁
        sem_post(&p_sem);  // 发送信号量通知生产者有空位置
        sleep(10);  // 模拟消费者处理产品的时间
        consum();  // 消费函数（模拟消费）
    }
}

int main() {
    pthread_t tid_1, tid_2;

    // 初始化各个信号量
    sem_init(&p_sem, 0, TOTAL_NUM - INIT_NUM);  // 生产者信号量初始化为仓库容量减去当前产品数量
    sem_init(&c_sem, 0, INIT_NUM);  // 消费者信号量初始化为当前产品数量
    sem_init(&sh_sem, 0, 1);  // 共享资源锁的初始值为 1，表示仓库未被加锁

    // 创建生产者和消费者线程
    //线程标识符、线程属性设置、线程函数起始地址(函数指针)、传参
    pthread_create(&tid_1, NULL, productor, NULL);
    pthread_create(&tid_2, NULL, consumer, NULL);

    // 等待线程结束
    pthread_join(tid_1, NULL);
    pthread_join(tid_2, NULL);

    return 0;
}
