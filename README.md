# RC Plane Xbox Controller 

This project is made to provide simple and begginer friendly controls for RC planes, using HC-12 transceivers and only an XBox controller!
For now you can control direction and throttle, which will sufice to test airframes, but I plan to add more functionality, coming soon...
I assume you have at least some experience with Arduinos and circuits, but even then, don't worry, this is a simple set-up.
With the following requirements, using this code you can make a simple RC Plane, you just need, well, the plane.

## Requirements

- Xbox Controller
- Windows Computer
- 2 Arduinos
- 2 HC-12 Transceivers (set them up before doing any of this pls)
- Plane Components (3 Servos, an ESC, an electrical motor and a battery)


Now on how to set-up everything

## Featured Code

1. **Windows Controller Interface** - This is the Controller.exe file, which will send the Serial data to the ground arduino, the cpp file is there too for you to check the code
2. **Ground Station** - The ground.ino file that you will have to upload into the ground arduino
3. **Flight Controller** - The Air.ino file, that you will have to upload into the arduino in the plane
4. **Schematic** - Not code, but essential to follow, as it is the wiring followed by the code

## Set-up
1. Make sure you have the Arduino IDE installed as you will need it to upload the code and for the next step
2. Set-up the HC-12 transceivers, [this is a good guide on how to](https://howtomechatronics.com/tutorials/arduino/arduino-and-hc-12-long-range-wireless-communication-module/), change the channel and set the power to max as you will want to flight this plane far.
3. Upload the Ground.ino into your ground station from the Arduino IDE, and upload the Air.ino file into the arduino in the plane.
4. Follow the schematic to build the ground station and plane
5. Connect you controller, make sure the Arduino IDE is closed (so it doesn't take up the Serial Port) and open Controller.exe, press <Enter> on your COM port, et voila, press <A> with your controller, it should vibrate
6. You should be able to fly !
7. (Recommended) Your ESC is probably working in a range of 1.2ms to 1.9ms instead of 1ms to 2ms, you can change this by setting it up. Use the controller to set throttle to max, then turn on the ESC, you'll hear beeping, then turn the throttle down as quick as possible, and you will hear more beeping, it's set, turn off the ESC. Next time you turn on the setup and ESC just don't turn on the throttle, the code starts it at 0 anyways, just don't touch it until you connect the ESC, that way you will not be resseting the ESC, just using it.

**Have fun and fly safe! 🛩️**
