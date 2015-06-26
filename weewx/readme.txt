Arduino restarts each time the Serial port is opened
----------------------------------------------------

Try:
A 10microF capacitor between pin RESET and pin GND to disable that function hardware side.
http://arduino.stackexchange.com/questions/439/why-does-starting-the-serial-monitor-restart-the-sketch

Copy mws.py in $weewxroot/bin/user/

Add to $weewxroot/weewx.conf:
----------------------------

station_type = MWS #was: Simulator

##############################################################################

[MWS]

    driver = user.mws
    loop_interval = 15
    port='/dev/ttyACM0'

##############################################################################

