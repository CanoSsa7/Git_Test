#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define random(x) (rand() % x)
#define MULTIPLE 3

typedef struct page_s {
    int n;                      //number
    int v;                      //visit flag
}page;

char *menu[] = {
    "f - FIFO",
    "r - LRU",
    "o - OPT",
    "c - CLOCK",
    "q - quit",
    NULL
};

int getchoice(char *greet, char *choices[]) {
    int chosen = 0;
    int selected;
    char **option;

    do {
        printf("Choice: %s\n", greet);
        option = choices;
        while(*option) {
            printf("%s\n", *option);
            option++;
        }
        do {
            selected = getchar();
        } while(selected == '\n');
        option = choices;
        while(*option) {
            if(selected == *option[0]) {
                chosen = 1;
                break;
            }
            option++;
        }
        if(!chosen) {
            printf("Incorrect choice, select again\n");
        }
    } while(!chosen);
    return selected;
}


void buildPageReference(int size, page **reference, page *program) {
    int i;
    int n;
    printf("Page reference : ");
    for(i=0;i<size;i++) {
        n = random(size/MULTIPLE);
        reference[i] = &program[n];
        program[n].n = n;
        program[n].v = 0;
        printf("| %d ", n);
    }
    printf("\n");
}

void print(int n, page *frame, int size) {
    int i;

    printf("no. %d step: ", n);
    for(i=0;i<size;i++) {
        printf("| %d ", frame[i].n);
    }
    printf("\n");
}

int Search(int n, page *list, int size) {
    int i;
    for(i=0;i<size;i++) {
        if(list[i].n == n) return i;
    }
    return -1;
}//判断是否在驻留集中

int findNext(int n, page **list, int start, int size) {
    int count = size;
    int i;
    for(i=start;i<size;i++) {
        if(list[i]->n == n) break;
        else count++;
    }
    return count;
}

int findLastMax(page *frame, int size) {
    int tmp=0,s,i,j=0;
    for(i=0;i<size;i++) {
        s = frame[i].v;
        if(s > tmp) {
            tmp = s;
            j = i;
        }
    }
    return j;
}


int findLastMin(page *frame, int size) {
    int tmp=frame[0].v,s,i,j=0;
    //printf("| %d ", tmp);
    for(i=1;i<size;i++) {
        s = frame[i].v;
       // printf("| %d ", s);
        if(s < tmp) {
            tmp = s;
            j = i;
        }
    }
   // printf("\n");
    return j;
}

int fifo(int fsize, page *frame, int rsize, page **pageR) {
    int i, j=0,p=0;
    int f=0;
    for(i=0;i<fsize;i++) frame[i].n = -1;//初始化驻留集中所有框为-1
    for(i=0;i<rsize;i++) {
        if(Search(pageR[i]->n, frame, fsize)!=-1);//在驻留集中的情况，直接跳转打印
        else if(i < fsize || p < fsize) {
            frame[p].n=pageR[i]->n;//不在驻留集中，但驻留集中有空位置，那就直接添加上
            p++;  //p是操作内存块驻留集的
        }
        else {
            frame[j%fsize].n = pageR[i]->n;//驻留集中没有空位了，那么就开始置换
            j++; //j是控制置换算法的
            f++; //记录缺页数
        }
        print(i, frame, fsize);
    }//遍历所有可能的输入
    printf("page fault : %d\n", f);

}
//替换出最久没有被访问的页面
int lru(int fsize, page *frame, int rsize, page **pageR) {
    int i, j, p=0, q;
    int f=0;
    for(i=0;i<fsize;i++) {
        frame[i].n = -1;
        frame[i].v = 0;
    }//初始化驻留集
    for(i=0;i < rsize;i++) {
        for(j=0;j < fsize;j++) {
            if(frame[j].n!=-1) frame[j].v++; //每一次都将驻留集中每个frame的访问位++
        }
        //每次都要遍历检查，是否被访问了，被访问了就让v为0
        q = Search(pageR[i]->n, frame, fsize);
        if(q!=-1) frame[q].v=0; //驻留集中存在该页面，将访问位置0
        else if(i<fsize || p<fsize) { //有空位
            frame[p].n=pageR[i]->n;
            p++;
        }else {  //没空位，执行替换算法
            q = findLastMax(frame, fsize);
            frame[q].n = pageR[i]->n;
            frame[q].v = 0;
            f++;
        }
        print(i, frame, fsize);
   }//遍历操作
   printf("page fault : %d\n", f);
}
int OPT(int fsize, page *frame, int rsize, page **pageR){
    int i,j=0,p=0,q;
    int f=0;
    int pos;
    int len=-1;
    int flag=0;
    //初始化
    for(i=0;i<fsize;i++){
        frame[i].n=-1;
        frame[i].v=0;
    }
    //遍历
    for(i=0;i<rsize;i++){
        q = Search(pageR[i]->n,frame,fsize);
        //驻留集中已经存在
        if(q!=-1);
        //驻留集中有空余
        else if(i<fsize || p<fsize){
            frame[p].n=pageR[i]->n;
            p++;
        }
        //置换
        else{
            //看后面的，遍历后面的，找到最近不会再使用的
            int k,k2;
            for(k=0;k<fsize;k++){ //先遍历内存驻留集
                if(flag==1)
                    break;
                for(k2=i+1;k2 < rsize;k2++){ //对于内存中的每个页面，遍历未来的页面访问序列
                    if(pageR[k2]->n==frame[k].n){
                        //找到最远的
                        if(k2-i>len){ //用变量len来维护最长距离
                            len=k2-i;
                            pos=k; //pos维护应该换出的页面
                        }
                    }
                    //如果驻留集里面的这个页不再被使用，那就用它来替换,即遍历到了r的最后一个了
                    else if(k2 == rsize-1 && len!=99999){//并且要保证它是第一个以后都不被用的
                        flag=1;
                        len=99999;
                        pos=k;
                        //frame[k].n=pageR[i]->n;
                        //f++;
                        //break;
                    }
                }
            }
            frame[pos].n=pageR[i]->n;
            f++;

        }
        print(i,frame,fsize);
    }
    printf("page fault:%d\n",f);

}
int Clock(int fsize, page *frame, int rsize, page **pageR){
    int i,j,p=0,q;
    int ptr=0;//工作指针
    int f=0;
    //初始化
    for(i=0;i<fsize;i++){
        frame[i].n=-1;
        frame[i].v=0;
    }
    for(i=0;i<rsize;i++){
        //驻留集中已经有它存在
        q = Search(pageR[i]->n,frame,fsize);
        if(q != -1){
            frame[q].v=1;//此时只需要将v设置为1，其它都不变
        }
        //驻留集中有空白块的情况
        else if(i<fsize || p<fsize){
            frame[p].n=pageR[i]->n;
            frame[p].v=1;
            p++;
            ptr++;//指针下移
            ptr=ptr%fsize;
        }
        //置换的情况
        else{
            int k;
            //开始遍历驻留集，指针跳转，最多跳一个驻留集+1的长度
            for(k=0;k<=fsize;k++){
                //找到了
                if(frame[ptr].v==0){ //找到访问位为0的页面，换出
                    frame[ptr].n=pageR[i]->n;
                    frame[ptr].v=1;
                    ptr++;
                    ptr=ptr%fsize;
                    f++; //缺页数++
                    break;
                }
                //没找到
                else{
                    frame[ptr].v=0; //把访问位从1置为0
                    ptr++;
                    ptr=ptr%fsize;
                }
            }
        }
        print(i,frame,fsize);
    }//遍历
    printf("page fault :%d\n",f);

}
int main() {
    int choice = 0;
    int logSize;
    int phySize;
    page *program;
    page *frame;
    page **pageR; //指向访问页面序列
    int prSize; //访问序列长度

    srand((int)time(0));
    printf("Enter number of pages in program: ");
    scanf("%d", &logSize);

    printf("Enter number of frames in physical memory: ");
    scanf("%d", &phySize);

    program = (page *)malloc(sizeof(int)*2*logSize);
    frame = (page *)malloc(sizeof(int)*2*phySize);

    prSize = logSize * MULTIPLE;
    pageR = (page **)malloc(sizeof(int*)*prSize);
    buildPageReference(prSize, pageR, program);

    do {
        choice = getchoice("Please select an action", menu);
        printf("You have chosen: %c\n", choice);
        switch(choice) {
        case 'f' :
            fifo(phySize, frame, prSize, pageR);
            break;
        case 'r' :
            lru(phySize, frame, prSize, pageR);
            break;
        case 'o' :
            OPT(phySize, frame, prSize, pageR);
            break;
        case 'c' :
            Clock(phySize, frame, prSize, pageR);
            break;
        default: break;
        }
    }while(choice != 'q');
    exit(0);
}