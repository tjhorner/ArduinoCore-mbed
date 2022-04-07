# Custom ArduinoCore-mbed

This is a custom version of [ArduinoCore-mbed](https://github.com/arduino/ArduinoCore-mbed) which modifies some hardcoded values in `variants/RASPBERRY_PI_PICO` so that it works with my PCB business card project. It's unfortunate that I need to do this, and that you can't make your own variants without forking the entire thing.

Anyway, I just changed the `PIN_LED` to `31` since the v3 of the business card does not have a built-in LED, and the pin that it was set to before was `25`, which I am using for the Neopixels. This would potentially cause issues because it uses this for USB status indication (which I also disabled). The more "correct" way to do this is probably to create another variant for the business card but I am lazy and it looks like it's a lot of steps to actually do that. I just wanna change one variable, man!

Once I finish v4, this should no longer be needed, since there will actually be a status LED on pin 25 (the Neopixels were moved to 24).