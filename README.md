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
