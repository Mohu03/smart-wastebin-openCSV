import cv2
import tensorflow as tf
import RPi.GPIO as GPIO
import time
from utils import classify_waste, send_app_notification

# Load trained model
model = tf.keras.models.load_model("model/waste_classifier.h5")

# GPIO setup for servo motors
BIO_SERVO_PIN = 17
NONBIO_SERVO_PIN = 27

GPIO.setmode(GPIO.BCM)
GPIO.setup(BIO_SERVO_PIN, GPIO.OUT)
GPIO.setup(NONBIO_SERVO_PIN, GPIO.OUT)

bio_servo = GPIO.PWM(BIO_SERVO_PIN, 50)
nonbio_servo = GPIO.PWM(NONBIO_SERVO_PIN, 50)

bio_servo.start(0)
nonbio_servo.start(0)

# Bin fill status
bio_bin_fill = 0
nonbio_bin_fill = 0
BIN_CAPACITY = 5  # Items before bin is considered full

def open_bin(servo):
    """Rotate servo to open bin"""
    servo.ChangeDutyCycle(7)  # adjust angle according to your servo
    time.sleep(1)
    servo.ChangeDutyCycle(0)
    time.sleep(1)

# Start camera
cap = cv2.VideoCapture(0)

try:
    while True:
        ret, frame = cap.read()
        if not ret:
            break

        # Resize frame for model
        resized_frame = cv2.resize(frame, (224,224))
        label = classify_waste(model, resized_frame)
        print("Detected:", label)

        # Show frame with label
        cv2.putText(frame, label, (10,30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0,255,0), 2)
        cv2.imshow("Smart Waste Bin", frame)

        # Open correct bin and update fill
        if label == "Bio Waste":
            open_bin(bio_servo)
            bio_bin_fill += 1
            if bio_bin_fill >= BIN_CAPACITY:
                send_app_notification("Bio waste bin is full. Please empty it!")
                bio_bin_fill = 0
        else:
            open_bin(nonbio_servo)
            nonbio_bin_fill += 1
            if nonbio_bin_fill >= BIN_CAPACITY:
                send_app_notification("Non-bio waste bin is full. Please clear it!")
                nonbio_bin_fill = 0

        # Exit on 'q'
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

finally:
    # Cleanup
    cap.release()
    cv2.destroyAllWindows()
    bio_servo.stop()
    nonbio_servo.stop()
    GPIO.cleanup()
