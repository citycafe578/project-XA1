from flask import Flask, render_template, jsonify, request
import datetime
import os

received_data = []  # 用於存儲接收到的資料
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
    return jsonify(success=True, message="File created successfully", file_path=path)

@app.route("/stop_record", methods=['GET'])
def stop_record():
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


@app.route("/data", methods=['POST'])
def receive_data():
    global received_data, path
    data = request.json.get("data")
    if data:
        received_data.append(data)
        print(f"Received data from ESP32: {data}")
        if path:
            with open(path, 'a') as file:
                file.write(data + '\n')
        return jsonify(success=True, message="Data received")
    return jsonify(success=False, message="No data received")

@app.route("/get_data", methods=['GET'])
def get_data():
    return jsonify(received_data)

if __name__ == "__main__":
    app.run(debug=True)