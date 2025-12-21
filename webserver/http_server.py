import os
from flask import Flask, request
from flask_httpauth import HTTPBasicAuth
from werkzeug.security import generate_password_hash, check_password_hash

app = Flask(__name__)
auth = HTTPBasicAuth()

APP_ADDRESS=os.environ.get('APP_ADDRESS','127.0.0.1')
APP_PORT=os.environ.get('APP_PORT',8080)
DEFAULT_USER=os.environ.get('DEFAULT_USER')
DEFAULT_PASS=os.environ.get('DEFAULT_PASS')
app.config['SECRET_KEY'] = os.environ.get('SECRET_KEY')
DEBUG=os.environ.get('DEBUG',True)

# A dictionary to store user credentials (hashed passwords for security)
users = {
    DEFAULT_USER: generate_password_hash(DEFAULT_PASS)
}

@auth.verify_password
def verify_password(username, password):
    """
    Callback function to verify the provided username and password.
    It should return the username upon successful verification.
    """
    if username in users and check_password_hash(users.get(username), password):
        return username
    return None

@app.route('/data', methods=['POST'])
@auth.login_required # Apply the authentication decorator to the POST route
def submit_data():
    """
    This function will only run if the basic authentication is successful.
    """
    data = request.get_json() # Get JSON data from the POST request body
    if data:
        return f"Data received from {auth.current_user()}: {data}", 200
    else:
        return "No data received", 400

if __name__ == '__main__':
    # Set a secret key for session management (required by Flask-Login and good practice for Flask apps)
    app.run(debug=DEBUG, host=APP_ADDRESS, port=APP_PORT)
