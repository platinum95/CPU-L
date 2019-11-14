#include "gfxinfo.h"

#define LOG_ERR 1
#define LOG_WARN 2
#define LOG_INFO 3
#define LOG_DEBUG 4

#define LOG_LEVEL LOG_DEBUG

#define LOG_LEVEL_STR(lvl) \
    ( lvl == 1 ? "ERROR: " : lvl == 2 ? "WARNING: " : lvl == 3 ? "INFO: " : "DEBUG: " )

#define LOG(lvl, msg)     \
    if (lvl <= LOG_LEVEL) \
    std::cout << LOG_LEVEL_STR(lvl) << msg << std::endl


int main(int argc, char **argv)
{

    GfxInfo gfxInfo;
    gfxInfo.populateInfo();

    LOG( LOG_INFO, "OpenGL version string: " << gfxInfo.glCoreInfo.versionString );
    for( const auto & devProp : gfxInfo.vkDevProperties ){
        LOG( LOG_INFO, "Vulkan Device: " << devProp.vkDeviceProps.deviceName );
        LOG( LOG_INFO, "Vulkan driver info string: " << devProp.vkDeviceDriverProps.driverInfo );
        LOG( LOG_INFO, "Vulkan driver name string: " << devProp.vkDeviceDriverProps.driverName );
    }

    LOG( LOG_INFO, "Kernel version " << gfxInfo.kernelInfo.release );
    
    return 0;
}