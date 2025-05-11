import serial
import time
import threading
import subprocess
import webbrowser
import queue
from shared import data_queue

data_queue = queue.Queue()  # 全域變數 queue

def start_flask_server():
    flask_process = subprocess.Popen(["python", "web/app.py"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    return flask_process

def stop_flask_server(flask_process):
    flask_process.terminate()
    flask_process.wait()
    print("Flask server stopped.")

def receive_data(ser):
    try:
        while True:
            data = ser.readline().decode('utf-8').strip()
            if data:
                print(f"Received: {data}")
                data_queue.put(data)  # 將資料放入 queue
                print(f"Data added to queue: {data}")
    except KeyboardInterrupt:
        print("Serial closed in receive_data")
        ser.close()

if __name__ == "__main__":
    flask_process = start_flask_server()
    webbrowser.open("http://127.0.0.1:5000")
    ser = serial.Serial("/dev/ttyUSB0", 115200, timeout=1.0)
    time.sleep(1)
    print("Serial connection established")

    receive_thread = threading.Thread(target=receive_data, args=(ser,))
    receive_thread.daemon = True
    receive_thread.start()

    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print("Program terminated.")
        ser.close()
        stop_flask_server(flask_process)