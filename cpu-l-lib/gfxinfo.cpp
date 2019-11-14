/*
 * Copyright (C) 1999-2006  Brian Paul   All Rights Reserved.
 * Modified by Peter Mullen 2019
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#define MESA_DEBUG

#include "gfxinfo.h"
#include <GL/gl.h>
#include <assert.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "glxinfo_lib.h"
#include <stdexcept>
#include <algorithm>
#include <alloca.h>

static int version;

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

/**
 * Choose a simple FB Config.
 */
static GLXFBConfig *
choose_fb_config(Display *dpy, int scrnum)
{
    int fbAttribSingle[] = {
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_RED_SIZE, 1,
        GLX_GREEN_SIZE, 1,
        GLX_BLUE_SIZE, 1,
        GLX_DOUBLEBUFFER, False,
        None};
    int fbAttribDouble[] = {
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_RED_SIZE, 1,
        GLX_GREEN_SIZE, 1,
        GLX_BLUE_SIZE, 1,
        GLX_DOUBLEBUFFER, True,
        None};
    GLXFBConfig *configs;
    int nConfigs;

    configs = glXChooseFBConfig(dpy, scrnum, fbAttribSingle, &nConfigs);
    if (!configs)
        configs = glXChooseFBConfig(dpy, scrnum, fbAttribDouble, &nConfigs);

    return configs;
}

static Bool CreateContextErrorFlag;

static int
create_context_error_handler(Display *dpy, XErrorEvent *error)
{
    (void)dpy;
    (void)error->error_code;
    CreateContextErrorFlag = True;
    return 0;
}

/**
 * Try to create a GLX context of the given version with flags/options.
 * Note: A version number is required in order to get a core profile
 * (at least w/ NVIDIA).
 */
static GLXContext
create_context_flags(Display *dpy, GLXFBConfig fbconfig, int major, int minor,
                     int contextFlags, int profileMask, bool direct)
{
#ifdef GLX_ARB_create_context
    static PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB_func = 0;
    static Bool firstCall = True;
    int (*old_handler)(Display *, XErrorEvent *);
    GLXContext context;
    int attribs[20];
    int n = 0;

    if (firstCall)
    {
        /* See if we have GLX_ARB_create_context_profile and get pointer to
       * glXCreateContextAttribsARB() function.
       */
        const char *glxExt = glXQueryExtensionsString(dpy, 0);
        if (extension_supported("GLX_ARB_create_context_profile", glxExt))
        {
            glXCreateContextAttribsARB_func = (PFNGLXCREATECONTEXTATTRIBSARBPROC)
                glXGetProcAddress((const GLubyte *)"glXCreateContextAttribsARB");
        }
        firstCall = False;
    }

    if (!glXCreateContextAttribsARB_func)
        return 0;

    /* setup attribute array */
    if (major)
    {
        attribs[n++] = GLX_CONTEXT_MAJOR_VERSION_ARB;
        attribs[n++] = major;
        attribs[n++] = GLX_CONTEXT_MINOR_VERSION_ARB;
        attribs[n++] = minor;
    }
    if (contextFlags)
    {
        attribs[n++] = GLX_CONTEXT_FLAGS_ARB;
        attribs[n++] = contextFlags;
    }
#ifdef GLX_ARB_create_context_profile
    if (profileMask)
    {
        attribs[n++] = GLX_CONTEXT_PROFILE_MASK_ARB;
        attribs[n++] = profileMask;
    }
#endif
    attribs[n++] = 0;

    /* install X error handler */
    old_handler = XSetErrorHandler(create_context_error_handler);
    CreateContextErrorFlag = False;

    /* try creating context */
    context = glXCreateContextAttribsARB_func(dpy,
                                              fbconfig,
                                              0, /* share_context */
                                              direct,
                                              attribs);

    /* restore error handler */
    XSetErrorHandler(old_handler);

    if (CreateContextErrorFlag)
        context = 0;

    if (context && direct)
    {
        if (!glXIsDirect(dpy, context))
        {
            glXDestroyContext(dpy, context);
            return 0;
        }
    }

    return context;
#else
    return 0;
#endif
}

/**
 * Try to create a GLX context of the newest version.
 */
static GLXContext
create_context_with_config(Display *dpy, GLXFBConfig config,
                           bool coreProfile, bool es2Profile, bool direct)
{
    GLXContext ctx = 0;

    if (coreProfile)
    {
        /* Try to create a core profile, starting with the newest version of
       * GL that we're aware of.  If we don't specify the version
       */
        int i;
        for (i = 0; gl_versions[i].major > 0; i++)
        {
            /* don't bother below GL 3.0 */
            if (gl_versions[i].major == 3 &&
                gl_versions[i].minor == 0)
                return 0;
            ctx = create_context_flags(dpy, config,
                                       gl_versions[i].major,
                                       gl_versions[i].minor,
                                       0x0,
                                       GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
                                       direct);
            if (ctx)
                return ctx;
        }
        /* couldn't get core profile context */
        return 0;
    }

    if (es2Profile)
    {
#ifdef GLX_CONTEXT_ES2_PROFILE_BIT_EXT
        if (extension_supported("GLX_EXT_create_context_es2_profile",
                                glXQueryExtensionsString(dpy, 0)))
        {
            ctx = create_context_flags(dpy, config, 2, 0, 0x0,
                                       GLX_CONTEXT_ES2_PROFILE_BIT_EXT,
                                       direct);
            return ctx;
        }
#endif
        return 0;
    }

    /* GLX should return a context of the latest GL version that supports
    * the full profile.
    */
    ctx = glXCreateNewContext(dpy, config, GLX_RGBA_TYPE, NULL, direct);

    /* make sure the context is direct, if direct was requested */
    if (ctx && direct)
    {
        if (!glXIsDirect(dpy, ctx))
        {
            glXDestroyContext(dpy, ctx);
            return 0;
        }
    }

    return ctx;
}

static XVisualInfo *
choose_xvisinfo(Display *dpy, int scrnum)
{
    int attribSingle[] = {
        GLX_RGBA,
        GLX_RED_SIZE, 1,
        GLX_GREEN_SIZE, 1,
        GLX_BLUE_SIZE, 1,
        None};
    int attribDouble[] = {
        GLX_RGBA,
        GLX_RED_SIZE, 1,
        GLX_GREEN_SIZE, 1,
        GLX_BLUE_SIZE, 1,
        GLX_DOUBLEBUFFER,
        None};
    XVisualInfo *visinfo;

    visinfo = glXChooseVisual(dpy, scrnum, attribSingle);
    if (!visinfo)
        visinfo = glXChooseVisual(dpy, scrnum, attribDouble);

    return visinfo;
}

/**
 * GL Error checking/warning.
 */
static void
CheckError(int line)
{
    int n;
    n = glGetError();
    if (n)
        printf("Warning: GL error 0x%x at line %d\n", n, line);
}

static void
query_renderer(void)
{
#ifdef GLX_MESA_query_renderer
    PFNGLXQUERYCURRENTRENDERERINTEGERMESAPROC queryInteger;
    PFNGLXQUERYCURRENTRENDERERSTRINGMESAPROC queryString;
    unsigned int v[3];

    queryInteger = (PFNGLXQUERYCURRENTRENDERERINTEGERMESAPROC)
        glXGetProcAddressARB((const GLubyte *)"glXQueryCurrentRendererIntegerMESA");
    queryString = (PFNGLXQUERYCURRENTRENDERERSTRINGMESAPROC)
        glXGetProcAddressARB((const GLubyte *)"glXQueryCurrentRendererStringMESA");

    //printf("Extended renderer info (GLX_MESA_query_renderer):\n");
    queryInteger(GLX_RENDERER_VENDOR_ID_MESA, v);
    //printf("    Vendor: %s (0x%x)\n",
    //   queryString(GLX_RENDERER_VENDOR_ID_MESA), *v);
    queryInteger(GLX_RENDERER_DEVICE_ID_MESA, v);
    //printf("    Device: %s (0x%x)\n",
    //   queryString(GLX_RENDERER_DEVICE_ID_MESA), *v);
    queryInteger(GLX_RENDERER_VERSION_MESA, v);
    //printf("    Version: %d.%d.%d\n", v[0], v[1], v[2]);
    queryInteger(GLX_RENDERER_ACCELERATED_MESA, v);
    //printf("    Accelerated: %s\n", *v ? "yes" : "no");
    queryInteger(GLX_RENDERER_VIDEO_MEMORY_MESA, v);
    //printf("    Video memory: %dMB\n", *v);
    queryInteger(GLX_RENDERER_UNIFIED_MEMORY_ARCHITECTURE_MESA, v);
    //printf("    Unified memory: %s\n", *v ? "yes" : "no");
    queryInteger(GLX_RENDERER_PREFERRED_PROFILE_MESA, v);
    //printf("    Preferred profile: %s (0x%x)\n",
    //   *v == GLX_CONTEXT_CORE_PROFILE_BIT_ARB ? "core" :
    //   *v == GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB ? "compat" :
    //   "unknown", *v);
    queryInteger(GLX_RENDERER_OPENGL_CORE_PROFILE_VERSION_MESA, v);
    //printf("    Max core profile version: %d.%d\n", v[0], v[1]);
    queryInteger(GLX_RENDERER_OPENGL_COMPATIBILITY_PROFILE_VERSION_MESA, v);
    //printf("    Max compat profile version: %d.%d\n", v[0], v[1]);
    queryInteger(GLX_RENDERER_OPENGL_ES_PROFILE_VERSION_MESA, v);
    //printf("    Max GLES1 profile version: %d.%d\n", v[0], v[1]);
    queryInteger(GLX_RENDERER_OPENGL_ES2_PROFILE_VERSION_MESA, v);
    //printf("    Max GLES[23] profile version: %d.%d\n", v[0], v[1]);
#endif
}

static bool
populateGlStruct( Display *dpy,
                  bool coreProfile, bool es2Profile,
                  bool coreWorked, OpenGlInfo *glInfoStruct)
{
    Window win;
    XSetWindowAttributes attr;
    unsigned long mask;
    Window root;
    GLXContext ctx = NULL;
    XVisualInfo *visinfo;
    int width = 100, height = 100;
    GLXFBConfig *fbconfigs;
    int scrnum = 0;

    const char *oglstring = coreProfile ? "OpenGL core profile" : es2Profile ? "OpenGL ES profile" : "OpenGL";

    root = RootWindow(dpy, scrnum);
    /*
    * Choose FBConfig or XVisualInfo and create a context.
    */
    fbconfigs = choose_fb_config(dpy, scrnum);
    if (fbconfigs)
    {
        ctx = create_context_with_config(dpy, fbconfigs[0],
                                         coreProfile, es2Profile,
                                         true);
        if (!ctx && !coreProfile)
        {
            /* try indirect */
            ctx = create_context_with_config(dpy, fbconfigs[0],
                                             coreProfile, es2Profile, false);
        }

        visinfo = glXGetVisualFromFBConfig(dpy, fbconfigs[0]);
        XFree(fbconfigs);
    }
    else if (!coreProfile && !es2Profile)
    {
        visinfo = choose_xvisinfo(dpy, scrnum);
        if (visinfo)
            ctx = glXCreateContext(dpy, visinfo, NULL, true);
    }
    else
        visinfo = NULL;

    if (!visinfo && !coreProfile && !es2Profile)
    {
        fprintf(stderr, "Error: couldn't find RGB GLX visual or fbconfig\n");
        return False;
    }

    if (!ctx)
    {
        if (!coreProfile && !es2Profile)
            fprintf(stderr, "Error: glXCreateContext failed\n");
        XFree(visinfo);
        return False;
    }

    /*
    * Create a window so that we can just bind the context.
    */
    attr.background_pixel = 0;
    attr.border_pixel = 0;
    attr.colormap = XCreateColormap(dpy, root, visinfo->visual, AllocNone);
    attr.event_mask = StructureNotifyMask | ExposureMask;
    mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;
    win = XCreateWindow(dpy, root, 0, 0, width, height,
                        0, visinfo->depth, InputOutput,
                        visinfo->visual, mask, &attr);

    if (glXMakeCurrent(dpy, win, ctx))
    {
        /*
        const char *serverVendor = glXQueryServerString(dpy, scrnum, GLX_VENDOR);
        const char *serverVersion = glXQueryServerString(dpy, scrnum, GLX_VERSION);
        const char *serverExtensions = glXQueryServerString(dpy, scrnum, GLX_EXTENSIONS);
        const char *clientVendor = glXGetClientString(dpy, GLX_VENDOR);
        const char *clientVersion = glXGetClientString(dpy, GLX_VERSION);
        const char *clientExtensions = glXGetClientString(dpy, GLX_EXTENSIONS);
        const char *glxExtensions = glXQueryExtensionsString(dpy, scrnum);
        */
        const char *glVendor = (const char *)glGetString(GL_VENDOR);
        const char *glRenderer = (const char *)glGetString(GL_RENDERER);
        const char *glVersion = (const char *)glGetString(GL_VERSION);
        glInfoStruct->vendorString = std::string(glVendor);
        glInfoStruct->rendererString = std::string(glRenderer);
        glInfoStruct->versionString = std::string(glVersion);

        char *glExtensions = NULL;
        int glxVersionMajor = 0;
        int glxVersionMinor = 0;
        char *displayName = NULL;
        char *colon = NULL, *period = NULL;
        struct ext_functions extfuncs;

        CheckError(__LINE__);

        /* Get some ext functions */
        extfuncs.GetProgramivARB = (GETPROGRAMIVARBPROC)
            glXGetProcAddressARB((GLubyte *)"glGetProgramivARB");
        extfuncs.GetStringi = (GETSTRINGIPROC)
            glXGetProcAddressARB((GLubyte *)"glGetStringi");
        extfuncs.GetConvolutionParameteriv = (GETCONVOLUTIONPARAMETERIVPROC)
            glXGetProcAddressARB((GLubyte *)"glGetConvolutionParameteriv");

        if (!glXQueryVersion(dpy, &glxVersionMajor, &glxVersionMinor))
        {
            fprintf(stderr, "Error: glXQueryVersion failed\n");
            exit(1);
        }

        /* Get list of GL extensions */
        if (coreProfile && extfuncs.GetStringi)
            glExtensions = build_core_profile_extension_list(&extfuncs);
        if (!glExtensions)
        {
            coreProfile = False;
            glExtensions = (char *)glGetString(GL_EXTENSIONS);
        }

        CheckError(__LINE__);

        if (!coreWorked)
        {
            /* Strip the screen number from the display name, if present. */
            if (!(displayName = (char *)malloc(strlen(DisplayString(dpy)) + 1)))
            {
                fprintf(stderr, "Error: malloc() failed\n");
                exit(1);
            }
            strcpy(displayName, DisplayString(dpy));
            colon = strrchr(displayName, ':');
            if (colon)
            {
                period = strchr(colon, '.');
                if (period)
                    *period = '\0';
            }

            //printf("display: %s  screen: %d\n", displayName, scrnum);
            free(displayName);

            //if (strstr(glxExtensions, "GLX_MESA_query_renderer"))
            //    query_renderer();
            //print_gpu_memory_info(glExtensions);
            //printf("OpenGL vendor string: %s\n", glVendor);
            //printf("OpenGL renderer string: %s\n", glRenderer);
        }
        else
            //printf("\n");

            //printf("%s version string: %s\n", oglstring, glVersion);

            version = (glVersion[0] - '0') * 10 + (glVersion[2] - '0');

        CheckError(__LINE__);

#ifdef GL_VERSION_2_0
        if (version >= 20)
        {
            char *v = (char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
            glInfoStruct->shadingLangVersionString = std::string(v);
            //printf("%s shading language version string: %s\n", oglstring, v);
        }
#endif
        CheckError(__LINE__);
#ifdef GL_VERSION_3_0
        if (version >= 30 && !es2Profile)
        {
            GLint flags;
            glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
            //printf("%s context flags: %s\n", oglstring, context_flags_string(flags));
        }
#endif
        CheckError(__LINE__);
#ifdef GL_VERSION_3_2
        if (version >= 32 && !es2Profile)
        {
            GLint mask;
            glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &mask);
            //printf("%s profile mask: %s\n", oglstring, profile_mask_string(mask));
        }
#endif

        CheckError(__LINE__);

        if (coreProfile)
            free(glExtensions);
    }
    else
    {
        fprintf(stderr, "Error: glXMakeCurrent failed\n");
    }

    glXDestroyContext(dpy, ctx);
    XFree(visinfo);
    XDestroyWindow(dpy, win);
    XSync(dpy, 1);
    return True;
}

/*
 * Stand-alone Mesa doesn't really implement the GLX protocol so it
 * doesn't really know the GLX attributes associated with an X visual.
 * The first time a visual is presented to Mesa's pseudo-GLX it
 * attaches ancilliary buffers to it (like depth and stencil).
 * But that usually only works if glXChooseVisual is used.
 * This function calls glXChooseVisual() to sort of "prime the pump"
 * for Mesa's GLX so that the visuals that get reported actually
 * reflect what applications will see.
 * This has no effect when using true GLX.
 
static void
mesa_hack(Display *dpy, int scrnum)
{
   static int attribs[] = {
      GLX_RGBA,
      GLX_RED_SIZE, 1,
      GLX_GREEN_SIZE, 1,
      GLX_BLUE_SIZE, 1,
      GLX_DEPTH_SIZE, 1,
      GLX_STENCIL_SIZE, 1,
      GLX_ACCUM_RED_SIZE, 1,
      GLX_ACCUM_GREEN_SIZE, 1,
      GLX_ACCUM_BLUE_SIZE, 1,
      GLX_ACCUM_ALPHA_SIZE, 1,
      GLX_DOUBLEBUFFER,
      None
   };
   XVisualInfo *visinfo;

   visinfo = glXChooseVisual(dpy, scrnum, attribs);
   if (visinfo)
      XFree(visinfo);
}
*/

/*
GLXInfo class
TODO - eventually move everything in here
 */

GfxInfo::GfxInfo()
{
    return;
}

GfxInfo::~GfxInfo()
{
    return;
}

int GfxInfo::populateGlInfo(){
    Display *dpy;
    dpy = XOpenDisplay( nullptr );
    if (!dpy)
    {
        LOG(LOG_ERR, "Unable to open display " << XDisplayName(nullptr));
        throw std::runtime_error( "Unable to open display" );
        return -1;
    }

    // mesa_hack(dpy, 0);

    LOG( LOG_DEBUG, "Grabbing OpenGL info for base, core, and ES" );
    this->glCoreInfo.available = populateGlStruct( dpy,
                                                   true, false, false, &this->glCoreInfo );

    this->glInfo.available = populateGlStruct( dpy, false, false,
                                               this->glCoreInfo.available, &this->glInfo );

    this->glEsInfo.available = populateGlStruct( dpy, false, true, true, &this->glEsInfo );

    LOG( LOG_DEBUG, "Closing OpenGL display" );
    XCloseDisplay(dpy);
    return 0;
}
const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

int GfxInfo::populateVkInfo(){
    VkInstance vkInstance;
    VkInstanceCreateInfo vkCreateInfo = {};
    VkApplicationInfo vkAppInfo = {};
    vkAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    vkAppInfo.pApplicationName = "CPU-L";
    vkAppInfo.applicationVersion = 1;
    vkAppInfo.pEngineName = nullptr;
    vkAppInfo.engineVersion = 0;
    vkAppInfo.apiVersion = VK_API_VERSION_1_0;


    vkCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    vkCreateInfo.pApplicationInfo = &vkAppInfo;

    // Enable all available instance extensions
    std::vector< VkExtensionProperties > vkExtProps;
    uint32_t extCount = 0;
    vkEnumerateInstanceExtensionProperties( nullptr, &extCount, nullptr );
    vkExtProps.resize( extCount );
    vkEnumerateInstanceExtensionProperties( nullptr, &extCount, vkExtProps.data() );

    
    std::vector< std::string > inst_exts_str;
    for (auto &ext : vkExtProps) inst_exts_str.push_back( std::string( ext.extensionName ) );

    std::vector<const char *> inst_exts;
    for (auto &ext : inst_exts_str) inst_exts.push_back( ext.c_str() );

    if( std::find( inst_exts_str.begin(), inst_exts_str.end(), VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME ) == inst_exts_str.end() ){
        LOG( LOG_WARN, "Physical device extension not available" );
        return -1;
    }

    vkCreateInfo.enabledExtensionCount = static_cast<uint32_t>(inst_exts.size());
    vkCreateInfo.ppEnabledExtensionNames = inst_exts.data();

    //vkCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    //vkCreateInfo.ppEnabledLayerNames = validationLayers.data();

    vkCreateInfo.enabledLayerCount = 0; // No layers required here

    // Create a Vulkan instance
    if ( vkCreateInstance( &vkCreateInfo, nullptr, &vkInstance ) != VK_SUCCESS ) {
        LOG( LOG_WARN, "Could not create Vulkan instance" );
        return -1;
    }

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices( vkInstance, &deviceCount, nullptr );
    
    if( !deviceCount ){
        LOG( LOG_WARN, "No Vulkan-compatible devices found" );
        vkDestroyInstance( vkInstance, nullptr );
        return -1;
    }

    LOG( LOG_DEBUG, "Found " << deviceCount << " Vulkan-compatible device(s)" );
    std::vector< VkPhysicalDevice > vkDevices( deviceCount );
    vkEnumeratePhysicalDevices( vkInstance, &deviceCount, vkDevices.data() );

    this->vkDevProperties.reserve( deviceCount );

    for( const auto &dev : vkDevices ){
        VkPhysicalDeviceProperties devProps;
        vkGetPhysicalDeviceProperties( dev, &devProps );
        LOG( LOG_DEBUG, "Getting driver info for device " << devProps.deviceName );

        // Need to make sure ext VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME exists
        std::vector< VkExtensionProperties > vkExtensionProperties;


        uint32_t devExtCount = 0;
        vkEnumerateDeviceExtensionProperties( dev, nullptr, &devExtCount, nullptr );
        vkExtensionProperties.resize( devExtCount );
        vkEnumerateDeviceExtensionProperties( dev, nullptr, &devExtCount, vkExtensionProperties.data() );

        // Lambda to check if a given extension is available for this device
        auto checkDevExt = [ &vkExtensionProperties ]( std::string extName ){ 
            return std::find_if( vkExtensionProperties.begin(), vkExtensionProperties.end(),
                          [ extName ]( VkExtensionProperties const & vkProp ) { return std::string( vkProp.extensionName ) == extName; }
                        ) == vkExtensionProperties.end();
        };

        if( checkDevExt( VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME ) && checkDevExt( VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME ) ){
            LOG( LOG_WARN, "Could not find driver-property extension for device " << devProps.deviceName );
            continue;
        }

        VkPhysicalDeviceProperties2 devProps2;
        VkPhysicalDeviceDriverPropertiesKHR driverProps;

        driverProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES_KHR;
        driverProps.pNext = nullptr;
        devProps2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        devProps2.pNext = &driverProps;

        vkGetPhysicalDeviceProperties2( dev, &devProps2 );

        VkDeviceInfoProperties ourDevProps { devProps, driverProps };
        this->vkDevProperties.push_back( ourDevProps );
        
    }

    vkDestroyInstance( vkInstance, nullptr );
    return 0;
}

int GfxInfo::populateKernelInfo(){

    if( auto ret = uname( &this->kernelInfo ) ){
        auto err = errno;
        LOG( LOG_WARN, "Could not get kernel information, error" << errno );
        return err;
    }

    return 0;
}

int GfxInfo::populateInfo()
{
    auto glPopulatedErr = this->populateGlInfo();
    if( glPopulatedErr ){
        LOG( LOG_WARN, "Gl information may not be fully populated" );
    }

    auto vkPopulatedErr = this->populateVkInfo();
    if( vkPopulatedErr ){
        LOG( LOG_WARN, "Vulkan information may not be fully populated" );
    }

    auto kernPopulatedErr = this->populateKernelInfo();
    if( kernPopulatedErr ){
        LOG( LOG_WARN, "Kernel information may not populated" );
    }
    
    return ( glPopulatedErr | vkPopulatedErr | kernPopulatedErr ) ? -1 : 0;
}