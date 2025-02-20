from flask import Flask, render_template, redirect, url_for, jsonify
from fastapi import FastAPI, WebSocket
import datetime
import update
import os
import sys

sys.path.append('../sending')
import main

app = Flask(__name__)
path = None

@app.route("/", methods = ['GET', 'POST'])
def home():
    image_url = url_for('static', filename='imgs/nodata.png')
    return render_template("index.html")

@app.route("/record", methods = ['GET'])
def record():
    global path
    now = datetime.datetime.now()
    name = f"{now.date()}_{now.strftime('%H-%M-%S')}.txt"
    path = f"web/record/{name}"
    with open(path, 'w') as file:
        file.write('start\n')
    update.start_thread(path)
    return jsonify(success=True, message="File created successfully", file_path=path)

@app.route("/stop_record", methods = ['GET'])
def stop_record():
    update.stop_update()
    return jsonify(success=True, message="File stopped successfully")

@app.route("/get_file")
def get_file():
    file_path = "web/record"
    files = os.listdir(file_path)
    return jsonify(files)

@app.route("/get_received_data", methods = ['GET'])
def get_received_data():
    return jsonify(main.received_data)

if __name__ == "__main__":
    app.run(debug=True)