from flask import Flask, render_template, redirect, url_for
import sqlite3
app = Flask(__name__)

@app.route("/", methods=['GET', 'POST'])
def home():
    image_url = url_for('static', filename='imgs/nodata.png')
    return render_template("index.html")

if __name__ == "__main__":
    app.run(debug = True)