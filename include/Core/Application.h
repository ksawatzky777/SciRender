#pragma once

// Macro include file.
#include "SciRenderPCH.h"

// Project includes.
#include "Core/Layers.h"
#include "Core/Window.h"
#include "Core/ImGuiLayer.h"

namespace SciRenderer
{
  // Singleton application class.
  class Application
  {
  public:
    Application(const std::string &name = "Editor Viewport");
    virtual ~Application();

    // Push layers and overlays.
    void pushLayer(Layer* layer);
    void pushOverlay(Layer* overlay);

    // Close the application.
    void close();

    // Getters.
    static inline Application* getInstance() { return Application::appInstance; }
    inline Window* getWindow() { return this->appWindow; }

  protected:
    // The application instance.
    static Application* appInstance;

    // Determines if the application should continue to run or not.
    bool running, isMinimized;

    // Application name.
    std::string name;

    // The layer stack for the application.
    LayerCollection layerStack;

    // The main application window.
    Window* appWindow;

    // The ImGui layer. Implements the ImGui boilerplate behavior.
    ImGuiLayer* imLayer;

    // The last frame time.
    float lastTime;

  private:
    // Functions for behavior.
    void run();
    bool onWindowClose();
    bool onWindowResize();
  };
}
