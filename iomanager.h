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
            // ���¼�
            NONE = 0x0,
            // ���¼�(EPOLLIN)
            READ = 0x1,
            // д�¼�(EPOLLOUT)
            WRITE = 0x4,
        };
    private:
        struct FdContext {
            typedef Mutex MutexType;
           
            struct EventContext {
                // �¼�ִ�еĵ�����
                Scheduler* scheduler = nullptr;
                //�¼�Э��
                Fiber::ptr fiber;
                // �¼��Ļص�����
                std::function<void()> cb;
            };

            EventContext& getContext(Event event);

            
            void resetContext(EventContext& ctx);

            void triggerEvent(Event event);

            // ���¼�������
            EventContext read;
            // д�¼�������
            EventContext write;
            // �¼������ľ��
            int fd = 0;
            // ��ǰ���¼�
            Event events = NONE;
            // �¼���Mutex
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
        // epoll �ļ����
        int m_epfd = 0;
        // pipe �ļ����
        int m_tickleFds[2];
        // ��ǰ�ȴ�ִ�е��¼�����
        std::atomic<size_t> m_pendingEventCount = { 0 };
        // IOManager��Mutex
        RWMutexType m_mutex;
        // socket�¼������ĵ�����
        std::vector<FdContext*> m_fdContexts;
    };

}

#endif