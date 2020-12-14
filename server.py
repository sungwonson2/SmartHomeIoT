from flask import Flask, jsonify, render_template, request, Response
import webbrowser
import time
import random
import json
from datetime import datetime

app = Flask(__name__)

@app.route('/chart-data')
def chart_data():
    def generate_random_data():
        while True:
            json_data = json.dumps(
                {'time': datetime.now().strftime('%Y-%m-%d %H:%M:%S'), 'value': random.random() * 100})
            yield "data:{}\n\n".format(json_data)
            time.sleep(1)
    return Response(generate_random_data(), mimetype='text/event-stream')

@app.route("/")
def index():
	return render_template('index.html')

if __name__ == '__main__':
	port = int(os.environ.get('PORT', 5000))
	app.run(host='0.0.0.0', port=port, debug=False)
