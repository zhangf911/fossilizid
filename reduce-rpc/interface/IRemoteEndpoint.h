/*
 * IRemoteEndpoint.h
 *
 *  Created on: 2014-10-30
 *      Author: qianqians
 */
#ifndef _IRemoteEndpoint_h
#define _IRemoteEndpoint_h

#include <boost/shared_ptr.hpp>

#include "../service/session.h"
	
namespace Fossilizid{
namespace reduce_rpc{

class IRemoteEndpoint{
public:
	IRemoteEndpoint(){}
	~IRemoteEndpoint(){}

private:
	boost::shared_ptr<session> _session;

	friend IRemoteEndpoint ConnectService(char * ip, short port);

};

IRemoteEndpoint ConnectService(char * ip, short port);

} /* namespace reduce_rpc */
} /* namespace Fossilizid */

#endif //_IRemoteEndpoint_h