import time
import threading
import datetime

update_thread = None
stop_record = threading.Event()

def update(path):
    if path == "stop":
        file.close()
        stop_update()
    with open(path, 'a') as file:
        while True:
            now = datetime.datetime.now()
            file.write(now.strftime('%H-%M-%S'))
            file.write('\n')
            file.flush()
            time.sleep(1)

def start_thread(path):
    global update_thread, stop_record
    stop_record.clear()
    update_thread = threading.Thread(target = update, args = (path))
    update_thread.start()

def stop_update():
    global stop_record
    stop_record.set()

    if update_thread == True:
        update_thread.join
