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
#import traceback, _thread
#import threading

# Code from stackoverflow: https://stackoverflow.com/questions/474528/what-is-the-best-way-to-repeatedly-execute-a-function-every-x-seconds
# Credit to user Alfe for solution
# THIS CODE IS NOW DISABLED AND NOT IN USE
#def every(delay, task):
#  next_time = time.time() + delay
#  while True:
#    time.sleep(max(0, next_time - time.time()))
#    try:
#      task()
#    except Exception:
#      msg = json.dumps(off_msg)
#      topic = "esp32/sub"
#      myMQTTClient.publish(topic, msg, 0)  
#      quit()
#    next_time += (time.time() - next_time) // delay * delay + delay

#def foo():
#  global compareTemp
#  global receivedTemp
#  global button
#  if(compareTemp == receivedTemp):
#    print("Connection timeout. Please try again")
#    msg = json.dumps(off_msg)
#    topic = "esp32/sub"
#    myMQTTClient.publish(topic, msg, 0) 
#    button = "q"
#    _thread.interrupt_main()
#  else:
#    compareTemp = receivedTemp

# Variable Declaration
    
compareTemp = -1
receivedTemp = -1
desiredTemp = -2
desiredMode = -3
desiredBeverage = ""
button = "r"

on_msg = {
  "message": "ON"
}

off_msg = {
  "message": "OFF"
}

temp_msg = {
  "message": desiredMode
}


# Assigning thing name to client.
try:
  myMQTTClient = AWSIoTMQTTClient("JoeComputer")
except:
  print("Could not use assigned thing name. Please contact assigned IT personnel.")
  quit()

# Connection configuration
try:
  myMQTTClient.configureEndpoint("a2sxi23t654h6m-ats.iot.us-east-1.amazonaws.com", 8883)
  myMQTTClient.configureCredentials("./OldCreds/AmazonRootCA1.pem","./OldCreds/ba5bac6c2092529549b5d5adfe683961003160e7fb508ff5545f53b4d9ec5562-private.pem.key", "./OldCreds/ba5bac6c2092529549b5d5adfe683961003160e7fb508ff5545f53b4d9ec5562-certificate.pem.crt.txt")
  #myMQTTClient.configureEndpoint("a3g4e66hhemyfb-ats.iot.us-east-1.amazonaws.com", 8883)
  #myMQTTClient.configureCredentials("./AmazonRootCA1.pem","./Private.key", "./DeviceCert.txt")
except:
  print("Could not connect client. Configuration error.")
  quit()

# Connecting to client
try:
  myMQTTClient.connect()
except:
  print("Could not connect client. Connection error.")
  quit()

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
  quit()

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
  quit()

# Callback function
# This executes upon the local machine receiving a message over the subscription topic
def customCallback(client,userdata,message):
    global receivedTemp
    print(message.payload)
    temp = json.loads(message.payload)
    receivedTemp = temp["temperature"]

print("Enter q to quit")

#threading.Thread(target=lambda: every(3, foo)).start()

"""
Subscribes to the IoT topic (ESP32/pub). Continuously checks if the temperature received is the desired temperature.
If it is the desired temperature, the loop is exited and the off message is submitted.
If the user enters "q" the loop is exited early and the off message is submitted.
"""

while not(receivedTemp >= desiredTemp) and (button != "q"):
  button = "r"
  try:
    myMQTTClient.subscribe("esp32/pub", 0, customCallback)
  except:
    print("Communication error. Shutting device down. Please run code again.")
    msg = json.dumps(off_msg)
    topic = "esp32/sub"
    myMQTTClient.publish(topic, msg, 0)  
    quit()
  button = input()

# Unsubscribes the local machine
myMQTTClient.unsubscribe("esp32/pub")
print("Client unsubscribed")   

try:  
  msg = json.dumps(off_msg)
  topic = "esp32/sub"
  myMQTTClient.publish(topic, msg, 0)  
  print("Device is now shutting off")  
except:
  print("Communication error. Unable to shut device down. Please run code again.")
  quit()

# Disconnects the local machine from the IoT network  
myMQTTClient.disconnect()
print("Client Disconnected")