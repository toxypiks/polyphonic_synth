#include <stdio.h>
#include <jack/ringbuffer.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "oscillator.h"
#include "ray_out_buffer.h"
#include "jack_stuff.h"
#include "ui_stuff.h"
#include "ffmpeg_stuff.h"
#include "thread_stuff.h"
#include "model_gen_signal.h"
#include "adsr.h"
#include "lf_queue.h"
#include "msg_handler.h"
#include "midi_msg.h"
#include "adsr_display_handler.h"
#include "stb_ds.h"

int main(void) {

    FfmpegStuff ffmpeg_stuff = {0};
    ffmpeg_stuff.enable = false;
    ffmpeg_stuff.fps = 60;

    size_t window_factor = 80;
    size_t screen_width = (16*window_factor);
    size_t screen_height = (9*window_factor);

    int ret = ffmpeg_start_rendering(&ffmpeg_stuff, screen_width, screen_height);
    if (ret != 0) {
        return -1;
    }

    // adsr view Ui stuff and model

    JackStuff* jack_stuff = create_jack_stuff("SineWaveWithJack", 192000);
    ThreadStuff* thread_stuff = create_thread_stuff(jack_stuff);

    // start model thread
    pthread_t model_gen_signal_thread;
    pthread_create(&model_gen_signal_thread, NULL, model_gen_signal_thread_fct, (void*) thread_stuff);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screen_width, screen_height, "sine_wave");
    SetTargetFPS(ffmpeg_stuff.fps);
    RayOutBuffer ray_out_buffer = create_ray_out_buffer(10000);

    UiStuff* ui_stuff = create_ui_stuff(screen_width, screen_height);
    LayoutStack ls = {0};

    bool is_virt_keyboard_on = false;
    bool is_virt_keyboard_on_prev = false;
    bool is_reset_pressed = false;

    bool is_right_oct_pressed = false;
    bool is_left_oct_pressed = false;

    int octave = 3;

    MsgHdl msg_hdl = {0};
    // TODO somehing like a hashmap for multiple MidiMsgs
    // improve set_midi_msg
    MidiMsg midi_msg_in = {0};
    ADSRDisplayHandler adsr_display_handler = {.adsr_display_map = NULL};
    float* adsr_heights = NULL;
    float* adsr_lengths = NULL;
    // TODO set capicity

    msg_hdl_add_key2fct(&msg_hdl, "adsr_display_msg", set_adsr_display_wrapper, (void*)&adsr_display_handler);
    msg_hdl_add_key2fct(&msg_hdl, "midi_msg", set_midi_msg, (void*)&midi_msg_in);

    size_t virt_keyboard_key = 0;
    size_t virt_keyboard_key_prev = 0;
    float virt_keyboard_freq = 0;

    float custom_color[4] = {1.0, 0.0, 1.0, 0.4};

    while(!WindowShouldClose()) {
        msg_hdling(&msg_hdl, &thread_stuff->raylib_msg_queue);
        print_adsr_display_hash_map(&adsr_display_handler);

        // copy adsr display msgs to dynamic array

        int adsr_display_map_len = hmlen(adsr_display_handler.adsr_display_map);
        arrfree(adsr_lengths);
        arrfree(adsr_heights);
        if(adsr_display_map_len > 0) {
            for (size_t i = 0; i < adsr_display_map_len; ++i) {
                arrpush(adsr_lengths, adsr_display_handler.adsr_display_map[i].value.adsr_length);
                arrpush(adsr_heights, adsr_display_handler.adsr_display_map[i].value.adsr_height);
            }
        }

        // TODO ~Setter for text ->better update for ui_stuff
        // TODO Seperate value for label from actual parameter for change frequency
        virt_keyboard_freq = 440.0 * pow(2.0, (virt_keyboard_key - 9.0)/12.0);
        // ui_stuff->text.freq = 50.0 + 1000.0 * ui_stuff->slider_freq.scroll;
        ui_stuff->text.freq = virt_keyboard_freq;
        ui_stuff->text.vol = 1.0 * ui_stuff->slider_vol.scroll;

        ADSR adsr_msg = {
            .attack = ui_stuff->adsr.attack.scroll,
            .decay = ui_stuff->adsr.decay.scroll,
            .sustain = ui_stuff->adsr.sustain.scroll,
            .release = ui_stuff->adsr.release.scroll,
        };

        // send messages through msg_queue
        int ret_adsr = lf_queue_push(&thread_stuff->model_msg_queue, "adsr", (void*)&adsr_msg, sizeof(ADSR));

        int ret_vol = lf_queue_push(&thread_stuff->model_msg_queue, "vol", (void*)&ui_stuff->text.vol, sizeof(float));

        if (is_virt_keyboard_on != is_virt_keyboard_on_prev) {
            MidiMsg midi_msg_out = {
                .key  = is_virt_keyboard_on ? virt_keyboard_key + octave*12
                                            : virt_keyboard_key_prev + octave*12,
                .vel   = 1.0f,
                .is_on = is_virt_keyboard_on,
                .time_stamp = 0
            };
            int ret_midi_msg = lf_queue_push(&thread_stuff->model_msg_queue, "midi_msg", (void*)&midi_msg_out, sizeof(MidiMsg));
        }

        if (jack_stuff->ringbuffer_video) {
            float output_buffer[1024];
            size_t num_bytes = jack_ringbuffer_read_space(jack_stuff->ringbuffer_video);

            if (num_bytes >= (1024* sizeof(float))) {
                jack_ringbuffer_read(jack_stuff->ringbuffer_video, (char*)output_buffer, 1024 * sizeof(float));
                copy_to_ray_out_buffer(&ray_out_buffer, output_buffer, 1024);
            }
        }

        float w = GetRenderWidth();
        float h = GetRenderHeight();
        SetShaderValue(ui_stuff->adsr.rec_shader,
                       ui_stuff->adsr.rec_shader_color_param_loc,
                       custom_color, SHADER_UNIFORM_VEC4);
        SetShaderValue(ui_stuff->adsr.circ_shader,
                       ui_stuff->adsr.circ_shader_color_param_loc,
                       custom_color, SHADER_UNIFORM_VEC4);

        BeginDrawing();
        // TODO check record toggle on
        BeginTextureMode(ui_stuff->screen);
        ClearBackground(BLACK);

        layout_stack_push(&ls, LO_VERT, ui_rect(0, 0, w, h), 3, 0);
        layout_stack_push(&ls, LO_HORZ, layout_stack_slot(&ls), 3, 0);
        reset_button_widget(layout_stack_slot(&ls), PINK, &is_reset_pressed);
        text_widget(layout_stack_slot(&ls), &ui_stuff->text);
        // reset_button_widget(layout_stack_slot(&ls), PINK, &is_reset_pressed );
        oct_trans_button_widget(layout_stack_slot(&ls), &octave, &is_left_oct_pressed, &is_right_oct_pressed);
        layout_stack_pop(&ls);
        signal_widget(layout_stack_slot(&ls), &ray_out_buffer, BLUE);
        layout_stack_push(&ls, LO_HORZ, layout_stack_slot(&ls),3,0);

        adsr_widget(layout_stack_slot(&ls), &ui_stuff->adsr, adsr_heights, adsr_lengths, adsr_display_map_len);
        is_virt_keyboard_on_prev = is_virt_keyboard_on;
        virt_keyboard_key_prev = virt_keyboard_key;

        // TODO translate to poly midi msgs
        size_t dummy_key;
        bool dummy_pressed;
        size_t dummy_key_in = 0;
        bool dummy_pressed_in = false;
        octave_widget(layout_stack_slot(&ls),
                      &virt_keyboard_key,
                      &is_virt_keyboard_on,
                      dummy_key_in,
                      dummy_pressed_in);

        slider_widget(layout_stack_slot(&ls), &ui_stuff->slider_vol);

        layout_stack_pop(&ls);
        layout_stack_pop(&ls);
        EndTextureMode();

        Vector2 pos_rect = {0,0};
        Rectangle flip_rect = {0, 0, screen_width, - 1*(int)screen_height};

        DrawTextureRec(ui_stuff->screen.texture,
        flip_rect,
        pos_rect,
        WHITE);

        EndDrawing();
        assert(ls.count == 0);

        Image image = LoadImageFromTexture(ui_stuff->screen.texture);
        ffmpeg_send_frame(&ffmpeg_stuff, image.data, screen_width, screen_height);
        // TODO check if screen_width and screen_height can change <- window resizable

        UnloadImage(image);

        if (is_reset_pressed) {
            ui_stuff->adsr.attack.scroll = 0.05f;
            ui_stuff->adsr.decay.scroll = 0.25f;
            ui_stuff->adsr.sustain.scroll = 0.5f;
            ui_stuff->adsr.release.scroll = 0.2f;
        }
        // TODO clean adsr_display_handler
    }
    layout_stack_delete(&ls);
    CloseWindow();
    ffmpeg_end_rendering(&ffmpeg_stuff);
    arrfree(adsr_lengths);
    arrfree(adsr_heights);
    // TODO free adsr_display_handler

    thread_stuff->is_running = false;
    pthread_join(model_gen_signal_thread, NULL);

    ui_stuff_clear(ui_stuff);
    jack_stuff_clear(jack_stuff);
    return 0;
}
