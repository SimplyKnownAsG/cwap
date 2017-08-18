
PROJECT:=cwap

SRC_DIR=$(PROJECT)
OBJ_DIR=$(BUILD_DIR)
BIN_DIR=$(BUILD_DIR)/bin


include MakefileFunctions.in

CLANG_CXX_FLAGS := `llvm-config --cxxflags | sed -r 's/(-specs[^ ]+|-Wno-maybe-uninitialized|-fno-exceptions)//g'`
CLANG_LD_FLAGS := `llvm-config --ldflags` -lclang

ifeq ($(UNAME_S), Darwin)
	CLANG_CXX_FLAGS+= -stdlib=libstdc++
endif

CXX=clang++
LD=clang++
CXXFLAGS= -std=c++1y $(CLANG_CXX_FLAGS) -Wno-covered-switch-default -I. -c $< -o $@
LDFLAGS= $(CLANG_LD_FLAGS) -Wno-covered-switch-default
LDSHARED=clang++ -Wall -shared -o $@ $^ $< $(LDFLAGS)
LDSTATIC=ar crs $@
LDEXE=clang++ -Wall -Wcovered-switch-default -o $@ $(CLANG_LD_FLAGS)
LIB$(PROJECT)=$(BIN_DIR)/lib$(PROJECT).a
PY_EXT=$(BIN_DIR)/_$(PROJECT).so

$(PROJECT)EXEC=$(BIN_DIR)/$(PROJECT).exe

ifeq ($(MAKECMDGOALS),debug)
	BUILD_DIR=debug
	CXXFLAGS+= -g
	LDFLAGS+= -g
else
	CXXFLAGS+= -O2
	BUILD_DIR=release
endif

HEADER_LIST:=$(PROJECT)/$(PROJECT).hpp
VERSION_FILES:=$(PROJECT)/Version.cpp
AUTO_FILES:=%_wrap.hpp %_wrap.cpp $(HEADER_LIST) $(VERSION_FILES)

SRC = $(filter-out $(AUTO_FILES),$(call rwildcard, $(SRC_DIR), *.cpp))
HEADERS = $(filter-out $(AUTO_FILES), $(call rwildcard, $(SRC_DIR), *.hpp))
OBJ = $(patsubst %.cpp,$(OBJ_DIR)/%.obj,$(SRC))

CPP_TEST_EXEC := $(sort $(patsubst %.cpp, $(BIN_DIR)/%.exe, $(call rwildcard, tests examples, *.cpp)))
CPP_TEST_OBJ := $(patsubst $(BIN_DIR)/%.exe,$(OBJ_DIR)/%.obj,$(CPP_TEST_EXEC))

all: $(VERSION_FILES) test

# this must be placed after your .DEFAULT_GOAL, or you can manually state what it is
# https://www.gnu.org/software/make/manual/html_node/Special-Variables.html
$(call create_dirs,$(PY_EXT) $(LIB$(PROJECT)) $(OBJ) $(CPP_TEST_EXEC) $(CPP_TEST_OBJ))

debug: test

clean:
	-$(RM) -r ./debug/ ./release/
	-$(RM) $(HEADER_LIST) $(VERSION_FILES)

clobber:
	-git clean -fxd --exclude .vscode/

$(HEADER_LIST): $(HEADERS)
	@echo "// this file is auto-generated, do not modify" > $@
	@$(foreach hpp,$^,echo '#include "$(hpp)"' >> $@;)
	@echo "" >> $@

$(VERSION_FILES): $(HEADERS) $(SRC)
	@# generate Version source
	@echo '#include "$(PROJECT)/Version.hpp"' > $(PROJECT)/Version.cpp
	@echo 'namespace $(PROJECT) {' >> $(PROJECT)/Version.cpp
	@echo '    const std::string Version::git_sha1 = "'`git rev-parse HEAD`'";' >> $(PROJECT)/Version.cpp
	@echo '' >> $(PROJECT)/Version.cpp
	@echo '    const std::string Version::git_description = "'`git describe --always --dirty`'";' >> $(PROJECT)/Version.cpp
	@echo '}' >> $(PROJECT)/Version.cpp

# test: $($(PROJECT)EXEC) test_cpp test_py
test: $($(PROJECT)EXEC) test_cpp

test_py: $(PY_EXT)
	$(call colorecho,PYTHONPATH=$(BIN_DIR) python -m unittest discover -bvs tests/pytests)

$(LIB$(PROJECT)): $(filter-out %Main.obj %main.obj,$(OBJ)) $(OBJ_DIR)/$(PROJECT)/Version.obj | $(TARGET_DIRS)
	$(call colorecho,$(LDSTATIC) $^)

$($(PROJECT)EXEC): $(LIB$(PROJECT)) $(filter %Main.obj %main.obj,$(OBJ)) | $(TARGET_DIRS)
	$(call colorecho,$(LDEXE) $^ $(CLANG_LD_FLAGS))

$(PY_EXT): $(LIB$(PROJECT)) $(SRC_DIR)/$(PROJECT)_wrap.cpp | $(TARGET_DIRS)
	$(call colorecho,$(LDSHARED))

test_cpp: $(CPP_TEST_EXEC)
	$(foreach cpp_test_exec, $^, $(call colorecho,$(cpp_test_exec)))

$(BIN_DIR)/%.exe: $(OBJ_DIR)/%.obj $(LIB$(PROJECT)) | $(TARGET_DIRS) $(HEADER_LIST)
	$(call colorecho,$(LDEXE) $(filter-out $(PROJECT)/$(PROJECT).hpp,$^) $(CLANG_LD_FLAGS))

$(SRC_DIR)/$(PROJECT)_wrap.cpp $(SRC_DIR)/$(PROJECT)_wrap.hpp: $(HEADER_LIST)
	$(call colorecho,)

$(OBJ_DIR)/%.obj: %.cpp %.hpp | $(TARGET_DIRS)
	$(call colorecho,$(CXX) $(CXXFLAGS))

$(OBJ_DIR)/%.obj: %.cpp | $(TARGET_DIRS)
	$(call colorecho,$(CXX) $(CXXFLAGS))

documentation: apidoc

apidoc: $(HEADERS) $(patsubst %.py,%.log,$(call rwildcard,examples,*.py))
	$(call colorecho,doxygen doxyconf)

