
from flask import Flask, render_template, jsonify
import serial
import threading
import time

app = Flask(__name__)

# Update the serial port to match your system
SERIAL_PORT = '/dev/cu.usbserial-110'  # Replace with your actual serial port
BAUD_RATE = 9600
TIMEOUT = 1

data = {
    'product': '',
    'price': 0.0,
    'total': 0.0,
    'count': 0
}

# Attempt to open the serial port with exception handling
try:
    arduino = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=TIMEOUT)
    time.sleep(2)  # Allow some time for the connection to establish
except serial.SerialException as e:
    print(f"Error opening serial port: {e}")
    exit()

def read_from_arduino():
    global data
    while True:
        try:
            line = arduino.readline().decode('utf-8').strip()
            if line:
                # Simple parsing example, assuming the format is consistent
                if 'Product' in line:
                    parts = line.split()
                    if len(parts) > 1:
                        data['product'] = parts[1]
                elif 'Price' in line:
                    parts = line.split()
                    if len(parts) > 1:
                        try:
                            data['price'] = float(parts[1])
                        except ValueError:
                            print(f"Invalid price format: {parts[1]}")
                elif 'Total Price' in line:
                    parts = line.split()
                    if len(parts) > 1:
                        try:
                            data['total'] = float(parts[1])
                        except ValueError:
                            print(f"Invalid total price format: {parts[1]}")
                elif 'Total Products' in line:
                    parts = line.split()
                    if len(parts) > 1:
                        try:
                            data['count'] = int(parts[1])
                        except ValueError:
                            print(f"Invalid product count format: {parts[1]}")
        except serial.SerialException as e:
            print(f"Serial error: {e}")
            break
        except Exception as e:
            print(f"Unexpected error: {e}")

# Start the reading thread
threading.Thread(target=read_from_arduino, daemon=True).start()

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/data')
def get_data():
    return jsonify(data)

if __name__ == '__main__':
    app.run(debug=True)
