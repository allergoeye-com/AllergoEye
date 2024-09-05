/*
 * android_excep.cpp
 *
 *  Created on: Feb 2, 2015
 *      Author: alex
 */
#ifdef ANDROID_NDK
#include <iostream>
#include <vector>
#include <algorithm>
#undef _Unwind_Ptr

#include <unwind.h>
#pragma pack(push)
#pragma pack(0)

typedef struct {
    size_t count;
    void** addrs;
} stack_crawl_state_t;
#pragma pack(pop)
static _Unwind_Reason_Code trace_function(_Unwind_Context *context, void *arg)
{
    stack_crawl_state_t* state = (stack_crawl_state_t*)arg;
    void* ip = 0;
    if (state->count)
    {
        ip = (void*)_Unwind_GetIP(context);

        if (ip)
        {
            state->addrs[0] = ip;
            state->addrs++;
            state->count--;
        }
    }
    if (!ip || state->count < 2)
    	return _URC_END_OF_STACK;
    return _URC_NO_REASON;
}
int backtrace(void** addrs, size_t size)
{
	stack_crawl_state_t state;
	state.count = size;
	state.addrs = addrs;
	_Unwind_Backtrace(trace_function, (void*)&state);
	return size - state.count;
//	return (char **)addrs;
}

/*
int backtrace(void** addrs, size_t size)
{
	stack_crawl_state_t state;
	state.count = size;
	state.addrs = addrs;
	_Unwind_Backtrace(trace_function, (void*)&state);
	return size - state.count;
}
*/
#endif



