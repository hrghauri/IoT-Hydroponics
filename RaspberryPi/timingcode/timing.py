import time
import datetime
import serial

#ser = serial.Serial('/dev/ttyACM0', 9600)
ser = serial.Serial("/dev/ttyACM2", 9600)



while True:
	#ser.write('2')
	#x = "PI: TIME: " + str(int(round(time.time() * 1000))) 
	x = "PI: PUMP: OFF"
	ser.write(x.encode())
	#print (datetime.datetime.now())
	print(x)
	time.sleep(5)


