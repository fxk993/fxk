#ifndef __IOMANAGER_H__
#define __IOMANAGER_H__
#include "scheduler.h"
#include "timer.h"

namespace sylar {

    
    class IOManager : public Scheduler, public TimerManager {
    public:
        typedef std::shared_ptr<IOManager> ptr;
        typedef RWMutex RWMutexType;

        
        enum Event {
            // 无事件
            NONE = 0x0,
            // 读事件(EPOLLIN)
            READ = 0x1,
            // 写事件(EPOLLOUT)
            WRITE = 0x4,
        };
    private:
        struct FdContext {
            typedef Mutex MutexType;
           
            struct EventContext {
                // 事件执行的调度器
                Scheduler* scheduler = nullptr;
                //事件协程
                Fiber::ptr fiber;
                // 事件的回调函数
                std::function<void()> cb;
            };

            EventContext& getContext(Event event);

            
            void resetContext(EventContext& ctx);

            void triggerEvent(Event event);

            // 读事件上下文
            EventContext read;
            // 写事件上下文
            EventContext write;
            // 事件关联的句柄
            int fd = 0;
            // 当前的事件
            Event events = NONE;
            // 事件的Mutex
            MutexType mutex;
        };

    public:
       
        IOManager(size_t threads = 1, bool use_caller = true, const std::string& name = "");

       
        ~IOManager();

        int addEvent(int fd, Event event, std::function<void()> cb = nullptr);

        
        bool delEvent(int fd, Event event);

        
        bool cancelEvent(int fd, Event event);

       
        bool cancelAll(int fd);

        
        static IOManager* GetThis();
    protected:
        void tickle() override;
        bool stopping() override;
        void idle() override;
        void onTimerInsertedAtFront() override;

        
        void contextResize(size_t size);

        bool stopping(uint64_t& timeout);
    private:
        // epoll 文件句柄
        int m_epfd = 0;
        // pipe 文件句柄
        int m_tickleFds[2];
        // 当前等待执行的事件数量
        std::atomic<size_t> m_pendingEventCount = { 0 };
        // IOManager的Mutex
        RWMutexType m_mutex;
        // socket事件上下文的容器
        std::vector<FdContext*> m_fdContexts;
    };

}

#endif