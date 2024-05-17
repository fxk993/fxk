#include"fiber.h"

static std::atomic<uint64_t> s_fiber_id{ 0 };
static std::atomic<uint64_t> s_fiber_count{ 0 };

static thread_local Fiber* t_fiber = nullptr;
static thread_local Fiber::ptr t_threadFiber = nullptr;

static  ConfigVar<uint32_t>::ptr g_fiber_stack_size =
config::Lookup<uint32_t>("fiber.stack_size", 128 * 1024, "fiber stack size");
class MallocStackAllocator {
public:
	static void* Alloc(size_t size)
	{
		return malloc(size);
	}
	static void Dealloc(void* vp, size_t size)

	{
		return free(vp);
	}
};
using StackAllcoator = MallocStackAllocator;
//获取当前协程id
uint64_t Fiber::GetFiberId()
{
	if (t_fiber)
	{
		return t_fiber->getId();
	}
	return 0;
}
//构造主协程
Fiber::Fiber() {
	m_state = EXEC;
	SetThis(this);//将当前协程设置为主协程
	if (getcontext(&m_ctx))
	{
		SYLAR_ASSERT2(false, "getcontext");
	}
	++s_fiber_count;
	SYLAR_LOG_DEBUG(g_logger) << "Fiber::Fiber main";
}
//创建子协程
Fiber::Fiber(std::function<void()>cb, size_t stacksize, bool use_caller)
	:m_id(++s_fiber_id), m_cb(cb)
{
	++s_fiber_count;
	m_stacksize = stacksize ? stacksize : g_fiber_stack_size->getValue();
	m_stack = StackAllocator::Alloc(m_stacksize);
	if (getcontext(&m_ctx))
	{
		SYLAR_ASSERT2(false, "getcontext");
	}
	m_ctx.uc_link = nullptr;
	m_ctx.uc_stack.ss_sp = m_stack;
	m_ctx.uc_stack.ss_size = m_stacksize;
	if (!use_caller)
	{
		makecontext(&m_ctx,&Fiber::MainFunc,0)
	}
	else {
		makecontext(&m_ctx, &Fiber::CallerMainFunc, 0);
	}
	SYLAR_LOG_DEBUG(g_logger) << "Fiber::Fiber id=" << m_id;
}
Fiber::~Fiber()
{
	--s_fiber_count;
	if (m_stack)
	{
		SYLAR_ASSERT(m_state == TERM 
			|| m_state == EXCEPT 
			|| m_state == INIT);
		StackAllocator::Dealloc(m_stack, m_stacksize);

	}
	else
	{
		SYLAR_ASSERT(!m_cb);
		SYLAR_ASSERT(m_state == EXEC);

		Fiber* cur = t_fiber;
		if (cur == this)
		{
			SetThis(nullptr);
		}
	}
	SYLAR_LOG_DEBUG(g_logger) << "Fiber::~Fiber id=" << m_id 
		<< "total=" << s_fiber_count;
}
//重置协程，改变协程绑定的函数，不释放原来的协程，
void Fiber::reset(std::function<void()>cb)
{
	SYLAR_ASSERT(m_stack);
	SYLAR_ASSERT(m_state == TERM
		|| m_state == EXCEPT || m_state == INIT);
	m_cb = cb;
	if (getcontext(&m_ctx))
	{
		SYLAR_ASSERT2(false, "getcontext");
	}
	m_ctx.uc_link = nullptr;
	m_ctx.uc_stack.ss_sp = m_stack;
	m_ctx.uc_stack.ss_size = m_stacksize;
	makecontext(&m_ctx, &Fiber::MainFunc, 0);
	m_state = INIT;
}
//从主协程切换到本协程执行
void Fiber::call()
{
	SetThis(this);
	m_state = EXEC;
	if (swapcontext(&t_threadFiber->m_ctx, &m_ctx))
	{
		SYALR_ASSERT2(false, "swapcontext");
	}
}
//从本协程切换到主协程
void Fiber::back()
{
	SetThis(t_threadFiber.get());
	if (swapcontext(&m_ctx, &t_threadFiber->m_ctx))
	{
		SYALR_ASSERT2(false, "swapcontext");
	}
}
void Fiber::swapIn()
{
	SetThis(this);
	SYLAR_ASSERT(m_state != EXEC);
	m_state = EXEC;
	if (swapcontext(&Schedular::GetMainFiber()->m_ctx, &m_ctx))
	{
		SYLAR_ASSERT2(false, "swapcontext");
	}
}
//设置当前协程
vodi Fiber::SetThis(Fiber* f)
{
	t_fiber = f;
}
//获取当前协程，如果不存在则创建主协程，并返回主协程
Fiber::_ptr Fiber::GetThis()
{
	if (t_fiber)
	{
		return t_fiber->shared_from_this();
	}
	Fiber::ptr main_fiber(new Fiber);//如果当前携程不存在，创建主协程
	SYLAR_ASSERT(t_fiber == main_fiber.get());
	t_threadFiber = main_fiber;
	return t_fiber->shared_from_this();
}
void Fiber::YieldToReady()
{
	Fiber::ptr cur = GetThis();
	SYLAR_ASSERT(cur->m_state == EXEC);
	cur->m_state = READY;
	cur->swapOut();
}
void Fiber::YieldToHold()
{
	Fiber::ptr cur = GetThis();
	SYLAR_ASSERT(cur->m_state == EXEC);
	cur->swapOut();
}
uint64_t Fiber::TotalFibers()
{
	return s_fiber_count;
}
void Fiber::MainFunc()
{
	Fiber::ptr cur = GetThis();
	SYLAR_ASSERT(cur);
	try {
		cur->m_cb;
		cur->m_cb = nullptr;
		cur->m_state = TERM;
	}
	catch (std::exception& ex)
	{
		cur->m_state = EXCEPT;
		SYLAR_LOG_ERROR(g_logger) << "Fiber Except:" << ex.what()
			<< "fiber_id=" << cur->getId() << std::endl
			<< sylar::BacktraceToString();
	}
	catch (...)
	{
		cur->m_state = EXCEPT;
		SYALR_LOG_ERROR(g_logger) << "Fiber Except" << "fiber_id=" << cur->getId() << 
			std::endl << sylar::BacktraceToString();
	}
	auto raw_ptr = cur.get();
	cur > reset();
	raw_ptr->swapOut();
	SYLAR_ASSERT2(false, "never reach fiber_id=" +
		std::to_string(raw_ptr->getId()));
}
void Fiber::CallerMainFunc()
{
	Fiber::ptr cur = GetThis();
	SYLAR_ASSERT(cur);
	try {
		cur->m_cb();
		cur->m_cb = nullptr;
		cur->m_state = TERM;
	}
	catch (std::exception& ex) {
		cur->m_state = EXCEPT;
		SYLAR_LOG_ERROR(g_logger) << "Fiber Except:" << ex.what()
			<< "fiber_id=" << cur->getId()
			<< std::endl << sylar::BacktraceToString();
	}
	catch (...) //捕捉所有错误
	{
		cur->m_state = EXCEPT;
		SYALR_LOG_ERROR(g_logger) << "Fiber Except" << "fiber_id=" << cur->getId() <<
			std::endl << sylar::BacktraceToString();
	}
	auto raw_ptr = cur.get();
	cur > reset();
	raw_ptr->back();
	SYLAR_ASSERT2(false, "never reach fiber_id=" +
		std::to_string(raw_ptr->getId()));
}


