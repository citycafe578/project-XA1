import serial
import time
import threading
import joystick
import pymysql
from datetime import datetime

def send_data(ser):
    try:
        while True:
            data_to_send = joystick_data.run()
            message = ' '.join([str(data) for data in data_to_send])
            message += '\n'
            ser.write(message.encode('utf-8'))
            time.sleep(0.1)
    except KeyboardInterrupt:
        print("Serial closed")
        ser.close()

def receive_data(ser):
    try:
        while True:
            if ser.in_waiting > 0:
                data_received = ser.readline().decode('utf-8').strip()
                print(f"Received data: {data_received}")
    except KeyboardInterrupt:
        print("Serial closed")
        ser.close()

joystick_data = joystick.Joysticker()  

ser = serial.Serial("/dev/ttyUSB0", 9600, timeout=1.0)
time.sleep(1)
print("Serial ok")

send_thread = threading.Thread(target=send_data, args=(ser,))
receive_thread = threading.Thread(target=receive_data, args=(ser,))

send_thread.daemon = True
receive_thread.daemon = True

send_thread.start()
receive_thread.start()

try:
    while True:
        time.sleep(1)
except KeyboardInterrupt:
    print("Program terminated.")