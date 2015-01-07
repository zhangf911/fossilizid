#include "..\..\reduce\service\connectservice.h"
#include "../../reduce/service/locale_obj.h"
#include "../../reduce/service/rpcsession.h"
#include "../../reduce/service/uuid.h"

boost::shared_ptr<Fossilizid::reduce::session> ConnectService(boost::shared_ptr<Fossilizid::reduce::session> s){
	Json::Value value;
	value["suuid"] = Fossilizid::reduce::UUID();
	value["epuuid"] = Fossilizid::reduce::UUID();
	value["eventtype"] = "connect_server";
	value["globalobjarray"] = Json::Value(Json::arrayValue);
	Fossilizid::reduce::_service_handle->global_obj_lock();
	for (Fossilizid::reduce::service::global_obj_iterator it = Fossilizid::reduce::_service_handle->global_obj_begin(); it != Fossilizid::reduce::_service_handle->global_obj_end(); it++){
		Json::Value objinfo;
		objinfo["classname"] = it->second->class_name();
		objinfo["objid"] = it->second->objid();
		value["globalobjarray"].append(objinfo);
	}
	Fossilizid::reduce::_service_handle->global_obj_unlock();

	boost::shared_ptr<Json::Value> ret = 0;
	s->do_sync_push(s, value["suuid"].asString(), value, ret, 15);

	return Fossilizid::reduce::_service_handle->get_rpcsession(value["epuuid"].asString());
}

class account{
private:
	boost::shared_ptr<Fossilizid::reduce::obj> obj;
	boost::shared_ptr<Fossilizid::reduce::rpcsession> s;

public:
	account(boost::shared_ptr<Fossilizid::reduce::obj> _obj, boost::shared_ptr<Fossilizid::reduce::session> _s){
		s = boost::static_pointer_cast<Fossilizid::reduce::rpcsession>(_s);
		obj = _obj;
	}

	bool login(){
		Json::Value value;
		value["epuuid"] = s->epuuid();
		value["suuid"] = Fossilizid::reduce::UUID();
		value["objuuid"] = obj->objid();
		value["eventtype"] = "rpc_event";
		value["rpc_event_type"] = "call_rpc_mothed";
		value["fnargv"] = Json::Value(Json::objectValue);
		value["fnname"] = "login";
		
		boost::shared_ptr<Json::Value> ret = 0;
		s->do_sync_push(s, value["suuid"].asString(), value, ret, 15);

		if ((*ret)["suuid"] != value["suuid"]){
			throw std::exception("error suuid");
		}

		Json::FastWriter write;
		printf("%s\n", write.write(*ret).c_str());

		return (*ret)["ret"].asBool();
	}
};

int main(){
	Fossilizid::reduce::connectservice _service;

	_service.init();

	boost::shared_ptr<Fossilizid::reduce::rpcsession> s = boost::static_pointer_cast<Fossilizid::reduce::rpcsession>(ConnectService(_service.connect("127.0.0.1", 7777)));

	boost::shared_ptr<Fossilizid::reduce::obj> _obj = s->get_global_obj("account");
	if (_obj != 0){
		account a(_obj, s);
		a.login();
		printf("login sucess\n");
	}

	while (1){
		_service.join();
	}

	return 0;
}