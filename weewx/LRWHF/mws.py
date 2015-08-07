#!/usr/bin/env python
#
# Copyright 2014 Matthew Wall
# See the file LICENSE.txt for your rights.
# Modified by Yann Chemin for IWMI MWS station
# March 2015 Public Domain if previous copyright allows.
# Building Manual for the Mobile Weather Station is here:
# http://www.iwmi.cgiar.org/resources/mobile-weather-stations/


"""Driver for MWS weather stations.

Modified from weewx/trunk/drivers/ws1.py 

Previous notes from mws.py:
--------------------------
Thanks to Steve (sesykes71) for the testing that made this driver possible.

Thanks to Jay Nugent (WB8TKL) and KRK6 for weather-2.kr6k-V2.1
  http://server1.nuge.com/~weather/
"""

from __future__ import with_statement
import serial
import syslog
import time

import weewx.drivers

DRIVER_NAME = 'MWS'
DRIVER_VERSION = '0.1'


def loader(config_dict, _):
    return MWSDriver(**config_dict[DRIVER_NAME])

def confeditor_loader():
    return MWSConfEditor()


INHG_PER_MBAR = 0.0295333727
METER_PER_FOOT = 0.3048
MILE_PER_KM = 0.621371

DEFAULT_PORT = '/dev/ttyACM0'
DEBUG_READ = 0


def logmsg(level, msg):
    syslog.syslog(level, 'mws: %s' % msg)

def logdbg(msg):
    logmsg(syslog.LOG_DEBUG, msg)

def loginf(msg):
    logmsg(syslog.LOG_INFO, msg)

def logerr(msg):
    logmsg(syslog.LOG_ERR, msg)

class MWSDriver(weewx.drivers.AbstractDevice):
    """weewx driver that communicates with an IWMI-MWS station
    
    port - serial port
    [Required. Default is /dev/ttyACM0]

    polling_interval - how often to query the serial interface, seconds
    [Optional. Default is 1]

    max_tries - how often to retry serial communication before giving up
    [Optional. Default is 5]

    retry_wait - how long to wait, in seconds, before retrying after a failure
    [Optional. Default is 10]
    """
    def __init__(self, **stn_dict):
        self.port = stn_dict.get('port', DEFAULT_PORT)
        self.polling_interval = float(stn_dict.get('polling_interval', 1))
        self.max_tries = int(stn_dict.get('max_tries', 5))
        self.retry_wait = int(stn_dict.get('retry_wait', 10))
        self.last_rain = None
        loginf('driver version is %s' % DRIVER_VERSION)
        loginf('using serial port %s' % self.port)
        loginf('polling interval is %s' % self.polling_interval)
        global DEBUG_READ
        DEBUG_READ = int(stn_dict.get('debug_read', DEBUG_READ))

    @property
    def hardware_name(self):
        return "MWS"

    def genLoopPackets(self):
        ntries = 0
        while ntries < self.max_tries:
            ntries += 1
            try:
                packet = {'dateTime': int(time.time() + 0.5),
                          'usUnits': weewx.US}
                # open a new connection to the station for each reading
                with Station(self.port) as station:
                    readings = station.get_readings()
                data = Station.parse_readings(readings)
                packet.update(data)
                self._augment_packet(packet)
                ntries = 0
                yield packet
                if self.polling_interval:
                    time.sleep(self.polling_interval)
            except (serial.serialutil.SerialException, weewx.WeeWxIOError), e:
                logerr("Failed attempt %d of %d to get LOOP data: %s" %
                       (ntries, self.max_tries, e))
                time.sleep(self.retry_wait)
        else:
            msg = "Max retries (%d) exceeded for LOOP data" % self.max_tries
            logerr(msg)
            raise weewx.RetriesExceeded(msg)

    def _augment_packet(self, packet):
        # calculate the rain delta from rain total
        #if self.last_rain is not None:
        #    packet['rain'] = packet['long_term_rain'] - self.last_rain
        #else:
        #    packet['rain'] = None
        #self.last_rain = packet['long_term_rain']

        # no wind direction when wind speed is zero
        if 'windSpeed' in packet and not packet['windSpeed']:
            packet['windDir'] = None


class Station(object):
    def __init__(self, port):
        self.port = port
        self.baudrate = 9600
        self.timeout = 60
        self.serial_port = None

    def __enter__(self):
        self.open()
        return self

    def __exit__(self, _, value, traceback):
        self.close()

    def open(self):
        logdbg("open serial port %s" % self.port)
        self.serial_port = serial.Serial(self.port, self.baudrate,
                                         timeout=self.timeout)

    def close(self):
        if self.serial_port is not None:
            logdbg("close serial port %s" % self.port)
            self.serial_port.close()
            self.serial_port = None

    def read(self):
	buff=[]
	try:
		buf = self.serial_port.readline()
		i = 0
		for i in range(5):
			buf += self.serial_port.readline()
			i+=1
		buff=buf.split('\r\n')[-2].split(',')
       		n = len(buff)
       		if DEBUG_READ:
           		logdbg(buff)
	except:
		#do nothing
		n = 0
	return buff

    def get_readings(self):
        b = self.read()
        if DEBUG_READ:
            logdbg(b)
        return b

    @staticmethod
    def parse_readings(b):
        """
	  MWS station emits csv data format:

          #Serial.print("lon,lat,altitude,sats,date,GMTtime,winddir");
          #Serial.print(",windspeedms,windgustms,windspdms_avg2m,winddir_avg2m,windgustms_10m,windgustdir_10m");
          #Serial.print(",humidity,tempc,rainhourmm,raindailymm,rainindicate,rain5minmm,pressure,batt_lvl,light_lvl");

          [0]longitude
          [1]latitude
          [2]altitude
          [3]satellites number
          [4]date
          [5]GMT time = GPS time
          [6]winddir (0-360)
          [7]windspeed (m/s)
          [8]windgust (m/s)
          [9]windgust direction (0-360)
          [10]wind speed avg 2 minutes (m/s)
          [11]wind direction avg 2 minutes (0-360)
          [12]wind gust speed avg 10 min (m/s)
          [13]wind gust dir avg 10 min (0-360)
          [14]humidity (%)
          [15]temperature (Celsius)
          [16]rain hourly (mm/h)
          [17]rain daily (mm/d)
          [18]rain indicator 5 mins (0/1 flag)
          [19]rain 5 minutes (mm/5min)
          [20]pressure (hPa)
          [21]Battery Level (V)
          [22]Light level
        """
        data = dict()
	if(b[2]!='nan'):
        	data['altimeter'] = float(b[2])  # GPS altitude
	else:
        	data['altimeter'] = -999.0  # GPS altitude
	if(b[8]!='nan'):
        	data['windGust'] = float(b[8])*3.6*MILE_PER_KM  # mph
	else:
        	data['windGust'] = -999.0  # mph
	if(b[9]!='nan'):
        	data['windGustDir'] = float(b[9])  # degrees
	else:
        	data['windGustDir'] = -999.0  # degrees
	if(b[10]!='nan'):
		data['windSpeed'] = float(b[10])*3.6*MILE_PER_KM  # mph
	else:
		data['windSpeed'] = -999.0  # mph
	if(b[11]!='nan'):
		data['windDir'] = float(b[11])  # compass degrees
	else:
		data['windDir'] = -999.0  # compass degrees
	if(b[14]!='nan'):
        	data['inHumidity'] = float(b[14])  # percent
	else:
        	data['inHumidity'] = -999.0  # percent	
	if(b[14]!='nan'):
        	data['outHumidity'] = float(b[14])  # percent #just to try
	else:
        	data['outHumidity'] = -999.0  # percent #just to try
	if(b[15]!='nan'):
        	data['outTemp'] = float(b[15])*9.0/5.0+32.0  # degree_F #Just to try
	else:
        	data['outTemp'] = -999.0  # degree_F #Just to try
	if(b[15]!='nan'):
        	data['inTemp'] = float(b[15])*9.0/5.0+32.0  # degree_F
	else:
        	data['inTemp'] = -999.0  # degree_F
	if(b[16]!='nan'):
        	data['rainRate'] = float(b[16])*0.03937  # inch/hour
	else:
        	data['rainRate'] = -999.0  # inch/hour
	if(b[17]!='nan'):
        	data['daily_rain'] = float(b[17])*0.03937  # inch
	else:
        	data['daily_rain'] = -999.0  # inch
	if(b[19]!='nan'):
        	data['rain'] = float(b[19])*0.03937  # inch reset every 5min
	else:
        	data['rain'] = -999.0  # inch reset every 5min (gps!)
	if(b[20]!='nan'):
        	data['pressure'] = float(b[20])/100.0*INHG_PER_MBAR # 1000*inHg
	else:
        	data['pressure'] = -999.0 # 1000*inHg
	if(b[20]!='nan'):
        	data['barometer'] = float(b[20])/100.0*INHG_PER_MBAR # 1000*inHg
	else:
        	data['barometer'] = -999.0 # 1000*inHg
	if(b[21]!='nan'):
        	data['supplyVoltage'] = float(b[21])  # Voltage
	else:
        	data['supplyVoltage'] = -999.0  # Voltage
        if DEBUG_READ:
            logdbg(data)
        return data


class MWSConfEditor(weewx.drivers.AbstractConfEditor):
    @property
    def default_stanza(self):
        return """
[MWS]
    # This section is for the IWMI MWS series of weather stations.

    # Serial port such as /dev/ttyS0, /dev/ttyUSB0, or /dev/cuaU0
    port = /dev/ttyUSB0

    # The driver to use:
    driver = weewx.drivers.mws
"""

    def prompt_for_settings(self):
        print "Specify the serial port on which the station is connected, for"
        print "example /dev/ttyUSB0 or /dev/ttyACM0."
        port = self._prompt('port', '/dev/ttyACM0')
        return {'port': port}


# define a main entry point for basic testing of the station without weewx
# engine and service overhead.  invoke this as follows from the weewx root dir:
#
# Ubuntu standard set up with user driver:
# PYTHONPATH=bin python /usr/share/weewx/user/mws.py

if __name__ == '__main__':
    import optparse

    usage = """%prog [options] [--help]"""

    syslog.openlog('mws', syslog.LOG_PID | syslog.LOG_CONS)
    syslog.setlogmask(syslog.LOG_UPTO(syslog.LOG_DEBUG))
    parser = optparse.OptionParser(usage=usage)
    parser.add_option('--version', dest='version', action='store_true',
                      help='display driver version')
    parser.add_option('--port', dest='port', metavar='PORT',
                      help='serial port to which the station is connected',
                      default=DEFAULT_PORT)
    (options, args) = parser.parse_args()

    if options.version:
        print "MWS driver version %s" % DRIVER_VERSION
        exit(0)

    with Station(options.port) as s:
        while True:
            print time.time(), s.get_readings() 

