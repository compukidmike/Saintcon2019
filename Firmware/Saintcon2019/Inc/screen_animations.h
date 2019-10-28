/*
 * screen_animations.h
 *
 *  Created on: Oct 15, 2019
 *      Author: bashNinja
 */

#ifndef SCREEN_ANIMATIONS_H_
#define SCREEN_ANIMATIONS_H_

int waterfall();
int line_wave();
int rainbow_triangles();
int weirdRainbow();
int screen_cycle();
RGB pulseLEDcolor(RGB pixelColor, uint8_t step);
float mapf(float value, float c_min, float c_max, float t_min, float t_max);
int plasma();
float bound(float value, float max, float min);
int LED_waveEffect();
int customMessage();

#endif /* SCREEN_ANIMATIONS_H_ */
