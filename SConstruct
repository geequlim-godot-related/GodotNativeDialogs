#!/usr/bin/env python
import os, subprocess

# Local dependency paths
godot_headers_path = ARGUMENTS.get("gd_headers", os.getenv("GODOT_HEADERS", "#godot_headers/"))
cpp_bindings_path = ARGUMENTS.get("cpp_bindings", os.getenv("GD_CPP_BINDINGS", "#cpp_bindings/"))
# default to release build, add target=debug to build debug build
target = ARGUMENTS.get("target", "release")

# platform= makes it in line with Godots scons file, keeping p for backwards compatibility
platform = ARGUMENTS.get("p", "linux")
platform = ARGUMENTS.get("platform", platform)

# This makes sure to keep the session environment variables on windows, 
# that way you can run scons in a vs 2017 prompt and it will find all the required tools
env = Environment()
env['platform'] = platform
if platform == "windows":
    env = Environment(ENV = os.environ)

if ARGUMENTS.get("use_llvm", "no") == "yes":
    env["CXX"] = "clang++"

if platform == "osx":
    env.Append(CCFLAGS = ['-g','-O3', '-arch', 'x86_64', '-std=c++14'])
    env.Append(LINKFLAGS = ['-arch', 'x86_64'])
elif platform == "linux":
    env.Append(CCFLAGS = ['-fPIC', '-g','-O3', '-std=c++14'])
    env.Append(CXXFLAGS='-std=c++0x')
    env.Append(LINKFLAGS = ['-Wl,-R,\'$$ORIGIN\''])
elif platform == "windows":
    if target == "debug":
        env.Append(CCFLAGS = ['-EHsc', '-D_DEBUG', '/MDd'])
    else:
        env.Append(CCFLAGS = ['-O2', '-EHsc', '-DNDEBUG', '/MD'])

# GDNative headers & GDNativeCPP
env.Append(CPPPATH=['.', godot_headers_path])
env.Append(CPPPATH=[cpp_bindings_path + '/include/'])
env.Append(CPPPATH=[cpp_bindings_path + '/include/core'])
env.Append(LIBPATH=[cpp_bindings_path + '/bin'])
if (os.name == "nt" and os.getenv("VCINSTALLDIR")):
    env.Append(LINKFLAGS=['godot_cpp_bindings.lib'])
else:
    env.Append(LIBS=['godot_cpp_bindings'])


# ------------------------------ methods ---------------------------------------
def add_source_files(self, filetype):
    import glob
    import string
    if type(filetype) == type(""):
        return glob.glob(self.Dir('.').abspath + "/" + filetype)
    else:
        return filetype
env.__class__.add_source_files = add_source_files

# ------------------------------ modules ---------------------------------------
Export('env')
for m in os.listdir('modules'):
    if os.path.isdir(os.path.join('modules', m)):
        sub_script = os.path.join('modules', m, 'SCsub')
        if os.path.isfile(sub_script):
            SConscript(sub_script)
