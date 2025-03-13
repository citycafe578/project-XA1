import serial
import time
import threading
import joystick
import pymysql
from datetime import datetime

def send_data(ser):
    try:
        while True:
            data_to_send = "hello, arduino"
            ser.write((data_to_send + '\n').encode('utf-8')) # 確保發送的資料以換行符結尾
            time.sleep(1)
    except KeyboardInterrupt:
        print("Serial closed")
        ser.close()
    except serial.SerialException as e:
        print(f"Serial exception: {e}")
        ser.close()

def receive_data(ser):
    try:
        while True:
            if ser.in_waiting > 0:
                try:
                    data_received = ser.readline().decode('utf-8').strip()
                    print(f"Received data: {data_received}")
                except serial.SerialException as e:
                    print(f"Serial exception: {e}")
                    ser.close()
                    break
    except KeyboardInterrupt:
        print("Serial closed")
        ser.close()
    except serial.SerialException as e:
        print(f"Serial exception: {e}")
        ser.close()

def main():
    while True:
        try:
            ser = serial.Serial("/dev/ttyUSB0", 9600, timeout=1.0)
            time.sleep(1)
            print("Serial ok")

            send_thread = threading.Thread(target=send_data, args=(ser,))
            receive_thread = threading.Thread(target=receive_data, args=(ser,))

            send_thread.daemon = True
            receive_thread.daemon = True

            send_thread.start()
            receive_thread.start()

            send_thread.join()
            receive_thread.join()
        except serial.SerialException as e:
            print(f"Serial exception: {e}")
            time.sleep(5)
        except KeyboardInterrupt:
            print("Program terminated.")
            break

if __name__ == "__main__":
    main()