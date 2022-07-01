# Ultimate Blind Test Buzzer

## Brief
The Ultimate Blind Test Buzzer (ubt_buzzer) is a companion device for the Ultimate Blind Test. Its main goal
is to provide a basic user interface to allow blind test players to compete.

## Architecture
The buzzer relies on ESP32 with a button connected to it. The buzzer must connect to an access point which offers access to the Ultimate Blind Test server. Once connected, the Ultimate Blind Test server and the buzzer communicate over websocket, for configuration and for statuses (e.g : button pushed)

## Quickstart
* Install ESP IDF SDK
* activate SDK with `export.sh`
* use `idf.py build` to build the project
* plug the buzzer to computer with an USB cable
* use `idf.py flash` to reflash the buzzer
* advanced user : you can use `idf.py monitor` to manage both previous steps and get device logs

## Buzzer network configuration
The project has a default network configuration applied, which can be tuned if needed. You can edit the network configuration by executing `idf.py menuconfig`, then selecting the "Buzzer configuration" menu.

## IDE configuration
The project has a basic Visual Studio code configuration, enable to Vscode "tasks" :
* build
* flash
Moreover, Intellisense is able to resolve ESP IDF symbols if your ESP IDF SK is at the same level than the project. You can edit ESP IDF path in `.vscode/c_cpp_properties.json` if you want to set another path.
