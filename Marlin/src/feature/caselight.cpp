/**
 * Marlin 3D Printer Firmware
 * Copyright (C) 2016 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "../inc/MarlinConfig.h"
#include "../feature/Max7219_Debug_LEDs.h"

#if HAS_CASE_LIGHT

uint8_t case_light_brightness = CASE_LIGHT_DEFAULT_BRIGHTNESS;
bool case_light_on = CASE_LIGHT_DEFAULT_ON;

#if ENABLED(CASE_LIGHT_USE_NEOPIXEL)
  #include "leds/leds.h"
  LEDColor case_light_color =
    #ifdef CASE_LIGHT_NEOPIXEL_COLOR
      CASE_LIGHT_NEOPIXEL_COLOR
    #else
      { 255, 255, 255, 255 }
    #endif
  ;
#endif

/**
 * The following are needed because ARM chips ignore a "WRITE(CASE_LIGHT_PIN,x)" command to the pins that
 * are directly controlled by the PWM module. In order to turn them off the brightness level needs to be
 * set to off.  Since we can't use the pwm register to save the last brightness level we need a variable
 * to save it.
 */
uint8_t case_light_brightness_sav;   // saves brighness info so can restore when "M355 S1" received
bool case_light_arg_flag;  // flag to notify if S or P argument type

#ifndef INVERT_CASE_LIGHT
  #define INVERT_CASE_LIGHT false
#endif

void update_case_light() {

//Max7219::led_toggle(0, 1);

  if (!(case_light_arg_flag && !case_light_on))
    case_light_brightness_sav = case_light_brightness;  // save brightness except if this is an S0 argument

//Max7219::led_toggle(1, 1);

  if (case_light_arg_flag && case_light_on)
    case_light_brightness = case_light_brightness_sav;  // restore last brightens if this is an S1 argument

//Max7219::led_toggle(2, 1);

  const uint8_t i = case_light_on ? case_light_brightness : 0, n10ct = INVERT_CASE_LIGHT ? 255 - i : i;

  #if ENABLED(CASE_LIGHT_USE_NEOPIXEL)

//Max7219::led_toggle(3, 1);

    leds.set_color(
      MakeLEDColor(case_light_color.r, case_light_color.g, case_light_color.b, case_light_color.w, n10ct),
      false
    );

  #else // !CASE_LIGHT_USE_NEOPIXEL

    SET_OUTPUT(CASE_LIGHT_PIN);
    if (USEABLE_HARDWARE_PWM(CASE_LIGHT_PIN))
{//Max7219::led_toggle(4, 1);

      analogWrite(CASE_LIGHT_PIN, n10ct);
}
    else {

//Max7219::led_toggle(5, 1);

      const bool s = case_light_on ? !INVERT_CASE_LIGHT : INVERT_CASE_LIGHT;
      WRITE(CASE_LIGHT_PIN, s ? HIGH : LOW);
    }

  #endif // !CASE_LIGHT_USE_NEOPIXEL
}

#endif // HAS_CASE_LIGHT
