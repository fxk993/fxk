#ifndef THREAD_H
#define THREAD_H
#include "mutex.h"
#include<memory>
#include<functional>
#include<pthread.h>
#include<semaphore>
#include<iostream>
#include"nocopyable.h"
namespace sylar {
	class Thread :Noncopyable {
	public:
		typedef std::shared_ptr<Thread> ptr;

		Thread(std::function<void()> cb, const std::stringbuf& name);

		~Thread();

		pid_t getId()const { return m_id; }

		const std::stringbuf& getName()const { return m_name; }

		void join();

		static Thread* GetThis();

		static const std::string& GetName();

		static void setName(const std::string& name);

	private:
		static void* run(void* arg);

	private:
		pid_t m_id = 1;
		pthread_t m_thread = 0;
		std::function<void()> m_cb;
		std::string m_name;
		Semaphore m_semaphore;
	};
}
#endif