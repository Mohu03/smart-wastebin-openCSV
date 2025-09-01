from flask import Flask, jsonify
import threading
import time

app = Flask(__name__)

# Bin status dictionary
bin_status = {
    "bio": "Empty",
    "nonbio": "Empty"
}

# Simulated bin fill counters
bio_bin_fill = 0
nonbio_bin_fill = 0
BIN_CAPACITY = 5

def simulate_bin_filling():
    """Simulate waste being added"""
    global bio_bin_fill, nonbio_bin_fill, bin_status
    while True:
        time.sleep(10)  # Every 10 seconds simulate a waste item
        # Randomly add waste (replace with your real sensor detection)
        import random
        waste_type = random.choice(["bio", "nonbio"])
        if waste_type == "bio":
            bio_bin_fill += 1
            if bio_bin_fill >= BIN_CAPACITY:
                bin_status["bio"] = "Full"
                bio_bin_fill = 0
            else:
                bin_status["bio"] = f"{bio_bin_fill}/{BIN_CAPACITY}"
        else:
            nonbio_bin_fill += 1
            if nonbio_bin_fill >= BIN_CAPACITY:
                bin_status["nonbio"] = "Full"
                nonbio_bin_fill = 0
            else:
                bin_status["nonbio"] = f"{nonbio_bin_fill}/{BIN_CAPACITY}"

# Start simulation in background
threading.Thread(target=simulate_bin_filling, daemon=True).start()

@app.route("/status")
def status():
    """Return bin status as JSON"""
    return jsonify(bin_status)

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
