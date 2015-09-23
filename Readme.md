

WIP json API to remote control some basic functions of obs-studio. Examples/info is limited to linux at the moment.

Compile
=======

Until some cmake stuff happens the simple makefile will have to do. Please edit the `OBS_LIB`, `OBS_INCLUDE` and `PLUGIN_INSTALL_DIR` to the relevant paths.

* `make`
* `make install`

example calls
=============

Send fake HotKey event
----------------------

Find your keybindings in `$HOME/.config/obs-studio/basic/profiles/$PROFILE_NAME/basic.ini`

examples written with these bindings bound to start/stop recording.

* `curl -X POST -d '{"action":"hotkey","key":"OBS_KEY_NUMSLASH","control":"true"}' http://localhost:8080/`
* `curl -X POST -d '{"action":"hotkey","key":"OBS_KEY_NUMASTERISK","control":"false"}' http://localhost:8080/`