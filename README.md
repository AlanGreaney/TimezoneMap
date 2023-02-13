# Timezone Map

An arduino project using 3d printed map tiles and 4-character 8-segment displays to show timezones around the world

<img src="https://raw.githubusercontent.com/AlanGreaney/TimezoneMap/main/img/mapMounted.jpg" width="75%">

## 3d Print Process

The final version at time of writing is on 8 3d-printed tiles - I downloaded this Topographical 3d Data: https://cults3d.com/en/3d-model/art/elevation-world-map-no-multi-color-printer-needed

The tiles were printed white, then two coats of black acrylic paint was put on all water locations, then a final two coats of white paint on the land areas.

Measurements of the "TM1637" spec segement displays were taken and rectangles to fit them cut out of the map data in Blender.

https://smile.amazon.com/gp/product/B01DKISMXK/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1

## Coding Process

The arduino is an Arduino Uno R3. The code is written to use the Arduino's internal clock as it's time-keeping. Arduino libraries are used to help with managing the Segment Displays, and the Datetime Objects. The arduino code takes in a number, ie int(1050) would be 10:50, so a function to take the hour:mm from the time object and convert it to a single number had to be made, and handle adding leading zeros.

Upon plugging in the Arduino, the timezone (PST, EST, UST, JPN) displays as text for 10 seconds before switching to show the time. 

To keep wiring to a minimum behind the unit, only 2 buttons are used. The 'time' button changes the hour on hold, and the minute on tap. The 'function' button toggles Daylight Savings On/Off when held, and changes the brightness of the screens on tap.

<img src="https://raw.githubusercontent.com/AlanGreaney/profile/main/images/mapWip2.jpg" width="75%">

The code for handling button presses is manually handled by reading how long the buttons stay HIGH. Holding both buttons makes the clock run at 10x speed for party mode - also just debugging.

Even though the main function runs every 10ms (espcially to responsively handle button presses), the updates are only sent to the screens when the hh:mm actually changes from the current, in order to reduce power usage for being left on 24/7. So far, no heating issues have been encountered. The final whole frame hangs slightly off the wall so that some airflow can home in behind the display.

## Framing/Mounting

The 3d printed tiles were glued to a large foam board, specifically one recycled from my sister's wedding! 

<img src="https://raw.githubusercontent.com/AlanGreaney/TimezoneMap/main/img/wipImage1.jpg" width="75%">

The wires are taped down with electrical tape, and double sided tape to hold the breadboard flat. The holes for the display units were pureposely printed a bit too small, and had to be sanded large to perfectly press fit into place.

The frame was made with pre-primed wood trim, cut to size, and spray painted black. Glue and nails with wood filler sanded down were used to hold the frame together, along with smaller pieces of wood behind the board to mount it to the frame. Screws were drilled halfway up the back of the frame with a wire strung between them to mount on a normal wall hanger.

## Improvements/thoughts

- V2 will use resin-printed plates to reduce the amount of print lines on the final board, and the final fit between boards. Some warping/moving occured during glueing
- The internal millis() clock for this Arduino rolls-over after 50 days, and the clock will probably need reset. A different timing solution may need to be found.
- I'm very pleased with the button code - it feels VERY good to use - but making it into a function would be nice, although they have very different use cases right now. If I had 5+ buttons, using callbacks would be much better for button handling, but right now that would be more cumbersome than it's worth.
- Manually painting all the water/land took a LONG time. I have ideas for 3d printing a negative of the land to make spray/airbrush painting do-able.
