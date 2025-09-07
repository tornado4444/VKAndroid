#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <SDL3/SDL_main.h>

#include <vulkan/vulkan.h>

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

class Window {
public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }
public:
    SDL_AppResult initWindow(){
        if(!SDL_Init(SDL_INIT_VIDEO)){
            SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
            return SDL_APP_FAILURE;
        }

        if(!SDL_CreateWindowAndRenderer("Vulkan Triangle", WIDTH, HEIGHT, NULL, &window, &renderer)) {
            SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
            return SDL_APP_FAILURE;
        }

        return SDL_APP_CONTINUE;
    }

    void initVulkan() {

    }

    void mainLoop() {

    }

    void cleanup() {

    }
};

void SDL_AppQuit(void* appstate, SDL_AppResult result) {

}