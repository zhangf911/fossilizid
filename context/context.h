/*
 * context.h
 *
 *  Created on: 2014-10-15
 *      Author: qianqians
 */
#ifndef _context_h
#define _context_h

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#ifdef _WINDOWS
#include <Windows.h>
#endif

namespace Fossilizid{
namespace context{

class context{
public:
	context(boost::function<void()> fn);
	~context();

	void operator()();

private:
#ifdef _WINDOWS
	static VOID CALLBACK contextFiberProc(_In_ PVOID lpParameter);
#endif

public:
#ifdef _WINDOWS
	LPVOID pFiber;
#endif

private:
	boost::function<void()> contextfn;

	friend void yield(context * ct);
	friend boost::shared_ptr<context>  make_context();

};

void yield(context * ct);

} /* namespace context */
} /* namespace Fossilizid */

#endif //_context_h