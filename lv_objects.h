/**
 * @file lv_objects.h
 *
 */

#ifndef LV_OBJECTS_H
#define LV_OBJECTS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"
#include "lv_conf.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_objects(void);

uint8_t get_alpha(void);
void set_alpha(uint8_t val);
void chart1_set_next(int16_t val);
void chart2_set_next(int16_t val);
void chart3_set_next(int16_t val);
void chart4_set_next(int16_t val);
void chart5_set_next(int16_t val);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_OBJECTS_H*/
