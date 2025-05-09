import time
import threading
import datetime

update_thread = None
stop_record = threading.Event()

def update(path, drone_data):
    with open(path, 'a') as file:
        while not stop_record.is_set():
            now = datetime.datetime.now()
            data = drone_data()  # 調用 drone_data 函式以獲取資料
            file.write(now.strftime('%H-%M-%S') + ' ' + data + '\n')
            print(data)
            file.flush()
            time.sleep(1)

def start_thread(file_path, drone_data):
    global update_thread
    stop_record.clear()
    update_thread = threading.Thread(target=update, args=(file_path, drone_data))
    update_thread.start()

def stop_update():
    stop_record.set()
    if update_thread is not None:
        update_thread.join()