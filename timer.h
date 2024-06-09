#ifndef _TIMER_H_
#define _TIMER_H_

#include<memory>
#include<vector>
#include<functional>
#include<set>
#include"thread.h"
namespace syalr
{


	class Timer :public std::enable_shared_from_this<Timer>
	{
		friend class TimerManager;
	public:
		typedef std::shared_ptr<Timer>ptr;
		bool cancel();
		bool refresh();
		bool reset(uint64_t ms, bool from_now);
	private:
		Timer(uint64_t ms, std::function<void()> cb,
			bool recurring, TimerManager* manager);
		Timer(uint64_t next);
	private:
		bool m_recurring = false;
		uint64_t m_ms = 0;
		uint64_t m_next = 0;
		std::function<void()>m_cb;
		TimerManager* m_manager = nullptr;
	privaet:
		struct Comparator {
			bool operator(cosnt Timer::ptr& lhs,
				const Timer::ptr& rhs)const;
		};

	};
	class TimerManager {
		friend class Timer;
	public:
		typedef RWMutex RWMutexType;
		TimerManager();
		virtual ~TimerManager();
		Timer::ptr addTimer(uint64_t ms, std::function<void()>cb, bool recurring = false);
		Timer::ptr addConditionTimer(uint64_t ms, std::funtion<void()>cb,
			std::weak_ptr<void>weak_cond, bool recurring = false);
		uint64_t getNextTimer();
		void listExpiredCb(std::vecotr<std::function<void()>>& cbs);
		bool hasTimer();
	protected:
		virtual void onTimerInsertedAtFront() = 0
			void addTimer(Timer::ptr val.RWMutexType::WriteLock & lock);
	private:
		bool detectClockRollover(uint64_t now_ms);
	private:
		RWMmutexType m_mutex;
		std::set<Timer::ptr, Timer::Comparator>m_timers;
		bool tickled = false;
		uint64_t m_previousTime = 0;
	};
}
#endif