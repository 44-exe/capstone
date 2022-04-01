import csv
import time
from os.path import exists
from serial_com import Serial_Com

save_time = round(time.time())
file_name_50 = f'training_data/{save_time}_training_data_50.csv'
file_name_mp = f'training_data/{save_time}_training_data_mp.csv'


def write_mpp():
    if exists(file_name_mp):
        with open(file_name_mp, 'a', newline='') as file:
            fieldnames = ['duty_mpp', 'volt_mpp', 'curr_mpp']
            writer = csv.DictWriter(file, fieldnames=fieldnames)
            # writer.writerow({'duty_mpp': sc.PV_duty, 'volt_mpp': sc.PV_volt[-1], 'curr_mpp': sc.PV_current[-1]})
            tmp_write = {'duty_mpp': sc.PV_duty, 'volt_mpp': sc.PV_volt[-1], 'curr_mpp': sc.PV_current[-1]}
            writer.writerow(tmp_write)
            print(f"training_data_mpp: {tmp_write}")
    else:
        with open(file_name_mp, 'w', newline='') as file:
            fieldnames = ['duty_mpp', 'volt_mpp', 'curr_mpp']
            writer = csv.DictWriter(file, fieldnames=fieldnames)
            writer.writeheader()
            tmp_write = {'duty_mpp': sc.PV_duty, 'volt_mpp': sc.PV_volt[-1], 'curr_mpp': sc.PV_current[-1]}
            writer.writerow(tmp_write)
            print(f"training_data_mpp: {tmp_write}")


def write_50():
    if exists(file_name_50):
        with open(file_name_50, 'a', newline='') as file:
            fieldnames = ['duty_50', 'volt_50', 'curr_50']
            writer = csv.DictWriter(file, fieldnames=fieldnames)
            tmp_write = {'duty_50': sc.PV_duty, 'volt_50': sc.PV_volt[-1], 'curr_50': sc.PV_current[-1]}
            writer.writerow(tmp_write)
            print(f"training_data_50: {tmp_write}")
    else:
        with open(file_name_50, 'w', newline='') as file:
            fieldnames = ['duty_50', 'volt_50', 'curr_50']
            writer = csv.DictWriter(file, fieldnames=fieldnames)
            writer.writeheader()
            tmp_write = {'duty_50': sc.PV_duty, 'volt_50': sc.PV_volt[-1], 'curr_50': sc.PV_current[-1]}
            writer.writerow(tmp_write)
            print(f"training_data_50: {tmp_write}")


sc = Serial_Com()
time.sleep(2)
sc.start_arduino()
while True:
    sc.read_serial()
    if sc.dut_xy is True:  # save data for MPP
        sc.dut_xy = False  # ack
        write_mpp()  # this function writes csv file
        sc.clean_data()  # reset data
    if sc.run_50 is True:  # save data for dut=50%
        sc.run_50 = False  # ack
        write_50()  # this function writes csv file

