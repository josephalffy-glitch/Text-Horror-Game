#include "extern/imgui/imgui.h"
#include "extern/imgui/backends/imgui_impl_glfw.h"
#include "extern/imgui/backends/imgui_impl_opengl3.h"
#include <stdio.h>
#include "glfw/include/GLFW/glfw3.h"
#include <string>
#include <vector>
#include <array>
#include <sstream>
#include <iostream>
#include "player.h"
#include "Room.h"
#include "GameEngine.h"
#include "Items.h"
#include "PowerMeterMinigame.h"
#include "loadgame.h"

using namespace ImGui;

// Forward declare GLFW error callback
static void glfw_error_callback(int error, const char* description);

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

    // Initialize room tracking after GameEngine is fully constructed
    gameEngine.visitRoom("Bedroom");
    Room* currentRoomPtr = gameEngine.getCurrentRoom();
	RoomEnum currentRoomEnum = currentRoomPtr->getRoomEnum();
    std::string currentRoomName = (currentRoomPtr != nullptr) ? currentRoomPtr->getName() : "Bedroom";
    static std::string currentStorageLocation = "";

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
    static bool canTravel = true;
    static bool playerCreated = false;
    static bool titleScreenDone = false;
    static bool showQuitWindow = false;
    static bool showLoadWindow = false;
    static bool showSaveWindow = false;
    static bool movedToKitchen = false;
    static bool movedToLivingRoom = false;
    static bool momCalls = false;
	static bool showStorageWindow = false;

    
    // Power Meter Minigame state
    bool powerGameActive = false; 
    PowerMeterMinigame powerGame;
    static bool powerGameFinished = false;
    static bool powerGameWin = false;
    static bool powerOutageTriggered = false; // New tracker for the story
    static bool TakePowerDamage = false;
    static bool powerGameResult = false;
    static bool powerResultAcknowledged = false;
 



    //Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        NewFrame();
        // check if player is dead (new logic)
        if (player.getCurrentHealth() == 0) 
        {
            Begin("Game Over", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
            TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "YOU DIED.");
            Text("Seems like you ran out of health.");
            Text("Maybe use a bandage next time?");
            
            if (Button("Restart")) 
            {
                // reset everything
                player.heal(100); // full heal
                powerOutageTriggered = false;
                powerGameFinished = false;
                canTravel = true;
                movedToKitchen = false;
                movedToLivingRoom = false;
                gameEngine.visitRoom("Bedroom");
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
                // Open the dedicated Load Game window where the user can pick a file
                showLoadWindow = true;
            }
            End();

            // If Load window was requested from title screen, render it here
            if (showLoadWindow) {
                renderLoadGameWindow(showLoadWindow, gameEngine, message_log);
            }

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
                // Start in the bedroom when a new player is created
                gameEngine.visitRoom("Bedroom");
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

        // Quick Prev/Next scene navigation buttons
        /*if (Button("Prev Scene")) {
            if (canTravel) {
                gameEngine.visitPrevRoom();
                showStorageWindow = false;
                currentStorageLocation.clear();
            } else {
                push_log("You are not allowed to leave.");
            }
        }
        SameLine();
        if (Button("Next Scene")) {
            if (canTravel) {
                gameEngine.visitNextRoom();
                showStorageWindow = false;
                currentStorageLocation.clear();
            } else {
                push_log("You are not allowed to leave.");
            }
        }

		currentRoomPtr = gameEngine.getCurrentRoom();
		currentRoomEnum = currentRoomPtr->getRoomEnum();
		currentRoomName = (currentRoomPtr != nullptr) ? currentRoomPtr->getName() : "Unknown";*/

        currentRoomPtr = gameEngine.getCurrentRoom();
        if (currentRoomPtr != nullptr)
        {
            currentRoomEnum = currentRoomPtr->getRoomEnum();
            currentRoomName = currentRoomPtr->getName();
        }

		switch (currentRoomEnum)
        {
			case RoomEnum::ROOM_BEDROOM:
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
                    gameEngine.lookInStorageLocation(currentRoomName, currentStorageLocation, showStorageWindow, message_log);
                    //checkedDrawer = true;
                }
            

                if (!checkedTrophy && Button("Check out the trophy")) 
                {
                    push_log("The trophy is shiny and reminds you of your victories. " + player.getName() + " is printed on it.");
                    checkedTrophy = true;
                }


                // Offer an explicit Continue button to move to the kitchen
                if (rested/* && checkedDrawer*/ && checkedTrophy && !movedToKitchen) 
                {
                    TextWrapped("You are ready to continue.");
                    if (Button("Continue")) 
                    {
                        momCalls = true;
                        push_log("\n>> Mom: Sweetie! Breakfeast is ready!\n");
                        push_log("She has always called you that\nIt's a little embarrassing, but mostly endearing\n");
                        //gameEngine.visitRoom("Kitchen");
                        showStorageWindow = false;
                        //currentStorageLocation.clear();
                        //movedToKitchen = true;
                    }
                }
                break;
            }

			case RoomEnum::ROOM_KITCHEN:
            {
                TextWrapped("You run to the kitchen, ready for a fresh meal.\nYour mom turns around with a plate of waffles drenched in syrup.\nAs she sets the plate down, you notice her right hand is bandaged up.");
                currentArt = gameEngine.printArt("Kitchen");

                // Story trackers
                static bool ateBreakfast = false;
                static bool askedAboutHand = false;
                static bool checkedCabinet = false; // Tracker for the third required action

                if (!ateBreakfast && Button("Eat breakfast")) 
                {
                    player.heal(1); // Assuming breakfast heals slightly, like the old logic
                    push_log("You ate breakfast. (health updated)");
                    ateBreakfast = true;
                }

                if (!askedAboutHand && Button("Ask your mom about her hand")) 
                {
                    push_log(">> " + player.getName() + ": What happened to your hand mom?");
                    push_log(">> Mom: Oh! I was just cleaning the basement and touched some broken glass.\nNo big deal sweetie.");
                    askedAboutHand = true;
                }

                // Logic to open the Cabinet storage location (to get Syrup/Cookie)
                if (!checkedCabinet && Button("Search Kitchen Cabinet")) 
                {
                    currentStorageLocation = "Cabinet";
                    gameEngine.lookInStorageLocation(currentRoomName, currentStorageLocation, showStorageWindow, message_log);
                    //checkedCabinet = true; 
                    push_log("You open the kitchen cabinet. You can take a look inside the new window.");
                }
                
                //-------------------------POWER OUTAGE EVENT START-------------------------------
                // The power outage occurs when all main kitchen actions are completed
                if (ateBreakfast && askedAboutHand && /*checkedCabinet && */!powerOutageTriggered) 
                {
                    push_log("\n------------------------------------------------");
                    push_log("A sudden darkness envelops the kitchen. The lights flicker and die.");
                    push_log(">> Mom: Oh no, the power must be out! I hate when that happens.");
                    push_log("You hear a strange humming sound from the basement door.");
                    push_log(">> Mom: Honey, can you go check the auxiliary power unit down in the cellar?");
                    push_log("------------------------------------------------\n");
                    
                    powerGameActive = true;
                    powerOutageTriggered = true; // Set to true so this only triggers once
                    canTravel = false; //cant visit other rooms
                }

                // The minigame display window (uses the existing structure)
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
                    Begin("Power Meter Result", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
                        if (powerGameWin) 
                        {
                            Text("Success! The power is back.");
                            //push_log("\n>> Mom: That's my kid! The lights are back on. Thanks, sweetie!\n");
                        }
                     
                        else 
                        {
                            // health penalty added here
                            if(!TakePowerDamage)
                            {
                                player.takeDamage(2);
                                TakePowerDamage = true;
                            }
                            Text("FAILURE! A power surge zaps you.");
                            //push_log("\nOH NO! A surge hit the box, and you took damage! Maybe be more careful next time!\n");
                            //push_log("\n>> Mom: Well, the lights are back on, but it seem unstable.\n");
                        }

                        //fixes looping message
                        if (!powerGameResult) 
                        {
                            push_log(powerGameWin ? "\n>> Mom: That's my kid! The lights are back on. Thanks, sweetie!\n"
                                                : "\nOH NO! A surge hit the box, and you took damage! Maybe be more careful next time!\n"
                                                    "\n>> Mom: Well, the lights are back on, but it seems unstable.\n");
                            powerGameResult = true;
                        }
                    

                    if (Button("Close")) 
                    {
                        powerGameFinished = false;
                        powerGameActive = false;
                        canTravel = true;
                        powerResultAcknowledged = true;
                    }
                    End();
                }
                //-------------------------POWER OUTAGE EVENT END---------------------------------


                // Offer a Continue button to move to the living room after the power event
                if  (powerOutageTriggered && powerResultAcknowledged && !movedToLivingRoom)
                {
                    TextWrapped("You can continue to the Living Room now.");
                    if (Button("Continue")) 
                    {
                        gameEngine.visitRoom("Living Room");
                        showStorageWindow = false;
                        currentStorageLocation.clear();
                        push_log("You finish breakfast and get ready for the day. Moving to Living Room...");

                        movedToLivingRoom = true;
                        powerGameFinished = false;
                        powerGameResult = false;
                        TakePowerDamage = false;
                        powerResultAcknowledged = false;
                    }
                }
                break;
            }

			case RoomEnum::ROOM_LIVING:
            {
                TextWrapped("You are in the living room. In front of a sofa, a large, blank TV sits on a stand, emitting a faint, high-pitched whine.");
                currentArt = gameEngine.printArt("LivingRoom"); 

                static bool inspectedTV = false;
                static bool endingTriggered = false;

                if (!inspectedTV && Button("Inspect TV")) 
                {
                    push_log("You walk up to the TV. The screen explodes into blinding white static. \nA voice cuts through: 'RUN'.");
                    currentArt = gameEngine.printArt("BrokenMirror"); // Creepy static flash
                    inspectedTV = true;
                }

                // branching horror ending
                if (inspectedTV && !endingTriggered) 
                {
                    TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
                        "The atmosphere turns heavy. Panic sets in. What do you do?");
                    
                    // check if player has the pocket mirror item
                    std::array<Item, 5> inv = player.getInventory();
                    bool hasPocketMirror = false;
                    for (const auto& item : inv) 
                    {
                        if (item.getName() == "mirror") 
                        {
                            hasPocketMirror = true;
                            break;
                        }
                    }

                    // option 1: the mirror (requires mirror)
                    if (hasPocketMirror) 
                    {
                        if (Button("Check Reflection (Use Mirror)")) 
                        {
                            canTravel = false;
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
                        canTravel = false;
                        push_log("\nYou sprint to the door and grab the handle. It's scorching hot!");
                        push_log("You recoil. The locks begin to turn from the *outside*. Someone is coming in.");
                        push_log("\n**END OF DAY 1: NO ESCAPE**");
                        endingTriggered = true;
                    }

                    // option 3: the cry for help 
                    if (Button("Scream for Mom")) 
                    {
                        canTravel = false;
                        push_log("\n'MOM! HELP!' you scream.");
                        push_log("The static stops instantly. A whisper comes from the dark hallway behind you.");
                        push_log(">> Mom: 'I told you... never to raise your voice in this house.'");
                        push_log("\n**END OF DAY 1: DISCIPLINE**");
                        endingTriggered = true;
                    }
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

        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 5.0f)
        {
            ImGui::SetScrollHereY(1.0f); // Auto-scroll to bottom
        }

        EndChild();

//---------------continue, saving and quiting buttons----------------

        // Continue button to advance the scene/story (shows confirmation modal)
        {
            // determine next room based on current room enum
            std::string nextRoomName;
            bool hasNext = false;
            switch (currentRoomEnum) {
                case RoomEnum::ROOM_BEDROOM:
                    nextRoomName = "Kitchen";
                    hasNext = true;
                    break;
                case RoomEnum::ROOM_KITCHEN:
                    nextRoomName = "Living Room";
                    hasNext = true;
                    break;
                case RoomEnum::ROOM_LIVING:
                default:
                    hasNext = false;
                    break;
            }

            static std::string pendingNextRoom = "";

        }

        if(Button("Save"))
        {
            // open save slot chooser (three fixed slots)
            showSaveWindow = true;
        }

        SameLine();
        if(Button("Quit"))
        {
            showQuitWindow = true;
        }

                
        if(showQuitWindow)
        {
            Begin("Quit");
            TextUnformatted("Quit Game? (Make sure you have saved!");

            if (Button("Yes"))
            {
                titleScreenDone = false;
                showQuitWindow = false;
            }
            
            if(Button("No"))
            {
                showQuitWindow = false;
            }
            End();
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
                std::string btnLabel = "Use##" + std::to_string(i); //creates unique button id
                if (Button(btnLabel.c_str())) 
                {
                    gameEngine.useItem(inv[i].getName(), message_log);
                }
            }
        }


//------------------Health-------------------
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
        if (showStorageWindow) 
        {
            Room* currentRoomPtr = gameEngine.getCurrentRoom();
            string roomName = (currentRoomPtr != nullptr) ? currentRoomPtr->getName() : "Bedroom";
            gameEngine.renderStorageLocationWindow(roomName, currentStorageLocation, showStorageWindow, message_log);
        }


        End();
//----------------Visited Rooms Window-------------------------
        SetNextWindowSize(ImVec2(150, 200), ImGuiCond_Always);
        Begin("Visited Rooms", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);

         vector<Room>* visitedRooms = gameEngine.getVisitedRooms(); //gets vector with all visited rooms
         for(Room& room : *visitedRooms) //runs through all visited room objects
         {
            Text("%s", room.getName().c_str());
            SameLine();
            string btn = "Visit##" + room.getName(); //creates unique button ids
            
            if(Button(btn.c_str()))
            {
                if(canTravel)
                {
                    gameEngine.visitRoom(room.getName());
                    showStorageWindow = false;
                    currentStorageLocation.clear();
                }
                else
                {
                    push_log("You are not allowed to leave.");
                }
                
            }
         }
//-----------------------transitions---------------------------
         if (momCalls)
         {
            OpenPopup("MomCalls");
         }

        if (BeginPopupModal("MomCalls", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            TextWrapped("Do you want to go to the kitchen?");

            if (Button("Go to Kitchen"))
            {
                gameEngine.visitRoom("Kitchen");
                movedToKitchen = true;
                momCalls = false;
                CloseCurrentPopup();
            }

            SameLine();

            if (Button("Stay Here"))
            {
                momCalls = false;
                CloseCurrentPopup();
            }

            EndPopup();
}


        End();


        // Render load/save windows if requested
        if (showLoadWindow) 
        {
            renderLoadGameWindow(showLoadWindow, gameEngine, message_log);
        }
        if (showSaveWindow) 
        {
            renderSaveGameWindow(showSaveWindow, gameEngine, message_log);
        }

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

