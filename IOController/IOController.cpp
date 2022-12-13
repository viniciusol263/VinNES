#include "IOController.h"

namespace IOController {

    IOController::IOController() :
     m_window(nullptr), m_renderer(nullptr), m_texture(nullptr)
    {
        SDL_Event event;
        SDL_SetTextureBlendMode(m_texture, SDL_BLENDMODE_BLEND);

        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            printf("could not initialize sdl2: %s\n", SDL_GetError());
        }
        SDL_CreateWindowAndRenderer(screenWidth, screenHeigth, 0, &m_window, &m_renderer);
        SDL_SetWindowSize(m_window, screenWidth*10, screenHeigth*15);
        SDL_RenderSetScale(m_renderer, 10.0, 15.0);
        m_texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, screenWidth, screenHeigth);
        for(auto iterator = 0; iterator < screenWidth*screenHeigth; iterator++) {
           m_textureBuffer[iterator] = 0xFF000000;
        }

        SDL_UpdateTexture(m_texture, NULL, &m_textureBuffer, screenWidth*sizeof(uint32_t));

        SDL_RenderCopy(m_renderer, m_texture, NULL, &renderRect);

        SDL_RenderPresent(m_renderer);

    }

    IOController::~IOController() 
    {
        free(m_window);
        free(m_renderer);
        free(m_texture);

        SDL_DestroyRenderer(m_renderer);
        SDL_DestroyWindow(m_window);
        SDL_Quit();
    }

    Utils::ErrorCode IOController::SetPixelColor(uint32_t xPos, uint32_t yPos, uint32_t color) {
        if(xPos > screenWidth || yPos > screenHeigth) return Utils::ErrorCode::RETURN_OUT_OF_BOUND_ARRAY;
        m_textureBuffer[xPos + (yPos * screenWidth)] = color;
        return Utils::ErrorCode::RETURN_OK;
    }

    Utils::ErrorCode IOController::SetScreenTexture(uint8_t *pixelBuffer) {
        if(pixelBuffer == nullptr) return Utils::ErrorCode::RETURN_NO_RESOURCE_ON_POINTER;
        for(auto counter = 0; counter < 64*32; counter ++) {
            for(auto bitcounter = 0; bitcounter < 8; bitcounter++) {               
                m_textureBuffer[counter + bitcounter] = (((pixelBuffer[counter] >> (bitcounter)) & 0b1) == 0b1) ? 0xFFFFFFFF : 0xFF000000;
            }
            
        }
        return Utils::ErrorCode::RETURN_OK;
    }


    Utils::ErrorCode IOController::RenderScreen() {
        SDL_UpdateTexture(m_texture, NULL, &m_textureBuffer, screenWidth*sizeof(uint32_t));
        SDL_RenderCopy(m_renderer, m_texture, NULL, &renderRect);
        SDL_RenderPresent(m_renderer);
        return Utils::ErrorCode::RETURN_OK;
    }

    SDL_KeyboardEvent IOController::GetKeyboardEvent() {
        SDL_Event event;
        SDL_PollEvent(&event);
        return event.key;
    }

};