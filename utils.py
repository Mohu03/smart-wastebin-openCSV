import numpy as np
from tensorflow.keras.preprocessing import image
from tensorflow.keras.applications.mobilenet_v2 import preprocess_input
from pushover import Client

# Replace with your Pushover credentials
USER_KEY = "YOUR_USER_KEY"       # Your personal Pushover user key
API_TOKEN = "YOUR_API_TOKEN"     # Your Pushover application API token


def classify_waste(model, img_frame, img_size=(224,224)):
  
    """Classify image as bio or non-bio waste"""
  
    img_array = image.img_to_array(img_frame)
    img_array = np.expand_dims(img_array, axis=0)
    img_array = preprocess_input(img_array)
    
    pred = model.predict(img_array)[0][0]
    label = "Bio Waste" if pred < 0.5 else "Non-Bio Waste"
    return label

def send_app_notification(message):
  
    """Send a notification via Pushover"""
  
    client = Client(USER_KEY, api_token=API_TOKEN)
    client.send_message(message, title="Smart Bin Alert")
    print("Notification sent:", message)
