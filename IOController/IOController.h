#ifndef IO_CONTROLLER_H
#define IO_CONTROLLER_H

#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include "SDL2/SDL_render.h"
#include "Utils/Utils.h"
#include <iostream>
#include <memory>


const int screenWidth = 64;
const int screenHeigth = 32;
const SDL_Rect renderRect = { .x = 0,  .y = 0, .w = screenWidth, .h = screenHeigth };
const SDL_Rect realRenderRect = { .x = 0,  .y = 0, .w = screenWidth * 10, .h = screenHeigth * 15};

namespace IOController {
    class IOController {
        public:
        IOController();
        ~IOController();
        Utils::ErrorCode SetPixelColor(uint32_t xPos, uint32_t yPos, uint32_t color);
        Utils::ErrorCode SetScreenTexture(uint8_t *pixelBuffer);
        Utils::ErrorCode RenderScreen();
        SDL_KeyboardEvent GetKeyboardEvent();

        private:
        SDL_Window* m_window;
        SDL_Renderer* m_renderer;
        SDL_Texture *m_texture;
        uint32_t m_textureBuffer[screenWidth*screenHeigth];
    };

}
using IOControllerPtr = std::shared_ptr<IOController::IOController>;

#endif