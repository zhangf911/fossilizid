/*
 * rpcsession.h
 *
 *  Created on: 2014-10-8
 *      Author: qianqians
 */
#ifndef _rpcsession_h
#define _rpcsession_h

#include <functional>
#include <unordered_map>
#include <string>

#include <boost/cstdint.hpp>
#include <boost/thread.hpp>

#include "../json/jsoncpp/include/json/json.h"

#include "uuid.h"
#include "session.h"

namespace Fossilizid{
namespace reduce_rpc{

class obj;

class rpcsession : public session{
public:
	rpcsession(remote_queue::CHANNEL _ch);
	~rpcsession();

	void reset(remote_queue::CHANNEL ch);

public:
	virtual void do_time(boost::uint64_t time);

	virtual void do_pop(boost::shared_ptr<session> session, Json::Value & value);

	virtual bool do_push(boost::shared_ptr<session> session, Json::Value & value);

	virtual void do_logic();

private:
	remote_queue::CHANNEL ch;

private:
	std::unordered_map<uuid, boost::shared_ptr<obj> > mapremoteobj;

};

} /* namespace reduce_rpc */
} /* namespace Fossilizid */

#endif //_rpcsession_h