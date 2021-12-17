#include <unistd.h>


#include "Queue.h"

namespace Tls {

Queue::Queue()
{
    int32_t ret = init_internal();
}

Queue::Queue(uint32_t maxElement)
{
    init_internal();
    mMaxElement = maxElement;
}

Queue::Queue(bool ascendingOrder, int (*cmp)(void *, void *))
{
    init_internal();
    mSort = true;
	mAscendingOrder = ascendingOrder;
	cmp_ele = cmp;
}

Queue::Queue(uint32_t maxElement, bool ascendingOrder, int (*cmp)(void *, void *))
{
    init_internal();
    mMaxElement = maxElement;
    mSort = true;
	mAscendingOrder = ascendingOrder;
	cmp_ele = cmp;
}

Queue::~Queue()
{
    release_internal(0, NULL);
}

int32_t Queue::push(void *ele)
{
    if (Q_OK != lock_internal())
        return Q_ERR_LOCK;

    int8_t ret = put_internal(ele, NULL);

    if (Q_OK != unlock_internal())
		return Q_ERR_LOCK;

    return ret;
}

int32_t Queue::pushAndWait(void *ele)
{
    if (Q_OK != lock_internal())
        return Q_ERR_LOCK;

    int8_t ret = put_internal(ele, pthread_cond_wait);

    if (Q_OK != unlock_internal())
		return Q_ERR_LOCK;

    return ret;
}

int32_t Queue::pop(void **e)
{
    if (Q_OK != lock_internal())
        return Q_ERR_LOCK;
	
	int8_t ret = get_internal(e, NULL, NULL, NULL);

	if (Q_OK != unlock_internal())
		return Q_ERR_LOCK;

	return ret;
}

int32_t Queue::popAndWait(void **e)
{
    *e = NULL;

	if (Q_OK != lock_internal())
        return Q_ERR_LOCK;
	
	int8_t ret = get_internal(e, pthread_cond_wait, NULL, NULL);

	if (Q_OK != unlock_internal())
		return Q_ERR_LOCK;

	return ret;
}


int32_t Queue::flush()
{
    if (Q_OK != lock_internal())
        return Q_ERR_LOCK;

    int8_t ret = flush_internal(0, NULL);

    if (Q_OK != unlock_internal())
		return Q_ERR_LOCK;
    return ret;
}
int32_t Queue::flushAndFreeData(void (*ff)(void *))
{
	if (Q_OK != lock_internal())
        return Q_ERR_LOCK;

	int8_t ret = flush_internal(true, ff);

	if (Q_OK != unlock_internal())
		return Q_ERR_LOCK;

	return ret;
}


uint32_t Queue::getCnt()
{
    uint32_t cnt = 0;
    if (Q_OK != lock_internal())
        //return Q_ERR_LOCK;

    cnt = mElementCnt;

    if (Q_OK != unlock_internal())
		//return Q_ERR_LOCK;

    return cnt;
}


bool Queue::isEmpty()
{
    bool isEmpty;
        
    if (Q_OK != lock_internal())
        //return Q_ERR_LOCK;
    
	if(mFirstElement == NULL || mLastElement == NULL)
		isEmpty = true;
	else
		isEmpty = false;
	
	if (Q_OK != unlock_internal())
		//return Q_ERR_LOCK;

	return isEmpty;
}


int32_t Queue::setAllowedNewData(bool allowed)
{
    if (Q_OK != lock_internal())
        return Q_ERR_LOCK;

    mAllowedNewData = allowed;

    if (Q_OK != unlock_internal())
		return Q_ERR_LOCK;

    if(mAllowedNewData == false) {
		// notify waiting threads, when new data isn't accepted
		if (mCondGet) {
		    pthread_cond_broadcast(mCondGet);
        }
        if (mCondPut) {
		    pthread_cond_broadcast(mCondPut);
        }
	}

	return Q_OK;
}

bool Queue::isAllowedNewData()
{
	if (Q_OK != lock_internal())
        return Q_ERR_LOCK;

	bool allowed = mAllowedNewData;

	if (Q_OK != unlock_internal())
		return Q_ERR_LOCK;

	return allowed;
}


int32_t Queue::init_internal()
{
    mMutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	if(mMutex == NULL) {
		return Q_ERR_MEM;
	}
	pthread_mutex_init(mMutex, NULL);
		
	mCondGet = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));
	if(mCondGet == NULL) {
		pthread_mutex_destroy(mMutex);
		free(mMutex);
		return Q_ERR_MEM;
	}
	pthread_cond_init(mCondGet, NULL);

	mCondPut = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));
	if(mCondPut == NULL) {
		pthread_cond_destroy(mCondGet);
		free(mCondGet);
		pthread_mutex_destroy(mMutex);
		free(mMutex);
		return Q_ERR_MEM;
	}
	pthread_cond_init(mCondPut, NULL);
		
	mFirstElement = NULL;
	mLastElement = NULL;
	mElementCnt = 0;
    mMaxElement = 0;
	mAllowedNewData = true;
	mSort = false;
	mAscendingOrder = 1;
	cmp_ele = NULL;
}

int32_t Queue::release_internal(bool canFreeData, void(*ff)(void *))
{
    // this method will not immediately return on error,
	// it will try to release all the memory that was allocated.
	int error = Q_OK;
	// make sure no new data comes and wake all waiting threads
	error = setAllowedNewData(false);
	error = lock_internal();
	
	// release internal element memory
	error = flush_internal(canFreeData, ff);
	
	// destroy lock and queue etc
	error = pthread_cond_destroy(mCondGet);
	free(mCondGet);
	error = pthread_cond_destroy(mCondPut);
	free(mCondPut);
	
	error = unlock_internal();
	while(EBUSY == (error = pthread_mutex_destroy(mMutex)))
		usleep(100*1000);

	free(mMutex);
	
	return error;
}

int32_t Queue::lock_internal()
{
    // all errors are unrecoverable for us
	if(0 != pthread_mutex_lock(mMutex))
		return Q_ERR_LOCK;
	return Q_OK;
}
int32_t Queue::unlock_internal()
{
    // all errors are unrecoverable for us
	if(0 != pthread_mutex_unlock(mMutex))
		return Q_ERR_LOCK;
	return Q_OK;
}

int32_t Queue::flush_internal(bool canFreeData, void (*ff)(void *))
{
    Element *qe = mFirstElement;
	Element *nqe = NULL;
	while(qe != NULL) {
		nqe = qe->next;
		if(canFreeData && ff == NULL) {
			free(qe->data);
		} else if(canFreeData && ff != NULL) {
			ff(qe->data);
		}
		free(qe);
		qe = nqe;
	}
	mFirstElement = NULL;
	mLastElement = NULL;
	mElementCnt = 0;
	
	return Q_OK;
}

int32_t Queue::put_internal(void *ele, int (*action)(pthread_cond_t *, pthread_mutex_t *))
{	
	if(mAllowedNewData == false) { // no new data allowed
		return Q_ERR_NONEWDATA;
	}
	
	// max_elements already reached?
	// if condition _needs_ to be in sync with while loop below!
	if(mElementCnt == (INT32_MAX - 1) || (mMaxElement != 0 && mElementCnt == mMaxElement)) {
		if(action == NULL) {
			return Q_ERR_NUM_ELEMENTS;
		} else {
			while ((mElementCnt == (INT32_MAX - 1) || (mMaxElement != 0 && mElementCnt == mMaxElement)) && mAllowedNewData) {
				action(mCondPut, mMutex);
            }
			if(mAllowedNewData == false) {
				return Q_ERR_NONEWDATA;
			}
		}
	}
	
	Element *new_el = (Element *)malloc(sizeof(Element));
	if(new_el == NULL) { // could not allocate memory for new elements
		return Q_ERR_MEM;
	}
	new_el->data = ele;
	new_el->next = NULL;
	
	if(mSort == false || mFirstElement == NULL) {
		// insert at the end when we don't want to sort or the queue is empty
		if(mLastElement == NULL)
			mFirstElement = new_el;
		else
			mLastElement->next = new_el;
		mLastElement = new_el;
	} else {
		// search appropriate place to sort element in
		Element *s = mFirstElement; // s != NULL, because of if condition above
		Element *t = NULL;
		int asc_first_el = (mAscendingOrder == true && cmp_ele(s->data, ele) >= 0);
		int desc_first_el = (mAscendingOrder == false && cmp_ele(s->data, ele) <= 0);
		
		if(asc_first_el == 0 && desc_first_el == 0) {
			// element will be inserted between s and t
			for(s = mFirstElement, t = s->next; s != NULL && t != NULL; s = t, t = t->next) {
				if(mAscendingOrder != false && cmp_ele(s->data, ele) <= 0 && cmp_ele(ele, t->data) <= 0) {
					// asc: s <= e <= t
					break;
				} else if(mAscendingOrder == false && cmp_ele(s->data, ele) >= 0 && cmp_ele(ele, t->data) >= 0) {
					// desc: s >= e >= t
					break;
				}
			}
			// actually insert
			s->next = new_el;
			new_el->next = t;
			if(t == NULL)
				mLastElement = new_el;
		} else if(asc_first_el != 0 || desc_first_el != 0) {
			// add at front
			new_el->next = mFirstElement;
			mFirstElement = new_el;
		}
	}
	mElementCnt++;
	// notify only one waiting thread, so that we don't have to check and fall to sleep because we were to slow
	pthread_cond_signal(mCondGet);
	
	return Q_OK;
}

int32_t Queue::get_internal(void **e, int (*action)(pthread_cond_t *, pthread_mutex_t *), int (*cmp)(void *, void *), void *cmpel)
{
    // are elements in the queue?
	if(mElementCnt == 0) {
		if(action == NULL) {
			*e = NULL;
			return Q_ERR_NUM_ELEMENTS;
		} else {
			while(mElementCnt == 0 && mAllowedNewData){
				action(mCondGet, mMutex);
            }
			if (mElementCnt == 0 && mAllowedNewData == false) {
				return Q_ERR_NONEWDATA;
             }
		}
	}
	
	// get first element (which fulfills the requirements)
	Element *el_prev = NULL, *el = mFirstElement;
	while(cmp != NULL && el != NULL && 0 != cmp(el, cmpel)) {
		el_prev = el;
		el = el->next;
	}
	if(el != NULL && el_prev == NULL) {
		// first element is removed
		mFirstElement = el->next;
		if(mFirstElement == NULL)
			mLastElement = NULL;
		mElementCnt--;
		*e = el->data;
		free(el);
	} else if(el != NULL && el_prev != NULL) {
		// element in the middle is removed
		el_prev->next = el->next;
		mElementCnt--;
		*e = el->data;
		free(el);
	} else {
		// element is invalid
		*e = NULL;
		return Q_ERR_INVALID_ELEMENT;
	}

	// notify only one waiting thread
	pthread_cond_signal(mCondPut);

	return Q_OK;
}



}
