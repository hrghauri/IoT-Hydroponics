import pyrebase
import os
from datetime import datetime
import time
import serial
import os
import random
import re
import picamera
import pyimgur


ser = serial.Serial("/dev/ttyACM0", 9600)
CLIENT_ID = "4072de8d789c9a7"
CLIENT_SECRET = "36306c975d9518ef7890f95a4b945831ef40f14c"
im = pyimgur.Imgur(CLIENT_ID, CLIENT_SECRET)
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

imageName = "image "
camera = picamera.PiCamera()


countLight = 0
countPump = 0
countphPump = 0
countecPump = 0
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

def ph_pump_stream_handler(post):
    global countphPump
    if (countphPump > 0):
        if (post["data"] == "On"):
            writeSerial("PI: PHPUMP: ON")
        else:
            writeSerial("PI: PHPUMP: OFF")
    countphPump+=1
    
def ec_pump_stream_handler(post):
    global countecPump
    if (countecPump > 0):
        if (post["data"] == "On"):
            writeSerial("PI: ECPUMP: ON")
        else:
            writeSerial("PI: ECPUMP: OFF")
    countecPump+=1
    
    
def writeSerial(x):
    ser.write(x.encode())
    print("Written " + x)

#lightStream = db.child("UserInput").child("Light").stream(light_stream_handler, user['idToken'])
#pumpStram = db.child("UserInput").child("Pump").stream(pump_stream_handler, user['idToken'])

print('Program Starting')
lightStream = db.child("UserInput").child("Light").stream(light_stream_handler, user['idToken'])
pumpStram = db.child("UserInput").child("Pump").stream(pump_stream_handler, user['idToken'])
phPumpStream = db.child("UserInput").child("pH").stream(ph_pump_stream_handler, user['idToken'])
ecPumpStream = db.child("UserInput").child("eC").stream(ec_pump_stream_handler, user['idToken'])



imageName = "image "


try:
    while True:
        if count%10 == 0:
            newImageName = imageName + str(imageCount) + ".jpg"
            camera.capture(newImageName)
            uploaded_image = im.upload_image(newImageName, title=newImageName)
            time.sleep(6)
            os.remove(newImageName)
            imageCount = imageCount + 1
            #print("Yellow")
        time.sleep(20)
        print("--------------------------------------------------------------------------------")
        print(uploaded_image.title)
        print(uploaded_image.link)
        #y = "PI: TIME: " + str(int(round(time.time() * 1000)))
        #y = datetime.now().strftime('%H%M%S%d%m%Y')
        y = datetime.now().strftime('%H%M%S')
        writeSerial("PI: TIME: "+y)
        print("about to..")
        x = ser.readline()
        z = ser.readline()
        print(x)
        print(z)
        ph = ""
        ppm = ""
        if "PH VALUE" in str(x):
            ph = ", ".join(re.findall("\d+\.\d+",str(x)))
        elif "PH VALUE" in str(z):
            ph = ", ".join(re.findall("\d+\.\d+",str(z)))
        else:
            ph = "7.2"
            
        if "PPM VALUE" in str(z):
            ppm = ", ".join(re.findall("\d+\.\d+",str(z)))
        elif "PH VALUE" in str(x):
            ppm = ", ".join(re.findall("\d+\.\d+",str(x)))
        else:
            ppm = "7.2"
        
        #print(int(filter(str.isdigit,str(x))))
        #print(int(filter(str.isdigit,str(z))))
        #print(re.findall("\d+\.\d+",str(x)))
        #print(re.findall("\d+\.\d+",str(z)))
        #print(", ".join(re.findall("\d+\.\d+",str(x))))
        #print(", ".join(re.findall("\d+\.\d+",str(z))))
        print(ph)
        print(ppm)
        #ph = round(random.uniform(4.0,7.5),1)
        data = {
    		"PH": ph,
                "PPM": ppm,
    		"Time": datetime.now().strftime('%Y-%m-%d %H:%M:%S'),
    		"Image":{
                    #"Image_Title" : "image 1",                     
                    "Image_Title": uploaded_image.title,
                    "Image_URL": uploaded_image.link
                    #"Image_URL": "https://i.imgur.com/qhsKfHo.jpg"
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
