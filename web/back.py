from flask import Flask, render_template, redirect, url_for
import os
import sqlite3
import time
import datetime
app = Flask(__name__)

@app.route("/", methods=['GET', 'POST'])
def home():
    image_url = url_for('static', filename='imgs/nodata.png')
    return render_template("index.html")

def record():
    time = datetime.now()
    name = str(datetime.date.today()) + "_"+ str(time.strftime("%H:%M:%S"))
    path = 'web/record/' + name + '.txt'
    with open(path, 'w') as file:
        file.write('這是檔案的內容。')


if __name__ == "__main__":
    app.run(debug = True)