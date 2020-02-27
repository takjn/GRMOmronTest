/**
 * @file lv_objects.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_objects.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/


/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * slider;
static lv_obj_t * chart1;
static lv_obj_t * chart2;
static lv_obj_t * chart3;
static lv_obj_t * chart4;
static lv_obj_t * chart5;
static lv_chart_series_t * dl1;
static lv_chart_series_t * dl2;
static lv_chart_series_t * dl3;
static lv_chart_series_t * dl4;
static lv_chart_series_t * dl5;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

uint8_t get_alpha(void)
{
    return lv_slider_get_value(slider);
}

void set_alpha(uint8_t val)
{
    lv_slider_set_value(slider, val, LV_ANIM_OFF);
}

void chart1_set_next(int16_t val)
{
    lv_chart_set_next(chart1, dl1, val);
}

void chart2_set_next(int16_t val)
{
    lv_chart_set_next(chart2, dl2, val);
}

void chart3_set_next(int16_t val)
{
    lv_chart_set_next(chart3, dl3, val);
}

void chart4_set_next(int16_t val)
{
    lv_chart_set_next(chart4, dl4, val);
}

void chart5_set_next(int16_t val)
{
    lv_chart_set_next(chart5, dl5, val);
}

/**
 * Create some objects
 */
void lv_objects(void)
{
    // // Start LittlevGL theme
    // uint16_t hue = 0;
    // lv_test_theme_1(lv_theme_material_init(hue, NULL));

    /********************
     * CREATE A SCREEN
     *******************/
    /* Create a new screen and load it
     * Screen can be created from any type object type
     * Now a Page is used which is an objects with scrollable content*/
    lv_obj_t * scr = lv_page_create(NULL, NULL);
    lv_disp_load_scr(scr);

    /********************************************************************************
     * LEFT
     ********************************************************************************/
    /****************
     * ADD A SLIDER
     ****************/
    slider = lv_slider_create(scr, NULL);
    lv_obj_set_size(slider, lv_obj_get_width(scr) / 2 - 100, LV_DPI / 3);
    lv_obj_set_x(slider, 50);
    lv_obj_set_y(slider, 480 + 80);
    lv_slider_set_range(slider, 0, 15);
    lv_slider_set_value(slider, 7, false);

    static lv_style_t st;
    lv_style_copy( &st, &lv_style_plain );
    st.text.color = LV_COLOR_BLACK;
    st.text.font = &lv_font_roboto_28;
    lv_obj_t * title = lv_label_create(scr, NULL);
    lv_label_set_text(title, "OMRON sensor evaluation kit on GR-MANGO");
    lv_obj_set_style(title, &st);
    lv_obj_align(title, slider, LV_ALIGN_IN_TOP_LEFT, 0,80);


    /********************************************************************************
     * RIGHT
     ********************************************************************************/

    /****************
     * CREATE A CHART
     ****************/
    chart1 = lv_chart_create(scr, NULL);
    lv_obj_set_x(chart1, 640 + 20);                        /*Set the x coordinate*/
    lv_obj_set_y(chart1, 10);

    lv_obj_set_size(chart1, lv_obj_get_width(scr) / 2 - 60, lv_obj_get_height(scr) / 6);
    lv_chart_set_div_line_count(chart1, 0, 0);
    lv_chart_set_series_width(chart1, 3);
    lv_chart_set_range(chart1, 20, 40);
    lv_chart_set_point_count(chart1, 10);

    /*Add a data series and set some points*/
    dl1 = lv_chart_add_series(chart1, LV_COLOR_BLACK);
    lv_chart_set_next(chart1, dl1, 10);
    lv_chart_set_next(chart1, dl1, 25);
    lv_chart_set_next(chart1, dl1, 45);
    lv_chart_set_next(chart1, dl1, 80);

    lv_obj_t * label1 = lv_label_create(scr, NULL); /*First parameters (scr) is the parent*/
    lv_label_set_text(label1, "[SHT30-DIS-B] Temperature sensor [degC]");
    lv_obj_align(label1, chart1, LV_ALIGN_IN_TOP_LEFT, 10, 10);

    /****************
     * CREATE A CHART
     ****************/
    chart2 = lv_chart_create(scr, NULL);
    lv_obj_set_size(chart2, lv_obj_get_width(scr) / 2 - 60, lv_obj_get_height(scr) / 6);
    lv_obj_align(chart2, chart1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
    lv_chart_set_div_line_count(chart2, 0, 0);
    lv_chart_set_series_width(chart2, 3);
    lv_chart_set_range(chart2, 0, 100);
    lv_chart_set_point_count(chart2, 10);

    /*Add a data series and set some points*/
    dl2 = lv_chart_add_series(chart2, LV_COLOR_BLACK);

    lv_obj_t * label2 = lv_label_create(scr, NULL); /*First parameters (scr) is the parent*/
    lv_label_set_text(label2, "[SHT30-DIS-B] Humidity sensor [%RH]");
    lv_obj_align(label2, chart1, LV_ALIGN_OUT_BOTTOM_LEFT, 10, 30);

    /****************
     * CREATE A CHART
     ****************/
    chart3 = lv_chart_create(scr, NULL);
    lv_obj_set_size(chart3, lv_obj_get_width(scr) / 2 - 60, lv_obj_get_height(scr) / 6);
    lv_obj_align(chart3, chart2, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
    lv_chart_set_div_line_count(chart3, 0, 0);
    lv_chart_set_series_width(chart3, 3);
    lv_chart_set_range(chart3, 0, 500);
    lv_chart_set_point_count(chart3, 10);

    /*Add a data series and set some points*/
    dl3 = lv_chart_add_series(chart3, LV_COLOR_BLACK);

    lv_obj_t * label3 = lv_label_create(scr, NULL); /*First parameters (scr) is the parent*/
    lv_label_set_text(label3, "[OPT3001DNP] Ambient light sensor [lx]");
    lv_obj_align(label3, chart2, LV_ALIGN_OUT_BOTTOM_LEFT, 10, 30);

    /****************
     * CREATE A CHART
     ****************/
    chart4 = lv_chart_create(scr, NULL);
    lv_obj_set_size(chart4, lv_obj_get_width(scr) / 2 - 60, lv_obj_get_height(scr) / 6);
    lv_obj_align(chart4, chart3, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
    lv_chart_set_div_line_count(chart4, 0, 0);
    lv_chart_set_series_width(chart4, 3);
    lv_chart_set_range(chart4, 1000, 1030);
    lv_chart_set_point_count(chart4, 10);

    /*Add a data series and set some points*/
    dl4 = lv_chart_add_series(chart4, LV_COLOR_BLACK);

    lv_obj_t * label4 = lv_label_create(scr, NULL); /*First parameters (scr) is the parent*/
    lv_label_set_text(label4, "[2SMPB-02E] MEMS digital barometric pressure sensor [hPa]");
    lv_obj_align(label4, chart3, LV_ALIGN_OUT_BOTTOM_LEFT, 10, 30);

    /****************
     * CREATE A CHART
     ****************/
    chart5 = lv_chart_create(scr, NULL);
    lv_obj_set_size(chart5, lv_obj_get_width(scr) / 2 - 60, lv_obj_get_height(scr) / 6);
    lv_obj_align(chart5, chart4, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
    lv_chart_set_div_line_count(chart5, 0, 0);
    lv_chart_set_series_width(chart5, 3);
    lv_chart_set_range(chart5, 20, 40);
    lv_chart_set_point_count(chart5, 10);

    /*Add a data series and set some points*/
    dl5 = lv_chart_add_series(chart5, LV_COLOR_BLACK);

    lv_obj_t * label5 = lv_label_create(scr, NULL); /*First parameters (scr) is the parent*/
    lv_label_set_text(label5, "[2SMPB-02E] MEMS digital barometric pressure sensor [degC]");
    lv_obj_align(label5, chart4, LV_ALIGN_OUT_BOTTOM_LEFT, 10, 30);

}

/**********************
 *   STATIC FUNCTIONS
 **********************/
