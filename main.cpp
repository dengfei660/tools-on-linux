#include "Log.h"
#include "Thread.h"
#include <unordered_map>
using namespace Tls;
using namespace std;

#define TAG "main"

class MyThread: public Thread {
  public:
    MyThread() {
        FATAL("thread new");
    };
    virtual ~MyThread(){
        FATAL("thread del");
    };
    void readyToRun();
    virtual bool threadLoop();
  private:
    int cnt;
	Condition mCondition;
	Mutex mMutex;
};

void MyThread::readyToRun()
{
    cnt = 0;
}
bool MyThread::threadLoop()
{
    FATAL("thread loop cnt: %d +",++cnt);
    Tls::Mutex::Autolock _l(mMutex);
	//mCondition.waitRelative(mMutex, 10000);
	FATAL("thread loop cnt: %d",cnt);
    return true;
}

struct Key{
    char *key;
};

struct Value {
   int value;
};

int main()
{
    Logger_set_level(6);
    FATAL("print test %d",1);
    ERROR("print test %d",1);
    WARNING("print test %d",1);
    INFO("print test %d",1);
    DEBUG("print test %d",1);
    TRACE1("print test %d",1);
    TRACE2("print test %d",1);
    TRACE3("print test %d",1);
    Key key;
    key.key = "key???";
    Value value;
    value.value = 122021;
    unordered_map<int64_t,int64_t> hashMap;
    pair<int64_t, int64_t> myshopping ((int64_t)&key,(int64_t)&value);
    hashMap.insert (myshopping);
    for (auto& x: hashMap) {
        Key *key = (Key *)((void *)(x.first));
        Value *value = (Value *)x.second;
        FATAL("key:%s,value:%d",key->key,value->value);
    }

    MyThread *myThread = new MyThread();
    myThread->run("te");
    getchar();
	FATAL("pause");
    myThread->pause();
	getchar();
	myThread->resume();
	getchar();
    myThread->requestExitAndWait();
    delete myThread;
    return 0;
}