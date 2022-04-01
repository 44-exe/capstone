import numpy as np
import matplotlib.pyplot as plt
from serial_com import Serial_Com

# plt.axis([0, 10, 0, 1])
#
# for i in range(10):
#     y = np.random.random()
#     plt.scatter(i, y)
#     plt.pause(0.05)
#
# plt.show()


while True:
    sc = Serial_Com()
    max_len = 1005
    while True:
        sc.read_serial()
        # if (len(sc.PV_volt) > max_len) or (sc.PV_duty > 90):
        if len(sc.PV_volt) > max_len:
            # if len(sc.PV_volt) != len(sc.PV_power):
            #     continue
            # print(f"Plotting power/voltage")
            # plt.scatter(sc.PV_volt[5:max_len-5], sc.PV_power[5:max_len-5])

            # print(f"Plotting power/item_num")
            plt.scatter(sc.item_num[5:max_len-5], sc.PV_power[5:max_len-5])
            # plt.scatter(sc.item_num[5:max_len-5], sc.PV_volt[5:max_len-5])

            # print(f"Plotting current/voltage")
            # plt.scatter(sc.PV_volt[5:max_len-5], sc.PV_current[5:max_len-5])
            # plt.scatter(sc.PV_duty[0:max_len - 5], sc.PV_power[0:max_len - 5])

            plt.pause(0.001)
            break
    plt.show()
