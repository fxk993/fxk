#ifndef _FIBER_H_
#include<memory>
#include<functional>
#include<ucontext.h>
namesapce sylar{

	class Schedular;
class Fiber :public std::enable_shared_from_this<Fiber>
{
	friend class Schedular;
public:
	typedef std::shared_ptr<Fiber>prt;
	enum State
	{
		INIT,
		HOLD,
		EXEC,
		TERM,
		READY,
		EXCEPET
	};
private:
	//私有构造函数u，不允许类外调用，通过getthis获取主协程
	Fiber();
public:
	//子协程构造函数
	Fiber(std::function<void()> cb,
		size_t stacksize = 0,
		bool use_caller = false);
		~Fiber();
	void reset(std::function<void()>cb);
	void swapIn();
	void swapOut();
	void call();
	void back();
	uint64_t getId()const { return m_id; }
	state getState()const { return m_state; }
public:
	static void SetThis(Fiber* f);
	static Fiber::ptr GetThis();
	static void YieldToReady();
	static void YieldToHold();
	static void MainFunc();
	static void CallerMainFunc();
	static uint64_t GetFiberId();
private:
	uint64_t m_id = 0;
	uint32_t m_stacksize = 0;
	State m_state = INIT;
	ucontext_t m_ctx;
	void* m_stack = nullptr;
	std::function<void()>m_cb;
};
}