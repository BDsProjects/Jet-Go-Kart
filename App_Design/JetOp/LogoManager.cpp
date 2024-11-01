#include "LogoManager.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>

LogoManager::~LogoManager() {
    // Cleanup all textures
    for (auto& pair : m_logos) {
        pair.second.cleanup();
    }
}

bool LogoManager::LoadLogo(const char* filename, const char* identifier) {
    TextureData texData;
    
    // Load image using stb_image
    int channels;
    unsigned char* data = stbi_load(filename, &texData.width, &texData.height, &channels, STBI_rgb_alpha);
    
    if (!data) {
        std::cerr << "Failed to load image: " << filename << "\n";
        return false;
    }

    // Generate OpenGL texture
    glGenTextures(1, &texData.textureID);
    glBindTexture(GL_TEXTURE_2D, texData.textureID);

    // Setup texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texData.width, texData.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    // Free image data
    stbi_image_free(data);

    // Store texture data
    m_logos[identifier] = texData;
    
    return true;
}

void LogoManager::RenderLogo(const char* identifier, const ImVec2& size, const ImVec4& tint_col) {
    auto it = m_logos.find(identifier);
    if (it == m_logos.end()) {
        return;
    }

    const TextureData& texData = it->second;
    ImVec2 renderSize = size;
    
    // If no size specified, use original image size
    if (renderSize.x <= 0 || renderSize.y <= 0) {
        renderSize = ImVec2((float)texData.width, (float)texData.height);
    }

    ImGui::Image((void*)(intptr_t)texData.textureID, renderSize, 
                ImVec2(0, 0), ImVec2(1, 1), tint_col);
}

ImVec2 LogoManager::GetLogoSize(const char* identifier) {
    auto it = m_logos.find(identifier);
    if (it != m_logos.end()) {
        return ImVec2((float)it->second.width, (float)it->second.height);
    }
    return ImVec2(0, 0);
}

bool LogoManager::IsLogoLoaded(const char* identifier) {
    return m_logos.find(identifier) != m_logos.end();
}
