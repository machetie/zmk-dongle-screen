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

static void bg_opacity_anim_cb(void *var, int32_t value)
{
    lv_obj_set_style_bg_opa(var, value, 0);
}

lv_obj_t *zmk_display_status_screen()
{
    lv_obj_t *screen;

    screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(screen, 255, LV_PART_MAIN);

    // Base gradient layer - Static deep blue
    lv_obj_t *bg_base = lv_obj_create(screen);
    lv_obj_set_size(bg_base, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(bg_base, lv_color_hex(0x0a0a2e), 0);
    lv_obj_set_style_bg_opa(bg_base, LV_OPA_COVER, 0);
    lv_obj_clear_flag(bg_base, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_width(bg_base, 0, 0);
    lv_obj_set_style_pad_all(bg_base, 0, 0);
    lv_obj_set_style_radius(bg_base, 0, 0);

    // Glow layer 1 - Purple glow
    lv_obj_t *glow1 = lv_obj_create(screen);
    lv_obj_set_size(glow1, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(glow1, lv_color_hex(0x4d0080), 0);
    lv_obj_set_style_bg_opa(glow1, 40, 0);
    lv_obj_clear_flag(glow1, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_width(glow1, 0, 0);
    lv_obj_set_style_pad_all(glow1, 0, 0);
    lv_obj_set_style_radius(glow1, 0, 0);

    // Glow layer 2 - Blue glow
    lv_obj_t *glow2 = lv_obj_create(screen);
    lv_obj_set_size(glow2, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(glow2, lv_color_hex(0x0066cc), 0);
    lv_obj_set_style_bg_opa(glow2, 30, 0);
    lv_obj_clear_flag(glow2, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_width(glow2, 0, 0);
    lv_obj_set_style_pad_all(glow2, 0, 0);
    lv_obj_set_style_radius(glow2, 0, 0);

    // Glow layer 3 - Cyan glow
    lv_obj_t *glow3 = lv_obj_create(screen);
    lv_obj_set_size(glow3, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(glow3, lv_color_hex(0x00cccc), 0);
    lv_obj_set_style_bg_opa(glow3, 20, 0);
    lv_obj_clear_flag(glow3, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_width(glow3, 0, 0);
    lv_obj_set_style_pad_all(glow3, 0, 0);
    lv_obj_set_style_radius(glow3, 0, 0);

    // Smooth opacity animation for glow1 (purple) - slowest
    lv_anim_t anim1;
    lv_anim_init(&anim1);
    lv_anim_set_var(&anim1, glow1);
    lv_anim_set_values(&anim1, 20, 60);
    lv_anim_set_time(&anim1, 3500);
    lv_anim_set_exec_cb(&anim1, bg_opacity_anim_cb);
    lv_anim_set_path_cb(&anim1, lv_anim_path_ease_in_out);
    lv_anim_set_repeat_count(&anim1, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_playback_time(&anim1, 3500);
    lv_anim_start(&anim1);

    // Smooth opacity animation for glow2 (blue) - medium speed, phase offset
    lv_anim_t anim2;
    lv_anim_init(&anim2);
    lv_anim_set_var(&anim2, glow2);
    lv_anim_set_values(&anim2, 15, 50);
    lv_anim_set_time(&anim2, 4000);
    lv_anim_set_exec_cb(&anim2, bg_opacity_anim_cb);
    lv_anim_set_path_cb(&anim2, lv_anim_path_ease_in_out);
    lv_anim_set_repeat_count(&anim2, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_playback_time(&anim2, 4000);
    lv_anim_set_playback_delay(&anim2, 1000);
    lv_anim_start(&anim2);

    // Smooth opacity animation for glow3 (cyan) - fastest, different phase
    lv_anim_t anim3;
    lv_anim_init(&anim3);
    lv_anim_set_var(&anim3, glow3);
    lv_anim_set_values(&anim3, 10, 40);
    lv_anim_set_time(&anim3, 3000);
    lv_anim_set_exec_cb(&anim3, bg_opacity_anim_cb);
    lv_anim_set_path_cb(&anim3, lv_anim_path_ease_in_out);
    lv_anim_set_repeat_count(&anim3, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_playback_time(&anim3, 3000);
    lv_anim_set_playback_delay(&anim3, 2000);
    lv_anim_start(&anim3);

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