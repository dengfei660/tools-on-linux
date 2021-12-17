#include <sys/time.h>
#include <stdarg.h>
#include <sys/types.h>
#include <limits.h>
#include <time.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>

#include <unistd.h>
#include "Times.h"
#include "ErrorCode.h"
#include "Poll.h"
#include "Logger.h"

#define TAG "Poll"

namespace Tls {

#define INCREASE_ACTIVE_FDS 8

Poll::Poll(bool controllable)
      : mControllable(controllable)
{
    mFds = NULL;
    mFdsCnt = 0;
    mFdsMaxCnt = 0;

    mActiveFds = NULL;
    mActiveFdsCnt = 0;

    mWaiting.store(0);
    mControlPending.store(0);
    mFlushing.store(0);

    //init control sockets
    int control_sock[2];

    if (socketpair (PF_UNIX, SOCK_STREAM, 0, control_sock) < 0) {
        mControlReadFd = -1;
        mControlWriteFd = -1;
    } else {
        mControlReadFd = control_sock[0];
        mControlWriteFd = control_sock[1];
    }
    addFd(mControlReadFd);
    setFdReadable(mControlReadFd,true);
}

Poll::~Poll()
{
    if (mActiveFds) {
        free(mActiveFds);
        mActiveFds = NULL;
    }

    if (mControlWriteFd >= 0) {
        close (mControlWriteFd);
    }

    if (mControlReadFd >= 0) {
        close (mControlReadFd);
    }

    if (mFds) {
        free(mFds);
        mFds = NULL;
    }
}

int Poll::addFd(int fd)
{
    Tls::Mutex::Autolock _l(mMutex);
    struct pollfd *pfd = findFd(fd);
    if (pfd) {
        return NO_ERROR;
    }

    if ((mFdsCnt + 1) > mFdsMaxCnt) {
        mFdsMaxCnt += INCREASE_ACTIVE_FDS;
        mFds = (struct pollfd *)realloc(mFds,mFdsMaxCnt*sizeof(struct pollfd));
        if (!mFds) {
            ERROR("NO memory");
            return ERROR_NO_MEMORY;
        }
    }

    mFds[mFdsCnt].fd = fd;
    mFds[mFdsCnt].events = POLLERR | POLLNVAL | POLLHUP;
    mFds[mFdsCnt].revents = 0;
    mFdsCnt++;
    //DEBUG("mFds:%p,maxcnt:%d,cnt:%d",mFds,mFdsMaxCnt,mFdsCnt);
    return NO_ERROR;
}

int Poll::removeFd(int fd)
{
    Tls::Mutex::Autolock _l(mMutex);
    for (int i = 0; i < mFdsCnt; i++) {
        struct pollfd *pfd = &mFds[i];
        if (pfd->fd == fd) {
            memmove(mFds+i*sizeof(struct pollfd),mFds+(i+1)*sizeof(struct pollfd), 1);
            mFdsCnt--;
            return NO_ERROR;
        }
    }
    return NO_ERROR;
}

int Poll::setFdReadable(int fd, bool readable)
{
    Tls::Mutex::Autolock _l(mMutex);
    struct pollfd * pfd = findFd(fd);
    if (!pfd) {
        return ERROR_BAD_VALUE;
    }
    if (readable) {
        pfd->events |= POLLIN;
    } else {
        pfd->events &= ~POLLIN;
    }

    return NO_ERROR;
}

int Poll::setFdWritable(int fd, bool writable)
{
    Tls::Mutex::Autolock _l(mMutex);
    struct pollfd * pfd = findFd(fd);
    if (!pfd) {
        return ERROR_BAD_VALUE;
    }
    if (writable) {
        pfd->events |= POLLOUT;
    } else {
        pfd->events &= ~POLLOUT;
    }

    return NO_ERROR;
}

int Poll::wait(int64_t timeoutNs /*nanosecond*/)
{
    int oldwaiting;
    int size = 0;

    oldwaiting = mWaiting.load();

    mWaiting.fetch_add(1);

    if (oldwaiting > 0) { //had other thread waiting
        goto tag_already_waiting;
    }

    if (mFlushing.load()) {
        goto tag_flushing;
    }

    //lock when set nfds,realloc active nfds
    {
        Tls::Mutex::Autolock _l(mMutex);
        mActiveFdsCnt = mFdsCnt;
        mActiveFds = (struct pollfd *)realloc(mActiveFds, mActiveFdsCnt*sizeof(struct pollfd));
        if (!mActiveFds) { //no memory???
             goto tag_success;
        }
        memcpy(mActiveFds, mFds, mActiveFdsCnt*sizeof(struct pollfd));
        //DEBUG("activefds:%p,mActiveFdsCnt:%d",mActiveFds,mActiveFdsCnt);
    }

    do {
        int64_t t = -1; //nanosecond
        if (timeoutNs > 0) {
            t = timeoutNs;
        }
        //DEBUG("waiting");
        mActiveFdsCnt = poll(mActiveFds, mActiveFdsCnt, t);
        //DEBUG("waiting end");
        if (mFlushing.load()) {
            goto tag_flushing;
        }
    } while(0);

tag_success:
    mWaiting.fetch_sub(1);
    return mActiveFdsCnt;
tag_already_waiting:
    mWaiting.fetch_sub(1);
    errno = EPERM;
    return -1;
tag_flushing:
    mWaiting.fetch_sub(1);
    errno = EBUSY;
    return -1;
}

void Poll::setFlushing(bool flushing)
{
    /* update the new state first */
    if (flushing) {
        mFlushing.store(1);
    } else {
        mFlushing.store(0);
    }

    if (mFlushing.load() && mControllable && mWaiting.load() > 0) {
        /* we are flushing, controllable and waiting, wake up the waiter. When we
        * stop the flushing operation we don't clear the wakeup fd here, this will
        * happen in the _wait() thread. */
        raiseWakeup();
    }
}

struct pollfd * Poll::findFd(int fd)
{
    for (int i = 0; i < mFdsCnt; i++) {
        struct pollfd *pfd = &mFds[i];
        if (pfd->fd == fd) {
            return pfd;
        }
    }
    return NULL;
}

bool Poll::wakeEvent()
{
    ssize_t num_written;
    while ((num_written = write (mControlWriteFd, "W", 1)) != 1) {
        if (num_written == -1 && errno != EAGAIN && errno != EINTR) {
            FATAL("failed to wake event: %s", strerror (errno));
            return false;
        }
    }
    return true;
}

bool Poll::releaseEvent()
{
    char buf[1] = { '\0' };
    ssize_t num_read;
    while ((num_read = read (mControlReadFd, buf, 1)) != 1) {
        if (num_read == -1 && errno != EAGAIN && errno != EINTR) {
            FATAL ("failed to release event: %s", strerror (errno));
            return false;
        }
    }
     return true;
}
bool Poll::raiseWakeup()
{
    bool result = true;

    /* makes testing control_pending and WAKE_EVENT() atomic. */
    Tls::Mutex::Autolock _l(mMutex);
    //DEBUG("mControlPending:%d",mControlPending.load());
    if (mControlPending.load() == 0) {
        /* raise when nothing pending */
        //GST_LOG ("%p: raise", set);
        result = wakeEvent();
    }

    if (result) {
        mControlPending.fetch_add(1);
    }

     return result;
}

bool Poll::releaseWakeup()
{
    bool result = false;

    /* makes testing/modifying control_pending and RELEASE_EVENT() atomic. */
    Tls::Mutex::Autolock _l(mMutex);

    if (mControlPending.load() > 0) {
        /* release, only if this was the last pending. */
        if (mControlPending.load() == 1) {
            //GST_LOG ("%p: release", set);
            result = releaseEvent();
        } else {
            result = true;
        }

        if (result) {
            mControlPending.fetch_sub(1);
        }
    } else {
        errno = EWOULDBLOCK;
    }

    return result;
}

bool Poll::releaseAllWakeup()
{
    bool result = false;

    /* makes testing/modifying control_pending and RELEASE_EVENT() atomic. */
    Tls::Mutex::Autolock _l(mMutex);

    if (mControlPending.load() > 0) {
        //GST_LOG ("%p: release", set);
        result = releaseEvent();
        if (result) {
            mControlPending.store(0);
        }
    } else {
        errno = EWOULDBLOCK;
    }

    return result;
}

}