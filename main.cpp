#include "imgui.h"
#include "extern/imgui/backends/imgui_impl_glfw.h"
#include "extern/imgui/backends/imgui_impl_opengl3.h"
#include <stdio.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <iostream>
#include "player.h"
#include "GameEngine.h"

using namespace std;

// Forward declare GLFW error callback
static void glfw_error_callback(int error, const char* description);

// Room enum
enum Rooms {
    ROOM_BEDROOM,
    ROOM_KITCHEN,
    ROOM_LIVING,
    ROOM_COUNT
};

int main(int, char**)
{
    // -------------------- GLFW / OpenGL setup --------------------
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Text Adventure - ImGui UI", NULL, NULL);
    if (!window) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // -------------------- ImGui setup --------------------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags = ImGuiConfigFlags_None;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // -------------------- Game objects --------------------
    Player player;
    GameEngine gameEngine;

    Rooms currentRoom = ROOM_BEDROOM;

    // Message log for GUI
    std::string message_log;
    auto push_log = [&](const std::string& s) {
        message_log += s + "\n";
    };

    // ASCII art display
    string currentArt;

    // -------------------- Character creation --------------------
    static char nameBuffer[128] = "";
    static char hairBuffer[64] = "";
    static char eyesBuffer[64] = "";
    static bool playerCreated = false;

    // Inventory buttons need unique IDs
    int inventoryButtonID = 0;

    // -------------------- Main loop --------------------
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // -------------------- Character creation window --------------------
        if (!playerCreated) {
            ImGui::Begin("Character Creation", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::Text("Welcome adventurer! Create your character:");
            ImGui::InputText("Name", nameBuffer, IM_ARRAYSIZE(nameBuffer));
            ImGui::InputText("Hair Color", hairBuffer, IM_ARRAYSIZE(hairBuffer));
            ImGui::InputText("Eye Color", eyesBuffer, IM_ARRAYSIZE(eyesBuffer));

            if (ImGui::Button("Create Player")) {
                player.setName(nameBuffer);
                player.setHair(hairBuffer);
                player.setEyes(eyesBuffer);
                playerCreated = true;
                push_log("Player created!");
            }

            ImGui::End();

            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(0.10f, 0.12f, 0.14f, 1.00f);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window);

            continue; // skip the rest of the loop until player is created
        }

        // -------------------- Rooms panel --------------------
        ImGui::Begin("Rooms", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
        if (ImGui::Button("Bedroom")) currentRoom = ROOM_BEDROOM;
        if (ImGui::Button("Kitchen")) currentRoom = ROOM_KITCHEN;
        if (ImGui::Button("Living Room")) currentRoom = ROOM_LIVING;
        ImGui::End();

        // -------------------- Scene panel --------------------
        ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoCollapse);

        // Bedroom actions
        if (currentRoom == ROOM_BEDROOM) {
            ImGui::TextWrapped("You are in your bedroom.");
            if (ImGui::Button("Lie down on the bed and rest")) {
                player.updateHealth("rest");
                push_log("You rested on the bed. (health updated)");
            }
            if (ImGui::Button("Look in the drawer")) {
                player.addToInventory("Old Key");
                push_log("You found an Old Key and added it to your inventory.");
            }
            if (ImGui::Button("Check out the trophy")) {
                push_log("The trophy is shiny and reminds you of your victories.");
            }
            if (ImGui::Button("Show ASCII Art")) {
                currentArt = gameEngine.printArt("Bedroom");
            }
        }
        // Kitchen actions
        else if (currentRoom == ROOM_KITCHEN) {
            ImGui::TextWrapped("You are in the kitchen. Delicious smells fill the air.");
            if (ImGui::Button("Eat breakfast")) {
                player.updateHealth("breakfast");
                push_log("You ate breakfast. (health updated)");
            }
            if (ImGui::Button("Search cupboards")) {
                player.addToInventory("Cupboard Note");
                push_log("You found a note in the cupboards.");
            }
            if (ImGui::Button("Show ASCII Art")) {
                currentArt = gameEngine.printArt("Kitchen");
            }
        }
        // Living room
        else if (currentRoom == ROOM_LIVING) {
            ImGui::TextWrapped("You are in the living room. A TV sits on a stand.");
            if (ImGui::Button("Inspect TV")) {
                push_log("The TV is blank. Nothing interesting.");
            }
        }

        // Display ASCII Art if available
        if (!currentArt.empty()) {
            ImGui::Separator();
            ImGui::TextUnformatted(currentArt.c_str());
        }

        // Message log
        ImGui::Separator();
        ImGui::TextWrapped("Message Log:");
        ImGui::BeginChild("LogRegion", ImVec2(0,150), true, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::TextUnformatted(message_log.c_str());
        ImGui::EndChild();

        ImGui::End(); // Scene

        // -------------------- Player panel --------------------
        ImGui::Begin("Player", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::Text("Name: %s", player.getName().c_str());
        ImGui::Text("Hair: %s", player.getHair().c_str());
        ImGui::Text("Eyes: %s", player.getEyes().c_str());

        ImGui::Separator();
        ImGui::Text("Inventory:");
        auto inv = player.getInventory();
        for (size_t i = 0; i < inv.size(); ++i) {
            std::string item = inv[i];
            if (item.empty()) item = "(empty)";
            ImGui::Text("%d: %s", (int)i+1, item.c_str());

            // Button to use item
            ImGui::SameLine();
            std::string btnLabel = "Use##" + std::to_string(i);
            if (!inv[i].empty() && ImGui::Button(btnLabel.c_str())) {
                player.checkInventory();
                push_log("Used item: " + inv[i]);
            }
        }

        if (ImGui::Button("Check Health")) {
            player.checkHealth();
            push_log("Checked health (console output).");
        }

        ImGui::End(); // Player

        // -------------------- Rendering --------------------
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.10f, 0.12f, 0.14f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // -------------------- Cleanup --------------------
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