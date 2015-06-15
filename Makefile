
#Props to github.com/bazukas/obs-qtwebkit for most of this file

#TODO: change to using cmake or some such (load settings/vars from existing obs install?)

#------------------------
#Settings to change pre-compile
#------------------------

ifndef OBS_INCLUDE
#OBS_INCLUDE = /usr/include/obs
OBS_INCLUDE = /home/admalledd/bin/obs/include
endif
ifndef OBS_LIB
#OBS_LIB = /usr/lib
OBS_LIB = /home/admalledd/bin/obs/bin/64bit
endif

CXXFLAGS = -std=c++11 -Wall -g -fPIC -I$(OBS_INCLUDE) -I./src -D_DEBUG
CXX      = g++
RM       = /bin/rm -rf
LDFLAGS  = -L$(OBS_LIB)
LDLIBS_LIB   = -lobs -lrt

LIB = build/obs-rcon.so
LIB_OBJ = build/rcon-main.o build/rcon-server.o build/mongoose.o

PLUGIN_BUILD_DIR = build/obs-rcon
PLUGIN_INSTALL_DIR = ~/.obs-studio/plugins/obs-rcon

all: plugin

.PHONY: plugin
plugin: $(LIB)
	mkdir -p $(PLUGIN_BUILD_DIR)/bin
	cp $(LIB) $(PLUGIN_BUILD_DIR)/bin

.PHONY: install
install: plugin
	mkdir -p $(PLUGIN_INSTALL_DIR)
	cp -r $(PLUGIN_BUILD_DIR)/* $(PLUGIN_INSTALL_DIR)

.PHONY: uninstall
uninstall:
	$(RM) $(PLUGIN_INSTALL_DIR)

$(LIB): $(LIB_OBJ)
	$(CXX) -shared $(LDFLAGS) $^ $(LDLIBS_LIB) -o $@

build/%.opp: src/%.cpp
	mkdir -p $(@D)
	$(CXX) -c $(CXXFLAGS) $< -o $@

build/%.o: src/%.c
	mkdir -p $(@D)
	$(CXX) -c $(CXXFLAGS) $< -o $@

.PHONY: clean
clean:
	$(RM) $(LIB_OBJ) $(LIB) $(PLUGIN_BUILD_DIR)