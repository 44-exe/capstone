# 
# This is the main code for machine learning MPPT
#

import matplotlib.pyplot as plt
from serial_com import SerialCom
from learning_model import LearningModel
import os
import time


#### set up machine learning
lm = LearningModel()
lm.read_file_content()
lm.calc_max_regression()



### set up arduino
os.system("sudo chmod a+rw /dev/ttyACM0")
build_path = "/home/aji/projects/capstone/ard_ml_mppt/ard_ml_mppt.ino"
build_upload_cmd = f"arduino-cli compile -b arduino:avr:uno {build_path} -u -p /dev/ttyACM0"
os.system(build_upload_cmd)
print("Building & uploading")
time.sleep(1)

prev_sync = 0
sc = SerialCom()
duty = 10

while True:
    sc.read_serial()
    sc.parse_data()
    # if sc.sync > prev_sync:  # checking if OC voltage is received
        # sc.sync = 0
    if sc.OC_volt > 0:
        print(f"sc.OC_volt: {sc.OC_volt}")
        lm.predict_duty(sc.OC_volt)
        sc.send_arduino(lm.predict_val)
        sc.OC_volt = 0

    # print(f"sc.data: {sc.data}")
    time.sleep(0.1)




