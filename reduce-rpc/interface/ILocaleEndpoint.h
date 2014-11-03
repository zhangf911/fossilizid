/*
 * ILocaleEndpoint.h
 *
 *  Created on: 2014-10-30
 *      Author: qianqians
 */
#ifndef _ILocaleEndpoint_h
#define _ILocaleEndpoint_h

#include "../service/acceptservice.h"

namespace Fossilizid{
namespace reduce_rpc{

#define ILocaleEndpoint(ip, port) service(ip, port);

} /* namespace reduce_rpc */
} /* namespace Fossilizid */

#endif //_ILocaleEndpoint_h