#include "extern/imgui/imgui.h"
#include "extern/imgui/backends/imgui_impl_glfw.h"
#include "extern/imgui/backends/imgui_impl_opengl3.h"
#include <stdio.h>
#include "glfw/include/GLFW/glfw3.h"
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include "player.h"
#include "GameEngine.h"
#include "Items.h"
#include "PowerMeterMinigame.h"

using namespace ImGui;


// Forward declare GLFW error callback
static void glfw_error_callback(int error, const char* description);

// Room enum
enum Rooms {
    ROOM_BEDROOM,
    ROOM_KITCHEN,
    ROOM_LIVING,
};

/*enum BedroomChoice
{   BedroomChoice_Default,  
    BedroomChoice_Mirror
};*/

int main(int, char**)
{
    //GLFW/OpenGL setup
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return 1;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Text Adventure", NULL, NULL);
    if (!window) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    //ImGui setup
    IMGUI_CHECKVERSION();
    CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags = ImGuiConfigFlags_None;
    StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    //Game objects
    GameEngine &gameEngine = GameEngine::getInstance();
    Player* playerPtr = gameEngine.getPlayer();
    Player& player = *playerPtr;
    player.takeDamage(1); //starts the player with one health down

    Rooms currentRoom = ROOM_BEDROOM;
    //BedroomChoice bdrChoice = BedroomChoice_Default;
    static bool showStorageWindow = false;
    static string currentRoomName = "";
    static string currentStorageLocation = "";

    // Message log for GUI
    std::string message_log;
    auto push_log = [&](const std::string& s) 
    {
        message_log += s + "\n";
    };

    // ASCII art display
    std::string currentArt;

    // -------------------- Buffers and Trackers --------------------
    static char nameBuffer[128] = "";
    static char hairBuffer[64] = "";
    static char eyesBuffer[64] = "";
    static char saveFileBuffer[64] = "";
    static bool playerCreated = false;
    static bool titleScreenDone = false;
    static bool saveClicked = false;
    static bool quitClicked = false;
    bool powerGameActive = false; 
    PowerMeterMinigame powerGame;
    bool powerGameFinished = false;
    bool powerGameWin = false;

    //Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        NewFrame();
        //Title screen
        if(!titleScreenDone) //if title scene is not done
        {
            ImGui::SetNextWindowSize(ImVec2(900, 500), ImGuiCond_Always); //makes window larger
            Begin("Title Screen");
            currentArt = gameEngine.printArt("TitleScreen"); //sets up ascii art 

            if (!currentArt.empty())
            {
                Separator();
                float textWidth = ImGui::CalcTextSize(currentArt.c_str()).x; //gets art width
                float windowWidth = ImGui::GetWindowSize().x; //gets window width
                //centers the art
                float cursorPosX = (windowWidth - textWidth) * 0.5f;
                SetCursorPosX(cursorPosX);
                TextUnformatted(currentArt.c_str()); //shows art
            }

            float windowWidth = ImGui::GetWindowSize().x;
            float buttonWidth = ImGui::CalcTextSize("Start Game").x + ImGui::GetStyle().FramePadding.x * 2;
            //centers start game button
            float posX = (windowWidth - buttonWidth) * 0.5f; 
            SetCursorPosX(posX);

            if(Button("Start Game"))
            {
                titleScreenDone = true;
            }
            float buttonWidth2 = ImGui::CalcTextSize("Start Game").x + ImGui::GetStyle().FramePadding.x * 2; //gets button width
            //centers load game button
            float posX2 = (windowWidth - buttonWidth2) * 0.5f;
            ImGui::SetCursorPosX(posX2);
            if(Button("Load Game"))
            {
                //call loading function
                //gameEngine.loadGame("save.txt");
            }
            End();

            Render();
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(0.10f, 0.12f, 0.14f, 1.00f);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window);
            continue;
        }
        // -------------------- Character creation window --------------------
        if (!playerCreated && titleScreenDone) 
        {
            Begin("Character Creation", nullptr, ImGuiWindowFlags_AlwaysAutoResize); //creates window that autoresizes
            Text("Welcome adventurer! Create your character:"); //outputs text
            InputText("Name", nameBuffer, IM_ARRAYSIZE(nameBuffer)); //creates buffer for name
            InputText("Hair Color", hairBuffer, IM_ARRAYSIZE(hairBuffer)); //creates buffer for hair
            InputText("Eye Color", eyesBuffer, IM_ARRAYSIZE(eyesBuffer)); //creates buffer for eye color

            if (Button("Create Player")) //when player clicks create player button
            {
                player.setName(nameBuffer);
                player.setHair(hairBuffer);
                player.setEyes(eyesBuffer);
                playerCreated = true;
                push_log("Player created!");
            }
            End();

            Render();
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(0.10f, 0.12f, 0.14f, 1.00f);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window);
            continue; // skip the rest of the loop until player is created
        }

        
//-------------------Main Scene Window---------------------------
        Begin("Scene", nullptr, ImGuiWindowFlags_NoCollapse);

        switch (currentRoom)
        {
            case ROOM_BEDROOM:
            {
                TextWrapped("You are in your bedroom. It is a simple room with a bed and a nightstand.\nOn top of the nightstand is a trophy.");
                currentArt = gameEngine.printArt("Bedroom");

                // Track actions
                static bool rested = false;
                static bool checkedDrawer = false;
                static bool checkedTrophy = false;
                static bool mirrorScene = false;

                if (!rested && Button("Lie down on the bed and rest")) 
                {
                    player.heal(1);
                    push_log("You rested on the bed. (health updated)");
                    //update health here
                    rested = true;
                }

                if (!checkedDrawer && !mirrorScene && Button("Look in the drawer")) 
                {
                    currentStorageLocation = "Drawer";
                    currentRoomName = "Bedroom";
                    gameEngine.lookInStorageLocation(currentRoomName, currentStorageLocation, showStorageWindow, message_log);
                }

                if (mirrorScene) 
                {
                    if (Button("Yes")) 
                    {
                        push_log("\nYou open the mirror and see your " + player.getEyes() + " eyes staring back at you.\nYou decide to hold on to the mirror.\nIt could come in handy.\n");
                        gameEngine.pickUpItem("Bedroom", "Drawer", "mirror");
                        //itemWindow = true;
                        mirrorScene = false;
                        checkedDrawer = true;
                    }
                    if (Button("No")) 
                    {
                        push_log("You decide to ignore the mirror and close the drawer.");
                        mirrorScene = false;
                        checkedDrawer = true;
                    }
                }

                if (!checkedTrophy && !mirrorScene && Button("Check out the trophy")) 
                {
                    push_log("The trophy is shiny and reminds you of your victories. " + player.getName() + " is printed on it.");
                    //add new window with trophy ascii art

                    checkedTrophy = true;
                }


                // move to kitchen scene
                if (rested && checkedDrawer && checkedTrophy) 
                {
                    push_log("\n>> Mom: Sweetie! Breakfeast is ready!\n");
                    push_log("She has always called you that. It's a little embarrassing, but mostly endearing\n");
                    currentRoom = ROOM_KITCHEN;
                }
                break;
            }

            case ROOM_KITCHEN:
            {
                TextWrapped("\nYou run to the kitchen, ready for a fresh meal.\n");
                currentArt = gameEngine.printArt("Kitchen");
                //push_log("Your mom turns around with a plate of waffles drenched in syrup.\n");
                //push_log("As your mom sets the plate on the dining table you take a notice of her right hand.\nIt's bandaged up.\n");


                //track choices
                static bool ateBreakfast = false;
                static bool searchedCupboards = false;
                static bool askedAboutHand = false;

                if (!ateBreakfast && Button("Eat breakfast")) 
                {
                    //player.updateHealth("breakfast");
                    push_log("You ate breakfast. (health updated)");
                    ateBreakfast = true;
                }
                if (!askedAboutHand && Button("Ask your mom about her hand")) 
                {
                    push_log(player.getName() + ": What happened to your hand mom?\n");
                    askedAboutHand = true;
                }
//-------------------------POWER METER TEST-------------------------------
                if (Button("POWER METER GAME (FOR TESTING ONLY)"))
                {
                    powerGameActive = true;
                    powerGameFinished = false;
                    powerGame = PowerMeterMinigame();
                }

                if (powerGameActive && !powerGameFinished)
                {
                    Begin("Power Meter");

                    // Update the cursor position each frame
                    powerGame.update();

                    // Draw the meter bar
                    Text("=== AUXILIARY POWER CONTROL ===");
                    Text("Goal: stop the cursor (^) inside the '=' zone.");

                    std::string meter;
                    for (int i = 0; i < powerGame.meterWidth; ++i)
                    {
                        meter += (i >= powerGame.targetZoneStart && i < powerGame.targetZoneEnd) ? "=" : "-";

                    }
                    Text("[%s]", meter.c_str());

                    //Draw cursor
                    std::string cursorLine(powerGame.cursorPosition, ' ');
                    cursorLine += "^";
                    ImGui::Text("%s", cursorLine.c_str());

                    //Stop button
                    if (Button("Stop Cursor")) 
                    {
                        powerGameFinished = true;
                        powerGameWin = powerGame.checkWinCondition(powerGame.cursorPosition);
                        powerGameActive = false; //stop updating
                    }

                    End();
                }

                if (powerGameFinished)
                {
                    Begin("Power Meter Result");
                    if (powerGameWin) 
                    {
                        Text("The power is back.");
                    } else 
                    {
                        Text("Oh no, I missed it.");
                    }
                    if (Button("Close")) 
                    {
                        powerGameFinished = false;
                    }
                    End();
                }

//----------------------END POWER METER------------------------------------              

                //move to living room
                if (ateBreakfast && searchedCupboards) 
                {
                    currentRoom = ROOM_LIVING;
                    push_log("You finished all Kitchen actions. Moving to Living Room...");
                }
                break;
            }

            case ROOM_LIVING:
            {
                TextWrapped("You are in the living room. A TV sits on a stand.");
                currentArt = gameEngine.printArt("Living Room");

                static bool inspectedTV = false;

                if (!inspectedTV && Button("Inspect TV")) 
                {
                    push_log("The TV is blank. Nothing interesting.");
                    inspectedTV = true;
                }

                // All actions done
                if (inspectedTV) 
                {
                    push_log("You finished all Living Room actions. Adventure complete!");
                }
                break;
            }
        }

        // Display ASCII Art
        if (!currentArt.empty()) 
        {
            Separator(); //draws line
            TextUnformatted(currentArt.c_str());
        }

        // Message log
        Separator(); //draws line
        TextWrapped("Message Log:");
        BeginChild("LogRegion", ImVec2(0,150), true, ImGuiWindowFlags_HorizontalScrollbar);
        TextUnformatted(message_log.c_str());
        EndChild();

//---------------saving and quiting buttons----------------
        if(Button("Save"))
        {
            saveClicked = true;
            //gameEngine.saveGame("");
        }

        SameLine();
        if(Button("Quit"))
        {
            quitClicked = true;

        }


        End(); // Scene

        // Player Window
        Begin("Player", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize); 
        Text("Name: %s", player.getName().c_str());
        Text("Hair: %s", player.getHair().c_str());
        Text("Eyes: %s", player.getEyes().c_str());

//------------Inventory-------------------------
        Separator(); //draws line
        Text("Inventory:");
        array<Item, 5> inv = player.getInventory();
        for (size_t i = 0; i < inv.size(); ++i) 
        {
            std::string item = inv[i].getName();
            if (item.empty()) item = "(empty)";
            Text("%d: %s", (int)i+1, item.c_str());

            if (inv[i].getType() != ItemType::NULLITEM) 
            {
                SameLine();
                std::string btnLabel = "Use##" + std::to_string(i);
                if (Button(btnLabel.c_str())) 
                {
                    //gameEngine.useItem();
                    push_log("Used item: " + inv[i].getName());
                }
            }
        }



//------------------Health----------------------------
        Separator();//draws line   
        Text("Health:"); //prints health

        int current = player.getCurrentHealth(); //gets current player health
        int max = player.getMaxHealth(); //gets max player healht (5)

        // Draw hearts inline
        for (int i = 0; i < max; ++i)//loops through each "heart"
        {
            if (i < current) //check all hearts up to current and colors it
                TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "O"); // red heart
            else
                TextColored(ImVec4(0.5f, 0.5f, 0.5f, 0.5f), "X"); // gray hearSt
            SameLine(); //keeps hearts on same line
        }

        if (showStorageWindow) {
            gameEngine.renderStorageLocationWindow(currentRoomName, currentStorageLocation, showStorageWindow, message_log);
        }


        End();


        //Rendering
        Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.10f, 0.12f, 0.14f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    //Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}