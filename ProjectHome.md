Based on original firmware:
//   P.Francisco
//   Version    1.6
//   Texas Instruments, Inc
//   January 2011

I have added some features such as:

1) Day of week indication
2) Short beep each hour
3) Added indication of atmospheric pressure in hPa and mmHg
4) Added a forecast function (monitoring of delta Pressure in 15 minutes and making decission (Sunny, Rain, Stable))
5) Added time, date, day of week mode in bottom LCD screen (to view it in Altitude/ Temperature/ Accelermoter modes)...
6) Removed bug with negative altitude indication...
7) Added some other bugs (may be) :)

This is short manual for my version of "Date/Time" menu in Bottom Line:
By pressing "down" button you can choose different modes of date/time representation:
1) Day of week.DD
2) DD.MM
3) YYYY
4) HH:mm
5) ss

In "Altimeter" mode:
By pressing "UP" button you can choose different sub-modes:
1) Altimeter (there's an arrow showing sign and altitude in m/ft)
2) Atmosperic pressure in hPa (it is "/h" on the Top Line)
3) Atmosperic pressure in mmHg (it is "AM" on the Top Line)
4) Weather forecast mode (it is blinking R sign). In this mode it is continuously calculated Pressure Derivative due 15 minutes. Then it multiples by 4 (to get Derivative in 60 mins or 1 hour) and after 15 minutes it is showing weather forecast as: "UN H/UN L" - unstable high or low derivative!
"STAB" - stable weather
"RAIN" - rainy forecast
"SUN" - sunny forecast

I get this forecast method from:

http://www.vti.fi/midcom-serveattachmentguid-5ea0f5e98491572da5ae79b70f9f0a41/AN33_SCP1000_Pressure_Sensor_as_barometer_and_altimeter.pdf

You can shift pressure indication to "sea level" by manually adding offset for your location height. To do that, press "STAR" key for few seconds. Then you'll se "Alt" message on Bottom Line. Then press # and you see "OFFS" message. By pressing "UP" and "DOWN" keys choose offset between "sea level" pressure and sensor measuring pressure. This value must be entered in hPa units ("/h" sign on the screen as reminder)!

This project for IAR5.30.1 for MSP430. It is configured both for Kickstart and Unrestricted vesions.
Kickstart version uses precompiled version of drivers... So it will not let you to change drivers sourses :(

