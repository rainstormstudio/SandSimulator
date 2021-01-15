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

    // water
    std::vector<std::vector<float>> mass;
    std::vector<std::vector<float>> massBuffer;
    const float maxMass = 1.0;
    const float maxCompress = 0.02;
    const float minMass = 0.0001;
    const float minFlow = 0.01;
    const float maxSpeed = 1.0;

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

    float calcFlow(float totalMass) {
        if (totalMass <= 1.0) {
            return 1;
        } else if (totalMass < 2 * maxMass + maxCompress) {
            return (maxMass * maxMass + totalMass * maxCompress) / (maxMass + maxCompress);
        } else {
            return (totalMass + maxCompress) / 2;
        }
    }

    inline float constrain(float x, float min, float max) {
        if (x < min) {
            return min;
        } else if (x > max) {
            return max;
        } else {
            return x;
        }
    }

    void update_water(int x, int y) {
        float flow = 0.0;
        float remainingMass = mass[y][x];
        if (remainingMass <= 0.0) return;

        // below
        if (map[y + 1][x] == AIR || map[y + 1][x] == WATER) {
            flow = calcFlow(remainingMass + mass[y + 1][x]) - mass[y + 1][x];
            if (flow > minFlow) {
                flow *= 0.5;
            }
            flow = constrain(flow, 0, std::min(maxSpeed, remainingMass));
            massBuffer[y][x] -= flow;
            massBuffer[y + 1][x] += flow;
            remainingMass -= flow;
        }

        if (remainingMass <= 0.0) return;

        // right
        if (map[y][x + 1] == AIR || map[y][x + 1] == WATER) {
            flow = (mass[y][x] - mass[y][x + 1]) / 4.0;
            if (flow > minFlow) {
                flow *= 0.5;
            }
            flow = constrain(flow, 0, remainingMass);

            massBuffer[y][x] -= flow;
            massBuffer[y][x + 1] += flow;
            remainingMass -= flow;
        }

        if (remainingMass <= 0.0) return;

        // left
        if (map[y][x - 1] == AIR || map[y][x - 1] == WATER) {
            flow = (mass[y][x] - mass[y][x - 1]) / 4.0;
            if (flow > minFlow) {
                flow *= 0.5;
            }
            flow = constrain(flow, 0, remainingMass);

            massBuffer[y][x] -= flow;
            massBuffer[y][x - 1] += flow;
            remainingMass -= flow;
        }

        if (remainingMass <= 0.0) return;

        // up
        if (map[y - 1][x] == AIR || map[y - 1][x] == WATER) {
            flow = remainingMass - calcFlow(remainingMass + mass[y - 1][x]);
            if (flow > minFlow) {
                flow *= 0.5;
            }
            flow = constrain(flow, 0, std::min(maxSpeed, remainingMass));

            massBuffer[y][x] -= flow;
            massBuffer[y - 1][x] += flow;
            remainingMass -= flow;
        }
    }
public:
    const uint32_t mapWidth = 80;
    const uint32_t mapHeight = 60;

    bool onCreate() override {
        windowTitle = "Sand Simulator";
        map = std::vector<std::vector<CellID>>(mapHeight);
        mapBuffer = std::vector<std::vector<CellID>>(mapHeight);
        mass = std::vector<std::vector<float>>(mapHeight);
        massBuffer = std::vector<std::vector<float>>(mapHeight);
        for (int y = 0; y < mapHeight; y ++) {
            map[y] = std::vector<CellID>(mapWidth);
            mapBuffer[y] = std::vector<CellID>(mapWidth);
            mass[y] = std::vector<float>(mapWidth);
            massBuffer[y] = std::vector<float>(mapWidth);
            for (int x = 0; x < mapWidth; x ++) {
                map[y][x] = AIR;
                if (y == mapHeight - 1 || x == 0 || x == mapWidth - 1) {
                    map[y][x] = WALL;
                }
                mapBuffer[y][x] = AIR;
                mass[y][x] = 0.0;
                massBuffer[y][x] = 0.0;
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
            mass[mousePosY][mousePosX] = 1.0;
            //tick = true;
        }

        if (tick) {
            for (int y = 0; y < mapHeight; y ++) {
                for (int x = 0; x < mapWidth; x ++) {
                    mapBuffer[y][x] = AIR;
                    massBuffer[y][x] = mass[y][x];
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
            for (int y = 0; y < mapHeight; y ++) {
                for (int x = 0; x < mapWidth; x ++) {
                    mass[y][x] = massBuffer[y][x];
                    if (mapBuffer[y][x] == WALL) continue;
                    if (mass[y][x] > minMass) {
                        mapBuffer[y][x] = WATER;
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
                        drawPoint({x, y}, {66, 155, 245, constrain(255 * mass[y][x], 0, 255)});
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