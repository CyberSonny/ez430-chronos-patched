
#ifndef FORECAST_H_
#define FORECAST_H_

// *************************************************************************************************
// Include section

// *************************************************************************************************
// Prototypes section
extern void reset_altitude_measurement(void);
extern u8 is_altitude_measurement(void);
extern void start_altitude_measurement(void);
extern void stop_altitude_measurement(void);
extern void do_altitude_measurement(u8 filter);

// menu functions
extern void sx_forecast(u8 line);
extern void mx_forecast(u8 line);
extern void display_forecast(u8 line, u8 update);

// *************************************************************************************************
// Defines section
#define FORECAST_MEASUREMENT_TIMEOUT    (360 * 60u) // Stop altitude measurement after 360 minutes to
                                                   // save battery

// *************************************************************************************************
// Global Variable section
struct alt
{
    menu_t state;                                  // MENU_ITEM_NOT_VISIBLE, MENU_ITEM_VISIBLE
    u32 pressure;                                  // Pressure (Pa)
//    u16 temperature;                               // Temperature (K)
//    s16 altitude;                                  // Altitude (m)
//    s16 altitude_offset;                           // Altitude offset stored during calibration
    u16 timeout;                                   // Timeout
    u8  display;
//    s16 pressure_offset;                            // Pressure offset
//    u32 pressure_old;
};
extern struct forcast sForcast;

// *************************************************************************************************
// Extern section

#endif                                             /*FORECAST_H_ */
