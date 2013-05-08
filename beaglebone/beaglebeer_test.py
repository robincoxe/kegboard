# BeagleBeer Test 
# Robin Coxe (coxe@close-haul.com)
# 7 May 2013

# Built using PyBBIO: https://github.com/alexanderhiam/PyBBIO/wiki
# DS18B20 temperature sensor code courtesy of Adafruit:
# http://learn.adafruit.com/adafruits-raspberry-pi-lesson-11-ds18b20-temperature-sensing/software

import os
import glob
import time
from bbio import * 

base_dir = '/sys/bus/w1/devices/'

device_folder1 = glob.glob(base_dir + '28-0000046e7000')[0]
device_folder2= glob.glob(base_dir + '28-0000046e75b7')[0]
device_file = [ device_folder1 + '/w1_slave', device_folder2 + 
'/w1_slave']

FLOW_SENSORA = GPIO1_7
FLOW_SENSORB = GPIO1_3
tick_count = 0
COUNTS_PER_SERVING = 1400

# 5600 pulses/L
# 1 L = 33.814 oz
# 1 Vegware cup = 9 oz = 266.162 ml = 1490 counts
# 1 250 mL serving = 1400 counts

def flowa_tick():
       global tick_count  #yes, I know global variables are hideous.
       tick_count += 1 
       if(tick_count % COUNTS_PER_SERVING) == 0:
            servings = tick_count / COUNTS_PER_SERVING
            print "Flowmeter ticks:", tick_count            
	        print "Servings:", servings
  
def read_temp_raw(nsens):
    f = open(device_file[nsens], 'r')
    lines = f.readlines()
    f.close()
    return lines
 
def read_temp(nsens):
    lines = read_temp_raw(nsens)
    while lines[0].strip()[-3:] != 'YES':
        time.sleep(0.2)
        lines = read_temp_raw(nsens)
    equals_pos = lines[1].find('t=')
    if equals_pos != -1:
        temp_string = lines[1][equals_pos+2:]
        temp_c = float(temp_string) / 1000.0
        temp_f = temp_c * 9.0 / 5.0 + 32.0
        return temp_c, temp_f

def setup():
    tick_count = 0
    pinMode(FLOW_SENSORA, INPUT, pull=1)
    pinMode(FLOW_SENSORB, INPUT, pull=1)
    attachInterrupt(FLOW_SENSORA,flowa_tick, FALLING)
  	
def loop():
	print(read_temp(0))
    print(read_temp(1))
	time.sleep(60)

run (setup,loop)  #keep Arduino-style main loop for now.
