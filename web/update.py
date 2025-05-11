import time
import threading
import datetime
from queue import Queue

update_thread = None
stop_record = threading.Event()


def update(path, data_queue):
    with open(path, 'a') as file:
        while not stop_record.is_set():
            now = datetime.datetime.now()
            timestamp = now.strftime('%H-%M-%S')
            if not data_queue.empty():
                data = data_queue.get()
                file.write(f"{timestamp} {data}\n")
                print(f"Written to file: {timestamp} {data}")
            else:
                file.write(f"{timestamp}\n")
            file.flush()
            time.sleep(1)

def start_thread(file_path, data_queue):
    global update_thread
    stop_record.clear()
    update_thread = threading.Thread(target=update, args=(file_path, data_queue))
    update_thread.start()

def stop_update():
    stop_record.set()
    if update_thread is not None:
        update_thread.join()