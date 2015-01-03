#include "..\..\reduce-rpc\service\connectservice.h"
#include "../../reduce-rpc/service/locale_obj.h"
#include "../../reduce-rpc/service/rpcsession.h"
#include "../../reduce-rpc/service/uuid.h"

void ConnectService(boost::shared_ptr<Fossilizid::reduce_rpc::session> s){
	Json::Value value;
	value["suuid"] = Fossilizid::reduce_rpc::UUID();
	value["epuuid"] = Fossilizid::reduce_rpc::UUID();
	value["eventtype"] = "connect_server";
	value["globalobjarray"] = Json::Value(Json::arrayValue);
	Fossilizid::reduce_rpc::_service_handle->global_obj_lock();
	for (Fossilizid::reduce_rpc::service::global_obj_iterator it = Fossilizid::reduce_rpc::_service_handle->global_obj_begin(); it != Fossilizid::reduce_rpc::_service_handle->global_obj_end(); it++){
		Json::Value objinfo;
		objinfo["classname"] = it->second->class_name();
		objinfo["objid"] = it->second->objid();
		value["globalobjarray"].append(objinfo);
	}
	Fossilizid::reduce_rpc::_service_handle->global_obj_unlock();

	s->do_push(s, value);

	Fossilizid::reduce_rpc::_service_handle->wait(value["suuid"].asString(), 15);

	s = Fossilizid::reduce_rpc::_service_handle->get_rpcsession(value["epuuid"].asString());
}

class account{
private:
	boost::shared_ptr<Fossilizid::reduce_rpc::obj> obj;
	boost::shared_ptr<Fossilizid::reduce_rpc::rpcsession> s;

public:
	account(boost::shared_ptr<Fossilizid::reduce_rpc::obj> _obj, boost::shared_ptr<Fossilizid::reduce_rpc::session> _s){
		s = boost::static_pointer_cast<Fossilizid::reduce_rpc::rpcsession>(_s);
		obj = _obj;
	}

	bool login(){
		Json::Value value;
		value["epuuid"] = s->epuuid();
		value["suuid"] = Fossilizid::reduce_rpc::UUID();
		value["eventtype"] = "rpc_event";
		value["rpc_event_type"] = "call_rpc_mothed";
		value["fnargv"] = Json::Value(Json::objectValue);
		value["fnname"] = "login";
		s->do_push(s, value);

		boost::shared_ptr<Json::Value> ret = Fossilizid::reduce_rpc::_service_handle->wait(value["suuid"].asString(), 1);
		if ((*ret)["suuid"] != value["suuid"]){
			throw std::exception("error suuid");
		}

		printf((*ret).toStyledString().c_str());

		return (*ret)["ret"].asBool();
	}
};

int main(){
	Fossilizid::reduce_rpc::connectservice _service;

	_service.init();

	boost::shared_ptr<Fossilizid::reduce_rpc::rpcsession> s = boost::static_pointer_cast<Fossilizid::reduce_rpc::rpcsession>(_service.connect("127.0.0.1", 7777));
	ConnectService(s);

	boost::shared_ptr<Fossilizid::reduce_rpc::obj> _obj = s->get_global_obj("account");
	if (_obj != 0){
		account a(_obj, s);
		a.login();
	}

	while (1){
		_service.join();
	}

	return 0;
}