/*
 * IRemoteEndpoint.h
 *
 *  Created on: 2014-10-30
 *      Author: qianqians
 */
#ifndef _IRemoteEndpoint_h
#define _IRemoteEndpoint_h

#include "../service/connectservice.h"

namespace Fossilizid{
namespace reduce_rpc{

class IRemoteEndpoint : public connectservice{
public:
	void ConnectEndpoint(char * ip, short port);
};

} /* namespace reduce_rpc */
} /* namespace Fossilizid */

#endif //_IRemoteEndpoint_h