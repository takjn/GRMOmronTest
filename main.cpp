// SPDX-License-Identifier: MIT
/*
 * MIT License
 * Copyright (c) 2019 Renesas Electronics Corporation
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "mbed.h"
#include "EasyAttach_CameraAndLCD.h"
#include "AsciiFont.h"
#include "r_dk2_if.h"
#include "r_drp_simple_isp.h"
#include "r_drp_resize_bilinear.h"
#include "dcache-control.h"
#include "D6T_44L_06.h"
#include "SHT30_DIS_B.h"
#include "OPT3001DNP.h"
#include "BARO_2SMPB_02E.h"
#include "LIS2DW12.h"


#ifndef MBED_CONF_APP_LCD
    #error "MBED_CONF_APP_LCD is not set"
#endif

/*! Frame buffer stride: Frame buffer stride should be set to a multiple of 32 or 128
    in accordance with the frame buffer burst transfer mode. */
#define VIDEO_PIXEL_HW         (640)    /* VGA */
#define VIDEO_PIXEL_VW         (480)    /* VGA */

static DisplayBase Display;

// Buffer for video
#define FRAME_BUFFER_STRIDE    (((VIDEO_PIXEL_HW * 1) + 63u) & ~63u)
#define FRAME_BUFFER_STRIDE_2  (((VIDEO_PIXEL_HW * 2) + 31u) & ~31u)
#define FRAME_BUFFER_HEIGHT    (VIDEO_PIXEL_VW)
static uint8_t fbuf_bayer[FRAME_BUFFER_STRIDE * FRAME_BUFFER_HEIGHT]__attribute((aligned(128)));
static uint8_t fbuf_yuv[FRAME_BUFFER_STRIDE_2 * FRAME_BUFFER_HEIGHT]__attribute((aligned(32)));
static uint8_t fbuf_overlay[FRAME_BUFFER_STRIDE * FRAME_BUFFER_HEIGHT]__attribute((section("NC_BSS"),aligned(32)));
static const uint32_t clut_data_resut[] = {0x00000000, 0xff000000};  // ARGB8888

// Buffer for sensor data
#define SENSOR_RAW_BUFFER_STRIDE    (((4 * 1) + 31u) & ~31u)
#define SENSOR_RAW_BUFFER_HEIGHT    (4)
#define SENSOR_WORK_BUFFER_STRIDE    (((VIDEO_PIXEL_HW * 1) + 31u) & ~31u)
static uint8_t fbuf_work_0[SENSOR_RAW_BUFFER_STRIDE * SENSOR_RAW_BUFFER_HEIGHT]__attribute((section("NC_BSS")));
static uint8_t fbuf_work_1[SENSOR_WORK_BUFFER_STRIDE * FRAME_BUFFER_HEIGHT]__attribute((section("NC_BSS")));
static uint8_t fbuf_sensor_result[FRAME_BUFFER_STRIDE_2 * VIDEO_PIXEL_VW]__attribute((section("NC_BSS")));

// Variables for DRP
#define DRP_FLG_TILE_ALL       (R_DK2_TILE_0 | R_DK2_TILE_1 | R_DK2_TILE_2 | R_DK2_TILE_3 | R_DK2_TILE_4 | R_DK2_TILE_5)
#define DRP_FLG_CAMER_IN       (0x00000100)
static uint8_t drp_lib_id[R_DK2_TILE_NUM] = {0};
static Thread drpTask(osPriorityHigh, 1024*8);
static r_drp_simple_isp_t param_isp __attribute((section("NC_BSS")));
static r_drp_resize_bilinear_t param_resize_bilinear __attribute((section("NC_BSS")));
static uint8_t ram_drp_lib_simple_isp_bayer2yuv_6[sizeof(g_drp_lib_simple_isp_bayer2yuv_6)]__attribute((aligned(32)));
static uint8_t ram_drp_lib_resize_bilinear[sizeof(g_drp_lib_resize_bilinear)]__attribute((aligned(32)));

// Variables for omron sensor
// D6T_44L_06
static D6T_44L_06 d6t_44l(I2C_SDA, I2C_SCL);
static Thread sensorTask(osPriorityNormal, 1024*8);
#define TILE_TEMP_MARGIN_UPPER (20)
#define TILE_TEMP_MARGIN_UNDER (70)
static uint8_t display_alpha = 0xA;
static int16_t pdta;
static int16_t buf[16];

// 2JCIE-EV01-RP1
#define CONV_RAW_TO_MG(x) (int)((double)(x) * 4000.0 / 32767.0)
SHT30_DIS_B sht30(I2C_SDA, I2C_SCL);                    // [SHT30-DIS-B] : Temperature / humidity sensor
OPT3001DNP opt3001(I2C_SDA, I2C_SCL);                   // [OPT3001DNP]  : Ambient light sensor
BARO_2SMPB_02E baro_2smpb(I2C_SDA, I2C_SCL);            // [2SMPB-02E]   : MEMS digital barometric pressure sensor
LIS2DW12 lis2dw(SPI_MOSI, SPI_MISO, SPI_SCKL, SPI_SSL); // [LIS2DW12]    : MEMS digital motion sensor

static InterruptIn button0(USER_BUTTON0);
static InterruptIn button1(USER_BUTTON1);

/*******************************************************************************
* Function Name: normalize0to1
* Description  : Normalize thermal data of a reception packet to the range of 0-1.
*                Also the value outside min, max is saturated.
* Arguments    : data     - input tempetue data
*                min      - smallest threshold tempature value
*                           (The integer which set a centigrade to 10 times)
*                max      - highest threshold  tempature value
*                           (The integer which set a centigrade to 10 times)
* Return Value : normalized output data
*******************************************************************************/
float normalize0to1(int16_t data, int min, int max)
{
    float normalized;

    if (data <= min) {
        normalized = 0.0;
    }
    else if (max <= data) {
        normalized = 1.0;
    }
    else {
        normalized = ((float)(data - min)) / ((float)(max - min));
    }
    return normalized;
}
/*******************************************************************************
 End of function normalize0to1
*******************************************************************************/

/*******************************************************************************
* Function Name: conv_normalize_to_color
* Description  : Change a floating-point data of 0.0-1.0 to the pixel color of
*                ARGB4444.
*                Low temperature changes blue and high temperature to red.
* Arguments    : data        - Normalized thermal data from 0.0 to 1.0.
* Return Value : ARGB4444 pixel color data.
*******************************************************************************/
// static uint16_t conv_normalize_to_color(uint8_t alpha, float data) {
//     uint8_t green;
//     uint8_t blue;
//     uint8_t red;

//     if (0.0 == data) {
//         /* Display blue when the temperature is below the minimum. */
//         blue  = 0x0F;
//         green = 0x00;
//         red   = 0x00;
//     }
//     else if (1.0 == data) {
//         /* Display red when the maximum temperature is exceeded. */
//         blue  = 0x00;
//         green = 0x00;
//         red   = 0x0F;
//     }
//     else {
//         float cosval   = cos( 4 * M_PI * data);
//         int    color    = (int)((((-cosval)/2) + 0.5) * 15);
//         if (data < 0.25) {
//             blue  = 0xF;
//             green = color;
//             red   = 0x00;
//         }
//         else if (data < 0.50) {
//             blue  = color;
//             green = 0x0F;
//             red   = 0x00;
//         }
//         else if (data < 0.75) {
//             blue  = 0x00;
//             green = 0x0F;
//             red   = color;
//         }
//         else {
//             blue  = 0x00;
//             green = color;
//             red   = 0x0F;
//         }
//     }

//     return ((alpha << 12) | (red << 8) | (green << 4) | blue);
// }

// pre-calculation for speed up
static uint16_t colors[256];
static void init_colors() {
    uint8_t green;
    uint8_t blue;
    uint8_t red;

    for (int i=0;i<256;i++) {
        float cosval = cos( 4 * M_PI * (float(i)/255.0));
        uint8_t color = (uint8_t)((((-cosval)/2) + 0.5) * 15);

        if (0 == i) {
            /* Display blue when the temperature is below the minimum. */
            blue  = 0x0F;
            green = 0x00;
            red   = 0x00;
        }
        else if (255 == i) {
            /* Display red when the maximum temperature is exceeded. */
            blue  = 0x00;
            green = 0x00;
            red   = 0x0F;
        }
        else {
            if (i < 64) {
                blue  = 0xF;
                green = color;
                red   = 0x00;
            }
            else if (i < 128) {
                blue  = color;
                green = 0x0F;
                red   = 0x00;
            }
            else if (i < 192) {
                blue  = 0x00;
                green = 0x0F;
                red   = color;
            }
            else {
                blue  = 0x00;
                green = color;
                red   = 0x0F;
            }
        }

        colors[i] = 0;
        colors[i] = ((0 << 12) | (red << 8) | (green << 4) | blue);
    }
} 

/*******************************************************************************
 End of function conv_normalize_to_color
*******************************************************************************/

static void IntCallbackFunc_Vfield(DisplayBase::int_type_t int_type) {
    drpTask.flags_set(DRP_FLG_CAMER_IN);
}

static void cb_drp_finish(uint8_t id) {
    uint32_t tile_no;
    uint32_t set_flgs = 0;

    // Change the operation state of the DRP library notified by the argument to finish
    for (tile_no = 0; tile_no < R_DK2_TILE_NUM; tile_no++) {
        if (drp_lib_id[tile_no] == id) {
            set_flgs |= (1 << tile_no);
        }
    }
    drpTask.flags_set(set_flgs);
}

static void Start_Video_Camera(void) {
    // Video capture setting (progressive form fixed)
    Display.Video_Write_Setting(
        DisplayBase::VIDEO_INPUT_CHANNEL_0,
        DisplayBase::COL_SYS_NTSC_358,
        (void *)fbuf_bayer,
        FRAME_BUFFER_STRIDE,
        DisplayBase::VIDEO_FORMAT_RAW8,
        DisplayBase::WR_RD_WRSWA_NON,
        VIDEO_PIXEL_VW,
        VIDEO_PIXEL_HW
    );
    EasyAttach_CameraStart(Display, DisplayBase::VIDEO_INPUT_CHANNEL_0);
}

static void Start_LCD_Display(void) {
    DisplayBase::rect_t rect;
    DisplayBase::clut_t clut_param;

    // for camera image
    rect.vs = 0;
    rect.vw = VIDEO_PIXEL_VW;
    rect.hs = 0;
    rect.hw = VIDEO_PIXEL_HW;
    Display.Graphics_Read_Setting(
        DisplayBase::GRAPHICS_LAYER_0,
        (void *)fbuf_yuv,
        FRAME_BUFFER_STRIDE_2,
        DisplayBase::GRAPHICS_FORMAT_YCBCR422,
        DisplayBase::WR_RD_WRSWA_32_16_8BIT,
        &rect
    );
    Display.Graphics_Start(DisplayBase::GRAPHICS_LAYER_0);

    // for sensor image
    rect.vs = 0;
    rect.vw = VIDEO_PIXEL_VW;
    rect.hs = 0;
    rect.hw = VIDEO_PIXEL_HW;
    Display.Graphics_Read_Setting(
        DisplayBase::GRAPHICS_LAYER_2,
        (void *)fbuf_sensor_result,
        FRAME_BUFFER_STRIDE_2,
        DisplayBase::GRAPHICS_FORMAT_ARGB4444,
        DisplayBase::WR_RD_WRSWA_32_16BIT,
        &rect
    );
    Display.Graphics_Start(DisplayBase::GRAPHICS_LAYER_2);

    memset(fbuf_overlay, 0, sizeof(fbuf_overlay));
    clut_param.color_num = sizeof(clut_data_resut) / sizeof(uint32_t);
    clut_param.clut = clut_data_resut;

    rect.vs = 0;
    rect.vw = VIDEO_PIXEL_VW;
    rect.hs = 0;
    rect.hw = VIDEO_PIXEL_HW;
    Display.Graphics_Read_Setting(
        DisplayBase::GRAPHICS_LAYER_3,
        (void *)fbuf_overlay,
        FRAME_BUFFER_STRIDE,
        DisplayBase::GRAPHICS_FORMAT_CLUT8,
        DisplayBase::WR_RD_WRSWA_32_16_8BIT,
        &rect,
        &clut_param
    );
    Display.Graphics_Start(DisplayBase::GRAPHICS_LAYER_3);

    ThisThread::sleep_for(50);
    EasyAttach_LcdBacklight(true);
}

static void drp_task(void) {
    EasyAttach_Init(Display);
    // Interrupt callback function setting (Field end signal for recording function in scaler 0)
    Display.Graphics_Irq_Handler_Set(DisplayBase::INT_TYPE_S0_VFIELD, 0, IntCallbackFunc_Vfield);
    Start_Video_Camera();
    Start_LCD_Display();

    // Copy to RAM to increase the speed of dynamic loading.
    memcpy(ram_drp_lib_simple_isp_bayer2yuv_6, g_drp_lib_simple_isp_bayer2yuv_6, sizeof(ram_drp_lib_simple_isp_bayer2yuv_6));
    memcpy(ram_drp_lib_resize_bilinear, g_drp_lib_resize_bilinear, sizeof(ram_drp_lib_resize_bilinear));

    R_DK2_Initialize();

    char str[64];
    AsciiFont ascii_font(fbuf_overlay, VIDEO_PIXEL_HW, VIDEO_PIXEL_VW, FRAME_BUFFER_STRIDE, 1);

    while (true) {
        ThisThread::flags_wait_all(DRP_FLG_CAMER_IN);

        /* SimpleIsp bayer2yuv_6 */
        R_DK2_Load(ram_drp_lib_simple_isp_bayer2yuv_6,
            R_DK2_TILE_0,
            R_DK2_TILE_PATTERN_6, NULL, &cb_drp_finish, drp_lib_id);
        R_DK2_Activate(0, 0);
        memset(&param_isp, 0, sizeof(param_isp));
        param_isp.src    = (uint32_t)fbuf_bayer;
        param_isp.dst    = (uint32_t)fbuf_yuv;
        param_isp.width  = VIDEO_PIXEL_HW;
        param_isp.height = VIDEO_PIXEL_VW;
        param_isp.gain_r = 0x1800;
        param_isp.gain_g = 0x1000;
        param_isp.gain_b = 0x1C00;
        param_isp.bias_r = 0;
        param_isp.bias_g = 0;
        param_isp.bias_b = 0;
        R_DK2_Start(drp_lib_id[0], (void *)&param_isp, sizeof(r_drp_simple_isp_t));
        ThisThread::flags_wait_all(DRP_FLG_TILE_ALL);
        R_DK2_Unload(0, drp_lib_id);

        /* resize bilinear */
        R_DK2_Load(g_drp_lib_resize_bilinear,
            R_DK2_TILE_0,
            R_DK2_TILE_PATTERN_6, NULL, &cb_drp_finish, drp_lib_id);
        R_DK2_Activate(0, 0);
        memset(&param_resize_bilinear, 0, sizeof(param_resize_bilinear));
        param_resize_bilinear.src    = (uint32_t)fbuf_work_0;
        param_resize_bilinear.dst    = (uint32_t)fbuf_work_1;
        param_resize_bilinear.src_width  = 4;
        param_resize_bilinear.src_height = 4;
        param_resize_bilinear.dst_width  = VIDEO_PIXEL_HW;
        param_resize_bilinear.dst_height = VIDEO_PIXEL_VW;
        R_DK2_Start(drp_lib_id[0], (void *)&param_resize_bilinear, sizeof(r_drp_resize_bilinear_t));
        ThisThread::flags_wait_all(DRP_FLG_TILE_ALL);
        R_DK2_Unload(0, drp_lib_id);

        uint16_t *p = (uint16_t*)fbuf_sensor_result;
        for (uint i=0;i<sizeof(fbuf_work_1);i++) {
            // p[i] = conv_normalize_to_color(display_alpha, (float(fbuf_work_1[i]) / 255.0));
            p[i] = ((display_alpha << 12) | colors[fbuf_work_1[i]]);
        }

        // Overlay some message
        sprintf(str, "PTAT: %6.1f[degC]   ", pdta / 10.0);
        ascii_font.DrawStr(str, (AsciiFont::CHAR_PIX_WIDTH)*2, (AsciiFont::CHAR_PIX_HEIGHT)*2, 1, 2);
        for (int y=0;y<4;y++) {
            for (int x=0;x<4;x++) {
                int px = VIDEO_PIXEL_HW / 4 * (x + 1) - VIDEO_PIXEL_HW / 6;
                int py = VIDEO_PIXEL_VW / 4 * (y + 1) - VIDEO_PIXEL_VW / 7;

                sprintf(str, "%4.1f   ", buf[y*4+x] / 10.0);
                ascii_font.DrawStr(str, px, py, 1, 2);
            }
        }
    }
}

static void sensor_task(void) {
    int32_t humi, temp32;
    uint32_t illm;
    uint32_t pres, dp, dt;
    int16_t temp16;
    int16_t accl[3];

    printf("\x1b[2J");  // Clear screen

    // setup sensors
    d6t_44l.setup();
    baro_2smpb.setup();
    sht30.setup();
    opt3001.setup();
    lis2dw.setup();
    ThisThread::sleep_for(150);

    while (true) {
        ThisThread::sleep_for(10);

        if (d6t_44l.read(&pdta, &buf[0]) == false) {
            continue;
        }
        for (int i = 0; i < 16; i++) {
            uint8_t a = (normalize0to1(buf[i], pdta - TILE_TEMP_MARGIN_UNDER,  pdta + TILE_TEMP_MARGIN_UPPER) * 255.0);
            fbuf_work_0[i]=a;
        }

        printf("\x1b[%d;%dH", 0, 0);  // Move cursor (y , x)
        printf("GR-MANGO x Omron 2JCIE-EV01 Demo\r\n\r\n");

        sht30.read(&humi, &temp32);
        printf("[SHT30-DIS-B] Temperature / humidity sensor\r\n");
        printf("   temperature : %5.2f [degC]\r\n", temp32 / 100.0);
        printf("   humidity    : %5.2f [%%RH]\r\n", humi / 100.0);
        printf("\r\n");

        opt3001.read(&illm);
        printf("[OPT3001DNP] Ambient light sensor\r\n");
        printf("   illuminance : %5.2f [lx]\r\n", illm / 100.0);
        printf("\r\n");

        baro_2smpb.read(&pres, &temp16, &dp, &dt);
        printf("[2SMPB-02E] MEMS digital barometric pressure sensor\r\n");
        printf("   pressure    : %7.1f [Pa] (%08Xh)\r\n", pres / 10.0 , (unsigned int)dp);
        printf("   temperature : %5.2f [degC] (%08Xh)\r\n", temp16 / 100.0, (unsigned int)dt);
        printf("\r\n");

        lis2dw.read(accl);
        printf("[LIS2DW12] MEMS digital motion sensor\r\n");
        printf("   x : %5d [mg]\r\n", CONV_RAW_TO_MG(accl[0]));
        printf("   y : %5d [mg]\r\n", CONV_RAW_TO_MG(accl[1]));
        printf("   z : %5d [mg]\r\n", CONV_RAW_TO_MG(accl[2]));
        printf("\r\n");
    }
}

static void button_fall0(void) {
    display_alpha++;
    if (display_alpha>0xF) {
        display_alpha = 0x0;
    }
}

static void button_fall1(void) {
    display_alpha--;
}

int main(void) {
    // Pre-calculation for speed up
    init_colors();

    // Set up button
    button0.fall(&button_fall0);
    button1.fall(&button_fall1);

    // Start DRP task
    drpTask.start(callback(drp_task));

    // Start sensor task
    sensorTask.start(callback(sensor_task));

    wait(osWaitForever);
}

