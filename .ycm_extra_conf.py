
import subprocess


CLANG_CXX_FLAGS = subprocess.check_output('llvm-config --cxxflags', shell=True).strip().split()
CLANG_LD_FLAGS =  subprocess.check_output('llvm-config --ldflags', shell=True).strip().split() + ['-lclang']
# ifeq ($(UNAME_S), Darwin)
CLANG_CXX_FLAGS += ['-stdlib=libstdc++']
# endif

# ifeq ($(OS),Windows_NT)
if False:
    pass
    # CXXFLAGS= -nologo -EHa -fp:strict -MD "-IC:\Python27\include" -I. -c $< -Fo:$@
    # LDFLAGS= -nologo '-libpath:C:\Python27\libs'
    # LDSHARED=cl  -nologo -EHa -fp:strict -MD "-IC:\Python27\include" -I. -DLL $^ -link $(LDFLAGS) -DLL -out:$@
    # LDSTATIC=lib $(LDFLAGS) -out:$@
    # LDEXE=link $(LDFLAGS) -out:$@
    # LIB$(PROJECT)=$(BIN_DIR)/lib$(PROJECT).lib
    # PY_EXT=$(BIN_DIR)/_$(PROJECT).pyd
else:
    CXXFLAGS= '-fPIC -Wall -std=c++1y'.split() + CLANG_CXX_FLAGS + '-Wno-covered-switch-default -I/usr/include/python2.7 -I./ -c'.split()
    LDFLAGS= '-fPIC'.split() + CLANG_LD_FLAGS
    LDSHARED = '-Wall -shared -o'.split() + LDFLAGS

FLAGS = {
        'flags': CXXFLAGS
        }

def FlagsForFile(filename, **kwargs):
    return FLAGS

