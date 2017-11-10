
PROJECT:=cwap

SRC_DIR=src/$(PROJECT)
OBJ_DIR=$(BUILD_DIR)/obj
BIN_DIR=$(BUILD_DIR)/bin
DEP_DIR=./.dependencies


include MakefileFunctions.in

CLANG_CXX_FLAGS := `llvm-config --cxxflags | sed -r 's/(-specs[^ ]+|-Wno-maybe-uninitialized|-fno-exceptions)//g'`
CLANG_LD_FLAGS := `llvm-config --ldflags` -lclang

ifeq ($(UNAME_S), Darwin)
	CLANG_CXX_FLAGS+= -stdlib=libstdc++
endif

CXX=clang++
LD=clang++
CXXFLAGS= -std=c++1y $(CLANG_CXX_FLAGS) -Wno-covered-switch-default -I./src/ -I$(DEP_DIR) -c $< -o $@
LDFLAGS= $(CLANG_LD_FLAGS) -Wno-covered-switch-default
LDSHARED=clang++ -Wall -shared -o $@ $^ $< $(LDFLAGS)
LDSTATIC=ar crs $@
LDEXE=clang++ -Wall -Wcovered-switch-default -o $@ $(CLANG_LD_FLAGS)
LIB$(PROJECT)=$(BIN_DIR)/lib$(PROJECT).a
PY_EXT=$(BIN_DIR)/_$(PROJECT).so

$(PROJECT)EXEC=$(BIN_DIR)/$(PROJECT).exe

ifeq ($(MAKECMDGOALS),debug)
	BUILD_DIR=debug
	CXXFLAGS+= -g -O0
	LDFLAGS+= -g -O0
else
	CXXFLAGS+= -O2
	BUILD_DIR=release
endif

HEADER_LIST:=$(SRC_DIR)/$(PROJECT).hpp
VERSION_FILES:=$(SRC_DIR)/Version.cpp
TEST_MACROS_HEADER:=$(DEP_DIR)/catch.hpp
GLOB:=$(DEP_DIR)/globpp.hpp
DEPENDENCIES:=$(GLOB) $(TEST_MACROS_HEADER)
AUTO_FILES:=$(HEADER_LIST) $(VERSION_FILES) $(DEPENDENCIES)

SRC = $(filter-out $(AUTO_FILES),$(call rwildcard, $(SRC_DIR), *.cpp))
HEADERS = $(filter-out $(AUTO_FILES), $(call rwildcard, $(SRC_DIR), *.hpp))
OBJ = $(patsubst %.cpp,$(OBJ_DIR)/%.obj,$(SRC))

CPP_TEST_EXEC := $(sort $(patsubst %.cpp, $(BIN_DIR)/%.exe, $(call rwildcard, tests, *Main.cpp)))
CPP_TEST_OBJ := $(patsubst %.cpp,$(OBJ_DIR)/%.obj, $(call rwildcard, tests, *.cpp))

all: $(VERSION_FILES) test

# this must be placed after your .DEFAULT_GOAL, or you can manually state what it is
# https://www.gnu.org/software/make/manual/html_node/Special-Variables.html
$(call create_dirs,$(PY_EXT) $(LIB$(PROJECT)) $(OBJ) $(CPP_TEST_EXEC) $(CPP_TEST_OBJ))

debug: test

clean:
	-$(RM) -r ./debug/ ./release/

clobber:
	-$(RM) $(AUTO_FILES)
	-git clean -fxd --exclude .vscode/

$(HEADER_LIST): $(HEADERS)
	@echo "// this file is auto-generated, do not modify" > $@
	@$(foreach hpp,$^,echo '#include "$(patsubst src/%,%,$(hpp))"' >> $@;)
	@echo "" >> $@

$(VERSION_FILES): $(HEADERS) $(SRC)
	@# generate Version source
	@echo '#include "$(PROJECT)/Version.hpp"' > $(SRC_DIR)/Version.cpp
	@echo 'namespace $(PROJECT) {' >> $(SRC_DIR)/Version.cpp
	@echo '    const std::string Version::git_sha1 = "'`git rev-parse HEAD`'";' >> $(SRC_DIR)/Version.cpp
	@echo '' >> $(SRC_DIR)/Version.cpp
	@echo '    const std::string Version::git_description = "'`git describe --always --dirty`'";' >> $(SRC_DIR)/Version.cpp
	@echo '}' >> $(SRC_DIR)/Version.cpp

# test: $($(PROJECT)EXEC) test_cpp test_py
test: $($(PROJECT)EXEC) test_cpp

test_py: $(PY_EXT)
	$(call colorecho,PYTHONPATH=$(BIN_DIR) python -m unittest discover -bvs tests/pytests)

$(LIB$(PROJECT)): $(filter-out %Main.obj %main.obj,$(OBJ)) $(OBJ_DIR)/src/$(PROJECT)/Version.obj | $(TARGET_DIRS) $(HEADER_LIST)
	$(call colorecho,$(LDSTATIC) $^)

$($(PROJECT)EXEC): $(filter %Main.obj %main.obj,$(OBJ)) $(LIB$(PROJECT)) | $(TARGET_DIRS)
	$(call colorecho,$(LDEXE) $^ $(CLANG_LD_FLAGS))

$(PY_EXT): $(LIB$(PROJECT)) $(SRC_DIR)/$(PROJECT)_wrap.cpp | $(TARGET_DIRS)
	$(call colorecho,$(LDSHARED))

test_cpp: $(CPP_TEST_EXEC) | $(DEPENDENCIES)
	$(foreach cpp_test_exec, $^, $(call colorecho,$(cpp_test_exec)))

$(CPP_TEST_EXEC): $(CPP_TEST_OBJ) $(LIB$(PROJECT)) | $(DEPENDENCIES)
	$(call colorecho,$(LDEXE) $(filter-out $(PROJECT)/$(PROJECT).hpp,$^) $(CLANG_LD_FLAGS))

$(OBJ_DIR)/%.obj: %.cpp %.hpp | $(TARGET_DIRS) $(DEPENDENCIES)
	$(call colorecho,$(CXX) $(CXXFLAGS))

$(OBJ_DIR)/%.obj: %.cpp | $(TARGET_DIRS) $(DEPENDENCIES)
	$(call colorecho,$(CXX) $(CXXFLAGS))

documentation: apidoc

apidoc: $(HEADERS) $(patsubst %.py,%.log,$(call rwildcard,examples,*.py))
	$(call colorecho,doxygen doxyconf)

$(TEST_MACROS_HEADER):
	$(call colorecho,wget https://raw.githubusercontent.com/philsquared/Catch/master/single_include/catch.hpp -P $(DEP_DIR))

$(GLOB):
	$(call colorecho,wget https://raw.githubusercontent.com/SimplyKnownAsG/globpp/master/globpp.hpp -P $(DEP_DIR))

