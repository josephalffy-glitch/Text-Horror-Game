#include "loadgame.h"
#include "GameEngine.h"
#include <filesystem>
#include <imgui.h>
#include <sstream>

namespace fs = std::filesystem;

std::vector<std::string> listSaveFiles()
{
    std::vector<std::string> out;
    try {
        if (!fs::exists("SaveFiles")) return out;
        for (auto &entry : fs::directory_iterator("SaveFiles")) 
        {
            if (entry.is_regular_file())
            {
                out.push_back(entry.path().filename().string());
            }
        }
    }
    catch (...) 
    {
        // ignore filesystem errors, return what we have (possibly empty)
    }
    return out;
}

void renderLoadGameWindow(bool &show, GameEngine &engine, std::string &message_log)
{
    if (!show) return;

    ImGui::Begin("Load Game", &show, ImGuiWindowFlags_AlwaysAutoResize);

    // Present exactly three load slots matching the save slots
    const std::vector<std::string> slots = { "slot1.sav", "slot2.sav", "slot3.sav" };
    ImGui::TextUnformatted("Select a slot to load:");
    ImGui::Separator();
    for (size_t i = 0; i < slots.size(); ++i) {
        std::string fname = slots[i];
        std::string label = std::string("Slot ") + std::to_string(i+1) + std::string(" - ") + fname;
        bool exists = false;
        try { exists = fs::exists(std::string("SaveFiles/") + fname); } catch(...) { exists = false; }

        ImGui::TextUnformatted(label.c_str());
        ImGui::SameLine();
        if (!exists) {
            ImGui::TextColored(ImVec4(0.6f,0.6f,0.6f,1), "(empty)");
            ImGui::SameLine();
            ImGui::BeginDisabled();
            ImGui::Button((std::string("Load##slot") + std::to_string(i)).c_str());
            ImGui::EndDisabled();
        } else {
            if (ImGui::Button((std::string("Load##slot") + std::to_string(i)).c_str())) {
                engine.loadGame(fname);
                message_log += std::string("Loaded game: ") + fname + "\n";
                show = false;
            }
        }
    }

    ImGui::End();
}

void renderSaveGameWindow(bool &show, GameEngine &engine, std::string &message_log)
{
    if (!show) return;

    ImGui::Begin("Save Game", &show, ImGuiWindowFlags_AlwaysAutoResize);

    // fixed three-slot filenames
    const std::vector<std::string> slots = { "slot1.sav", "slot2.sav", "slot3.sav" };

    ImGui::TextUnformatted("Choose a slot to save (this will overwrite the slot):");
    ImGui::Separator();

    static int pendingSlot = -1;
    for (size_t i = 0; i < slots.size(); ++i) {
        std::string fname = slots[i];
        std::string label = std::string("Slot ") + std::to_string(i+1) + std::string(" - ") + fname;
        bool exists = false;
        try { exists = fs::exists(std::string("SaveFiles/") + fname); } catch(...) { exists = false; }

        ImGui::TextUnformatted(label.c_str());
        ImGui::SameLine();
        if (exists) ImGui::TextColored(ImVec4(1,0.6f,0,1), "(used)");
        ImGui::SameLine();
        std::string btn = std::string("Save##slot") + std::to_string(i);
        if (ImGui::Button(btn.c_str())) {
            pendingSlot = (int)i;
            ImGui::OpenPopup("Confirm Overwrite");
        }
    }

    // Confirm overwrite popup
    if (ImGui::BeginPopupModal("Confirm Overwrite", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("This will overwrite the selected save slot. Continue?");
        ImGui::Separator();
        if (ImGui::Button("Yes", ImVec2(120,0))) {
            if (pendingSlot >= 0 && pendingSlot < (int)slots.size()) {
                engine.saveGame(slots[pendingSlot]);
                message_log += std::string("Saved game to ") + slots[pendingSlot] + "\n";
            }
            pendingSlot = -1;
            ImGui::CloseCurrentPopup();
            show = false; // close save window after saving
        }
        ImGui::SameLine();
        if (ImGui::Button("No", ImVec2(120,0))) {
            pendingSlot = -1;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::Separator();
    ImGui::TextUnformatted("Tip: Use the Load menu to load one of these slots later.");

    ImGui::End();
}
