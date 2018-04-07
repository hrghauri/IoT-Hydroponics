import pyrebase
import os
from datetime import datetime


config = {
    "apiKey": "AIzaSyDtLjrWiB5Rb67sbXzDsac_beGujD2qQg8",
    "authDomain": "mylearningproject-f6c15.firebaseapp.com",
    "databaseURL": "https://mylearningproject-f6c15.firebaseio.com",
    "projectId": "mylearningproject-f6c15",
    "storageBucket": "mylearningproject-f6c15.appspot.com",
    "messagingSenderId": "189935592372"
  };

firebase = pyrebase.initialize_app(config)

auth = firebase.auth()
user = auth.sign_in_with_email_and_password("hrghauri@gmail.com", "hydroOttawa")


db = firebase.database()

countLight = 0
countPump = 0
def light_stream_handler(post):
    global countLight
    if (countLight > 0 ):
        if (post["data"] == "On"):
            print("Lights  On")
        else:
            print("Lights  Off")
    countLight+=1

def pump_stream_handler(post):
    global countPump
    if (countPump > 0 ):
        if (post["data"] == "On"):
            print("Pumps On")
        else:
            print("Pumps Off")
    countPump+=1


lightStream = db.child("UserInput").child("Light").stream(light_stream_handler, user['idToken'])
pumpStram = db.child("UserInput").child("Pump").stream(pump_stream_handler, user['idToken'])

print('hi')
