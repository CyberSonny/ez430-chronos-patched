// *************************************************************************************************
// Weather forecast functions.
// *************************************************************************************************

// *************************************************************************************************
// Include section

// system
#include "project.h"

// driver
#include "forecast.h"
#include "display.h"
#include "vti_ps.h"
#include "ports.h"
#include "timer.h"

// logic
#include "user.h"

// *************************************************************************************************
// Prototypes section

// *************************************************************************************************
// Defines section

// *************************************************************************************************
// Global Variable section
struct forecast sForecast;

// *************************************************************************************************
// Extern section

// Global flag for pressure sensor initialisation status
extern u8 ps_ok;

// *************************************************************************************************
// @fn          reset_altitude_measurement
// @brief       Reset altitude measurement.
// @param       none
// @return      none
// *************************************************************************************************
void reset_forecast_measurement(void)
{
    // Menu item is not visible
    sForecast.state = MENU_ITEM_NOT_VISIBLE;

    // Clear timeout counter
    sForecast.timeout = 0;

// Set default altitude value
//    sAlt.altitude = 0;

    // Pressure sensor ok?
    if (ps_ok)
    {
        // Do single conversion
        start_forecast_measurement();
        stop_forecast_measurement();
    }
}


// *************************************************************************************************
// @fn          is_forecast_measurement
// @brief       Forecast measurement check
// @param       none
// @return      u8              1=Measurement ongoing, 0=measurement off
// *************************************************************************************************
u8 is_forecast_measurement(void)
{
    return ((sForecast.state == MENU_ITEM_VISIBLE) && (sForecast.timeout > 0));
}

// *************************************************************************************************
// @fn          start_altitude_measurement
// @brief       Start altitude measurement
// @param       none
// @return      none
// *************************************************************************************************
void start_forecast_measurement(void)
{
    // Show warning if pressure sensor was not initialised properly
    if (!ps_ok)
    {
        display_chars(LCD_SEG_L1_2_0, (u8 *) "ERR", SEG_ON);
        return;
    }

    // Start pressure measurement if timeout has elapsed
    if (sForecast.timeout == 0)
    {
        // Enable DRDY IRQ on rising edge
        PS_INT_IFG &= ~PS_INT_PIN;
        PS_INT_IE |= PS_INT_PIN;

        // Start pressure sensor
        ps_start();
        
        // Set timeout counter only if sensor status was OK
        sForecast.timeout = FORECAST_MEASUREMENT_TIMEOUT;

        // Get updated pressure
        while ((PS_INT_IN & PS_INT_PIN) == 0) ;
        do_forecast_measurement(FILTER_OFF);
    }
}

// *************************************************************************************************
// @fn          stop_altitude_measurement
// @brief       Stop altitude measurement
// @param       none
// @return      none
// *************************************************************************************************
void stop_forecast_measurement(void)
{
    // Return if pressure sensor was not initialised properly
    if (!ps_ok)
        return;

    // Stop pressure sensor
    ps_stop();

    // Disable DRDY IRQ
    PS_INT_IE &= ~PS_INT_PIN;
    PS_INT_IFG &= ~PS_INT_PIN;

    // Clear timeout counter
    sForecast.timeout = 0;
}

// *************************************************************************************************
// @fn          do_altitude_measurement
// @brief       Perform single altitude measurement
// @param       u8 filter       Filter option
// @return      none
// *************************************************************************************************
void do_forecast_measurement(u8 filter)
{
    volatile u32 pressure;

    // If sensor is not ready, skip data read
    if ((PS_INT_IN & PS_INT_PIN) == 0)
        return;

    // Get pressure (format is 1Pa) from sensor
    pressure = ps_get_pa();

    // Store measured pressure value
    if (filter == FILTER_OFF)   //sAlt.pressure == 0)
    {
        sForecast.pressure = pressure;
    }
    else
    {
        // Filter current pressure
        pressure = (u32) ((pressure * 0.2) + (sAlt.pressure * 0.8));

        // Store average pressure
        sForecast.pressure = pressure;
    }
}

// *************************************************************************************************
// @fn          sx_altitude
// @brief       Altitude direct function.
// @param       u8 line LINE1, LINE2
// @return      none
// *************************************************************************************************
void sx_forecast(u8 line)
{
    // Function can be empty
    // Restarting of altitude measurement will be done by subsequent full display update
    if (sForecast.display == DISPLAY_DEFAULT_VIEW)
        sForecast.display = DISPLAY_ALTERNATIVE_VIEW;
//    else if (sDate.display == DISPLAY_ALTERNATIVE_VIEW)
//      sDate.display = DISPLAY_ALTERNATIVE_VIEW1;
    else
        sForecast.display = DISPLAY_DEFAULT_VIEW;  
  
}

// *************************************************************************************************
// @fn          mx_altitude
// @brief       Mx button handler to set the altitude offset.
// @param       u8 line         LINE1
// @return      none
// *************************************************************************************************
void mx_forecast(u8 line)
{
    u8 select;
    s32 altitude;
    s32 limit_high, limit_low;
    s32 offset;
    
    select = 0;  
    
    // Clear display
    clear_display_all();

    offset = sAlt.pressure_offset;
    // Set lower and upper limits for offset correction

    // Display "m" symbol
    display_symbol(LCD_UNIT_L1_M, SEG_ON);

    // Convert global variable to local variable
//    altitude = sAlt.altitude;        



    // Loop values until all are set or user breaks set
    while (1)
    {
        // Idle timeout: exit without saving
        if (sys.flag.idle_timeout)
            break;

        // Button STAR (short): save, then exit
        if (button.flag.star)
        {
            // Set display update flag
            display.flag.line1_full_update = 1;

            break;
        }
        switch (select)
         {
         case 0:
           // Set current altitude - offset is set when leaving function
           
           display_symbol(LCD_UNIT_L1_PER_H, SEG_OFF);  
           display_chars(LCD_SEG_L2_5_0, (u8 *) "   ALT", SEG_ON);
           
           set_value(&altitude, 4, 3, limit_low, limit_high, SETVALUE_DISPLAY_VALUE +
                    SETVALUE_FAST_MODE + SETVALUE_DISPLAY_ARROWS + SETVALUE_NEXT_VALUE, LCD_SEG_L1_3_0,
                    display_value);           
           select = 1;
           break;
         case 1:
           // Set pressure offset
           display_symbol(LCD_UNIT_L1_PER_H, SEG_ON);                      
           display_symbol(LCD_UNIT_L1_M, SEG_OFF);
           display_symbol(LCD_UNIT_L1_FT, SEG_OFF);               
           display_chars(LCD_SEG_L2_5_0, (u8 *) " OFFS", SEG_ON);           
          set_value(&offset, 4, 3, -100, 100, SETVALUE_DISPLAY_VALUE +
                  SETVALUE_FAST_MODE + SETVALUE_DISPLAY_ARROWS + SETVALUE_NEXT_VALUE, LCD_SEG_L1_3_0,
                  display_value);           
           select = 0;
           break;           
         }        
    }

    // Clear button flags
    button.all_flags = 0;
}

// *************************************************************************************************
// @fn          display_altitude
// @brief       Display routine. Supports display in meters and feet.
// @param       u8 line                 LINE1
//                              u8 update               DISPLAY_LINE_UPDATE_FULL,
// DISPLAY_LINE_UPDATE_PARTIAL, DISPLAY_LINE_CLEAR
// @return      none
// *************************************************************************************************
void display_forecast(u8 line, u8 update)
{
    u8 *str;
    s16 ft;

    // redraw whole screen
    if (update == DISPLAY_LINE_UPDATE_FULL)
    {
        // Enable pressure measurement
        sForecast.state = MENU_ITEM_VISIBLE;

        // Start measurement
        start_forecast_measurement();


        // Display altitude
        display_altitude(LINE1, DISPLAY_LINE_UPDATE_PARTIAL);
    }
    else if (update == DISPLAY_LINE_UPDATE_PARTIAL)
    {
        // Update display only while measurement is active
        if (sAlt.timeout > 0)
        {
          if (sAlt.display == DISPLAY_DEFAULT_VIEW)
          {
            display_symbol(LCD_UNIT_L1_PER_H, SEG_OFF);
            display_symbol(LCD_SYMB_AM, SEG_OFF);            
            if (sys.flag.use_metric_units)
            {
               // Display "m" symbol
               display_symbol(LCD_UNIT_L1_M, SEG_ON);
             }
            else
             {
              // Display "ft" symbol
              display_symbol(LCD_UNIT_L1_FT, SEG_ON);
              }        
            if (sys.flag.use_metric_units)
            {
                // Display altitude in xxxx m format, allow 3 leading blank digits
                if (sAlt.altitude >= 0)
                {
                    str = int_to_array(sAlt.altitude, 4, 3);
                    display_symbol(LCD_SYMB_ARROW_UP, SEG_ON);
                    display_symbol(LCD_SYMB_ARROW_DOWN, SEG_OFF);
                }
                else
                {
                    str = int_to_array((-1)*sAlt.altitude, 4, 3);
                    //const u8 neg_txt[3] ="NEG";
                    //str = (u8 *)neg_txt;
                    display_symbol(LCD_SYMB_ARROW_UP, SEG_OFF);
                    display_symbol(LCD_SYMB_ARROW_DOWN, SEG_ON);
                }
            }
            else
            {
                // Convert from meters to feet
                ft = convert_m_to_ft(sAlt.altitude);

                // Limit to 9999ft (3047m)
                if (ft > 9999)
                    ft = 9999;

                // Display altitude in xxxx ft format, allow 3 leading blank digits
                if (ft >= 0)
                {
                    str = int_to_array(ft, 4, 3);
                    display_symbol(LCD_SYMB_ARROW_UP, SEG_ON);
                    display_symbol(LCD_SYMB_ARROW_DOWN, SEG_OFF);
                }
                else
                {
                    str = int_to_array(ft * (-1), 4, 3);
                    display_symbol(LCD_SYMB_ARROW_UP, SEG_OFF);
                    display_symbol(LCD_SYMB_ARROW_DOWN, SEG_ON);
                }
            }
            display_chars(LCD_SEG_L1_3_0, str, SEG_ON);
          }
          else if (sAlt.display == DISPLAY_ALTERNATIVE_VIEW)
          {
            // Display Pressure in hPa
            
            u16 PressureToDisp = (u16) (((float)sAlt.pressure + 100.00 * (float) sAlt.pressure_offset) / 100.00 + 0.5);
            str = int_to_array(PressureToDisp, 4, 3);
            
            display_symbol(LCD_SYMB_AM, SEG_OFF);            
                    
            display_symbol(LCD_SYMB_ARROW_UP, SEG_OFF);
            display_symbol(LCD_SYMB_ARROW_DOWN, SEG_OFF);
        
            display_symbol(LCD_UNIT_L1_M, SEG_OFF);
            display_symbol(LCD_UNIT_L1_FT, SEG_OFF);
            display_symbol(LCD_UNIT_L1_PER_H, SEG_ON);
            
            display_chars(LCD_SEG_L1_3_0, str, SEG_ON);
          }
          else if (sAlt.display == DISPLAY_ALTERNATIVE_VIEW1)
          {
            // Display Pressure in mmHg
            u16 PressureToDisp = (u16) (3.0 * ((float)sAlt.pressure + 100.00 * (float)sAlt.pressure_offset) / 400.00  + 0.5);            
            str = int_to_array(PressureToDisp, 4, 3);
            
            display_symbol(LCD_SYMB_ARROW_UP, SEG_OFF);
            display_symbol(LCD_SYMB_ARROW_DOWN, SEG_OFF);
        
            display_symbol(LCD_UNIT_L1_M, SEG_OFF);
            display_symbol(LCD_UNIT_L1_FT, SEG_OFF);
            display_symbol(LCD_UNIT_L1_PER_H, SEG_OFF);
            
            display_symbol(LCD_SYMB_AM, SEG_ON);            
              
            display_chars(LCD_SEG_L1_3_0, str, SEG_ON);
            
          }
            
        }
    }
    else if (update == DISPLAY_LINE_CLEAR)
    {
        // Disable pressure measurement
        sAlt.state = MENU_ITEM_NOT_VISIBLE;

        // Stop measurement
        stop_altitude_measurement();

        // Clean up function-specific segments before leaving function
        display_symbol(LCD_UNIT_L1_M, SEG_OFF);
        display_symbol(LCD_UNIT_L1_FT, SEG_OFF);
        display_symbol(LCD_SYMB_ARROW_UP, SEG_OFF);
        display_symbol(LCD_SYMB_ARROW_DOWN, SEG_OFF);
        display_symbol(LCD_UNIT_L1_PER_H, SEG_OFF);    
        display_symbol(LCD_SYMB_AM, SEG_OFF);            
            
        sAlt.display = DISPLAY_DEFAULT_VIEW;
    }
}

