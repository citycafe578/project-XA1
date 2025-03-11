import time
import threading
import datetime

update_thread = None
stop_record = threading.Event()

def update(path):
    with open(path, 'a') as file:
        while not stop_record.is_set():
            now = datetime.datetime.now()
            file.write(now.strftime('%H-%M-%S') + '\n')
            file.flush()
            time.sleep(1)

def start_thread(file_path):
    global update_thread
    stop_record.clear()
    update_thread = threading.Thread(target=update, args=(file_path,))
    update_thread.start()

def stop_update():
    stop_record.set()
    if update_thread is not None:
        update_thread.join()