# Arduino Photo Booth

This is the code for a photo booth driven by an Arduino-based intervalometer connected to a Canon EOS 350D camera.


# Usage

Upload the sketch to your Arduino Uno or compatible micro controller (see [the official Arduino IDE documentation](http://arduino.cc/en/Guide/HomePage) for detailed instructions).

This project assumes the following components:

Pin | Components
--- | ----------
5   | [Arcade button microswitch](http://www.adafruit.com/products/1188), normally open
6   | LED
7   | Two chained [Adafruit NeoPixel Sticks](http://www.adafruit.com/products/1426)
8   | An single-channel [optoisolator](https://www.sparkfun.com/products/314) (autofocus)
9   | An single-channel optoisolator (shutter)


# License

This project is MIT licensed. See the included `LICENSE` file for details.