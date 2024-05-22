from flask import Flask, render_template, redirect, url_for, jsonify
import os
import sqlite3
import datetime
import update
app = Flask(__name__)

@app.route("/", methods=['GET', 'POST'])
def home():
    image_url = url_for('static', filename='imgs/nodata.png')
    return render_template("index.html")

@app.route("/record", methods=['GET'])
def record():
    now = datetime.datetime.now()
    name = f"{now.date()}_{now.strftime('%H-%M-%S')}.txt"
    path = f"web/record/{name}"
    with open(path, 'w') as file:
        file.write('start')
        update.update(path)
    return jsonify(success = True, message = "File created successfully", file_path=path)


@app.route("/stop_record", methods = ['GET'])
def stop_record():
    update.stop_update()
    return jsonify(success = True, message = "File stop successfully")

if __name__ == "__main__":
    app.run(debug = True)