#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

#include <algorithm>
#include <cmath>
#include <iostream>

#include "board.h"
#define WINDOW_SIZE  1000
#define COLOR_YELLOW 0xff00ffff
#define COLOR_RED    0xffff0000
#define COLOR_BLUE   0xff0000ff
#define COLOR_GRAY   0xff808080
#define COLOR_GREEN  0xff00ff00

int main(int, char**)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif
    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("Dear ImGui SDL2+OpenGL3 example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_SIZE, WINDOW_SIZE, window_flags);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    // Main loop
    bool done = false;

    board_c board;
    int debut_move = 0;

    while (!done) {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        
        // start render Ugolki
        float w = ImGui::GetIO().DisplaySize.x;
        float h = ImGui::GetIO().DisplaySize.y;
        float dim = MIN(w, h) - 6;
        ImGui::SetNextWindowPos(ImVec2(3, 3), ImGuiCond_Always);
        ImGui::SetNextWindowSizeConstraints(ImVec2(dim, dim), ImVec2(dim, dim));

        // main window
        if (ImGui::Begin("Ugolki", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar)) {
            ImVec2 pos = ImGui::GetCursorScreenPos();
            ImVec2 size = ImGui::GetContentRegionAvail();
            
            float cell_sz = size.x / BOARD_DIM;
            if (ImGui::BeginChild("Canvas")) {
                for (int i = 0; i < BOARD_DIM; i++) {
                    for (int j = 0; j < BOARD_DIM; j++) {
                        // the grid
                        ImGui::GetWindowDrawList()->AddRect(ImVec2(pos.x + j * cell_sz, pos.y + i * cell_sz),
                            ImVec2(pos.x + (j + 1) * cell_sz, pos.y + (i + 1) * cell_sz), COLOR_YELLOW);
                        // checkers
                        if (board.checkers[i][j] == 1)
                            ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(pos.x + (j + 0.5) * cell_sz, pos.y + (i + 0.5) * cell_sz), 0.4 * cell_sz, COLOR_RED);
                        if (board.checkers[i][j] == -1)
                            ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(pos.x + (j + 0.5) * cell_sz, pos.y + (i + 0.5) * cell_sz), 0.4 * cell_sz, COLOR_BLUE);
                    }
                }

                static bool gamer1_move = true;
                if (gamer1_move) {
                    if (debut_move++ < DEBUT_MOVES) {                        
                        board.debut(debut_move > 6, false, false);
                        std::cout << debut_move << "\n";
                    }
                    else {
                        if (!board.move())
                            board.debut(true, true, true);
                    }
                    gamer1_move = false;
                } else {
                    ImVec2 mouse_pos = ImGui::GetMousePos();
                    static bool mouse_drag = false;
                    static int over_j = -1, over_i = -1;
                    if (!mouse_drag) {
                        over_j = floor((mouse_pos.x - pos.x) / cell_sz);
                        over_i = floor((mouse_pos.y - pos.y) / cell_sz);
                    }
                    bool hover = over_j >= 0 && over_j < BOARD_DIM && over_i >= 0 && over_i < BOARD_DIM && 
                       board.checkers[over_i][over_j] == -1 &&
                       (over_i - 1 >= 0 && over_i - 1 < BOARD_DIM && !board.checkers[over_i - 1][over_j] ||
                        over_i + 1 >= 0 && over_i + 1 < BOARD_DIM && !board.checkers[over_i + 1][over_j] ||
                        over_j - 1 >= 0 && over_j - 1 < BOARD_DIM && !board.checkers[over_i][over_j - 1] ||
                        over_j + 1 >= 0 && over_j + 1 < BOARD_DIM && !board.checkers[over_i][over_j + 1]);
                    if (hover)
                        ImGui::GetWindowDrawList()->AddCircle(ImVec2(pos.x + (over_j + 0.5) * cell_sz, pos.y + (over_i + 0.5) * cell_sz), 0.4 * cell_sz, COLOR_GRAY, 0, 3);

                    static int over_jj = -1, over_ii = -1;
                    if (hover && ImGui::IsMouseDown(0)) {
                        over_jj = floor((mouse_pos.x - pos.x) / cell_sz);
                        over_ii = floor((mouse_pos.y - pos.y) / cell_sz);
                        if (over_jj >= 0 && over_jj < BOARD_DIM && over_ii >= 0 && over_ii < BOARD_DIM && (over_jj == over_j && over_ii == over_i
                            || (!board.checkers[over_ii][over_jj] 
                            && (abs(over_jj - over_j) == 1 && abs(over_ii - over_i) == 0 || abs(over_jj - over_j) == 0 && abs(over_ii - over_i) == 1)))) {
                            ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(mouse_pos.x, mouse_pos.y), 0.4 * cell_sz, COLOR_GREEN);
                            mouse_drag = true;
                        }
                        else
                            mouse_drag = false;
                    }
                    if (!ImGui::IsMouseDown(0)) {
                        if (mouse_drag && (over_jj != over_j || over_ii != over_i)) {
                            board.set_move2(over_i, over_j, over_ii, over_jj);
                            gamer1_move = true;
                            over_j = over_i = over_jj = over_ii = -1;
                        }
                        mouse_drag = false;
                    }
                }                
            }
            ImGui::EndChild();
        }
        ImGui::End();

        // Rendering
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
