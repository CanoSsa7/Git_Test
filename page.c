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
}//�ж��Ƿ���פ������

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
    for(i=0;i<fsize;i++) frame[i].n = -1;//��ʼ��פ���������п�Ϊ-1
    for(i=0;i<rsize;i++) {
        if(Search(pageR[i]->n, frame, fsize)!=-1);//��פ�����е������ֱ����ת��ӡ
        else if(i < fsize || p < fsize) {
            frame[p].n=pageR[i]->n;//����פ�����У���פ�������п�λ�ã��Ǿ�ֱ�������
            p++;  //p�ǲ����ڴ��פ������
        }
        else {
            frame[j%fsize].n = pageR[i]->n;//פ������û�п�λ�ˣ���ô�Ϳ�ʼ�û�
            j++; //j�ǿ����û��㷨��
            f++; //��¼ȱҳ��
        }
        print(i, frame, fsize);
    }//�������п��ܵ�����
    printf("page fault : %d\n", f);

}
//�滻�����û�б����ʵ�ҳ��
int lru(int fsize, page *frame, int rsize, page **pageR) {
    int i, j, p=0, q;
    int f=0;
    for(i=0;i<fsize;i++) {
        frame[i].n = -1;
        frame[i].v = 0;
    }//��ʼ��פ����
    for(i=0;i < rsize;i++) {
        for(j=0;j < fsize;j++) {
            if(frame[j].n!=-1) frame[j].v++; //ÿһ�ζ���פ������ÿ��frame�ķ���λ++
        }
        //ÿ�ζ�Ҫ������飬�Ƿ񱻷����ˣ��������˾���vΪ0
        q = Search(pageR[i]->n, frame, fsize);
        if(q!=-1) frame[q].v=0; //פ�����д��ڸ�ҳ�棬������λ��0
        else if(i<fsize || p<fsize) { //�п�λ
            frame[p].n=pageR[i]->n;
            p++;
        }else {  //û��λ��ִ���滻�㷨
            q = findLastMax(frame, fsize);
            frame[q].n = pageR[i]->n;
            frame[q].v = 0;
            f++;
        }
        print(i, frame, fsize);
   }//��������
   printf("page fault : %d\n", f);
}
int OPT(int fsize, page *frame, int rsize, page **pageR){
    int i,j=0,p=0,q;
    int f=0;
    int pos;
    int len=-1;
    int flag=0;
    //��ʼ��
    for(i=0;i<fsize;i++){
        frame[i].n=-1;
        frame[i].v=0;
    }
    //����
    for(i=0;i<rsize;i++){
        q = Search(pageR[i]->n,frame,fsize);
        //פ�������Ѿ�����
        if(q!=-1);
        //פ�������п���
        else if(i<fsize || p<fsize){
            frame[p].n=pageR[i]->n;
            p++;
        }
        //�û�
        else{
            //������ģ���������ģ��ҵ����������ʹ�õ�
            int k,k2;
            for(k=0;k<fsize;k++){ //�ȱ����ڴ�פ����
                if(flag==1)
                    break;
                for(k2=i+1;k2 < rsize;k2++){ //�����ڴ��е�ÿ��ҳ�棬����δ����ҳ���������
                    if(pageR[k2]->n==frame[k].n){
                        //�ҵ���Զ��
                        if(k2-i>len){ //�ñ���len��ά�������
                            len=k2-i;
                            pos=k; //posά��Ӧ�û�����ҳ��
                        }
                    }
                    //���פ������������ҳ���ٱ�ʹ�ã��Ǿ��������滻,����������r�����һ����
                    else if(k2 == rsize-1 && len!=99999){//����Ҫ��֤���ǵ�һ���Ժ󶼲����õ�
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
    int ptr=0;//����ָ��
    int f=0;
    //��ʼ��
    for(i=0;i<fsize;i++){
        frame[i].n=-1;
        frame[i].v=0;
    }
    for(i=0;i<rsize;i++){
        //פ�������Ѿ���������
        q = Search(pageR[i]->n,frame,fsize);
        if(q != -1){
            frame[q].v=1;//��ʱֻ��Ҫ��v����Ϊ1������������
        }
        //פ�������пհ׿�����
        else if(i<fsize || p<fsize){
            frame[p].n=pageR[i]->n;
            frame[p].v=1;
            p++;
            ptr++;//ָ������
            ptr=ptr%fsize;
        }
        //�û������
        else{
            int k;
            //��ʼ����פ������ָ����ת�������һ��פ����+1�ĳ���
            for(k=0;k<=fsize;k++){
                //�ҵ���
                if(frame[ptr].v==0){ //�ҵ�����λΪ0��ҳ�棬����
                    frame[ptr].n=pageR[i]->n;
                    frame[ptr].v=1;
                    ptr++;
                    ptr=ptr%fsize;
                    f++; //ȱҳ��++
                    break;
                }
                //û�ҵ�
                else{
                    frame[ptr].v=0; //�ѷ���λ��1��Ϊ0
                    ptr++;
                    ptr=ptr%fsize;
                }
            }
        }
        print(i,frame,fsize);
    }//����
    printf("page fault :%d\n",f);

}
int main() {
    int choice = 0;
    int logSize;
    int phySize;
    page *program;
    page *frame;
    page **pageR; //ָ�����ҳ������
    int prSize; //�������г���

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