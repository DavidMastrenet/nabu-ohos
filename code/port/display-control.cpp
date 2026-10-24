#include "v1_5/include/idisplay_composer_interface.h"
#include <cstdio>
#include <cstring>
#include <unistd.h>
int main(int argc, char **argv)
{
    using namespace OHOS::HDI::Display::Composer;
    auto *display = V1_5::IDisplayComposerInterface::Get(false);
    if (!display) { puts("No composer interface"); return 1; }
    V1_0::DisplayCapability cap;
    int ret = display->GetDisplayCapability(0, cap);
    printf("cap ret=%d type=%d name=%s\n", ret, cap.type, cap.name.c_str());
    if (argc > 1 && !strcmp(argv[1], "cycle")) {
        ret = display->SetDisplayPowerStatus(0, V1_0::POWER_STATUS_OFF);
        printf("power off=%d\n", ret); fflush(stdout);
        usleep(400000);
        ret = display->SetDisplayPowerStatus(0, V1_0::POWER_STATUS_ON);
        printf("power on=%d\n", ret); fflush(stdout);
    }
    ret = display->SetDisplayBacklight(0, 187);
    printf("backlight=%d\n", ret);
    delete display;
    return ret;
}
