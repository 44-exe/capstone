import numpy
import csv
import matplotlib.pyplot as plt
from sklearn.metrics import r2_score
from serial_com import SerialCom


model_file = "/home/aji/projects/capstone/machine_learning/model/trained_model.csv"


class LearningModel:
    def __init__(self):
        self.voltage_oc = []
        self.duty_mpp = []
        self.mymodel = None
        self.predict_val = 0

    def read_file_content(self):
        with open(model_file, mode='r') as csv_file:
            csv_reader = csv.DictReader(csv_file)
            line_count = 1
            self.voltage_oc = []
            self.duty_mpp = []
            for row in csv_reader:
                # print(f"row: {row}")
                for k, v in row.items():
                    self.voltage_oc.append(float(row['voltage_oc']))
                    self.duty_mpp.append(float(row['duty_mpp']))
                line_count += 1
            print(f'Processed {line_count} lines.')

    # print(f"voltage_oc: {voltage_oc}")
    # print(f"duty_mpp: {duty_mpp}")
    def calc_max_regression(self):
        max_r2 = 0
        n = 1
        for ii in range(100):
            testmodel = numpy.poly1d(numpy.polyfit(self.voltage_oc, self.duty_mpp, ii))
            test_train_r2 = r2_score(self.duty_mpp, testmodel(self.voltage_oc))
            print(f"test_train_r2: {test_train_r2}")
            if test_train_r2 > max_r2:
                max_r2 = test_train_r2
                n = ii

        print(f"calcualted maximum model: n: {n}  max_r2: {max_r2}")
        self.mymodel = numpy.poly1d(numpy.polyfit(self.voltage_oc, self.duty_mpp, n))

        train_r2 = r2_score(self.duty_mpp, self.mymodel(self.voltage_oc))
        print(f"rd score: {train_r2}")

    def predict_duty(self, oc_volt):
        self.predict_val = self.mymodel(oc_volt)
        print(f"duty prediction value: {self.predict_val}")

    def plot_data(self):
        # plot x,y data
        plt.scatter(self.voltage_oc, self.duty_mpp)

        # plot line
        myline = numpy.linspace(3, max(self.voltage_oc), 100)
        plt.plot(myline, self.mymodel(myline))
        plt.show()


if __name__ == "__main__":
    lm = LearningModel()
    lm.read_file_content()
    lm.calc_max_regression()
    lm.predict_duty(13)
    lm.plot_data()


