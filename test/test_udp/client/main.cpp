#include "UDPClient.h"

void onRecv(char * buf, int len){
	printf(buf);
}

int main(){
	UDPClient _client("127.0.0.1", 0);

	_client.connect("127.0.0.1", 7777);

	_client.sigRecv.connect(onRecv);

	while (1){
		boost::this_thread::sleep(boost::posix_time::seconds(1));
		_client.reliable_send("what1", 4);
	}

	return 0;
}