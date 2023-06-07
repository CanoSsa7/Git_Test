#include<stdio.h>
#include<sys/time.h>
#include<malloc.h>
#include<stdlib.h>

#define NR_TASKS 64   //ϵͳ֧�ֵĽ��̸���
#define TASK_RUNNING 0   //����̬
#define TASK_UNINTERRUPTIBLE 2   //�����жϵ�˯��״̬
#define TASK_ZOMBIE 3   //����̬

//���̱���
struct task_struct{
    long pid;        //���̺�
    long state;      //��������״̬
    long priority;   //������
    long counter;    //����ʣ��ʱ��Ƭ
    long start_time;  //���̿�ʼʱ��
    long excute_time; //����ִ��ʱ��
};

// ��ʼ���̱���
struct task_struct init_task = {
    .pid = 0,
    .state = 0,
    .priority = 0,
    .counter = 0,
    .start_time = 0,
    .excute_time = 0
};

// ��ǰ���еĽ���ָ��
struct task_struct *current = &init_task;

// ϵͳ�δ��������ڼ�¼ʱ��
unsigned long volatile jiffies = 0;

// ����ָ�����飬���ڴ洢ϵͳ�����еĽ���
struct task_struct* task[NR_TASKS] = {&init_task,};
#define  FIRST_TASK    task[0]
#define  LAST_TASK     task[NR_TASKS-1]

// ���̾�������
struct run_q {
    struct task_struct *data;
    struct run_q *next;
};
struct run_q *head=NULL,*end=NULL,*r_temp;

// ���̸���
#define  N_PROCESS 5
// �����ʱ��
#define  MAX_RUNTIME 100
// ���̳�ʼֵ
int process[N_PROCESS][2]={{0,3},{2,6},{4,4},{6,5},{8,2}};
// CPU �ܵ�����ʱ��
int totalExcuteTime = 0;
// ��������״̬�ļ�¼
int runState[N_PROCESS][MAX_RUNTIME] = {0};
// ��������
int total = 0;

// �ж��Ƿ����½��̵��������򴴽�����
void checkProcessCome();

// 0 �Ž��̵�������
void pause();

// FCFS ���ȳ���
void schedule_f();

// RR ���ȳ���
void schedule_r();

// ���ʣ��ʱ����ȳ���
void schedule_s();

// Feedback ���ȳ���
void schedule_o();

// �����л�����
void switch_to(int pid);

// �������ȼ��ĵ��ȳ���ĳ�ʼ��
void init();

// ���̵������庯��
void run(int pid);

// ��������
void myfork(int pid);

// �������
void delete(int pid);

// ����ָ�룬����ָ����Ⱥ���
typedef void funtype(void);
funtype *schedule = NULL;

int main(int argc,char **argv)
{
    int i,j;
    int choice;
    while(1){
        printf("please choice the schedule measure:\n");
        printf("f : �ȵ��ȷ���ĵ��Ȳ���\n");
        printf("r : ��ѭ�ĵ��Ȳ���\n");
        printf("s : ���ʣ��ʱ��ĵ��Ȳ���\n");
        printf("o : ����q=1���Ȳ���\n");
        printf("q : �˳�\n");
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

        //��ӡ���̵������
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
    // �������
    int i,next,c;
    int s=0;
    struct task_struct **p;

    // ��ʼ������
    c = 9999;  // ��ǰ������ȼ�����ʼֵΪ�ǳ������
    next = 0;  // ��һ��Ҫ���еĽ��̵� pid����ʼֵΪ 0
    i = 0;  // ���ڱ�����������ļ���������ʼֵΪ 0
    p = &task[0];  // ָ����������ĵ�һ�����̿��ƿ��ָ��

    // ���ϵͳ��ֻ��һ�������򽫸ý��̵�ʱ��Ƭ�����ȼ�������Ϊ 1
    if(total==1){
        current->counter=1;
        current->priority=1;
    }

    // �������������е����н��̿��ƿ�
    while((i++)<NR_TASKS){
        if(!*++p)  // �����ǰָ��Ϊ�գ�����������ѭ��
            continue;
        if((*p)->state==TASK_RUNNING&&(*p)->counter==0)  // ����ý���״̬Ϊ����̬��ʱ��Ƭ�Ѿ�����
            (*p)->counter=1;  // ����������ʱ��ƬΪ 1
    }

    // �ٴα����������������н��̣��ҳ��������ȼ������ʱ��Ƭ����ʣ��Ľ���
    i = 0;
    p = &task[0];
    while((i++)<NR_TASKS){
        if(!*++p)  // �����ǰָ��Ϊ�գ�����������ѭ��
            continue;
        if( (*p)->counter==1 &&(*p)->priority < c && (*p)->state==TASK_RUNNING) 
         // ����ý���ʱ��ƬΪ 1�����ȼ�С�ڵ�ǰ������ȼ���״̬Ϊ����̬
            c=(*p)->priority , next =i;  // �����������ȼ�����һ��Ҫ���еĽ��̵� pid
    }

    // �л������̿��ƿ�Ϊ next �Ľ�����
    switch_to(next);
}

void schedule_s(){
    int i,next,c;  //����ѭ������i����һ��Ҫִ�еĽ��̺�next�����ʣ��ʱ��c
    int s=0;  //��¼��ǰʱ��
    struct task_struct **p;  //������̿��ƿ�ָ�������ָ��p
    c = 9999;  //��ʼ�� c Ϊһ���ǳ����������Ϊ���ʼ�����ʣ��ʱ��
    next = 0;  //��ʼ����һ��Ҫִ�еĽ��̺�Ϊ 0
    i = NR_TASKS;  //��ʼ�� i Ϊ���̿��ƿ�����ĳ���
    p = &task[NR_TASKS];  //�� p ָ����̿��ƿ������ĩβ

    //�������̿��ƿ����飬�ҵ�ʣ��ִ��ʱ����̲����ھ���̬�Ľ���
    while(--i){
        if(!*--p)  //�����ǰ����ָ��Ϊ�գ���������ǰѭ��
            continue;
        s = process[(*p)->pid-1][1];  //��ȡ��ǰʱ��
        if((*p)->state == TASK_RUNNING && (s - (*p)->excute_time) < c){  
            //����ý��̴��ھ���̬������ʣ��ִ��ʱ��ȵ�ǰ��¼�����ʣ��ʱ��С
            c = s - (*p)->excute_time;  //�������ʣ��ʱ�� c
            next = i;  //������һ��Ҫִ�еĽ��̺� next
        }
    }

    switch_to(next);  //�л�����һ������
}

void schedule_f(){
    int i,next,c;  //����ѭ������i����һ��Ҫִ�еĽ��̺�next�����ȵ���Ľ��̿�ʼʱ��c
    struct task_struct **p;  //������̿��ƿ�ָ�������ָ��p
    c = 9999;  //��ʼ����СֵΪ������
    next = 0;  
    i = NR_TASKS;  //��ʼ��iΪ���̿��ƿ����鳤��
    p = &task[NR_TASKS];  //��pָ����̿��ƿ������ĩβ
    //�������̿��ƿ����飬�ҵ����ȵ��ﲢ���ھ���̬�Ľ���
    while(--i){
        if(!*--p)  //�����ǰ����ָ��Ϊ�գ���������ǰѭ��
            continue;
        if((*p)->state == TASK_RUNNING && (*p)->start_time < c)  
        //����ý��̴��ھ���̬�����俪ʼʱ��ȵ�ǰ��¼����Сʱ��С
            c = (*p)->start_time,next = i;  //������Сʱ�� c �ͽ��̺� next
    }
    switch_to(next);  //�л�����һ������
}

void schedule_r(){
    int next;   //��һ��Ҫִ�еĽ��̺�
    next = 0;
    if(current->state != TASK_RUNNING){  //�����ǰ���̲�������״̬
        r_temp = head;   //����������ͷָ���ֵ������ʱ����r_temp
        if(head==end){   //�������������ֻ��һ�����̣���ͷβָ��ȫ����Ϊ��
            head = NULL;
            end = NULL;
        }else{   //����ͷָ��ָ����һ�����̣�βָ��ָ���µĶ��н�β
            head = head->next;
            end->next = head;
        }
        free(r_temp);  //�ͷ���ʱ����ָ��Ľ��̿��ƿ�
    }else if(head){  //��������������в�Ϊ��
        head = head->next;  //��ͷָ��ָ����һ�����̣�βָ��ָ���µĶ��н�β
        end = end->next;
    }
    if(head) next = head->data->pid;  //����������в�Ϊ�գ�����һ��Ҫִ�еĽ��̺�����Ϊ����ͷ�����̵�pid
    switch_to(next);   //�л�����һ�����̵�ִ��
}

void switch_to(int pid){
    if(pid)
        run(pid);
    else
        pause();
}

void myfork(int pid){
    struct timeval now;  // ���� timeval �ṹ����� now
    struct run_q *p;  // ����������нṹ��ָ�� p
    task[pid] = (struct task_struct*)malloc(sizeof(struct task_struct));  // Ϊָ�� pid ������̿��ƿ�Ŀռ�
    task[pid]->state = TASK_UNINTERRUPTIBLE;  // ���ý��̵�״̬����Ϊ TASK_UNINTERRUPTIBLE�������ܱ����
    task[pid]->pid = pid;  // ���øý��̵� pid ����
    gettimeofday(&now,0);  // ��ȡ��ǰʱ��� now���������� now ������
    srand(now.tv_usec);  // ����ǰʱ�����΢�벿����Ϊ���������
    task[pid]->priority = 1;  // ��ʼ�����ȼ�Ϊ 1
    task[pid]->counter = 1;  // ��ʼ��ʱ��ƬΪ 1
    task[pid]->start_time = jiffies; 
    task[pid]->excute_time = 0; 
    task[pid]->state = TASK_RUNNING;  // ���ý��̵�״̬����Ϊ TASK_RUNNING����ʾ�����Ѿ�׼���ÿ��Ա�����
    total++;  // ����ϵͳ�����н��̵�����
    p = (struct run_q*)malloc(sizeof(struct run_q));  // Ϊָ�� pid �����������нṹ�壬������ָ�븳ֵ�� p
    p->data = task[pid];  // ���ý��̵�ָ�븳ֵ���������нṹ��� data ����
    if(head == NULL){  // �����ǰ����Ϊ�գ��� p ����Ϊͷ�ڵ㣬�������� next ָ��ָ������
        head = end = p;  
        head->next = end;  
        end->next = head;  
    }else{  // ���� p ��ӵ�����ĩβ������ end ��ָ��
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
    //�жϽ����Ƿ������꣬������򽫽���ɱ��
    if(task[pid]->excute_time==process[pid-1][1]){
        //task[pid]->end_time = jiffies;
        task[pid]->state = TASK_ZOMBIE;
    }
    //�ж����н����Ƿ������꣬����������
    if(jiffies>=totalExcuteTime) {
        total--;
        return;
    }
    checkProcessCome();
    schedule();
}