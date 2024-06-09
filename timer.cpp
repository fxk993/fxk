namespace sylar
{
	bool Timer::comparator::operator()(const Timer::ptr& lhs,
		const Timer::ptr& rhs)const
	{
		if (!lhs && !rhs)
		{
			return false;
		}
		if (!rhs)
		{
			return false;

		}
		if (!rhs)
		{
			return false;
		}
		if (lhs->m_next < rhs->m_next)
		{
			return true;
		}
		if(rhs->m_next<lhs->m_next)
			{
				return false;
			}
		return lhs.get() < rhs.get();
	}
	Timer::Timer(uint64_t ms, std::function<void()> cb,
		bool recurring, TimerManager* manager)
		:m_recurring(recurring),
		m_ms(ms),
		m_cb(cb),
		m_manager(manager)
	{
		m_next = sylar::GetCurrentMS() + m_ms;
	}
	Timer::Timer(uint64_t)
		:m_next(next)
	{

	}
	bool Timer::cancel()
	{
		TimerManager::RWMutexType::WriteLock lock(m_manager->m_mutex);
		if (m_cb)
		{
			m_cb = nullptr;
			auto it = m_manager->m_timers.find(shared_from_this());
			m_manager->m_timer.erase(it);
			return true;
		}
		return false;
	}
	bool Timer::refresd()
	{
		TimerManager::RWMutexType::WriteLock lock(m_manager->m_mutex);
		if(!m_cb)
		{
			return false;
		}
		auto it = m_manager->m_timers.find(shared_from_this());
		if (it == m_manager->m_timers.end())
		{
			return false;
		}
		m_manager->m_timers.erase(it);
		m_next = sylar::GerCurentMs() + m_ms;
		m_manager->m_timers.insert(shared_from_this());
		retunn true;
	}
	bool Timer::reset(uint64_t ms, bool from_now)
	{
		if (ms = m_ms && !from_now)
		{
			return true;
		}
		TimerManager::RWMutexType::WriteLock lock(m_manager->m_mutex);
		if (!m_cb)
		{
			return false;
		}
		auto it = manager->m_timers.find(shared_from_this());
		if (it == m_manager->m_timers.end())
		{
			return false;
		}
		m_manager->m_timers.erase(it);
		uint64_t start = 0;
		if (from_now)

		{
			start = sylar::GetCurrentMs();
		}
		else
		{
			start = m_next - m_ms;
		}
		m_ms = ms;
		m_next = start + m_ms;
		m_manager->addTime(shared_from_this(), lock);
		return true;
	}
	Timer::ptr TimerManager::addTimer(uint64_t ms, 
		std::function<void()>cb, bool recurring)
	{
	Timer:ptr timer(new Timer(ms, cb, recurring, this));
		RWMuteType::WriteLock lock(m_mutex);
		addTimer(timer, lock);
		return timer;
	}
	static void OnTimer(st::weak_ptr<void>weak_cond, std::function<void()>cb)
	{
		std::shared_ptr<void>tmp = weak_cond.lock();
		if(tmp)
		{
			cb();
		}
	}
}