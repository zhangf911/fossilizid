#include "..\..\reduce-rpc\service\acceptservice.h"
#include "../../context/context.h"
#include "../../reduce-rpc/service/locale_obj.h"
#include "../../reduce-rpc/service/rpcsession.h"

class account{
public:
	account() : obj(new Fossilizid::reduce_rpc::locale_obj("account")){
		obj->register_rpc_mothed(std::make_pair("login", boost::bind(&account::login_call_back, this, _1)));
		Fossilizid::reduce_rpc::_service_handle->register_global_obj(obj);
	}

	bool login(){
		return true;
	}

	Json::Value login_call_back(Json::Value & v){
		printf(v.toStyledString().c_str());

		boost::shared_ptr<Fossilizid::reduce_rpc::rpcsession> s = boost::static_pointer_cast<Fossilizid::reduce_rpc::rpcsession>(Fossilizid::reduce_rpc::_service_handle->get_current_session());

		Json::Value value;
		value["epuuid"] = s->epuuid();
		value["suuid"] = v["suuid"];
		value["eventtype"] = "rpc_event";
		value["rpc_event_type"] = "call_rpc_mothed";
		value["fnname"] = "login";
		value["ret"] = true;
		
		return value;
	}

private:
	boost::shared_ptr<Fossilizid::reduce_rpc::locale_obj> obj;
};

int main(){
	Fossilizid::reduce_rpc::acceptservice _service("127.0.0.1", 7777);

	_service.init();

	while (1){
		_service.join();
	}

	return 0;
}