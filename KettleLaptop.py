"""
File Name: KettleLaptop
Purpose: Python script to work in conjuction with the ESP32.
Turns the kettle on remotely and prints the current temperature to the console.
Submits the off signal once the desired temperature is reached.
"""

from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient
import sys
import time
import json
import sys

# Variable Declaration
receivedTemp = -1
desiredTemp = -2
desiredMode = -3
desiredBeverage = ""

# Assigning thing name to client.
try:
  myMQTTClient = AWSIoTMQTTClient("JoeComputer")
except:
  print("Could not connect client. Thing does not exist.")

# Connection configuration
try:
  myMQTTClient.configureEndpoint("a2sxi23t654h6m-ats.iot.us-east-1.amazonaws.com", 8883)
  myMQTTClient.configureCredentials("./OldCreds/AmazonRootCA1.pem","./OldCreds/ba5bac6c2092529549b5d5adfe683961003160e7fb508ff5545f53b4d9ec5562-private.pem.key", "./OldCreds/ba5bac6c2092529549b5d5adfe683961003160e7fb508ff5545f53b4d9ec5562-certificate.pem.crt.txt")
except:
  print("Could not connect client. Configuration error.")

# Connecting to client
try:
  myMQTTClient.connect()
except:
  print("Could not connect client. Connection error.")

"""
Menu for the program. User enters desired drink and the device assigns the correct mode to the ESP32.
The desired temperature is assigned for the local device.
"""
print("Options: green tea (gt), white tea (wt), dark roast coffee (dc), oolong tea (ot), medium roast coffee (mc), black tea (bt), light roast coffee (lc), herbal tea (ht)")
print("Please enter beverage:")
beverage = input()
while(desiredTemp < 0):
  if (beverage == 'gt'):
    desiredMode = 1
    desiredBeverage = "Green Tea"
    desiredTemp = 79
  elif (beverage == 'wt'):
    desiredMode = 2
    desiredBeverage = "White Tea"
    desiredTemp = 82
  elif (beverage == 'dc'):
    desiredMode = 3
    desiredBeverage = "Dark Roast Coffee"
    desiredTemp = 85
  elif (beverage == 'ot'):
    desiredMode = 4
    desiredBeverage = "Oolong Tea"
    desiredTemp = 88
  elif (beverage == 'mc'):
    desiredMode = 5
    desiredBeverage = "Medium Roast Coffee"
    desiredTemp = 91
  elif (beverage == 'bt'):
    desiredMode = 6
    desiredBeverage = "Black Tea"
    desiredTemp = 94
  elif (beverage == 'lc'):
    desiredMode = 7
    desiredBeverage = "Light Roast Coffee"
    desiredTemp = 97
  elif (beverage == 'ht'):
    desiredMode = 8
    desiredBeverage = "Herbal Tea"
    desiredTemp = 100
  else:
    print("Not a mode. Please enter a valid mode.")
  
print("Desired drink is", desiredBeverage, sep= " ")  
  
print("Setting up your kettle now...")

on_msg = {
  "message": "ON"
}

off_msg = {
  "message": "OFF"
}

temp_msg = {
  "message": desiredMode
}

# Submits the correct mode number to the ESP32
try:
  msg = json.dumps(temp_msg)
  topic = "esp32/sub"
  myMQTTClient.publish(topic, msg, 0)  
  print("Device is now set to desired temperature")
except:
  print("Device did not start. Please run code again.")
  msg = json.dumps(off_msg)
  topic = "esp32/sub"
  myMQTTClient.publish(topic, msg, 0)  

time.sleep(1)

# Submits the on message to the ESP32
try:
  msg = json.dumps(on_msg)
  topic = "esp32/sub"
  myMQTTClient.publish(topic, msg, 0)  
  print("Device is now heating up")
except:
  print("Device did not start. Please run code again.")
  msg = json.dumps(off_msg)
  topic = "esp32/sub"
  myMQTTClient.publish(topic, msg, 0)  

time.sleep(1)

# Callback function
# This executes upon the local machine receiving a message over the subscription topic
def customCallback(client,userdata,message):
    global receivedTemp
    print(message.payload)
    temp = json.loads(message.payload)
    receivedTemp = temp["temperature"]
    

"""
Subscribes to the IoT topic (ESP32/pub). Continuously checks if the temperature received is the desired temperature.
If it is the desired temperature, the loop is exited and the off message is submitted.
If the user enters "q" the loop is exited early and the off message is submitted.
"""

receivedTemp = -1
button = "r"

print("Press q to quit")
while not(receivedTemp >= desiredTemp) and (button != "q"):
  button = "r"
  try:
    myMQTTClient.subscribe("esp32/pub", 0, customCallback)
  except:
    print("Communication error. Shutting device down. Please run code again.")
    msg = json.dumps(off_msg)
    topic = "esp32/sub"
    myMQTTClient.publish(topic, msg, 0)  

# Unsubscribes the local machine
myMQTTClient.unsubscribe("esp32/pub")
print("Client unsubscribed")   

# Transmits the off message
try:  
  msg = json.dumps(off_msg)
  topic = "esp32/sub"
  myMQTTClient.publish(topic, msg, 0)  
  print("Device is now shutting off")  
except:
  print("Communication error. Unable to shut device down. Please run code again.")

# Disconnects the local machine from the IoT network    
myMQTTClient.disconnect()
print("Client Disconnected")
