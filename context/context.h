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

#ifdef _WINDOWS
typedef LPVOID context;
#endif

inline context makecontext(){ return ConvertThreadToFiber(0); }

context getcontext(boost::function<void()> fn);
void closecontext(context ct);
void yield(context ct);

} /* namespace context */
} /* namespace Fossilizid */

#endif //_context_h