#include "..\..\reduce\service\acceptservice.h"
#include "../../context/context.h"
#include "../../reduce/service/locale_obj.h"
#include "../../reduce/service/rpcsession.h"

class account{
public:
	account() : obj(new Fossilizid::reduce::locale_obj("account")){
		obj->register_rpc_mothed(std::make_pair("login", boost::bind(&account::login_call_back, this, _1)));
		Fossilizid::reduce::_service_handle->register_global_obj(obj);
	}

	bool login(){
		printf("login sucess\n");
		return true;
	}

	Json::Value login_call_back(Json::Value & v){
		Json::FastWriter write;
		printf("%s\n", write.write(v).c_str());

		boost::shared_ptr<Fossilizid::reduce::rpcsession> s = boost::static_pointer_cast<Fossilizid::reduce::rpcsession>(Fossilizid::reduce::_service_handle->get_current_session());

		Json::Value value;
		value["epuuid"] = s->epuuid();
		value["suuid"] = v["suuid"];
		value["eventtype"] = "rpc_event";
		value["rpc_event_type"] = "call_rpc_mothed_ret";
		value["fnname"] = "login";
		value["ret"] = login();
		
		return value;
	}

private:
	boost::shared_ptr<Fossilizid::reduce::locale_obj> obj;
};

int main(){
	Fossilizid::reduce::acceptservice _service("127.0.0.1", 7777);

	_service.init();

	account _account;

	while (1){
		_service.poll();
	}

	return 0;
}