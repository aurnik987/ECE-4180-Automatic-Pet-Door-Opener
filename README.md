# ECE-4180-Automatic-Pet-Door-Opener

## Team Members
Nikhil Auradkar, Salim Ben Ghorbel

# Project Description
The purpose of this project is to create a pet door that automatically unlocks when it senses a pet in front of it, using a combination of motion detection and Bluetooth sensing. The motivations for the project include creating a pet door that is more secure than a typical one.

As mentioned earlier, the door opens via a 'two-factor authentication' of a motion detector and Bluetooth scanner/beacon (the pet wears the beacon on its collar, and the scanner is placed on the door). If the motion detector triggers on movement, then the Bluetooth signal strength of the pet will be tested. If the strength is above a certain value, the pet will be let through the door.

A web server is created that allows the pet's owner to manually toggle the door latch on demand. The owner also has the option to change the door setting to 'manual' instead of 'automatic', which will send email notifications when the pet is detected at the door. 

## Parts List
2 X SparkFun ESP32 Thing https://www.sparkfun.com/products/13907

1 X SparkFun PIR Motion Sensor https://www.sparkfun.com/products/13285

1 X SparkFun Servo Hitec HT-422 Motor (Parts Kit) https://www.sparkfun.com/products/11884

1 X SparkFun Lithium 3.7A 850mAh Battery https://www.digikey.com/product-detail/en/sparkfun-electronics/PRT-13854/1568-1495-ND/6605201

1 X Adafruit 5V 2A Power Supply (Parts Kit) https://www.adafruit.com/product/1994

1 X Barrel Jack to Breadboard Adapter (Parts Kit)

Parts for Door (2x4s for the door frame, plywood for the door, hinges, screws, metal plate for latch stopper)

--------------------

The ESP32 Thing chip from Sparkfun has a Bluetooth module built in and is already interfaced by its developers through the Arduino IDE, which makes it convenient to use as both a Bluetooth scanner and Bluetooth beacon. The scanner chip is placed on the back of the door itself while the beacon chip is placed on the pet's collar. The beacon chip is powered by the lithium battery. The two ESP32 Thing chips establish a proximity value based on the strength of their Bluetooth connection (RSSI). 

The motion sensor is placed on the front of the door, looking downwards. When the motion sensor is triggered, the RSSI value of the Bluetooth connection is tested, and if it is strong enough, a command to turn the servo motor and lift the latch will be issued. The scanner chip, motion sensor, and servo motor will be powered by a 5V power supply with a barrel jack to breadboard pin adapter. 

## Design and Schematic
Initial Design:

Updated Design:

Schematic:

## Source Code
The source code was written in the Arduino IDE. Different code is uploaded to the ESP32 Thing scanner and the ESP32 Thing beacon.

## Photos and Demos
Photos:

Demo - manually opening/closing latch from web server:

Demo - web server changing its status to 'pet detected' when the pet is detected in manual mode:

Demo - web server changing its status to 'pet not detected' when the pet collar is out of range:

Demo - changing to 'automatic', latch is not opened when the pet collar is out of range:

Demo - the latch opening and closing when the motion sensor is triggered with the pet collar in range:
