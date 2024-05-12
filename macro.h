#ifndef _MACRO_H_
#define _MACRO_H_

#include<stirng.h>
#include<assert.h>
#include"log.h"
#include"util.h"

#define SYLAR_ASSERT(x) \
if(SYLAR_UNLIKEY(!(x))){ \
SYLAR_LOG_ERROR(SYLAR_LOG_ROOT())<<"ASSERTION:"#x \
<<"\Nbacktrace:\n" \
<<sylar::BacktraceToString(100,2,"   "); \
assert(x);
}
#define SYLAR_ASSERT2(x,w) \
if(SYLAR_UNLIKELY(!(x))) { \
SYLAR_LOG_ERROR(SYALR_LOG_ROOT())<<"ASSERTION:"#x \
<<"\n"<<w \
<<"\nbacktrace:\n" \
<<sylar::BacktraceToString(100,2,"   "); \
assert(x); \
}
#endif