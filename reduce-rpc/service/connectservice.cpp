/*
 * service.cpp
 *
 *  Created on: 2014-10-8
 *      Author: qianqians
 */
#include "connectservice.h"
#include "rpcsession.h"
#include "tempsession.h"
#include "obj.h"

#include "../../third_party/json/json_protocol.h"

namespace Fossilizid{
namespace reduce_rpc {

connectservice::connectservice() : service(){
	que = remote_queue::queue();
}

connectservice::~connectservice(){
	remote_queue::close(que);
}

void connectservice::connect(char * ip, short port){
	Fossilizid::remote_queue::ENDPOINT ep = Fossilizid::remote_queue::endpoint(ip, port);
	Fossilizid::remote_queue::CHANNEL ch = Fossilizid::remote_queue::connect(ep);

	if (ch != 0){
		boost::unique_lock<boost::shared_mutex> lock(mu_map_session);
		map_session[ch] = boost::shared_ptr<session>(new tempsession(ch));
	}
}

void connectservice::run_network(){
	while (isrun.load()){
		remote_queue::EVENT ev = remote_queue::queue(que);
		switch (ev.type)
		{
		case remote_queue::event_type_none:
			break;
				

		case remote_queue::event_type_recv:
			{	
				remote_queue::CHANNEL ch = ev.handle.ch;
				
				Json::Value value;
				while (remote_queue::pop(ch, value, json_parser::buf_to_json)){
					boost::shared_lock<boost::shared_mutex> lock(mu_map_session);
					map_session[ch]->do_pop(map_session[ch], value);
				}
			}
			break;

		case remote_queue::event_type_disconnect:
			{
				map_session.erase(ev.handle.ch);
				remote_queue::close(ev.handle.ch);
			}
			break;

		default:
			break;
		}
	}
}

} /* namespace reduce_rpc */
} /* namespace Fossilizid */