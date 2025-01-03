from flask import Flask, request, jsonify, render_template

app = Flask(__name__)

# Store the latest data for each node
latest_data = {}

@app.route('/')
def index():
    return render_template('index.html')

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5001)
