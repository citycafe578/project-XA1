from flask import Flask, render_template, jsonify, request
import datetime
import os
from queue import Queue
from threading import Event
from update import start_thread, stop_update
from decode import decode


app = Flask(__name__)
path = None
data_queue = Queue()

@app.route("/", methods=['GET', 'POST'])
def home():
    file_path = os.path.join("web", "record")
    files = os.listdir(file_path)
    return render_template("index.html", files=files)

@app.route("/record", methods=['GET'])
def record():
    global path
    now = datetime.datetime.now()
    name = f"{now.date()}_{now.strftime('%H-%M-%S')}.txt"
    path = os.path.join("web", "record", name)
    start_thread(path, data_queue)
    return jsonify(success=True, message="File created successfully", file_path=path)

@app.route("/stop_record", methods=['GET'])
def stop_record():
    stop_update()
    return jsonify(success=True, message="File stopped successfully")

@app.route("/get_file")
def get_file():
    file_path = os.path.join("web", "record")
    files = os.listdir(file_path)
    return jsonify(files)

@app.route("/get_file_content", methods=['POST'])
def get_file_content():
    file_name = request.json.get('file_name')
    file_path = os.path.join("web", "record", file_name)
    with open(file_path, 'r') as file:
        content = file.readlines()
    return jsonify(content)

@app.route("/get_queue_data", methods=['GET'])
def get_queue_data():
    if not data_queue.empty():
        data = decode(data_queue.get())
        return jsonify(success=True, data=data)
    else:
        return jsonify(success=False, message="Queue is empty")
