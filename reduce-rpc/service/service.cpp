/*
 * service.cpp
 *
 *  Created on: 2014-10-8
 *      Author: qianqians
 */
#include "service.h"
#include "rpcsession.h"
#include "tempsession.h"
#include "obj.h"

#include "../../third_party/json/json_protocol.h"

#ifdef _WINDOWS
#include <Windows.h>
boost::uint64_t _clock(){
	return GetTickCount64();
}
#endif

namespace Fossilizid{
namespace reduce_rpc {

service::service(){
	isrun.store(true);

	clockstamp = _clock();
	timestamp = time(0);

	_thread_group.create_thread(boost::bind(&service::run_network, this));
	_thread_group.create_thread(boost::bind(&service::run_logic, this));
}

service::~service(){
	isrun.store(false);

	_thread_group.join_all();

	for (auto v : map_session){
		remote_queue::close(v.first);
	}
	map_session.clear();
}

void service::create_rpcsession(uuid epuuid, remote_queue::CHANNEL ch){
	{
		boost::unique_lock<boost::shared_mutex> lock(mu_map_session);
		map_session.erase(ch);
	}

	{
		boost::unique_lock<boost::shared_mutex> lock(mu_map_channel);
		std::unordered_map<uuid, remote_queue::CHANNEL>::iterator it = map_channel.find(epuuid);
		map_channel[epuuid] = ch;
	}

	{

		boost::unique_lock<boost::shared_mutex> lock(mu_map_uuid_session);
		std::unordered_map<uuid, boost::shared_ptr<rpcsession> >::iterator it = map_uuid_session.find(epuuid);
		if (it == map_uuid_session.end()){
			map_uuid_session.insert(std::make_pair(epuuid, boost::shared_ptr<rpcsession>(new rpcsession(ch))));
		} else{
			map_uuid_session[epuuid]->reset(ch);
		}

		{
			boost::unique_lock<boost::shared_mutex> lock(mu_map_session);
			map_session[ch] = static_cast<boost::shared_ptr<session> >(map_uuid_session[epuuid]);
		}
	}
}

boost::shared_ptr<session> service::get_rpcsession(uuid epuuid){
	boost::unique_lock<boost::shared_mutex> lock(mu_map_uuid_session);
	std::unordered_map<uuid, boost::shared_ptr<rpcsession> >::iterator it = map_uuid_session.find(epuuid);
	if (it == map_uuid_session.end()){
		return 0;
	}

	return static_cast<boost::shared_ptr<session> >(it->second);
}

void service::run_logic(){
	std::function<context::context * () > _wake_up = [this](){
		boost::mutex::scoped_lock lock(mu_wait_context_list);
		for (std::unordered_map<uuid, std::tuple<uuid, context::context *, boost::uint64_t> >::iterator it = wait_context_list.begin(); it != wait_context_list.end();){
			if (std::get<2>(it->second) >= unixtime()){
				context::context * _context = std::get<1>(it->second);
				it = wait_context_list.erase(it);
				return _context;
			}
			else{
				it++;
			}
		}

		return (context::context *)0;
	};

	std::function<void()> _run_logic = [this, _wake_up](){
		while (isrun.load()){
			{
				timestamp += _clock() - clockstamp;

				boost::shared_lock<boost::shared_mutex> lock(mu_map_session);
				for (auto var : map_session){
					push_current_session(var.second);

					var.second->do_logic();

					var.second->do_time(timestamp);

					pop_current_session();
				}
			}

			context::context * _context = _wake_up();
			if (_context != 0){
				set_current_context(_context);
				(*_context)();
			}
		}
	};

	std::function<void()> _loop_main = [this, _run_logic, _wake_up](){
		while (isrun.load()){
			context::context * _run_logic_context = _wake_up();

			if (_run_logic_context == 0){
				_run_logic_context = new context::context(_run_logic);
			}

			set_current_context(_run_logic_context);
			(*_run_logic_context)();
		}
	};

	context::context _loop_main_context(_loop_main);
	tsp_loop_main_context.reset(&_loop_main_context);
	_loop_main_context();
}

void service::set_current_context(context::context * _context){
	tsp_context.reset(_context);
}

context::context * service::get_current_context(){
	return tsp_context.get();
}

void service::wait(uuid _uuid, context::context * _context, boost::uint64_t wait_time){
	{
		boost::mutex::scoped_lock lock(mu_wait_context_list);
		wait_context_list.insert(std::make_pair(_uuid, std::make_tuple(_uuid, _context, wait_time)));
	}

	context::context * _tsp_loop_main_context = tsp_loop_main_context.get();
	if (_tsp_loop_main_context != 0){
		wake_up(_tsp_loop_main_context);
	}else{
		throw std::exception("_tsp_loop_main_context is null");
	}
}

void service::wake_up(context::context * _context){
	yield(_context);
}

void service::register_global_obj(boost::shared_ptr<obj> obj){
	boost::mutex::scoped_lock lock(mu_map_global_obj);
	map_global_obj.insert(std::make_pair(obj->objid(), obj));
}

boost::uint64_t service::unixtime(){
	return timestamp;
}

void service::push_current_session(boost::shared_ptr<session> _session){
	std::stack<boost::shared_ptr<session> > * _tsp_stack = tsp_current_session.get();
	if (_tsp_stack == 0){
		_tsp_stack = new std::stack < boost::shared_ptr<session> >;
		tsp_current_session.reset(_tsp_stack);
	}
	_tsp_stack->push(_session);
}

void service::pop_current_session(){
	std::stack<boost::shared_ptr<session> > * _tsp_stack = tsp_current_session.get();
	if (_tsp_stack != 0){
		_tsp_stack->pop();
	}
}

boost::shared_ptr<session> service::get_current_session(){
	std::stack<boost::shared_ptr<session> > * _tsp_stack = tsp_current_session.get();
	if (_tsp_stack != 0){
		return _tsp_stack->top();
	}
	return 0;
}

void service::push_current_obj(boost::shared_ptr<obj> _obj){
	std::stack<boost::shared_ptr<obj> > * _tsp_stack = tsp_current_obj.get();
	if (_tsp_stack == 0){
		_tsp_stack = new std::stack < boost::shared_ptr<obj> >;
		tsp_current_obj.reset(_tsp_stack);
	}
	_tsp_stack->push(_obj);
}

void service::pop_current_obj(){
	std::stack<boost::shared_ptr<obj> > * _tsp_stack = tsp_current_obj.get();
	if (_tsp_stack != 0){
		_tsp_stack->pop();
	}
}

boost::shared_ptr<obj> service::get_current_obj(){
	std::stack<boost::shared_ptr<obj> > * _tsp_stack = tsp_current_obj.get();
	if (_tsp_stack != 0){
		return _tsp_stack->top();
	}
	return 0;
}


} /* namespace reduce_rpc */
} /* namespace Fossilizid */