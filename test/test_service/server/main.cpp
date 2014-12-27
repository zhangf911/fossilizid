#include <vector>
#include "UDPService.h"
#include "UDPConnect.h"

std::vector<boost::shared_ptr<UDPConnect> > vmap;

void onRecv(char * buf, int len){
	printf(buf);
}

void onConnect(boost::shared_ptr<UDPConnect> c){
	vmap.push_back(c);
	c->sigRecv.connect(onRecv);
}

int main(){
	UDPService _service("127.0.0.1", 7777);

	_service.sigConnect.connect(onConnect);

	while (1){
		boost::this_thread::sleep(boost::posix_time::seconds(1));
		for (auto v : vmap){
			v->reliable_send("what", 4);
		}
	}

	return 0;
}