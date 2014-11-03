/*
 * IRemoteEndpoint.cpp
 *
 *  Created on: 2014-10-8
 *      Author: qianqians
 */
#include "IRemoteEndpoint.h"

namespace Fossilizid{
namespace reduce_rpc {

void IRemoteEndpoint::ConnectEndpoint(char * ip, short port){
	connect(ip, port);
}

} /* namespace reduce_rpc */
} /* namespace Fossilizid */