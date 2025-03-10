from flask import Flask, render_template, jsonify, request
import datetime
import update
import os
import sys

# sys.path.append('../sending')
# import main

app = Flask(__name__)
path = None

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
    with open(path, 'w') as file:
        file.write('start\n')
    update.start_thread(path)
    return jsonify(success=True, message="File created successfully", file_path=path)

@app.route("/stop_record", methods=['GET'])
def stop_record():
    update.stop_update()
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


if __name__ == "__main__":
    app.run(debug=True)