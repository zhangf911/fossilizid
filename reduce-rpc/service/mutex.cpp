/*
 * mutex.cpp
 *
 *  Created on: 2014-11-3
 *      Author: qianqians
 */
#include "mutex.h"

namespace Fossilizid{
namespace reduce_rpc {

mutex::mutex(){
	_mutex = false;
	_mutexid = UUID();
}

mutex::~mutex(){
}

void mutex::lock(){
	if (_mutex){
		_mutex = true;
	} else {
		_service_handle->wait(_mutexid);
	}

}

void mutex::unlock(){
	if (_mutex){
		if (wait_context_list.empty()){
			boost::mutex::scoped_lock lock(_service_handle->mu_wake_up_vector);
			_service_handle->wait_weak_up_context.insert(std::make_pair(_mutexid, wait_context_list.back()));
			wait_context_list.pop_back();
		}
		_mutex = false;
	}
}

} /* namespace reduce_rpc */
} /* namespace Fossilizid */