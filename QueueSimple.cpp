#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <Mutex.h>

#include <Queue.h>
#include "Thread.h"
#include "Times.h"
#include "Logger.h"

using namespace Tls;
#define MAX_CNT 100//(100000000)
#define MAX_TYPE (1)
#define TAG "QueueSimple"

Tls::Mutex g_mutex;

typedef struct {
    int type;
    int value;
} Test;

void free_test(void *tt) {
    Test *t = (Test *)tt;
    if(t != NULL) {
        free(t);
    }
}

int cmp_int_ptr(Test *a, Test *b) {
    if(a->value < b->value)
        return -1;
    else if(a->value > b->value)
        return 1;
    else
        return 0;
}

void unsorted_mode() {
    Queue *q = new Queue();

}

void sorted_mode() {
    Queue *q = new Queue(true, (int (*)(void *, void *))cmp_int_ptr);
    
}

void sorted2_mode() {
    Queue *q = new Queue(3, 1, (int (*)(void *, void *))cmp_int_ptr);

}

class ProduceThread:public Thread {
  public:
    ProduceThread(int8_t type, Queue *q) {
        printf("ProduceThread new,type：%d\n",type);
        mType = type;
        cnt = 1;
        mQueue = q;
        allCnt = 0;
    };
    virtual ~ProduceThread(){
        printf("ProduceThread del\n");
    };
    void readyToRun(){
        mStartTimeNs = Tls::Times::getSystemTimeNs();
    };
    void readyToExit() {
        mStopTimeNs = Tls::Times::getSystemTimeNs();
        int64_t time = (mStopTimeNs - mStartTimeNs)/MAX_CNT;
        printf("push %d time:%lld ns\n",MAX_CNT,time);
    }
    virtual bool threadLoop(){
        Test *intPtr = (Test *)malloc(sizeof(Test));
        if (!intPtr) {
            printf("malloc failed\n");
            return false;
        }

        intPtr->type = mType;
        intPtr->value = cnt;

        int ret = mQueue->pushAndWait(intPtr);
        if (ret == Q_OK) {
            ++allCnt;
            if (allCnt%100000 == 0) {
                printf("push:%d, value:%d\n",intPtr->type,intPtr->value);
            }
        }
        ++cnt;
        if (cnt > MAX_CNT) {
            printf("ProduceThread %d reached Max cnt:%d\n",mType, cnt);
            return false;
        }
        //usleep(1*1000);
        return true;
    };
  private:
    int8_t mType;
    int cnt;
    int allCnt;
    Queue *mQueue;
    int64_t mStartTimeNs;
    int64_t mStopTimeNs;
};

class ConsumeThread:public Thread {
  public:
    ConsumeThread(Queue *q) {
        printf("ConsumeThread new\n");
        mQueue = q;
        allCnt = 0;
        for (int i = 0; i < MAX_TYPE; i++) {
            mStatics[i] = 1;
        }
    };
    virtual ~ConsumeThread(){
        printf("ConsumeThread del\n");
    };
        void readyToRun(){
        mStartTimeNs = Tls::Times::getSystemTimeNs();
    };
    void readyToExit() {
        mStopTimeNs = Tls::Times::getSystemTimeNs();
        int64_t time = (mStopTimeNs - mStartTimeNs)/MAX_CNT;
        printf("pop %d time:%lld ns\n",MAX_CNT,time);
    }
    virtual bool threadLoop(){
        Test *test;
        int ret = mQueue->popAndWait((void **) &test);
        if (ret != Q_OK) {
            printf("pop element failed\n");
            return false;
        }

        if (test->value%100000 == 0) {
            printf("pop:%d,value:%d\n",test->type,test->value,test);
        }
        if (test->value >= MAX_CNT) {
            printf("ConsumeThread %d reached max cnt %d\n",test->type, test->value);
            return false;
        }

        free(test);

        return true;
    };
  private:
    int allCnt;
    int mStatics[MAX_TYPE];
    Queue *mQueue;
    int64_t mStartTimeNs;
    int64_t mStopTimeNs;
};

int main(int argc, char *argv[]) {
    int mStatics[MAX_TYPE];
    //unsorted_mode();
    //sorted_mode();
    //sorted2_mode();

    //Queue *q = new Queue();
    //q->test();
#if 1
for(int j = 0; j < 1; j++) {
    Queue *q = new Queue();
    ConsumeThread *consumeThread[MAX_TYPE];
    ProduceThread *produceThread[MAX_TYPE];
    for (int i = 0; i < MAX_TYPE; i++) {
        mStatics[i] = 1;
        consumeThread[i] = new ConsumeThread(q);
        consumeThread[i]->run("consumeThread");
    }

    for (int i = 0; i < MAX_TYPE; i++) {
        produceThread[i] = new ProduceThread(i, q);
        produceThread[i]->run("produceThread");
    }

    //printf("ready all run\n");
    //usleep(1000*1000);
    printf("all run\n");

    //getchar();
    int proFound = 0;
    int comFound = 0;
    while (proFound < MAX_TYPE && comFound < MAX_TYPE) {
        proFound = 0;
        comFound = 0;
        for (int i = 0; i < MAX_TYPE; i++) {
            if (!produceThread[i]->isRunning()) {
                ++proFound;
            }
            if (!consumeThread[i]->isRunning()) {
                ++comFound;
            }
        }
        if (proFound >= MAX_TYPE && comFound >= MAX_TYPE)
            break;
        usleep(1000*1000);
    }

    //getchar();
    q->setAllowedNewData(false);
    delete q;
    printf("to exit thread\n");
    for (int i = 0; i < MAX_TYPE; i++) {
        delete produceThread[i];
        delete consumeThread[i];
    }

    
    printf("xxxxxxxxxxx\n");
    usleep(5*1000*1000);
}
#endif
    getchar();
    return 0;
}

