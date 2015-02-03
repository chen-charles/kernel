#ifndef __SYNC__
#define __SYNC__

#include    "type.h"
#include    "_object/_object.h"
#include    "mm.h"
#include    "Vector.h"


class Waitable: public _object
{
public:
    Waitable(uint64_t pID, bool isPrivate)
    {
        this->pID = pID;
        this->isPriv = isPrivate;
    }
    
    Waitable(): Waitable(0, false)
    {
        
    }
    
    ~Waitable()
    {
        
    }
    
    uint64_t getCreatorProcessID()
    {
        return this->pID;
    }
    
    bool isPrivate()
    {
        return this->isPriv;
    }
    
    virtual bool isTimeout() const = 0;
    virtual char* getName() //c-type str
    {
        return (char*)0;
    }    
    virtual bool isFinished() const = 0;
    virtual bool wait() = 0;        //should WaitForAnSingleObject return now ? true:false
private:
    uint64_t pID;
    bool isPriv;
    
protected:
    
};

class Timer: public Waitable
{
public:
    
    enum TimerMode
    {
        MOD_ONESHOT, MOD_PERIODIC, UNSPECIFIED
    };
    
    
    Timer(uint64_t pID, bool isPrivate): Waitable(pID, isPrivate)
    {
        mode = Timer::TimerMode::UNSPECIFIED;
        this->isTurnFinished = true;
    }
    
    Timer(): Waitable()
    {
        mode = Timer::TimerMode::UNSPECIFIED;
        this->isTurnFinished = true;
    }
    
    void setTimer(Timer::TimerMode mode, int interval)
    {
        this->mode = mode;
        this->interval = interval;
    }
    
    void start()
    {
        isTurnFinished = false;
        startTime = SYS_INTERNAL_TIME;
    }
    
    void end()
    {
        isTurnFinished = true;
    }
    
    bool isFinished() const
    {
        return this->isTurnFinished;
    }
    
    bool isTimeout() const
    {
        return false;
    }
    
    bool wait()
    {
        if (mode == Timer::TimerMode::UNSPECIFIED) return false;
        if (isTurnFinished) return false;
        
        switch (mode)
        {
        case MOD_ONESHOT:
            if (startTime+interval >= SYS_INTERNAL_TIME)
            {
                isTurnFinished = true;
                return true;
            }
            break;
        case MOD_PERIODIC:
            if (startTime+interval >= SYS_INTERNAL_TIME)
            {
                startTime = SYS_INTERNAL_TIME;
                return true;
            }
            break;
        }
        return false;
    }
    
    ~Timer()
    {
        
    }
    

    
private:
    TimerMode mode;
    bool isTurnFinished;
    int interval;
    int startTime;
};

class Mutex: public Waitable
{
public:
    Mutex(uint64_t pID, bool isPrivate): Waitable(pID, isPrivate)
    {
    }
    
    Mutex(): Waitable()
    {
    }
    
    bool open(int timeout)
    {
        this->startTime = SYS_INTERNAL_TIME;
        this->timeout = timeout;
        if (!this->isInUse) 
        {
            this->isInUse = true;
            return true;
        }
        else
        {
            //call system routine(fetch process id, then add waitable, then set state)
            //using the dumb way here
            while(!wait());
            if (!isTimeout()) 
            {
                isInUse = true;
                return true;
            }
            else return false;
        }
    }
    
    void release()
    {
        this->isInUse = false;
    }
    
    bool isTimeout() const
    {
        return startTime+timeout >= SYS_INTERNAL_TIME;
    }

    bool isFinished() const
    {
        return false;
    }
    
    bool wait()
    {
        if (isTimeout()) return true;
        else
        {
            if (!isInUse)
            {
                return true;
            }
            else return false;
        }
    }
    
private:
    bool isInUse = false;
    int timeout;
    int startTime;
};


//extern "C" Scheduler* scheduler;   //cpp_entry init

static Vector<Waitable*> *g_waitables_p;
extern "C" void init_sync();


#endif

