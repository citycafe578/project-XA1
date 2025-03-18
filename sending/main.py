import serial
import time
import threading
import requests
import subprocess
import webbrowser
from joystick import Joysticker

def start_flask_server():
    flask_process = subprocess.Popen(["python", "web/app.py"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    return flask_process

def stop_flask_server(flask_process):
    flask_process.terminate()
    flask_process.wait()
    print("Flask server stopped.")

def send_data(ser):
    try:
        while True:
            data_to_send = joystick_data.run()
            message = ' '.join(map(str, data_to_send)) + '\n'
            ser.reset_output_buffer()
            ser.write(message.encode('utf-8'))
            time.sleep(0.2)
    except KeyboardInterrupt:
        print("Serial closed in send_data")
        ser.close()

def receive_data(ser):
    try:
        while True:
            if ser.in_waiting > 0:
                data_received = ser.readline().decode('utf-8').strip()
                print(f"Received from Arduino: {data_received}")
                ser.reset_input_buffer()

                # 傳送資料到 Flask 後端
                try:
                    response = requests.post("http://127.0.0.1:5000/data", json={"data": data_received})
                    print(f"Server response: {response.status_code}")
                except requests.exceptions.RequestException as e:
                    print(f"Failed to send data to server: {e}")
    except KeyboardInterrupt:
        print("Serial closed in receive_data")
        ser.close()

if __name__ == "__main__":
    flask_process = start_flask_server()
    webbrowser.open("http://127.0.0.1:5000")
    joystick_data = Joysticker()
    ser = serial.Serial("/dev/ttyUSB0", 9600, timeout=1.0)
    time.sleep(1)
    print("Serial connection established")

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
        ser.close()
        stop_flask_server(flask_process)