

General info about the set up:
- Arduino Mega (x1)
- Sparkfun.com Weather Shield (https://www.sparkfun.com/products/12081)

Optional (plug n play):
- Sparkfun.com (https://www.sparkfun.com/products/9530)
- Generic LCD16x02 with I2C adaptor (more here: http://arduino-info.wikispaces.com/LCD-Blue-I2C)

Required male-to-male cables:
- I2C: Weather Shield A4 to Arduino Mega Pin20 (SDA)
- I2C: Weather Shield A5 to Arduino Mega Pin21 (SCL)

GPS "soft" switch mode:
- Communicates through D4/D5 (same as Arduino Uno R3)
- Power Control through D6 (same as Arduino Uno R3)
- D4/D5/D6 on Arduino Uno (spec sheet: T0/T1/AIN0)
- D7/D6/D5 on Arduino Mega (spec sheet: T0/T1/AIN1*)

*could not find AIN0 pin number in the Mega spec sheet
