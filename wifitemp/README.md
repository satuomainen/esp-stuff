# wifitemp

A wireless thermometer with ESP8266 and DS18B20.

![wifitemp](https://raw.github.com/satuomainen/esp-stuff/master/wifitemp/wifitemp.jpg)

Initially I wanted this thing to monitor the freezer in our garage but unfortunately
the wifi from our house is not available there. Now it just measures the upstairs
temperature in our house.


## The materials

Bill of materials:
* ESP8266 module
* DS18B20 temperature sensor + 4k7 Ohm resistor
* Step-down converter from 5V to 3V3 (I used a AMS1117-3.3)
* 5V power source (I used an old 5V phone charger)
* Some veroboard and bits of wire
* An enclosure
* Optional: A LED + resistor for the blinker option

There are plenty of instructions available in the Internet how to connect everything so
I won't repeat it here. Just make sure you have a proper ground rail if you solder 
everything to the veroboard. For example, solder a thick (0.75 mm) copper wire to run 
along the strip which has the ground connected to it. I am only a software guy so I 
cannot explain the theory why this is important. I only know that had a lot of trouble
getting the contraption working before the ground rail was made properly.


## Preparing the ESP8266

You will need a TTL USB-serial adapter to be able to connect the ESP8266 with your
computer. I made a thing which has the USB-serial adapter and a socket for the ESP8266
and the required jumpers to pull GPIO2 down. This tool allows me to just stick the
ESP8266 module in the socket and hook up the thing with the computer and flash away.
The same tool can then be later used when I transfer program files to and from the
ESP8266.

The first thing I did was to install the NodeMCU firmware to the ESP8266 module. The
Internet will tell you how, see for example [esptool](https://github.com/themadinventor/esptool).

The [nodemcu-firmware](https://github.com/nodemcu/nodemcu-firmware) repository comes
with a library that allows getting values from the DS18B20 temperature sensor. Look for
ds18b20.lua under the lua_modules, that file needs to go into the ESP826.


## Developing the software


Before soldering anything, I put the thing together on a breadboard and developed the
software. When I was happy with the features and the software quality, I soldered the
components onto a piece of veroboard and put the whole thing in an plastic box. Then I 
just powered up the thing and it started sending measurements just like it did form the
workbench. 

The final product does have a socket for the ESP8266 so that if I have to change the 
software I can just pop the module out and stick it in my flashing tool. The modules
are cheap but still it would be a waste to throw the module out just for a software
change.

When developing the software, you need to be able to send your program files into the
ESP8266. I used a tool called [ESPlorer](http://esp8266.ru/esplorer/), which seems to
do the job quite well.

The starting point for software development is the file `init.lua` in the module's 
root directory. The NodeMCU will run that file when it starts up.


## My implementation

I already had a hobby server which collects temperature, humidity and barometric
pressure data from a Raspberry Pi so I wanted to use the same server to store the
data from this gadget as well. The connection parameters are read from `config.json`,
which you can make by copying the `config.json.example` under the correct name
and configuring the Wifi access details and the servers you want the readings to be
posted. You can configure one or more servers.

Obviously the format in which the data is posted depends on the backend so you will
need to adapt the `httpcontroller.lua` to format the data. Most likely you would 
have to change the configuration data structure as well to suit your needs.

My measurement collection application does not have authentication other than an
API key which needs to be correct. But for example HTTP Basic Auth should be 
quite trivial to add to my example. If you need something more complex (e.g. OAuth),
you will probably be best off posting the data into Thingspeak and have other means
to forward it from there.

My implementation wants to be resilient to hardware glitches so it reboots every time
it has completed one cycle of work. In practice this means that when the device starts
up, it connects to configured the Wifi access point and after a successful connection
it posts the measurement. Then it will wait for the configured time period, reboot
and start over. The default settings post a reading every five minutes. I reckon that
the device probably survives a short cycle after power up better than if I had it
stay on and trigger the measurement with repeated timer activations.
