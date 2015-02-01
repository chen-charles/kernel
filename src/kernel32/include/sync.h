#ifndef __SYNC__
#define __SYNC__

#include    "type.h"
#include    "_object/_object.h"
#include    "mm.h"


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
    virtual char* getName() = 0;    //c-type str
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
    
    char* getName(){return 0;}
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

#endif
