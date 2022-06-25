# Ultimate Blind Test Buzzer

## Brief
The Ultimate Blind Test Buzzer (ubt_buzzer) is a companion device for the Ultimate Blind Test. Its main goal
is to provide a basic user interface to allow blind test players to compete.

## Architecture
The buzzer relies on ESP32 with a button connected to it. The buzzer must connect to an access point which offers access to the Ultimate Blind Test server. Once connected, the Ultimate Blind Test server and the buzzer communicate over websocket, for configuration and for statuses (e.g : button pushed)

## Quickstart
* Install ESP IDF SDK
* activated SDK with `export.sh`
* use `idf.py build` to build the project
* plug the buzzer to computer with an USB cable
* use `idf.py flash` to reflash the buzzer
* advanced user : you can use `idf.py monitor` to manage both previous steps and get device logs