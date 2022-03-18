import serial
import time

# setup serial for arduino
serialPort = serial.Serial(port="/dev/ttyACM0", baudrate=9600,
                           bytesize=8, timeout=2, stopbits=serial.STOPBITS_ONE)

serialString = ""                           # Used to hold data coming over UART


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
        self.pre_volt = 0
        self.pre_current = 0
        self.pre_power = 0

        self.PV_volt = 0
        self.PV_current = 0
        self.PV_power = 0
        pass

    def read_serial(self):
        if self.serialPort.in_waiting > 0:
            data = self.serialPort.readline()
            try:
                self.data = data.decode('Ascii')
                self.parse_data()
            except:
                print("Invalid data")
            # print(f"self.data: {self.data}")

    def parse_data(self):
        if self.data[0:7] == "PV_vol:":
            self.PV_volt = float(self.data[8:])
            print(f"self.PV_vol: {self.PV_volt}V", end='  ')
        if self.data[0:7] == "PV_cur:":
            self.PV_current = float(self.data[8:])
            print(f"self.PV_cur: {self.PV_current}A", end='  ')
        if self.data[0:7] == "PV_pow:":
            self.PV_power = float(self.data[8:])
            print(f"self.PV_pow: {self.PV_power}W")


def main():
    sc = Serial_Com()
    while True:
        sc.read_serial()
        # sc.parse_data()
        time.sleep(0.05)

if __name__=="__main__":
    main()



# End of File
