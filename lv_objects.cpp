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
static void btn_event_cb(lv_obj_t * btn, lv_event_t event);
static void ddlist_event_cb(lv_obj_t * ddlist, lv_event_t event);

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
    lv_obj_set_y(slider, 480 + 40);
    lv_slider_set_range(slider, 0, 15);
    lv_slider_set_value(slider, 7, false);


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

    // /***********************
    //  * CREATE TWO BUTTONS
    //  ***********************/
    // /*Create a button*/
    // lv_obj_t * btn1 = lv_btn_create(lv_disp_get_scr_act(NULL), NULL);         /*Create a button on the currently loaded screen*/
    // lv_obj_set_event_cb(btn1, btn_event_cb);                                  /*Set function to be called when the button is released*/
    // lv_obj_align(btn1, label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);               /*Align below the label*/

    // /*Create a label on the button (the 'label' variable can be reused)*/
    // label = lv_label_create(btn1, NULL);
    // lv_label_set_text(label, "Button 1");

    // /*Copy the previous button*/
    // lv_obj_t * btn2 = lv_btn_create(scr, btn1);                 /*Second parameter is an object to copy*/
    // lv_obj_align(btn2, btn1, LV_ALIGN_OUT_RIGHT_MID, 50, 0);    /*Align next to the prev. button.*/

    // /*Create a label on the button*/
    // label = lv_label_create(btn2, NULL);
    // lv_label_set_text(label, "Button 2");

    // /***********************
    //  * ADD A DROP DOWN LIST
    //  ************************/
    // lv_obj_t * ddlist = lv_ddlist_create(scr, NULL);                     /*Create a drop down list*/
    // lv_obj_align(ddlist, slider, LV_ALIGN_OUT_RIGHT_TOP, 50, 0);         /*Align next to the slider*/
    // lv_obj_set_top(ddlist, true);                                        /*Enable to be on the top when clicked*/
    // lv_ddlist_set_options(ddlist, "None\nLittle\nHalf\nA lot\nAll");     /*Set the options*/
    // lv_obj_set_event_cb(ddlist, ddlist_event_cb);                        /*Set function to call on new option is chosen*/

    // /****************
    //  * CREATE A CHART
    //  ****************/
    // lv_obj_t * chart = lv_chart_create(scr, NULL);                         /*Create the chart*/
    // lv_obj_set_size(chart, lv_obj_get_width(scr) / 2, lv_obj_get_width(scr) / 4);   /*Set the size*/
    // lv_obj_align(chart, slider, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 50);                   /*Align below the slider*/
    // lv_chart_set_series_width(chart, 3);                                            /*Set the line width*/

    // /*Add a RED data series and set some points*/
    // lv_chart_series_t * dl1 = lv_chart_add_series(chart, LV_COLOR_RED);
    // lv_chart_set_next(chart, dl1, 10);
    // lv_chart_set_next(chart, dl1, 25);
    // lv_chart_set_next(chart, dl1, 45);
    // lv_chart_set_next(chart, dl1, 80);

    // /*Add a BLUE data series and set some points*/
    // lv_chart_series_t * dl2 = lv_chart_add_series(chart, lv_color_make(0x40, 0x70, 0xC0));
    // lv_chart_set_next(chart, dl2, 10);
    // lv_chart_set_next(chart, dl2, 25);
    // lv_chart_set_next(chart, dl2, 45);
    // lv_chart_set_next(chart, dl2, 80);
    // lv_chart_set_next(chart, dl2, 75);
    // lv_chart_set_next(chart, dl2, 505);

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Called when a button is released
 * @param btn pointer to the released button
 * @param event the triggering event
 * @return LV_RES_OK because the object is not deleted in this function
 */
static void btn_event_cb(lv_obj_t * btn, lv_event_t event)
{
    if(event == LV_EVENT_RELEASED) {
        /*Increase the button width*/
        lv_coord_t width = lv_obj_get_width(btn);
        lv_obj_set_width(btn, width + 20);
    }
}

/**
 * Called when a new option is chosen in the drop down list
 * @param ddlist pointer to the drop down list
 * @param event the triggering event
 * @return LV_RES_OK because the object is not deleted in this function
 */
static  void ddlist_event_cb(lv_obj_t * ddlist, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
        uint16_t opt = lv_ddlist_get_selected(ddlist);            /*Get the id of selected option*/

        lv_slider_set_value(slider, (opt * 100) / 4, true);       /*Modify the slider value according to the selection*/
    }

}
