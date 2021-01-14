#define DEBUG_ENABLED 1
#include "R2DEngine.hpp"

class App : public R2DEngine {
    double time;
    bool tick;

    enum CellID {
        AIR,
        WALL,
        SAND,
        WATER
    };
    struct Particle {
        CellID id;
        Color color;
    };
    std::vector<std::vector<CellID>> map;
    std::vector<std::vector<CellID>> mapBuffer;

    void update_wall(int x, int y) {
        mapBuffer[y][x] = WALL;
    }

    void update_sand(int x, int y) {
        if (map[y + 1][x] == AIR) {
            mapBuffer[y + 1][x] = SAND;
            map[y][x] = AIR;
        } else if (map[y + 1][x - 1] == AIR) {
            mapBuffer[y + 1][x - 1] = SAND;
            map[y][x] = AIR;
        } else if (map[y + 1][x + 1] == AIR) {
            mapBuffer[y + 1][x + 1] = SAND;
            map[y][x] = AIR;
        } else {
            mapBuffer[y][x] = SAND;
        }
    }

    void update_water(int x, int y) {
        if (map[y + 1][x] == AIR) {
            mapBuffer[y + 1][x] = WATER;
            map[y][x] = AIR;
        } else if (map[y + 1][x - 1] == AIR) {
            mapBuffer[y + 1][x - 1] = WATER;
            map[y][x] = AIR;
        } else if (map[y + 1][x + 1] == AIR) {
            mapBuffer[y + 1][x + 1] = WATER;
            map[y][x] = AIR;
        } else if (map[y][x - 1] == AIR) {
            mapBuffer[y][x - 1] = WATER;
            map[y][x] = AIR;
        } else if (map[y][x + 1] == AIR) {
            mapBuffer[y][x + 1] = WATER;
            map[y][x] = AIR;
        } else {
            mapBuffer[y][x] = WATER;
        }
    }
public:
    const uint32_t mapWidth = 80;
    const uint32_t mapHeight = 60;

    bool onCreate() override {
        windowTitle = "Sand Simulator";
        map = std::vector<std::vector<CellID>>(mapHeight);
        mapBuffer = std::vector<std::vector<CellID>>(mapHeight);
        for (int y = 0; y < mapHeight; y ++) {
            map[y] = std::vector<CellID>(mapWidth);
            mapBuffer[y] = std::vector<CellID>(mapWidth);
            for (int x = 0; x < mapWidth; x ++) {
                map[y][x] = AIR;
                if (y == mapHeight - 1 || x == 0 || x == mapWidth - 1) {
                    map[y][x] = WALL;
                }
                mapBuffer[y][x] = AIR;
            }
        }
        time = 0.0;
        tick = false;
        return true;
    }

    bool onUpdate(double deltaTime) override {

        tick = false;
        time += deltaTime * 50;
        if (time >= 1.0) {
            tick = true;
            time = 0.0;
        }

        if (getMouseState(GLFW_MOUSE_BUTTON_RIGHT) == PRESS) {
            map[mousePosY][mousePosX] = WALL;
            //tick = true;
        } else if (getMouseState(GLFW_MOUSE_BUTTON_LEFT) == PRESS) {
            map[mousePosY][mousePosX] = SAND;
            //tick = true;
        } else if (getMouseState(GLFW_MOUSE_BUTTON_MIDDLE) == PRESS) {
            map[mousePosY][mousePosX] = WATER;
            //tick = true;
        }

        if (tick) {
            for (int y = 0; y < mapHeight; y ++) {
                for (int x = 0; x < mapWidth; x ++) {
                    mapBuffer[y][x] = AIR;
                }
            }
            for (int y = mapHeight - 1; y >= 0; y --) {
                for (int x = 0; x < mapWidth; x ++) {
                    CellID id = map[y][x];
                    switch (id) {
                        case AIR: {
                            break;
                        }
                        case WALL: {
                            update_wall(x, y);
                            break;
                        }
                        case SAND: {
                            update_sand(x, y);
                            break;
                        }
                        case WATER: {
                            update_water(x, y);
                            break;
                        }
                    }
                }
            }
        }

        for (int y = 0; y < mapHeight; y ++) {
            for (int x = 0; x < mapWidth; x ++) {
                if (tick) {
                    map[y][x] = mapBuffer[y][x];
                }
                CellID id = map[y][x];
                switch (id) {
                    case AIR: {
                        break;
                    }
                    case WALL: {
                        drawPoint({x, y}, {200, 200, 200});
                        break;
                    }
                    case SAND: {
                        drawPoint({x, y}, {200, 200, 50});
                        break;
                    }
                    case WATER: {
                        drawPoint({x, y}, {66, 155, 245});
                        break;
                    }
                }
            }
        }
        return true;
    }
};

int main() {
    App app;
    if (app.construct(1280, 720, app.mapWidth, app.mapHeight)) {
        app.init();
    }

    return 0;
}