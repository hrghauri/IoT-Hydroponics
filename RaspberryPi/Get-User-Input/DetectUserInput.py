import pyrebase
import os
from datetime import datetime
import time
import serial
import os
import random


ser = serial.Serial("/dev/ttyACM0", 9600)
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
count = 0
imageCount = 0
def light_stream_handler(post):
    global countLight
    if (countLight > 0 ):
        if (post["data"] == "On"):
            writeSerial("PI: LIGHTS: ON")
        else:
            writeSerial("PI: LIGHTS: OFF")
    countLight+=1

def pump_stream_handler(post):
    global countPump
    if (countPump > 0 ):
        if (post["data"] == "On"):
            writeSerial("PI: PUMP: ON")
        else:
            writeSerial("PI: PUMP: OFF")
    countPump+=1
    
    
def writeSerial(x):
    ser.write(x.encode())
    print("Written " + x)

#lightStream = db.child("UserInput").child("Light").stream(light_stream_handler, user['idToken'])
#pumpStram = db.child("UserInput").child("Pump").stream(pump_stream_handler, user['idToken'])

print('Program Starting')
lightStream = db.child("UserInput").child("Light").stream(light_stream_handler, user['idToken'])
pumpStram = db.child("UserInput").child("Pump").stream(pump_stream_handler, user['idToken'])




imageName = "image "


try:
    while True:
        if count%10 == 0:
            #newImageName = imageName + str(imageCount) + ".jpg"
	    #camera.capture(newImageName)
	    #uploaded_image = im.upload_image(newImageName, title=newImageName)
	    #time.sleep(6)
	    #os.remove(newImageName)
	    #imageCount = imageCount + 1
            print("Yellow")
        time.sleep(20)
        print("--------------------------------------------------------------------------------")
	#print(uploaded_image.title)
	#print(uploaded_image.link)
        #y = "PI: TIME: " + str(int(round(time.time() * 1000)))
        #y = datetime.now().strftime('%H%M%S%d%m%Y')
        y = datetime.now().strftime('%H%M%S')
        writeSerial("PI: TIME: "+y)
        #Or Reads
        ph = round(random.uniform(4.0,7.5),1)
        data = {
    		"PH": str(ph),
    		"Time": datetime.now().strftime('%Y-%m-%d %H:%M:%S'),
    		"Image":{
                    "Image_Title" : "image 1",                     
                    #"Image_Title": uploaded_image.title,
                    #"Image_URL": uploaded_image.link
                    "Image_URL": "https://i.imgur.com/qhsKfHo.jpg"
                    }
	}

        results = db.child("Readings").push(data, user['idToken'])
        print("Pushed to Firebase")
        user = auth.refresh(user['refreshToken'])
        #countLight = 0
        #countPump = 0

        time.sleep(4)
        count = count + 1
except KeyboardInterrupt:
	#camera.close()
	exit
