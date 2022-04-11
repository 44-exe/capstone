import csv
import fileinput
import time
import os
from os.path import exists
from serial_com import Serial_Com

save_time = round(time.time())
file_name_mp = f'training_data/{save_time}_training_data.csv'


def write_file():
    if exists(file_name_mp):
        with open(file_name_mp, 'a', newline='') as file:
            fieldnames = ['duty', 'voltage', 'current', 'power']
            writer = csv.DictWriter(file, fieldnames=fieldnames)
            tmp_write = {'duty': sc.PV_duty[-1], 'voltage': sc.PV_volt[-1],
                         'current': sc.PV_current[-1], 'power': sc.PV_power[-1]}
            writer.writerow(tmp_write)
            # print(f"training_data_mpp: {tmp_write}")
    else:
        with open(file_name_mp, 'w', newline='') as file:
            fieldnames = ['duty', 'voltage', 'current', 'power']
            writer = csv.DictWriter(file, fieldnames=fieldnames)
            writer.writeheader()
            tmp_write = {'duty': sc.PV_duty[-1], 'voltage': sc.PV_volt[-1],
                         'current': sc.PV_current[-1], 'power': sc.PV_power[-1]}
            writer.writerow(tmp_write)
            # print(f"training_data_mpp: {tmp_write}")


os.system("sudo chmod a+rw /dev/ttyACM0")
build_path = "/home/aji/projects/capstone/ard_training/ard_training.ino"
build_upload_cmd = f"arduino-cli compile -b arduino:avr:uno {build_path} -u -p /dev/ttyACM0"
os.system(build_upload_cmd)
print("Building & uploading")


sc = Serial_Com()
time.sleep(2)
# sc.start_arduino()
rec_count = 0
while True:
    sc.read_serial()
    if sc.sync >= 4:
        write_file()
        sc.sync = 0
    if sc.end_sweep is True:
        sc.end_sweep = False
        rec_count += 1
        print(f"sweep count: {rec_count}")
        if rec_count >= 10:
            rec_count = 0
            print("Creating new file")
            save_time = round(time.time())
            file_name_mp = f'training_data/{save_time}_training_data.csv'
            # print("Finished recording data, exiting program")
            # exit(0)  # successfully exit

