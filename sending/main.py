import serial
import time
import struct
import threading
import joystick
import pymysql
from datetime import datetime

received_data = []

def send_data(ser):
    try:
        while True:
            data_to_send = joystick_data.run()
            # 使用 struct 將陣列轉換為二進位格式
            message = struct.pack(f'{len(data_to_send)}f', *data_to_send)
            ser.write(message)
            time.sleep(0.1)
    except KeyboardInterrupt:
        print("Serial closed")
        ser.close()

def receive_data(ser):
    try:
        while True:
            if ser.in_waiting > 0:
                # 假設接收的資料長度與發送的資料長度相同
                data_length = len(joystick_data.run())
                receive_message = ser.read(data_length * 4)  # 每個 float 佔 4 個字節
                received_floats = struct.unpack(f'{data_length}f', receive_message)
                print(f"Received: {received_floats}")
                received_data.append(received_floats)
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
    print("Serial closed")
    ser.close()