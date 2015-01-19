/*
 * service.h
 *
 *  Created on: 2015-1-11
 *      Author: qianqians
 */
#ifndef _service_h
#define _service_h

#include <boost/shared_ptr.hpp>

namespace Fossilizid{
namespace juggle{

class service{
public:
	/*
	 * initialise service
	 */
	virtual void init() = 0;

	/*
	 * drive service work
	 */
	virtual void poll() = 0;

};

boost::shared_ptr<service> create_service();

} /* namespace juggle */
} /* namespace Fossilizid */

#endif //_service_h