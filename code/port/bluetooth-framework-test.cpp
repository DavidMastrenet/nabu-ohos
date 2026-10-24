#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <string>
namespace OHOS::Bluetooth {
class BluetoothHost {
public:
 static BluetoothHost& GetDefaultHost();
 int EnableBle(std::string,bool);
 int GetBluetoothState() const;
 int StartBtDiscovery();
 int CancelBtDiscovery();
};
}
int main(int argc,char **argv){
 auto& host=OHOS::Bluetooth::BluetoothHost::GetDefaultHost();
 printf("bluetooth state=%d enable=%d\n",host.GetBluetoothState(),host.EnableBle("nabu port test",false));fflush(stdout);
 for(int i=0;i<40;i++){usleep(250000);int state=host.GetBluetoothState();if(i%4==0)printf("bluetooth state=%d\n",state);if(state==2)break;}
 if(argc==2&&!strcmp(argv[1],"--scan")){
  int ret=host.StartBtDiscovery();printf("discovery start=%d\n",ret);fflush(stdout);sleep(6);printf("discovery stop=%d\n",host.CancelBtDiscovery());
 }
 printf("bluetooth final state=%d\n",host.GetBluetoothState());return 0;
}
