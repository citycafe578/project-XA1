import serial
import time
import threading
import subprocess
import webbrowser
import queue
import os
import sys
from shared import data_queue

sys.path.append(os.path.join(os.path.dirname(__file__), "web"))
import app

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
                data_queue.put(data)
                # print(f"Data added to queue: {data}")
    except KeyboardInterrupt:
        print("Serial closed in receive_data")
        ser.close()

if __name__ == "__main__":
    flask_thread = threading.Thread(target=lambda: app.app.run(debug=True, use_reloader=False))
    flask_thread.daemon = True
    flask_thread.start()
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