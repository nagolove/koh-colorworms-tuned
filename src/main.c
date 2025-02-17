#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS

#include "cimgui.h"
#include "cimgui_impl.h"
#include "koh_console.h"
#include "koh_hashers.h"
#include "koh_hotkey.h"
#include "koh_hotkey.h"
#include "koh_logger.h"
#include "koh_script.h"
#include "raylib.h"
#include "worms_effect.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(PLATFORM_WEB)
#include <emscripten.h>
#endif

#if defined(PLATFORM_WEB)
static const int screen_width = 1920;
static const int screen_height = 1080;
#else
static const int screen_width = 1920;
static const int screen_height = 1080;
#endif

static Camera2D camera = {0};
static HotkeyStorage hk = {0};
static WormsEffect_t effect = NULL;
Texture2D t = {};
int step_count = 30, 
    particles_count = 30;
bool effect_draw = true;

static void init(bool mt) {
    effect = worms_effect_new((WormsEffectInitOpts) { 
        .w = screen_width,
        .h = screen_height,
        .multithreads = mt,
        .step_count = step_count,
        .particles_count = particles_count,
    });
    t = LoadTextureFromImage(*worms_effect_bitmap(effect));
}

static void shutdown() {
    UnloadTexture(t);
    worms_effect_free(effect);
}

static void render_gui() {
    rlImGuiBegin();

    bool open = false;
    igShowDemoWindow(&open);

    bool wnd_open = true;
    ImGuiWindowFlags wnd_flags = ImGuiWindowFlags_AlwaysAutoResize;
    igBegin("tunning", &wnd_open, wnd_flags);

    WormsEffectOpts opts = worms_effect_options_get(effect);

    static bool mt = false;

    igCheckbox("draw effect", &effect_draw);

    if (igButton("recreate", (ImVec2){})) {
        shutdown();
        init(mt);
    }

    igSameLine(0., 10.);

    igCheckbox("openmp", &mt);

    igSliderInt("particles count", &particles_count, 0, 500, "%d", ImGuiSliderFlags_Logarithmic);
    igSliderInt("step count", &step_count, 0, 500, "%d", ImGuiSliderFlags_Logarithmic);

    igSeparator();

    igSliderInt("iteration_count", &opts.iteration_count, 0, 8000, "%d", ImGuiSliderFlags_Logarithmic);
    igSliderInt("glow_size", &opts.glow_size, 0, 1000, "%d", 0);
    igSliderInt("swap_direction_rarity", &opts.swap_direction_rarity, 1, 1000, "%d", 0);
    igSliderInt("blur_iteration_count", &opts.blur_iteration_count, 0, 1000, "%d", 0);
    igSliderInt("recolor_rarity", &opts.recolor_rarity, 1, 1000, "%d", 0);

    igSliderInt2("values", opts.value, 0, 1000, "%d", 0);
    igSliderInt2("glow", opts.glow, 0, 1000, "%d", 0);

    worms_effect_options_set(effect, opts);

    igEnd();

    rlImGuiEnd();
}


static void update_render() {
    ClearBackground(BLACK); 
    BeginDrawing();

    if (effect_draw)
        worms_effect_draw(effect);

    void *data = worms_effect_bitmap(effect)->data;
    UpdateTexture(t, data);
    DrawTexture(t, 0, 0, WHITE);

    render_gui();
    EndDrawing();

    char title[128] = {};
    sprintf(title, "fps %d", GetFPS());
    SetWindowTitle(title);
}

int main(void) {
    // {{{
    koh_hashers_init();
    camera.zoom = 1.0f;
    srand(time(NULL));
    InitWindow(screen_width, screen_height, "color worms");
    SetWindowMonitor(1);
    SetTargetFPS(60);

    rlImGuiSetup(&(struct igSetupOptions) {
            .dark = false,
            .font_path = "assets/djv.ttf",
            .font_size_pixels = 40,
    });

    logger_init();
    sc_init();
    sc_init_script();

    SetTraceLogLevel(LOG_ERROR);

    hotkey_init(&hk);
    console_init(&hk, &(struct ConsoleSetup) {
        .on_enable = NULL,
        .on_disable = NULL,
        .udata = NULL,
        .color_cursor = BLUE,
        .color_text = BLACK,
        .fnt_path = "assets/djv.ttf",
        .fnt_size = 20,
    });
    console_immediate_buffer_enable(true);

    init(false);
#if defined(PLATFORM_WEB)
    emscripten_set_main_loop_arg(update_render, NULL, target_fps, 1);
#else
    while (!WindowShouldClose()) {
        update_render();
    }
#endif
    shutdown();

    rlImGuiShutdown();

    CloseWindow();

    hotkey_shutdown(&hk);
    sc_shutdown();
    logger_shutdown();

    return 0;
}
