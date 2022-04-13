import numpy
import csv
import matplotlib.pyplot as plt
from sklearn.metrics import r2_score

model_file = "/home/aji/projects/capstone/machine_learning/model/trained_model.csv"

with open(model_file, mode='r') as csv_file:
    csv_reader = csv.DictReader(csv_file)
    line_count = 1
    voltage_oc = []
    duty_mpp = []
    for row in csv_reader:
        # print(f"row: {row}")
        for k, v in row.items():
            voltage_oc.append(float(row['voltage_oc']))
            duty_mpp.append(float(row['duty_mpp']))
        line_count += 1
    print(f'Processed {line_count} lines.')

# print(f"voltage_oc: {voltage_oc}")
# print(f"duty_mpp: {duty_mpp}")

max_r2 = 0
n = 1
for ii in range(100):
    testmodel = numpy.poly1d(numpy.polyfit(voltage_oc, duty_mpp, ii))
    test_train_r2 = r2_score(duty_mpp, testmodel(voltage_oc))
    print(f"test_train_r2: {test_train_r2}")
    if test_train_r2 > max_r2:
        max_r2 = test_train_r2
        n = ii

print(f"calcualted maximum model: n: {n}  max_r2: {max_r2}")
mymodel = numpy.poly1d(numpy.polyfit(voltage_oc, duty_mpp, n))
train_r2 = r2_score(duty_mpp, mymodel(voltage_oc))

print(f"rd score: {train_r2}")

print(f"prediction mymodel: {mymodel(16)}")

# plot x,y data
plt.scatter(voltage_oc, duty_mpp)

# plot line
myline = numpy.linspace(3, 20, 100)
plt.plot(myline, mymodel(myline))
plt.show()

