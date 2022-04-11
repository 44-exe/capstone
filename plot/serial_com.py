import serial
import time

# setup serial for arduino
# serialPort = serial.Serial(port="/dev/ttyACM0", baudrate=9600,
#                            bytesize=8, timeout=2, stopbits=serial.STOPBITS_ONE)

# serialString = ""                           # Used to hold data coming over UART


"""while(1):

    # Wait until there is data waiting in the serial buffer
    if serialPort.in_waiting > 0:
        # Read data out of the buffer until a carraige return / new line is found
        serialString = serialPort.readline()

        # Print the contents of the serial data
        data = serialString.decode('Ascii')
        print(f"data: {data}")
        # print(f"type(data): {type(data)}")
        if data[0:7] == "PV_volt":
            print(f"volt9: {data[10:]}")
        # time.sleep(1)

        # Tell the device connected over the serial port that we recevied the data!
        # The b at the beginning is used to indicate bytes!
        # serialPort.write(b"Thank you for sending data \r\n")"""

class Serial_Com:
    def __init__(self):
        self.serialPort = serial.Serial(port="/dev/ttyACM0", baudrate=9600,
                                   bytesize=8, timeout=2, stopbits=serial.STOPBITS_ONE)
        self.data = ''
        self.PV_volt = []
        self.PV_current = []
        self.PV_power = []
        self.PV_duty = []
        self.item_num = []
        self.counter = 0
        self.sync = 0
        self.end_sweep = False

    def read_serial(self):
        if self.serialPort.in_waiting > 0:
            data = self.serialPort.readline()
            try:
                self.data = data.decode('Ascii')
                # print(f"self.data: {self.data}")
                self.parse_data()
            except:
                print("Invalid data")
            # print(f"self.data: {self.data}")

    def parse_data(self):
        if self.data[0:7] == "PV_vol:":
            self.PV_volt.append(float(self.data[8:]))
            print(f"PV_vol: {self.PV_volt[-1]}V", end='  ')
            self.sync += 1
        if self.data[0:7] == "PV_cur:":
            self.PV_current.append(float(self.data[8:]))
            print(f"PV_cur: {self.PV_current[-1]}A", end='  ')
            self.sync += 1
        if self.data[0:7] == "PV_pow:":
            self.PV_power.append(float(self.data[8:]))
            self.item_num.append(int(self.counter))
            print(f"PV_pow: {self.PV_power[-1]}W", end='  ')
            self.counter += 1
            self.sync += 1
        if self.data[0:7] == "PV_dut:":
            self.PV_duty.append(float(self.data[8:]))
            print(f"PV_duty: {self.PV_duty[-1]}")
            self.sync += 1
        if self.data[0:12] == "end of sweep":
            print("end of sweep detected")
            self.end_sweep = True

    def clean_data(self):
        self.PV_volt.clear()
        self.PV_current.clear()
        self.PV_power.clear()
        self.PV_duty.clear()

    def start_arduino(self):
        self.serialPort.write(','.encode('UTF-8'))


def main():
    sc = Serial_Com()
    while True:
        sc.read_serial()
        # sc.parse_data()
        # time.sleep(0.05)

if __name__=="__main__":
    main()



# End of File
