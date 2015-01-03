/*
 * acceptservice.cpp
 *
 *  Created on: 2014-11-3
 *      Author: qianqians
 */
#include "acceptservice.h"
#include "rpcsession.h"
#include "tempsession.h"
#include "obj.h"

#include "../../third_party/json/json_protocol.h"

namespace Fossilizid{
namespace reduce {

acceptservice::acceptservice(char * ip, short port) : service(){
	ep = remoteq::endpoint(ip, port);
	que = remoteq::queue();
	acp = remoteq::acceptor(que, ep);
}

acceptservice::~acceptservice(){
	remoteq::close(que);
}

void acceptservice::run_network(){
	while (isrun.load()){
		remoteq::EVENT ev = remoteq::queue(que);
		switch (ev.type)
		{
		case remoteq::event_type_none:
			break;
				
		case remoteq::event_type_accept:
			{
				remoteq::CHANNEL ch = remoteq::accept(ev.handle.acp);
				if (ch != 0){
					boost::unique_lock<boost::shared_mutex> lock(mu_map_session);
					map_session[ch] = boost::shared_ptr<session>(new tempsession(ch, "acceptservice"));
				}
			}
			break;

		case remoteq::event_type_recv:
			{	
				remoteq::CHANNEL ch = ev.handle.ch;
				
				Json::Value value;
				while (remoteq::pop(ch, value, json_parser::buf_to_json)){
					Json::Value _suuid = value.get("suuid", Json::nullValue);
					if (_suuid.isNull()){
						continue;
					}

					{
						boost::shared_lock<boost::shared_mutex> lock(mu_map_session);
						map_session[ch]->do_pop(map_session[ch], value);
					}

					boost::mutex::scoped_lock lock(mu_wait_context_list);
					auto finduuidcontex = wait_context_list.find(_suuid.asString());
					if (finduuidcontex != wait_context_list.end()){
						std::get<3>(finduuidcontex->second) = boost::shared_ptr<Json::Value>(new Json::Value(value));
						boost::mutex::scoped_lock lock(mu_wake_up_set);
						wake_up_set.insert(_suuid.asString());
					}
				}
			}
			break;

		case remoteq::event_type_disconnect:
			{
				map_session.erase(ev.handle.ch);
				remoteq::close(ev.handle.ch);
			}
			break;

		default:
			break;
		}
	}
}

} /* namespace reduce */
} /* namespace Fossilizid */