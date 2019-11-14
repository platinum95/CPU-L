project( 'cpu-l-lib', 'cpp' )

pkg = import('pkgconfig')

vkdep = dependency( 'vulkan' )
gldep = dependency( 'opengl' )
glxdep = dependency( 'glx' )
x11dep = dependency( 'x11' )

libsrc = [ 'gfxinfo.cpp', 'glxinfo_common_lib.cpp' ]
gfxinfo_lib = library( 'gfxinfo', sources : libsrc, dependencies : [ vkdep, gldep, x11dep, glxdep ], install : true )

pkg.generate( libraries : gfxinfo_lib,
              version : '0.1',
              name : 'gfxinfo',
              description : 'Simple library for getting information on graphics stack' )

gfxinfo_dep = dependency( 'gfxinfo' )

testsrc = 'cpu-l-lib_test.cpp'
executable( 'cpu-l-lib_test', sources : testsrc, dependencies : gfxinfo_dep )
