#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#include <Queue.h>
#include "Thread.h"

using namespace Tls;

typedef struct {
	char *test;
} test;

void free_test(void *tt) {
    test *t = (test *)tt;
	if(t->test != NULL) {
        printf("free string %s\n", t->test);
		free(t->test);
    }
	free(tt);
}

int cmp_int_ptr(int *a, int *b) {
	if(*a < *b)
		return -1;
	else if(*a > *b)
		return 1;
	else
		return 0;
}

void unsorted_mode() {
	Queue *q = new Queue();
	
	char *t1 = (char *)malloc(5);
	char *t2 = (char *)malloc(10);
	char *t3 = (char *)malloc(15);
	char *t4 = (char *)malloc(8);

    memset(t1, 0, 5);
    memset(t2, 0, 10);
    memset(t3, 0, 15);
    memset(t4, 0, 8);

    strcat(t1, "abc1");
    strcat(t2, "desd2");
    strcat(t3, "sdbdsd3");
    strcat(t4, "dcdeg4");
	
	test *s1 = (test *)malloc(sizeof(test));
	s1->test = t1; 
	test *s2 = (test *)malloc(sizeof(test));
	s2->test = t2; 
	test *s3 = (test *)malloc(sizeof(test));
	s3->test = t3; 
	test *s4 = (test *)malloc(sizeof(test));
	s4->test = t4; 
	
	q->push(s1);
    q->push(s2);
    q->push(s3);
    q->push(s4);
	
	test *t;
	q->pop((void **)&t);
    printf("first string %s\n", t->test);
	free_test(t);
	q->pop((void **)&t);
    printf("second string %s\n", t->test);
	free_test(t);
	
	q->flushAndFreeData(free_test);
    delete q;
}

void sorted_mode() {
	Queue *q = new Queue(true, (int (*)(void *, void *))cmp_int_ptr);
	
	int *t1 = (int *)malloc(sizeof(int));
	int *t2 = (int *)malloc(sizeof(int));
	int *t3 = (int *)malloc(sizeof(int));
	int *t4 = (int *)malloc(sizeof(int));
	
	*t1 = 10;
	*t2 = 12;
	*t3 = 1;
	*t4 = 1;
	
	q->push(t1);
    q->push(t2);
    q->push(t3);
    q->push(t4);
	
	int *t;
	q->pop((void **)&t);
	printf("first int %i\n", *t);
	free(t);
	q->pop((void **)&t);
	printf("second int %i\n", *t);
	free(t);
	q->pop((void **)&t);
	printf("third int %i\n", *t);
	free(t);
	q->pop((void **)&t);
	printf("fourth int %i\n", *t);
	free(t);
	
    q->flush();
    delete q;
}

void sorted2_mode() {
	Queue *q = new Queue(3, 1, (int (*)(void *, void *))cmp_int_ptr);
	
	int t1 = 1;
    q->push(&t1);
	int t2 = 15;
    q->push(&t2);
	int t3 = 3;
    q->push(&t3);
	int t4 = 27;
    q->push(&t4);
	int t5 = 9;
    q->push(&t5);
  
    int *i;
    q->pop((void **)&i);
    printf("first element was %d\n", *i);
    q->pop((void **)&i);
    printf("second element was %d\n", *i);
    q->pop((void **)&i);
    printf("third element was %d\n", *i);
    q->pop((void **)&i);
    printf("fourth element was %p\n", i);
    q->pop((void **)&i);
    printf("fifth element was %p\n", i);
  
	q->flush();
    delete q;
}

class ProduceThread:public Thread {
  public:
    ProduceThread(Queue *q) {
        printf("ProduceThread new\n");
        cnt = 0;
        mQueue = q;
    };
    virtual ~ProduceThread(){
        printf("ProduceThread del\n");
    };
    void readyToRun(){
        
    };
    virtual bool threadLoop(){
        printf("push cnt:%d\n",cnt);
        mQueue->push(&cnt);
        cnt++;
        usleep(500*1000);
        return true;
    };
  private:
    int cnt;
	Queue *mQueue;
};

class ResumeThread:public Thread {
  public:
    ResumeThread(Queue *q) {
        printf("ResumeThread new\n");
        mQueue = q;
    };
    virtual ~ResumeThread(){
        printf("ResumeThread del\n");
    };
    void readyToRun(){};
    virtual bool threadLoop(){
        int *cnt = 0;
        int ret = mQueue->popAndWait((void **) &cnt);
        if (ret == Q_OK)
            printf("pop cnt:%d\n",*cnt);
        else
            usleep(500*1000);
        return true;
    };
  private:
    
	Queue *mQueue;
};

int main(int argc, char *argv[]) {
	//unsorted_mode();
	//sorted_mode();
	//sorted2_mode();

    Queue *q = new Queue();
    ResumeThread *resumeThread = new ResumeThread(q);
    resumeThread->run("resumeThread");

    getchar();

    ProduceThread *produceThread = new ProduceThread(q);
    produceThread->run("produceThread");

    getchar();
    q->setAllowedNewData(false);
    getchar();
    printf("to exit thread\n");
    resumeThread->requestExitAndWait();
    printf("exit resume thread\n");
    produceThread->requestExitAndWait();
    

    delete produceThread;
    delete resumeThread;
    delete q;
        
	
	return 0;
}

