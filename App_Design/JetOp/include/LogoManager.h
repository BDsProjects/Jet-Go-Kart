#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <string>
#include <unordered_map>

// Structure to hold texture information
struct TextureData {
    GLuint textureID = 0;
    int width = 0;
    int height = 0;
    
    void cleanup() {
        if (textureID != 0) {
            glDeleteTextures(1, &textureID);
            textureID = 0;
        }
    }
};

class LogoManager {
public:
    ~LogoManager();

    // Load a logo from file
    bool LoadLogo(const char* filename, const char* identifier);

    // Render a logo with ImGui
    void RenderLogo(const char* identifier, 
                   const ImVec2& size = ImVec2(0, 0),
                   const ImVec4& tint_col = ImVec4(1, 1, 1, 1));

    // Get the size of a loaded logo
    ImVec2 GetLogoSize(const char* identifier);

    // Check if a logo is loaded
    bool IsLogoLoaded(const char* identifier);

private:
    std::unordered_map<std::string, TextureData> m_logos;
};
