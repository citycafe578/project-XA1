from flask import Flask, render_template, redirect, url_for, jsonify
import os
import sqlite3
import time
import datetime
app = Flask(__name__)

@app.route("/", methods=['GET', 'POST'])
def home():
    image_url = url_for('static', filename='imgs/nodata.png')
    return render_template("index.html")

@app.route("/record", methods=['GET'])
def record():
    now = datetime.datetime.now()
    name = f"{now.date()}_{now.strftime('%H-%M-%S')}.txt"
    path = os.path.join('record', name)
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, 'w') as file:
        file.write('這是檔案的內容。')

    return jsonify(success = True, message = "File created successfully", file_path=path)


if __name__ == "__main__":
    app.run(debug = True)