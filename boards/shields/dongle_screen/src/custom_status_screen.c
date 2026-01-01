/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include "custom_status_screen.h"

#if CONFIG_DONGLE_SCREEN_OUTPUT_ACTIVE
#include "widgets/output_status.h"
static struct zmk_widget_output_status output_status_widget;
#endif

#if CONFIG_DONGLE_SCREEN_LAYER_ACTIVE
#include "widgets/layer_status.h"
static struct zmk_widget_layer_status layer_status_widget;
#endif

#if CONFIG_DONGLE_SCREEN_BATTERY_ACTIVE
#include "widgets/battery_status.h"
static struct zmk_widget_dongle_battery_status dongle_battery_status_widget;
#endif

#if CONFIG_DONGLE_SCREEN_WPM_ACTIVE
#include "widgets/wpm_status.h"
static struct zmk_widget_wpm_status wpm_status_widget;
#endif

#if CONFIG_DONGLE_SCREEN_MODIFIER_ACTIVE
#include "widgets/mod_status.h"
static struct zmk_widget_mod_status mod_widget;
#endif

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

lv_style_t global_style;

#define PARTICLE_COUNT 15
#define CANVAS_WIDTH 240
#define CANVAS_HEIGHT 135

struct particle {
    float x;
    float y;
    float vx;
    float vy;
    uint8_t brightness;
};

static struct particle particles[PARTICLE_COUNT];
static lv_obj_t *particle_canvas;
// Use reduced buffer - draw directly on smaller regions
static lv_color_t canvas_buf[120 * 68];
static struct k_timer particle_timer;

static int pseudo_rand(void) {
    static uint32_t seed = 12345;
    seed = (seed * 1103515245 + 12345) & 0x7fffffff;
    return seed;
}

static void init_particles(void) {
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        particles[i].x = (pseudo_rand() % CANVAS_WIDTH);
        particles[i].y = (pseudo_rand() % CANVAS_HEIGHT);
        particles[i].vx = ((pseudo_rand() % 100) - 50) / 100.0f;
        particles[i].vy = ((pseudo_rand() % 100) - 50) / 100.0f;
        particles[i].brightness = 100 + (pseudo_rand() % 156);
    }
}

static void update_particles(void) {
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        particles[i].x += particles[i].vx;
        particles[i].y += particles[i].vy;
        
        if (particles[i].x < 0) {
            particles[i].x = CANVAS_WIDTH;
        } else if (particles[i].x >= CANVAS_WIDTH) {
            particles[i].x = 0;
        }
        
        if (particles[i].y < 0) {
            particles[i].y = CANVAS_HEIGHT;
        } else if (particles[i].y >= CANVAS_HEIGHT) {
            particles[i].y = 0;
        }
    }
}

static void draw_particles(void) {
    lv_canvas_fill_bg(particle_canvas, lv_color_hex(0x0a0a2e), LV_OPA_COVER);
    
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        int x = (int)(particles[i].x / 2);
        int y = (int)(particles[i].y / 2);
        uint8_t bright = particles[i].brightness;
        
        lv_color_t color;
        if (i % 3 == 0) {
            color = lv_color_hex(0x4d80ff);
        } else if (i % 3 == 1) {
            color = lv_color_hex(0x80ffff);
        } else {
            color = lv_color_hex(0xcc66ff);
        }
        
        rect_dsc.bg_color = color;
        rect_dsc.bg_opa = bright;
        
        lv_canvas_draw_rect(particle_canvas, x - 1, y - 1, 3, 3, &rect_dsc);
        
        if (bright > 150) {
            rect_dsc.bg_opa = bright / 2;
            lv_canvas_draw_rect(particle_canvas, x - 2, y - 2, 5, 5, &rect_dsc);
        }
    }
}

static void particle_timer_cb(struct k_timer *timer) {
    update_particles();
    draw_particles();
}

lv_obj_t *zmk_display_status_screen()
{
    lv_obj_t *screen;

    screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x0a0a2e), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(screen, 255, LV_PART_MAIN);

    // Canvas for particle field - half resolution to save RAM
    particle_canvas = lv_canvas_create(screen);
    lv_canvas_set_buffer(particle_canvas, canvas_buf, 120, 68, LV_IMG_CF_TRUE_COLOR);
    lv_obj_set_size(particle_canvas, 240, 135);
    lv_obj_align(particle_canvas, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_border_width(particle_canvas, 0, 0);
    lv_obj_set_style_pad_all(particle_canvas, 0, 0);
    
    // Initialize and start particle system
    init_particles();
    draw_particles();
    
    k_timer_init(&particle_timer, particle_timer_cb, NULL);
    k_timer_start(&particle_timer, K_MSEC(50), K_MSEC(50));

    lv_style_init(&global_style);
    // lv_style_set_text_font(&global_style, &lv_font_unscii_8); // ToDo: Font is not recognized
    lv_style_set_text_color(&global_style, lv_color_white());
    lv_style_set_text_letter_space(&global_style, 1);
    lv_style_set_text_line_space(&global_style, 1);
    lv_obj_add_style(screen, &global_style, LV_PART_MAIN);

#if CONFIG_DONGLE_SCREEN_OUTPUT_ACTIVE
    zmk_widget_output_status_init(&output_status_widget, screen);
    lv_obj_align(zmk_widget_output_status_obj(&output_status_widget), LV_ALIGN_TOP_MID, 0, 10);
#endif

#if CONFIG_DONGLE_SCREEN_BATTERY_ACTIVE
    zmk_widget_dongle_battery_status_init(&dongle_battery_status_widget, screen);
    lv_obj_align(zmk_widget_dongle_battery_status_obj(&dongle_battery_status_widget), LV_ALIGN_BOTTOM_MID, 0, 0);
#endif

#if CONFIG_DONGLE_SCREEN_WPM_ACTIVE
    zmk_widget_wpm_status_init(&wpm_status_widget, screen);
    lv_obj_align(zmk_widget_wpm_status_obj(&wpm_status_widget), LV_ALIGN_TOP_LEFT, 20, 20);
#endif

#if CONFIG_DONGLE_SCREEN_LAYER_ACTIVE
    zmk_widget_layer_status_init(&layer_status_widget, screen);
    lv_obj_align(zmk_widget_layer_status_obj(&layer_status_widget), LV_ALIGN_CENTER, 0, 0);
#endif

#if CONFIG_DONGLE_SCREEN_MODIFIER_ACTIVE
    zmk_widget_mod_status_init(&mod_widget, screen);
    lv_obj_align(zmk_widget_mod_status_obj(&mod_widget), LV_ALIGN_CENTER, 0, 35);
#endif

    return screen;
}