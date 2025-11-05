// imgui_main.cpp
// A simple Dear ImGui frontend to replace console menus.
// Build with: compile and link GLFW, OpenGL3, and Dear ImGui backends (see instructions below).

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <stdio.h>
#include <GLFW/glfw3.h> // Will drag in OpenGL headers
#include <string>
#include <vector>
#include <array>
#include <iostream>
#include "player.h"
#include "GameEngine.h" // optional; we call printArt if available

// Forward-declare helper
static void glfw_error_callback(int error, const char* description);

enum Rooms {
    ROOM_BEDROOM,
    ROOM_KITCHEN,
    ROOM_LIVING,
    ROOM_COUNT
};

int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Text Adventure - ImGui UI", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    // Enable docking + viewports if you want   
    io.ConfigFlags = ImGuiConfigFlags_None;
    // Setup style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // ----------------- Game objects -----------------
    Player player; // uses your Player class
    GameEngine gameEngine; // optional; if many methods missing we use only implemented ones
    Rooms currentRoom = ROOM_BEDROOM;

    // Example player initial values (you can load save here)
    player.setName("Hero");
    player.setHair("Brown");
    player.setEyes("Green");

    // State for UI
    std::string message_log;
    auto push_log = [&](const std::string &s){
        message_log += s + "\n";
    };

    // Example: if Day1.h previously had bedroom actions, we map them here
    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        glfwPollEvents();

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Layout: left panel (rooms), center panel (scene & actions), right panel (player)
        ImGui::Begin("Rooms", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
        if (ImGui::Button("Bedroom")) currentRoom = ROOM_BEDROOM;
        if (ImGui::Button("Kitchen")) currentRoom = ROOM_KITCHEN;
        if (ImGui::Button("Living Room")) currentRoom = ROOM_LIVING;
        ImGui::End();

        // Center: scene
        ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoCollapse);
        if (currentRoom == ROOM_BEDROOM) {
            ImGui::TextWrapped("You are in your bedroom.");
            ImGui::Separator();
            if (ImGui::Button("Lie down on the bed and rest")) {
                player.updateHealth("rest");
                push_log("You rested on the bed. (health updated)");
            }
            if (ImGui::Button("Look in the drawer")) {
                player.addToInventory("Old Key");
                push_log("You found an Old Key and put it in your inventory.");
            }
            if (ImGui::Button("Check out the trophy")) {
                push_log("The trophy is dusty but you remember winning a contest.");
            }
            // show ASCII/art if GameEngine::printArt is implemented
            if (ImGui::Button("Show ASCII art (if available)")) {
                try {
                    gameEngine.printArt("bedroom"); // will use GameEngine's printArt if implemented
                    push_log("Printed ASCII art (to console).");
                } catch(...) {
                    push_log("printArt not available or failed.");
                }
            }
        }
        else if (currentRoom == ROOM_KITCHEN) {
            ImGui::TextWrapped("You are in the kitchen. There's a table and a breakfast bowl.");
            if (ImGui::Button("Eat breakfast")) {
                player.updateHealth("breakfast");
                push_log("You ate breakfast. (health updated)");
            }
            if (ImGui::Button("Search cupboards")) {
                player.addToInventory("Cupboard Note");
                push_log("You found a note in the cupboards.");
            }
        }
        else if (currentRoom == ROOM_LIVING) {
            ImGui::TextWrapped("You are in the living room. A TV sits on a stand.");
            if (ImGui::Button("Inspect TV")) {
                push_log("The TV is blank — maybe later.");
            }
        }
        ImGui::Separator();
        ImGui::TextWrapped("Message log:");
        ImGui::BeginChild("LogRegion", ImVec2(0,150), true, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::TextUnformatted(message_log.c_str());
        ImGui::EndChild();

        ImGui::End(); // Scene

        // Right: player panel
        ImGui::Begin("Player", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
        char buf[64];
        snprintf(buf, sizeof(buf), "Name: %s", player.getName().c_str());
        ImGui::Text("%s", buf);
        snprintf(buf, sizeof(buf), "Hair: %s", player.getHair().c_str());
        ImGui::Text("%s", buf);
        snprintf(buf, sizeof(buf), "Eyes: %s", player.getEyes().c_str());
        ImGui::Text("%s", buf);

        ImGui::Separator();
        ImGui::Text("Inventory:");
        auto inv = player.getInventory();
        for (size_t i=0;i<inv.size();++i) {
            std::string item = inv[i];
            if (item.empty()) item = "(empty)";
            ImGui::Text("%d: %s", (int)i+1, item.c_str());
            ImGui::SameLine();
            std::string btnLabel = "Use##" + std::to_string(i);
            if (!inv[i].empty() && ImGui::Button(btnLabel.c_str())) {
                // Example: using item triggers check or health update
                player.checkInventory(); // prints info to console currently
                push_log(std::string("Used item: ") + inv[i]);
                // You can implement item-specific logic here
            }
        }

        if (ImGui::Button("Check Health")) {
            player.checkHealth();
            push_log("Checked health (console output).");
        }

        ImGui::End(); // Player

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.10f, 0.12f, 0.14f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}
