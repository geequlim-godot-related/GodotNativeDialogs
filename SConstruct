#!/usr/bin/env python
import os, subprocess

# Local dependency paths
godot_headers_path = ARGUMENTS.get("godot_headers", os.getenv("GODOT_HEADERS", "#godot_headers/"))
cpp_bindings_path = ARGUMENTS.get("godot-cpp", os.getenv("GD_CPP_BINDINGS", "#cpp_bindings/"))
# default to release build, add target=debug to build debug build
target = ARGUMENTS.get("target", "debug")
cpp_library = "libgodot-cpp"
# platform= makes it in line with Godots scons file, keeping p for backwards compatibility
platform = ARGUMENTS.get("p", "linux")
platform = ARGUMENTS.get("platform", platform)
bits=64
# This makes sure to keep the session environment variables on windows, 
# that way you can run scons in a vs 2017 prompt and it will find all the required tools
env = Environment()
print(godot_headers_path)
print(cpp_bindings_path)
print(platform)
env['platform'] = platform
if ARGUMENTS.get("use_llvm", "no") == "yes":
    env["CXX"] = "clang++"

if platform == "osx":
    cpp_library += '.osx'
    env.Append(CCFLAGS = ['-g','-O3', '-arch', 'x86_64', '-std=c++14'])
    env.Append(LINKFLAGS = ['-arch', 'x86_64'])
elif platform == "linux":
    cpp_library += '.linux'
    env.Append(CCFLAGS = ['-fPIC', '-g','-O3', '-std=c++14'])
    env.Append(CXXFLAGS='-std=c++0x')
    env.Append(LINKFLAGS = ['-Wl,-R,\'$$ORIGIN\''])
elif platform == "windows":
    print("Compiling for windows")
    cpp_library += '.windows'
    if target == "debug":
        env.Append(CCFLAGS = ['-EHsc', '-D_DEBUG', '/MDd'])
    else:
        env.Append(CCFLAGS = ['-O2', '-EHsc', '-DNDEBUG', '/MD'])

# GDNative headers & GDNativeCPP
env.Append(CPPPATH=['.', 'godot-cpp/godot_headers/'])
env.Append(CPPPATH=['godot-cpp' + '/include/'])
env.Append(CPPPATH=['godot-cpp' + '/include/gen'])
env.Append(CPPPATH=['godot-cpp' + '/include/core'])
env.Append(LIBPATH=['godot-cpp' + '/bin'])
cpp_library += '.' + target + '.' + str(bits)
if (os.name == "nt" and os.getenv("VCINSTALLDIR")):
    env.Append(LINKFLAGS=[cpp_library + '.lib'])
else:
    env.Append(LIBS=[cpp_library])


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
