#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <stdio.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <iostream>
#include "player.h"

// Forward-declare helper
static void glfw_error_callback(int error, const char* description);

// ----------------- Scene Enum -----------------
enum Scene {
    SCENE_CHARACTER_SETUP,
    SCENE_BEDROOM,
    SCENE_MIRROR,
    SCENE_TROPHY,
    SCENE_KITCHEN,
    SCENE_KITCHEN_CABINET,
    SCENE_END
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
    glfwSwapInterval(1);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags = ImGuiConfigFlags_None;
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // ----------------- Game objects -----------------
    Player player;
    Scene currentScene = SCENE_CHARACTER_SETUP;

    // Message log
    std::string message_log;
    auto push_log = [&](const std::string &s){
        message_log += s + "\n";
    };

    // Temporary character input buffers
    char nameBuf[64] = "";
    char hairBuf[32] = "";
    char eyesBuf[32] = "";

    // Flags
    bool characterCreated = false;

    // ----------------- Main loop -----------------
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Adventure");

        // ----------------- Scenes -----------------
        switch(currentScene)
        {
            // -------- Character Setup --------
            case SCENE_CHARACTER_SETUP:
                ImGui::TextWrapped("Welcome adventurer! Let's set up your character.");

                ImGui::InputText("Name", nameBuf, sizeof(nameBuf));
                ImGui::InputText("Hair Color", hairBuf, sizeof(hairBuf));
                ImGui::InputText("Eye Color", eyesBuf, sizeof(eyesBuf));

                if (ImGui::Button("Confirm Character") && strlen(nameBuf) > 0) {
                    player.setName(nameBuf);
                    player.setHair(hairBuf);
                    player.setEyes(eyesBuf);
                    push_log("Character created: " + player.getName() + " (" + player.getHair() + " hair, " + player.getEyes() + " eyes)");
                    currentScene = SCENE_BEDROOM;
                }
                break;

            // -------- Bedroom --------
            case SCENE_BEDROOM:
                ImGui::TextWrapped("You are in your bedroom.");
                if (ImGui::Button("Lie down on the bed and rest")) {
                    player.updateHealth("rest");
                    push_log("You rested on the bed. (health updated)");
                }
                if (ImGui::Button("Look in the drawer")) {
                    currentScene = SCENE_MIRROR;
                    push_log("You see a small pocket mirror in the drawer...");
                }
                if (ImGui::Button("Check out the trophy")) {
                    currentScene = SCENE_TROPHY;
                }
                if (ImGui::Button("Go to the kitchen")) {
                    currentScene = SCENE_KITCHEN;
                    push_log("You run to the kitchen, ready for a fresh meal.");
                }
                break;

            // -------- Mirror --------
            case SCENE_MIRROR:
                ImGui::TextWrapped("You see a familiar face in the mirror...");
                if (ImGui::Button("Take a look")) {
                    player.addToInventory("mirror");
                    push_log("You took the mirror and added it to your inventory.");
                }
                if (ImGui::Button("Ignore mirror")) {
                    push_log("You leave the mirror for now.");
                }
                if (ImGui::Button("Return to bedroom")) {
                    currentScene = SCENE_BEDROOM;
                }
                break;

            // -------- Trophy --------
            case SCENE_TROPHY:
                ImGui::TextWrapped(("The trophy has your name: " + player.getName()).c_str());
                if (ImGui::Button("Return to bedroom")) currentScene = SCENE_BEDROOM;
                break;

            // -------- Kitchen --------
            case SCENE_KITCHEN:
                ImGui::TextWrapped("You are in the kitchen. Breakfast is ready!");
                if (ImGui::Button("Eat breakfast")) {
                    player.updateHealth("breakfast");
                    push_log("You ate breakfast. (health updated)");
                }
                if (ImGui::Button("Check mom's hand")) {
                    push_log("Mom's hand is bandaged after touching broken glass.");
                }
                if (ImGui::Button("Go to cabinet")) {
                    currentScene = SCENE_KITCHEN_CABINET;
                }
                if (ImGui::Button("Leave kitchen")) {
                    push_log("You finish breakfast and get ready for the day.");
                    currentScene = SCENE_END;
                }
                break;

            // -------- Kitchen Cabinet --------
            case SCENE_KITCHEN_CABINET:
                ImGui::TextWrapped("You see: cornstarch, syrup, salt, cookie");
                if (ImGui::Button("Grab cornstarch")) push_log("You put cornstarch back.");
                if (ImGui::Button("Grab syrup")) { player.addToInventory("syrup"); push_log("Added syrup to inventory."); }
                if (ImGui::Button("Grab salt")) push_log("You put salt back.");
                if (ImGui::Button("Grab cookie")) { player.addToInventory("cookie"); push_log("Added cookie to inventory."); }
                if (ImGui::Button("Return to kitchen")) currentScene = SCENE_KITCHEN;
                break;

            // -------- End --------
            case SCENE_END:
                ImGui::TextWrapped("Day 1 has concluded. To be continued...");
                if (ImGui::Button("Restart Adventure")) {
                    currentScene = SCENE_CHARACTER_SETUP;
                    player.clearInventory();
                    message_log.clear();
                    memset(nameBuf, 0, sizeof(nameBuf));
                    memset(hairBuf, 0, sizeof(hairBuf));
                    memset(eyesBuf, 0, sizeof(eyesBuf));
                }
                break;
        }

        // ----------------- Player Panel -----------------
        ImGui::Separator();
        ImGui::Text("Player Info:");
        ImGui::Text("Name: %s", player.getName().c_str());
        ImGui::Text("Hair: %s", player.getHair().c_str());
        ImGui::Text("Eyes: %s", player.getEyes().c_str());

        ImGui::Separator();
        ImGui::Text("Inventory:");
        auto inv = player.getInventory();
        for (size_t i=0; i<inv.size(); ++i) {
            std::string item = inv[i];
            if (item.empty()) item = "(empty)";
            ImGui::Text("%d: %s", (int)i+1, item.c_str());
            ImGui::SameLine();
            std::string btnLabel = "Use##" + std::to_string(i);
            if (!inv[i].empty() && ImGui::Button(btnLabel.c_str())) {
                push_log("Used item: " + inv[i]);
                // add item-specific effects here
            }
        }

        ImGui::Separator();
        ImGui::TextWrapped("Message log:");
        ImGui::BeginChild("LogRegion", ImVec2(0,150), true, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::TextUnformatted(message_log.c_str());
        ImGui::EndChild();

        ImGui::End(); // Adventure window

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
