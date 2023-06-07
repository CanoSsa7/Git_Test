#include<stdio.h>
#include<sys/time.h>
#include<malloc.h>
#include<stdlib.h>

#define NR_TASKS 64   //系统支持的进程个数
#define TASK_RUNNING 0   //就绪态
#define TASK_UNINTERRUPTIBLE 2   //不可中断的睡眠状态
#define TASK_ZOMBIE 3   //僵死态

//进程表项
struct task_struct{
    long pid;        //进程号
    long state;      //进程运行状态
    long priority;   //优先数
    long counter;    //进程剩余时间片
    long start_time;  //进程开始时间
    long excute_time; //进程执行时间
};

// 初始进程表项
struct task_struct init_task = {
    .pid = 0,
    .state = 0,
    .priority = 0,
    .counter = 0,
    .start_time = 0,
    .excute_time = 0
};

// 当前运行的进程指针
struct task_struct *current = &init_task;

// 系统滴答数，用于记录时间
unsigned long volatile jiffies = 0;

// 进程指针数组，用于存储系统中所有的进程
struct task_struct* task[NR_TASKS] = {&init_task,};
#define  FIRST_TASK    task[0]
#define  LAST_TASK     task[NR_TASKS-1]

// 进程就绪队列
struct run_q {
    struct task_struct *data;
    struct run_q *next;
};
struct run_q *head=NULL,*end=NULL,*r_temp;

// 进程个数
#define  N_PROCESS 5
// 最长运行时间
#define  MAX_RUNTIME 100
// 进程初始值
int process[N_PROCESS][2]={{0,3},{2,6},{4,4},{6,5},{8,2}};
// CPU 总的运行时间
int totalExcuteTime = 0;
// 进程运行状态的记录
int runState[N_PROCESS][MAX_RUNTIME] = {0};
// 进程总数
int total = 0;

// 判断是否有新进程到达，如果有则创建进程
void checkProcessCome();

// 0 号进程的运行体
void pause();

// FCFS 调度程序
void schedule_f();

// RR 调度程序
void schedule_r();

// 最短剩余时间调度程序
void schedule_s();

// Feedback 调度程序
void schedule_o();

// 进程切换函数
void switch_to(int pid);

// 基于优先级的调度程序的初始化
void init();

// 进程的运行体函数
void run(int pid);

// 创建进程
void myfork(int pid);

// 进程清除
void delete(int pid);

// 函数指针，用于指向调度函数
typedef void funtype(void);
funtype *schedule = NULL;

int main(int argc,char **argv)
{
    int i,j;
    int choice;
    while(1){
        printf("please choice the schedule measure:\n");
        printf("f : 先到先服务的调度策略\n");
        printf("r : 轮循的调度策略\n");
        printf("s : 最短剩余时间的调度策略\n");
        printf("o : 反馈q=1调度策略\n");
        printf("q : 退出\n");
        printf("choice = ");
        choice = getchar();
        if(choice == '\n')
            choice = getchar();
        switch(choice){
            case 'f': schedule = schedule_f;break;
            case 'r': schedule = schedule_r;break;
            case 's': schedule = schedule_s;break;
            case 'o': schedule = schedule_o;break;
            case 'q': return 0;
            default : {
                schedule = NULL;
                printf("please input the true symbol(p or f or r)!\n\n");
                continue;
            }
        }
        printf("task id   start  excute\n");
        for(i=0;i<N_PROCESS;i++){
            printf("task %2d: %6d %6d\n",i+1,process[i][0],process[i][1]);
            totalExcuteTime+=process[i][1];
        }
        
        init();

        //打印进程调度情况
        printf("time   : 0%*c%d\n",totalExcuteTime-2,' ',totalExcuteTime);
        for(i=0;i<N_PROCESS;i++){
            printf("task %2d: ",i+1);
            for(j=0;j<totalExcuteTime;j++){
                if(runState[i][j]==1) printf("#");
                else printf(" ");
                runState[i][j] = 0;
            }
            printf("\n");
        }
        while((head!=NULL)&&(head!=end)){
            r_temp = head;
            head = head->next;
            free(r_temp);
        }
        if(head){
            free(head);
            head = NULL;
            end = NULL;
        }
        current = &init_task;
        jiffies = 0;
        totalExcuteTime = 0;
        printf("\n");
    }
    return 0;
}
void schedule_o(){
    // 定义变量
    int i,next,c;
    int s=0;
    struct task_struct **p;

    // 初始化变量
    c = 9999;  // 当前最高优先级，初始值为非常大的数
    next = 0;  // 下一个要运行的进程的 pid，初始值为 0
    i = 0;  // 用于遍历任务数组的计数器，初始值为 0
    p = &task[0];  // 指向任务数组的第一个进程控制块的指针

    // 如果系统中只有一个进程则将该进程的时间片和优先级都设置为 1
    if(total==1){
        current->counter=1;
        current->priority=1;
    }

    // 遍历任务数组中的所有进程控制块
    while((i++)<NR_TASKS){
        if(!*++p)  // 如果当前指针为空，则跳过本次循环
            continue;
        if((*p)->state==TASK_RUNNING&&(*p)->counter==0)  // 如果该进程状态为运行态且时间片已经用完
            (*p)->counter=1;  // 则重新设置时间片为 1
    }

    // 再次遍历就绪队列中所有进程，找出其中优先级最高且时间片还有剩余的进程
    i = 0;
    p = &task[0];
    while((i++)<NR_TASKS){
        if(!*++p)  // 如果当前指针为空，则跳过本次循环
            continue;
        if( (*p)->counter==1 &&(*p)->priority < c && (*p)->state==TASK_RUNNING) 
         // 如果该进程时间片为 1，优先级小于当前最高优先级且状态为运行态
            c=(*p)->priority , next =i;  // 则更新最高优先级和下一个要运行的进程的 pid
    }

    // 切换到进程控制块为 next 的进程中
    switch_to(next);
}

void schedule_s(){
    int i,next,c;  //定义循环变量i、下一个要执行的进程号next和最短剩余时间c
    int s=0;  //记录当前时间
    struct task_struct **p;  //定义进程控制块指针数组的指针p
    c = 9999;  //初始化 c 为一个非常大的数，作为最初始的最短剩余时间
    next = 0;  //初始化下一个要执行的进程号为 0
    i = NR_TASKS;  //初始化 i 为进程控制块数组的长度
    p = &task[NR_TASKS];  //将 p 指向进程控制块数组的末尾

    //遍历进程控制块数组，找到剩余执行时间最短并处于就绪态的进程
    while(--i){
        if(!*--p)  //如果当前进程指针为空，则跳过当前循环
            continue;
        s = process[(*p)->pid-1][1];  //获取当前时间
        if((*p)->state == TASK_RUNNING && (s - (*p)->excute_time) < c){  
            //如果该进程处于就绪态并且其剩余执行时间比当前记录的最短剩余时间小
            c = s - (*p)->excute_time;  //更新最短剩余时间 c
            next = i;  //更新下一个要执行的进程号 next
        }
    }

    switch_to(next);  //切换到下一个进程
}

void schedule_f(){
    int i,next,c;  //定义循环变量i，下一个要执行的进程号next，最先到达的进程开始时间c
    struct task_struct **p;  //定义进程控制块指针数组的指针p
    c = 9999;  //初始化最小值为大整数
    next = 0;  
    i = NR_TASKS;  //初始化i为进程控制块数组长度
    p = &task[NR_TASKS];  //将p指向进程控制块数组的末尾
    //遍历进程控制块数组，找到最先到达并处于就绪态的进程
    while(--i){
        if(!*--p)  //如果当前进程指针为空，则跳过当前循环
            continue;
        if((*p)->state == TASK_RUNNING && (*p)->start_time < c)  
        //如果该进程处于就绪态并且其开始时间比当前记录的最小时间小
            c = (*p)->start_time,next = i;  //更新最小时间 c 和进程号 next
    }
    switch_to(next);  //切换到下一个进程
}

void schedule_r(){
    int next;   //下一个要执行的进程号
    next = 0;
    if(current->state != TASK_RUNNING){  //如果当前进程不在运行状态
        r_temp = head;   //将就绪队列头指针的值赋给临时变量r_temp
        if(head==end){   //如果就绪队列中只有一个进程，则将头尾指针全部置为空
            head = NULL;
            end = NULL;
        }else{   //否则将头指针指向下一个进程，尾指针指向新的队列结尾
            head = head->next;
            end->next = head;
        }
        free(r_temp);  //释放临时变量指向的进程控制块
    }else if(head){  //否则如果就绪队列不为空
        head = head->next;  //将头指针指向下一个进程，尾指针指向新的队列结尾
        end = end->next;
    }
    if(head) next = head->data->pid;  //如果就绪队列不为空，则将下一个要执行的进程号设置为队列头部进程的pid
    switch_to(next);   //切换到下一个进程的执行
}

void switch_to(int pid){
    if(pid)
        run(pid);
    else
        pause();
}

void myfork(int pid){
    struct timeval now;  // 定义 timeval 结构体变量 now
    struct run_q *p;  // 定义就绪队列结构体指针 p
    task[pid] = (struct task_struct*)malloc(sizeof(struct task_struct));  // 为指定 pid 分配进程控制块的空间
    task[pid]->state = TASK_UNINTERRUPTIBLE;  // 将该进程的状态设置为 TASK_UNINTERRUPTIBLE，即不能被打断
    task[pid]->pid = pid;  // 设置该进程的 pid 属性
    gettimeofday(&now,0);  // 获取当前时间戳 now，并保存在 now 变量中
    srand(now.tv_usec);  // 将当前时间戳的微秒部分作为随机数种子
    task[pid]->priority = 1;  // 初始化优先级为 1
    task[pid]->counter = 1;  // 初始化时间片为 1
    task[pid]->start_time = jiffies; 
    task[pid]->excute_time = 0; 
    task[pid]->state = TASK_RUNNING;  // 将该进程的状态设置为 TASK_RUNNING，表示进程已经准备好可以被调度
    total++;  // 增加系统中所有进程的数量
    p = (struct run_q*)malloc(sizeof(struct run_q));  // 为指定 pid 创建就绪队列结构体，并将其指针赋值给 p
    p->data = task[pid];  // 将该进程的指针赋值给就绪队列结构体的 data 属性
    if(head == NULL){  // 如果当前队列为空，则将 p 设置为头节点，并且让其 next 指针指向自身
        head = end = p;  
        head->next = end;  
        end->next = head;  
    }else{  // 否则将 p 添加到队列末尾并更新 end 的指针
        end->next = p;
        end = p;
        end->next = head;
    }
}


void delete(int pid){
    free(task[pid]);
}

void checkProcessCome(){
    int i;
    for(i=0;i<N_PROCESS;i++){
    if(process[i][0]==jiffies)
        myfork(i+1);
    }
}

void init(){
    int i;
    for(i=1;i<NR_TASKS;i++){
        task[i] = NULL;
    }
    checkProcessCome();
    schedule();
}

void pause(){
    current = task[0];
    //printf("running task %d\n",pid);
    //usleep(1000000);
    jiffies++;
    totalExcuteTime++;
    checkProcessCome();
    schedule();
}

void run(int pid){
    int i;
    current = task[pid];
    runState[pid-1][jiffies] = 1;
    //printf("running task %d\n",pid);
    //usleep(1000000);
    jiffies++;
    task[pid]->counter--;
    task[pid]->priority++;
    task[pid]->excute_time++;
    //判断进程是否运行完，如果是则将进程杀死
    if(task[pid]->excute_time==process[pid-1][1]){
        //task[pid]->end_time = jiffies;
        task[pid]->state = TASK_ZOMBIE;
    }
    //判断所有进程是否都运行完，如果是则结束
    if(jiffies>=totalExcuteTime) {
        total--;
        return;
    }
    checkProcessCome();
    schedule();
}