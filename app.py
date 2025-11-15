import subprocess
import os
from flask import Flask, request, jsonify, render_template

# --- Determine C program name ---
if os.name == 'nt': # 'nt' means Windows
    PROGRAM_NAME = ".\\spam_filter.exe"
else:
    PROGRAM_NAME = "./spam_filter"

# --- Initialize the Flask app ---
app = Flask(__name__, template_folder='templates') # Explicitly state templates folder

# --- Route 1: Serve the main webpage ---
@app.route("/")
def home():
    """Serves the index.html file as the main page."""
    return render_template("index.html")

# --- Route 2: Handle email filtering ---
@app.route("/filter", methods=["POST"])
def handle_filter():
    """
    Receives email data from the webpage, runs the C program,
    and returns the result.
    """
    data = request.json
    
    sender = data.get("sender")
    body_file = data.get("body_file")
    attachments = data.get("attachments", []) # Get list of attachments

    if not sender or not body_file:
        return jsonify({"error": "Sender and Body File are required"}), 400

    # Build the command, e.g., ["./spam_filter", "a@b.com", "body.txt", "file.exe"]
    command = [PROGRAM_NAME, sender, body_file] + attachments

    try:
        # --- MODIFICATION ---
        # We removed text=True to read raw bytes
        result = subprocess.run(
            command, 
            capture_output=True, 
            timeout=5,
            check=True # This makes it raise an error if C program fails
        )
        
        # --- MODIFICATION ---
        # Manually decode stdout and stderr, replacing bad characters
        full_output = result.stdout.decode('utf-8', errors='replace')
        stderr_output = result.stderr.decode('utf-8', errors='replace')
        
        # --- ROBUST CHECK ---
        final_result = "UNKNOWN"
        if full_output: 
            for line in full_output.splitlines():
                if "FINAL_RESULT:" in line:
                    final_result = line.split(":")[1]
                    break
        else:
            if stderr_output:
                full_output = f"C Program Error (stderr):\n{stderr_output}"
            else:
                full_output = "C Program produced no output."
        
        # Send a JSON response back to the webpage
        return jsonify({
            "status": final_result, 
            "log": full_output
        })

    except subprocess.CalledProcessError as e:
        # This catches if the C program returns a non-zero exit code (crashes)
        # --- MODIFICATION ---
        # We must decode the byte output from the error object as well
        stdout_err = e.stdout.decode('utf-8', errors='replace')
        stderr_err = e.stderr.decode('utf-8', errors='replace')
        
        error_message = f"C Program Failed (Exit Code {e.returncode}):\n"
        if stdout_err:
            error_message += f"--- STDOUT ---\n{stdout_err}\n"
        if stderr_err:
            error_message += f"--- STDERR ---\n{stderr_err}\n"
        return jsonify({"error": error_message}), 500
        
    except FileNotFoundError:
        return jsonify({"error": f"Program '{PROGRAM_NAME}' not found. Did you compile your C code?"}), 50

    except Exception as e:
        return jsonify({"error": str(e)}), 500

# --- Route 3: Handle database management ---
@app.route("/manage_db", methods=["POST"])
def handle_manage_db():
    data = request.json
    action = data.get("action") 
    email = data.get("email")

    if not action or not email:
        return jsonify({"error": "Action and Email are required"}), 400

    command = [PROGRAM_NAME, action, email]

    try:
        # --- MODIFICATION ---
        # Removed text=True
        result = subprocess.run(
            command, 
            capture_output=True, 
            timeout=3, 
            check=True
        )
        
        # --- MODIFICATION ---
        # Decode stdout and replace errors
        success_message = result.stdout.decode('utf-8', errors='replace')
        return jsonify({"message": success_message})

    except FileNotFoundError:
        return jsonify({"error": f"Program '{PROGRAM_NAME}' not found. Did you compile your C code?"}), 50
    except subprocess.CalledProcessError as e:
        # --- MODIFICATION ---
        # Decode stderr from the error object
        stderr_err = e.stderr.decode('utf-8', errors='replace')
        error_message = f"C Program Failed (Exit Code {e.returncode}):\n{stderr_err}"
        return jsonify({"error": error_message}), 500
    except Exception as e:
        return jsonify({"error": str(e)}), 500

# --- Start the server ---
if __name__ == "__main__":
    print(f"Starting server... Go to http://localhost:5000")
    app.run(debug=True, port=5000)