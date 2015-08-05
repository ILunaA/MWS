Arduino restarts each time the Serial port is opened
----------------------------------------------------

Try:
A 10microF capacitor between pin RESET and pin GND to disable that function hardware side.
http://arduino.stackexchange.com/questions/439/why-does-starting-the-serial-monitor-restart-the-sketch

Copy mws.py in $weewxroot/bin/user/ (in RPI it will be /home/weewx/bin/user/) 

Add to $weewxroot/weewx.conf (in RPI it will be /home/weewx/weewx.conf):
------------------------------------------------------------------------

station_type = MWS #was: Simulator

##############################################################################

[MWS]
    
    # Use a non standard driver called mws
    driver = user.mws
    # Give a 5 minute interval for data storage
    loop_interval = 300
    # RPI has this port detected
    port='/dev/ttyACM0'

##############################################################################

