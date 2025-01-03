from flask import Flask, request, jsonify, render_template

app = Flask(__name__)

# Store the latest data for each node
latest_data = {}

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/dashboard')
def dashboard():
    return render_template('dashboard.html')

@app.route('/receive-data', methods=['GET'])
def receive_data():
    global latest_data
    data = request.args.get('data')

    if data:
        try:
            # Assuming the data format is "node_id,lat,lon"
            node_id, lat, lon = data.split(",")
            latest_data[node_id] = {
                "lat": float(lat),
                "lon": float(lon),
            }
            return jsonify({"status": "success", "node_id": node_id, "lat": lat, "lon": lon})
        except ValueError:
            return jsonify({"status": "error", "message": "Invalid data format"})
    else:
        return jsonify({"status": "error", "message": "No data received"})

@app.route('/get-latest-data', methods=['GET'])
def get_latest_data():
    return jsonify(latest_data)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5001)
