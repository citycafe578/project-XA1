import threading
import subprocess

def run_script(script):
	subprocess.call(["python", script])

py1 = threading.Thread(target = run_script, args = ("data_sending.py",))
py2 = threading.Thread(target = run_script, args = ("joystick.py",))

py1.start()
py2.start()

py1.join()
py2.join()