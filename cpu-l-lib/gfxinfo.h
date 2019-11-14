#include <iostream>
#include <string>
#include <vulkan/vulkan.h>
#include <vector>
#include <utility>
#include <sys/utsname.h>

struct OpenGlInfo{
    bool available{ false };
    std::string vendorString;
    std::string rendererString;
    std::string deviceString;
    std::string versionString;
    std::string shadingLangVersionString;
};

struct VkDeviceInfoProperties{
    VkPhysicalDeviceProperties vkDeviceProps;
    VkPhysicalDeviceDriverPropertiesKHR vkDeviceDriverProps;
};

class GfxInfo{
public:
    GfxInfo();
    ~GfxInfo();

    int populateInfo();
    bool populated();

    OpenGlInfo glCoreInfo;
    OpenGlInfo glInfo;
    OpenGlInfo glEsInfo;
    
    std::vector< VkDeviceInfoProperties > vkDevProperties;

    utsname kernelInfo;

private:
    bool initialised{ false };
    int populateGlInfo();
    int populateVkInfo();
    int populateDxInfo();
    int populateDeviceInfo();
    int populateKernelInfo();
    
};

