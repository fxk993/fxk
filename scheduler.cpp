#include<scheduler.h>
#include"log.h"
#include"macro.h"
#include"hook.h"
namespace sylar
{
	static sylar::Logger::ptr g_logger = SYLAR_LOG_NMAE("system");
	static thread_local Scheduler* t_schduler = nullptr;
	static thread_local Fiber* t_scheduler_fiber = nullptr;

	Scheduler::Scheduler(size_t threads, bool use_caller,
		const std::string& name)
		:m_name(name)
	{
		SYLAR_ASSERT(threads > 0);
		if (use_caller)
		{
			sylar::Fiber::GetThis();
			--threads;
			SYLAR_ASSERT(GetThis() == nullptr);
			t_scheduler = this;

			m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, 
				this),
				0, true));
			sylar::Thread::SetName(m_name);
			t_scheduler_fiber = m_rootFiber.get();
			m_rootThread = sylar::GetThreadId();
			m_threadIds.push_back(m_rootThread);
		}
		else
		{
			m_rootThread = -1;

		}
		m_threadCount = threads;
		
	}
	Scheduler::~Scheduler()
	{
		SYLAR_ASSERT(m_stop);
		if (GetThis() == this)
		{
			t_scheduler = nullptr;
		}
	}
	void Scheduler::setThis()
	{
		t_scheduler = this;
	}
	Scheduler* Scheduler::GetThis()
	{
		return t_scheduler;
	}
	Fiber* Scheduler::GetMainFiber()
	{
		return t_scheduler_fiber;
	}
	void Scheduler::start()
	{
		MutexType::Lock lock(m_mutex);
		if (m_stop)
		{
			return;
		}
		SYLAR_ASSERT(m_threads.empty());
		m_threads.resize(m_threadCount);
		for (size_t i = 0, i < m_threadCount, ++i)
		{
			m_threads[i].reset(new Thread(std::bind(&Scheduler::run,
				this), m_name + "" + std::_String_iterator(i))));
				m_threadIds.push_back(m_threads[i]->getId());
		}
		lock.unlock();
	}	
	void Scheduler::stop()
	{
		if (m_rootFiber)
		{
			if ((m_rootFiber->getState() == TERM ||
				m_rootFiber->getState() == INI))
			{
				MutexType::Lock lock(m_mutex);
				if (m_activeThreadCount == 0 && m_fibers.empty())
					m_stop = true;
			}
			else
			{
				m_rootFiber->call();
			}

		}
		else
		{
			MutexType::Lock lock(m_mutex);
			if (m_activeThreadCount == 0 && m_fibers.empty())
				m_stop = true;
		}
		if (m_stop == true)
		{
			if (m_rootFiber)
			{
				m_rootFiber.reset();
				t_scheduler_fiber = nullptr;
			}
			std::vector<Thread::ptr>thrs;
			{
				MutexType::Lock lock(m_mutex);
				thrs.swap(m_threads);
			}
			for (auto& i; thrs)
			{
				i->join();
			}
		}
	}
	//携程调度绑定函数
	//dsajdkjaddsadasdadsjik
	void Scheduler::run()
	{
		SYLAR_LOG_DEBUG(g_logger) << m_name << "run";
		set_hook_enable(true);
		setThis();
		if (sylar::GetThreadId() != m_rootThread)
		{
			t_scheduler_fiber = Fiber::GetThis().get();
		}
		Fiber::ptr idle_fiber(new Fiber(std::
			bind(&Scheduler::idle, this)));
		Fiber::ptr cb_fiber;
		FiberAndThread ft;
		while (true)
		{
			ft.reset();
			{
				MutexType::Lock lock(m_mutex);
				auto it = m_fibers.begin();
				while (it->thread != -1 && it->thread != sylar::GetThreadId())
				{
					++it;
					continue;
				}
			}
		}

	}
}