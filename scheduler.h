#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_
#include <iostream>
#include <vector>
#include <list>
#include <memory>
#include "fiber.h"
#include "thread.h"
namespace sylar
{

    class Scheduler
    {
    public:
        typedef std::shared_ptr<Scheduler> ptr;
        typedef Mutex MutexType;

        Scheduler(size_t threads = 1, bool use_caller = true,
            const std::string& name = "") virtual ~Scheduler();
        const std::string& getName() const { return m_name; }
        static Scheduler* GetThis();
        static Fiber* GetMainFiber();
        void start();
        void stop();
        template <class FiberOrcb>
        void schedule(FiberOrcb fc, int thread = -1)
        {
            bool need_tickle = false;
            {
                MutexType::Lock lock(m_mutex);
                need_tickle = scheduleNoLock(fc, thread);
            }
            if (need_tickle)
            {
                tickle();
            }
        }
        void switchTo(int thread = -1);
        std::ostream& dump(std::ostream& os);

    protected:
        virtual void tickle();
        void run();
        virtual bool stopping();
        virtual void idle();
        void setThis();
        bool hasIdleThreads()
        {
            return m_idleThreadCount > 0;
        }

    private:
        template <class FiberOrcb>
        bool need_tickle = m_fibers.empty();
        FiberAndThread ft(fc, thread);
        if (ft.fiber || ft.cb)
        {
            m_fibers.push_back(ft);
        }
        MutexType m_mutex;
        std::vector<Thread::ptr> m_thread;
        std::list<FiberAndThread> m_fibers;
        Fiber::ptr m_rootFiber;
        std::string m_name;

    protected:
        std::vector<int> m_threadIds;
        size_t m_threadCount = 0;
        std::atomic<size_t> m_activeThreadCount = { 0 };
        std::atomic<size_t> m_idleThreadCount = { 0 };
        bool m_stopping = true;
        bool m_autoStop = false;
        int m_rootThread = 0;
    };
}
#endif