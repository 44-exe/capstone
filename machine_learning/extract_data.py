import csv
from os import walk


mypath = "/home/aji/projects/capstone/machine_learning/data"

file_list = []
for (dirpath, dirnames, filenames) in walk(mypath):
    print(f"dirpath: {dirpath}")
    file_list.extend(filenames)

if len(file_list) == 0:
    print(f"file_list: {file_list}")
    print("DEBUG: No files found!")
else:
    print(f"{len(file_list)} files found:")
    print(f"file_list: {file_list}")
    print("")

voltage_oc = []
power_mpp = []
duty_mpp = []

# for ii in range(1):
for ii in range(len(file_list)):
    file_dir = f"{mypath}/{file_list[ii]}"
    ######## OPEN AND READ .CSV FILE ########
    with open(file_dir, mode='r') as csv_file:
        csv_reader = csv.DictReader(csv_file)
        line_count = 1
        local_power = []
        local_duty = []
        for row in csv_reader:
            print(f"row: {row}")
            for k, v in row.items():
                local_power.append(row['power'])
                local_duty.append(row['duty'])
                if k == "duty" and v == '0.0':
                    print("0 duty detected!!")
                    # TODO: save open-circuit voltage here
                    voltage_oc.append(row['voltage'])

                if k == "duty" and v == '90.0':
                    print("End of sample detected!!")
                    # TODO: calcualte maximum power here and save
                    power_mpp.append(max(local_power))
                    duty_mpp.append(local_duty[local_power.index(power_mpp[-1])])
                    local_power.clear()
                    local_duty.clear()
            line_count += 1
    print(f'Processed {line_count} lines.')

print(f"voltage_oc: {voltage_oc}")
print(f"power_mpp: {power_mpp}")
print(f"duty_mpp: {duty_mpp}")

file_name = "/home/aji/projects/capstone/machine_learning/model/trained_model.csv"
with open(file_name, 'w', newline='') as file:
    fieldnames = ['voltage_oc', 'power_mpp', 'duty_mpp']
    writer = csv.DictWriter(file, fieldnames=fieldnames)
    writer.writeheader()
    for ii in range(len(voltage_oc)):
        tmp_write = {'voltage_oc': voltage_oc[ii], 'power_mpp': power_mpp[ii],
                     'duty_mpp': duty_mpp[ii]}
        writer.writerow(tmp_write)



