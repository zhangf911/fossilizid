/*
 * session.cpp
 *
 *  Created on: 2014-10-8
 *      Author: qianqians
 */
#include "service.h"
#include "tempsession.h"
#include "obj.h"
#include "remote_obj.h"

#include "../../third_party/json/json_protocol.h"

namespace Fossilizid{
namespace reduce_rpc {

tempsession::tempsession(remote_queue::CHANNEL _ch){
	ch = _ch;
}

tempsession::~tempsession(){
}

void tempsession::do_time(boost::uint64_t time){
}

void tempsession::do_pop(boost::shared_ptr<session> session, Json::Value & value){
	Json::Value eventtype = value.get("eventtype", Json::nullValue);
	if (eventtype.isNull()){
		return;
	}
	
	if (eventtype.asString() == "rpc_event"){
	} else if (eventtype.asString() == "create_obj"){
	} else if (eventtype.asString() == "connect_server"){
		do_connect_server(session, value);
	}
}

bool tempsession::do_push(boost::shared_ptr<session> session, Json::Value & value){
	if (session != 0){
		return remote_queue::push(static_cast<tempsession*>(session.get())->ch, value, boost::bind(json_parser::json_to_buf, _1));
	}

	return false;
}

void tempsession::do_logic(){
}

void tempsession::do_connect_server(boost::shared_ptr<session> session, Json::Value & value){
	Json::Value _epuuid = value.get("epuuid", Json::nullValue);
	if (_epuuid.isNull()){
		return;
	}

	_service_handle->create_rpcsession(_epuuid.asString(), ch);

	Json::Value _suuid = value.get("suuid", Json::nullValue);
	if (_suuid.isNull()){
		return;
	}

	Json::Value ret;
	ret["epuuid"] = _epuuid.asString();
	ret["suuid"] = _suuid.asString();
	ret["ret"] = "OK";
	do_push(session, ret);
}

} /* namespace reduce_rpc */
} /* namespace Fossilizid */