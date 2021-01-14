//#define USE_SDL2 1
#include "R2DEngine.hpp"

class App : public R2DEngine {
public:
    bool onCreate() override {
        windowTitle = "Sand Simulator";
        return true;
    }

    bool onUpdate(double deltaTime) override {
        return true;
    }
};

int main() {
    App app;
    if (app.construct(1280, 720)) {
        app.init();
    }

    return 0;
}