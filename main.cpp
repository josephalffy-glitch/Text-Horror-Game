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
#include "loadgame.h" 

using namespace ImGui;


// Forward declare GLFW error callback
static void glfw_error_callback(int error, const char* description);

// Room enum
enum Rooms {
    ROOM_BEDROOM,
    ROOM_KITCHEN,
    ROOM_LIVING,
};

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
    
    // Power Meter Minigame state
    bool powerGameActive = false; 
    PowerMeterMinigame powerGame;
    static bool powerGameFinished = false;
    static bool powerGameWin = false;
    static bool powerOutageTriggered = false; // New tracker for the story

    //Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        NewFrame();

        // check if player is dead
        if (player.getCurrentHealth() <= 0) 
        {
            Begin("game over", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
            TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "YOU DIED.");
            Text("seems like you ran out of health.");
            Text("maybe use a bandage next time?");
            
            if (Button("restart game")) 
            {
                // reset everything
                player.heal(100); // full heal
                currentRoom = ROOM_BEDROOM;
                powerOutageTriggered = false;
                powerGameFinished = false;
                push_log("\nyou woke up again. was it all a dream?");
            }
            End();

            // render the death screen and skip the rest
            Render();
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(0.0f, 0.0f, 0.0f, 1.00f); // black screen for death
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window);
            continue;
        }

        //Title screen
        if(!titleScreenDone) 
        {
            ImGui::SetNextWindowSize(ImVec2(900, 500), ImGuiCond_Always);
            Begin("Title Screen");
            currentArt = gameEngine.printArt("TitleScreen");

            if (!currentArt.empty())
            {
                Separator();
                float textWidth = ImGui::CalcTextSize(currentArt.c_str()).x;
                float windowWidth = ImGui::GetWindowSize().x;
                float cursorPosX = (windowWidth - textWidth) * 0.5f;
                SetCursorPosX(cursorPosX);
                TextUnformatted(currentArt.c_str());
            }

            float windowWidth = ImGui::GetWindowSize().x;
            float buttonWidth = ImGui::CalcTextSize("Start Game").x + ImGui::GetStyle().FramePadding.x * 2;
            float posX = (windowWidth - buttonWidth) * 0.5f; 
            SetCursorPosX(posX);

            if(Button("Start Game"))
            {
                titleScreenDone = true;
            }
            float buttonWidth2 = ImGui::CalcTextSize("Load Game").x + ImGui::GetStyle().FramePadding.x * 2;
            float posX2 = (windowWidth - buttonWidth2) * 0.5f;
            ImGui::SetCursorPosX(posX2);
            
            if(Button("Load Game"))
            {
                gameEngine.loadGame("save.txt");
                titleScreenDone = true;
                playerCreated = true;
                push_log("Game loaded!");
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
            Begin("Character Creation", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
            Text("Welcome adventurer! Create your character:");
            InputText("Name", nameBuffer, IM_ARRAYSIZE(nameBuffer));
            InputText("Hair Color", hairBuffer, IM_ARRAYSIZE(hairBuffer));
            InputText("Eye Color", eyesBuffer, IM_ARRAYSIZE(eyesBuffer));

            if (Button("Create Player"))
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
            continue;
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
                    rested = true;
                }

                if (!checkedDrawer && !mirrorScene && Button("Look in the drawer")) 
                {
                    currentStorageLocation = "Drawer";
                    currentRoomName = "Bedroom";
                    gameEngine.lookInStorageLocation(currentRoomName, currentStorageLocation, showStorageWindow, message_log);
                    checkedDrawer = true;
                }

                if (!checkedTrophy && Button("Check out the trophy")) 
                {
                    push_log("The trophy is shiny and reminds you of your victories. " + player.getName() + " is printed on it.");
                    checkedTrophy = true;
                }

                // move to kitchen scene
                if (rested && checkedDrawer && checkedTrophy) 
                {
                    // manual transition button
                    if (Button("Go to the Kitchen"))
                    {
                        push_log("\n>> Mom: Sweetie! Breakfeast is ready!\n");
                        push_log("She has always called you that. It's a little embarrassing, but mostly endearing\n");
                        currentRoom = ROOM_KITCHEN;
                    }
                }
                break;
            }

            case ROOM_KITCHEN:
            {
                TextWrapped("You run to the kitchen, ready for a fresh meal.\nYour mom turns around with a plate of waffles drenched in syrup.\nAs she sets the plate down, you notice her right hand is bandaged up.");
                currentArt = gameEngine.printArt("Kitchen");

                // Story trackers
                static bool ateBreakfast = false;
                static bool askedAboutHand = false;
                static bool checkedCabinet = false; // Tracker for the third required action

                if (!ateBreakfast && Button("Eat breakfast")) 
                {
                    player.heal(1); // Assuming breakfast heals slightly
                    push_log("You ate breakfast. (health updated)");
                    ateBreakfast = true;
                }

                if (!askedAboutHand && Button("Ask your mom about her hand")) 
                {
                    push_log(player.getName() + ": What happened to your hand mom?");
                    push_log(">> Mom: Oh! I was just cleaning the basement and touched some broken glass. No big deal sweetie.");
                    askedAboutHand = true;
                }

                // open the cabinet storage
                if (!checkedCabinet && Button("Search Kitchen Cabinet")) 
                {
                    currentStorageLocation = "Cabinet";
                    currentRoomName = "Kitchen";
                    gameEngine.lookInStorageLocation(currentRoomName, currentStorageLocation, showStorageWindow, message_log);
                    checkedCabinet = true; 
                    push_log("You open the kitchen cabinet. You can take a look inside the new window.");
                }
                
                //-------------------------POWER OUTAGE EVENT START-------------------------------
                // the power outage occurs when all main kitchen actions are completed
                if (ateBreakfast && askedAboutHand && checkedCabinet && !powerOutageTriggered) 
                {
                    push_log("\n------------------------------------------------");
                    push_log("A sudden darkness envelops the kitchen. The lights flicker and die.");
                    push_log(">> Mom: Oh no, the power must be out! I hate when that happens.");
                    push_log("You hear a strange humming sound from the basement door.");
                    push_log(">> Mom: Honey, can you go check the auxiliary power unit down in the cellar?");
                    push_log("------------------------------------------------\n");
                    
                    powerGameActive = true;
                    powerOutageTriggered = true; // set to true so this only triggers once
                }

                // The minigame display window 
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
                    }

                    End();
                }

                // Minigame Result window
                if (powerGameFinished)
                {
                    Begin("Power Meter Result", &powerGameFinished, ImGuiWindowFlags_AlwaysAutoResize);
                    if (powerGameWin) 
                    {
                        Text("Success! The power is back.");
                        push_log("\n>> Mom: That's my kid! The lights are back on. Thanks, sweetie!\n");
                    } else 
                    {
                        // health penalty 
                        player.takeDamage(1); 
                        Text("FAILURE! A power surge zaps you.");
                        push_log("\nOH NO! A surge hit the box, and you took damage! Maybe be more careful next time!\n");
                        
                        Text("Oh no, I missed it. The lights flicker back on but seem unstable.");
                        push_log("\n>> Mom: Well, the lights are back on, but that was a bit rough. Maybe be more careful next time!\n");
                    }
                    if (Button("Close")) 
                    {
                        powerGameFinished = false;
                        powerGameActive = false;
                    }
                    End();
                }
                //-------------------------POWER OUTAGE EVENT END--------------


                // move to living room only when minigame is completed and closed
                if (powerOutageTriggered && !powerGameActive && !powerGameFinished) 
                {
                    // manual transition button
                    if (Button("Go to the Living Room"))
                    {
                        currentRoom = ROOM_LIVING;
                        push_log("You finish breakfast and get ready for the day. Moving to Living Room...");
                    }
                }
                break;
            }

            case ROOM_LIVING:
            {
                TextWrapped("You are in the living room. A large, blank TV sits on a stand, emitting a faint, high-pitched whine.");
                currentArt = gameEngine.printArt("Bedroom"); // Use a neutral background art

                static bool inspectedTV = false;
                static bool endingTriggered = false;

                if (!inspectedTV && Button("Inspect TV")) 
                {
                    push_log("You walk up to the TV. The screen explodes into blinding white static. A voice cuts through: 'RUN'.");
                    currentArt = gameEngine.printArt("BrokenMirror"); // Creepy static flash
                    inspectedTV = true;
                }

                // branching horror ending
                if (inspectedTV && !endingTriggered) 
                {
                    TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "The atmosphere turns heavy. Panic sets in. What do you do?");
                    
                    // check if player has the pocket mirror item
                    std::array<Item, 5> inv = player.getInventory();
                    bool hasPocketMirror = false;
                    for (const auto& item : inv) {
                        if (item.getName() == "mirror") {
                            hasPocketMirror = true;
                            break;
                        }
                    }

                    // option 1: the mirror (requires item)
                    if (hasPocketMirror) 
                    {
                        if (Button("Check Reflection (Use Mirror)")) 
                        {
                            push_log("\nYou raise the mirror. The reflection shows the room is empty... but there is a hand on your shoulder.");
                            push_log("You look down. The hand is bandaged. The grip tightens.");
                            currentArt = gameEngine.printArt("BrokenMirror"); 
                            push_log("\n**END OF DAY 1: THE INVISIBLE TOUCH**");
                            endingTriggered = true;
                        }
                    } 

                    // option 2: the escape
                    if (Button("Bolt for the Front Door")) 
                    {
                        push_log("\nYou sprint to the door and grab the handle. It's scorching hot!");
                        push_log("You recoil. The locks begin to turn from the *outside*. Someone is coming in.");
                        push_log("\n**END OF DAY 1: NO ESCAPE**");
                        endingTriggered = true;
                    }

                    // option 3: the cry for help
                    if (Button("Scream for Mom")) 
                    {
                        push_log("\n'MOM! HELP!' you scream.");
                        push_log("The static stops instantly. A whisper comes from the dark hallway behind you.");
                        push_log(">> Mom: 'I told you... never to raise your voice in this house.'");
                        push_log("\n**END OF DAY 1: DISCIPLINE**");
                        endingTriggered = true;
                    }
                }

                // Final message
                if (endingTriggered) 
                {
                    Separator();
                    TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "DAY 1 HAS CONCLUDED.");
                }
                break;
            }
        }

        // Display ASCII Art
        if (!currentArt.empty()) 
        {
            Separator();
            TextUnformatted(currentArt.c_str());
        }

        // Message log
        Separator();
        TextWrapped("Message Log:");
        BeginChild("LogRegion", ImVec2(0,150), true, ImGuiWindowFlags_HorizontalScrollbar);
        TextUnformatted(message_log.c_str());
        EndChild();

//---------------saving and quiting buttons----------------
        if(Button("Save"))
        {
            saveClicked = true;
            gameEngine.saveGame("save.txt"); 
            push_log("Game Saved!");
        }

        SameLine();
        if(Button("Quit"))
        {
            quitClicked = true;
            glfwSetWindowShouldClose(window, true); 
        }


        End(); // Scene

        // Player Window
        Begin("Player", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize); 
        Text("Name: %s", player.getName().c_str());
        Text("Hair: %s", player.getHair().c_str());
        Text("Eyes: %s", player.getEyes().c_str());

//------------Inventory-------------------------
        Separator();
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
                    gameEngine.useItem(inv[i].getName(), message_log);
                }
            }
        }


//------------------Health----------------------------
        Separator();   
        Text("Health:");

        int current = player.getCurrentHealth();
        int max = player.getMaxHealth();

        // Draw hearts inline
        for (int i = 0; i < max; ++i)
        {
            if (i < current)
                TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "O"); // red heart
            else
                TextColored(ImVec4(0.5f, 0.5f, 0.5f, 0.5f), "X"); // gray heart
            SameLine();
        }

        // Render the Storage Location (Drawer/Cabinet) window if triggered
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
