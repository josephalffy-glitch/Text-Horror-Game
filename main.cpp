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

    // -------------------- Character creation --------------------
    static char nameBuffer[128] = "";
    static char hairBuffer[64] = "";
    static char eyesBuffer[64] = "";
    static bool playerCreated = false;
    static bool titleScreenDone = false;
    
    //bools
    //bool itemWindow = false;

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

            //float windowWidth2 = ImGui::GetWindowSize().x;
            float buttonWidth2 = ImGui::CalcTextSize("Start Game").x + ImGui::GetStyle().FramePadding.x * 2;

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

        
        //Scene window
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

                    /*
                    push_log("\nYou walk over to the drawer on your desk and take a look.\nInside you see a few miscellaneous items:");
                    push_log(">>>> pencils, a boxcutter, bandaids, a snack bar, a pocket mirror\n");
                    push_log("The mirror catches your eye. Would you like to take a look?");
                    //add new window with pocket mirror art
                    Begin("Item", nullptr, ImGuiWindowFlags_NoCollapse);
                    currentArt = gameEngine.printArt("PocketMirror");
                    End();
                    mirrorScene = true;
                    */
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
                    push_log(">> Mom: Sweetie! Breakfeast is ready!\n");
                    push_log("She has always called you that. It's a little embarrassing, but mostly endearing\n");
                    currentRoom = ROOM_KITCHEN;
                }
                break;
            }

            case ROOM_KITCHEN:
            {
                TextWrapped("You are in the kitchen. Delicious smells fill the air.");
                currentArt = gameEngine.printArt("Kitchen");

                //track choices
                static bool ateBreakfast = false;
                static bool searchedCupboards = false;

                if (!ateBreakfast && Button("Eat breakfast")) 
                {
                    //player.updateHealth("breakfast");
                    push_log("You ate breakfast. (health updated)");
                    ateBreakfast = true;
                }
                if (!searchedCupboards && Button("Search cupboards")) 
                {
                    //player.addToInventory("Cupboard Note");
                    push_log("You found a note in the cupboards.");
                    searchedCupboards = true;
                }

                // All actions done? Move to Living Room
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

        End(); // Scene

        // Player Window
        Begin("Player", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize); 
        Text("Name: %s", player.getName().c_str());
        Text("Hair: %s", player.getHair().c_str());
        Text("Eyes: %s", player.getEyes().c_str());


        Separator(); //draws line
        Text("Inventory:");
        array<Item, 5> inv = player.getInventory();
        for (size_t i = 0; i < inv.size(); ++i) 
        {
            std::string item = inv[i].getName();
            if (item.empty()) item = "(empty)";
            Text("%d: %s", (int)i+1, item.c_str());

            // Button to use item
            SameLine();
            std::string btnLabel = "Drop##" + std::to_string(i);
            if (!(inv[i].getType() == ItemType::NULLITEM) && Button(btnLabel.c_str())) 
            {
                //player.checkInventory();
                push_log("Used item: " + inv[i].getName());
                //use item function
            }
                        // Button to use item

            SameLine();
            std::string btnLabel2 = "Use##" + std::to_string(i);
            if (!!(inv[i].getType() == ItemType::NULLITEM) && Button(btnLabel2.c_str())) 
            {
                //player.dropItem();
                //gameEngine.dropItem();
                push_log("Dropped item: " + inv[i].getName());
                
            }
        }

        Separator();//draws line   

        //HEALTH
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

        /*//Item Visual Window
        if(Begin("Items", &itemWindow))
        {  
            Text("A closer look . . .");
            End();
        }

        Render();
        int display_w2, display_h2;
        glfwGetFramebufferSize(window, &display_w2, &display_h2);
        glViewport(0, 0, display_w2, display_h2);
        glClearColor(0.10f, 0.12f, 0.14f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);*/

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