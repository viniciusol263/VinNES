#include "VinNES.h"

namespace VinNES {

    VinNES::VinNES() : m_IOControllerHandler(std::make_shared<IOController::IOController>()), 
                       m_CPUInterpreterHandler(std::make_shared<CPUInterpreter::CPUInterpreter>())
    {
        auto running = true;
        auto startPeriod = std::chrono::high_resolution_clock::now();
        auto startDelayTimer = std::chrono::high_resolution_clock::now();
        SDL_Event keyEvent;
        uint32_t testColorMap[screenHeigth*screenWidth];
        for(auto counter = 0; counter < screenHeigth*screenWidth; counter++) testColorMap[counter] = 0xFFFF0000;
        while(running) {
            if(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - startDelayTimer).count() >= 16667) {
                startDelayTimer = std::chrono::high_resolution_clock::now();
                m_CPUInterpreterHandler->tickDelayTimer();
                m_CPUInterpreterHandler->tickSoundTimer();
            }
            if(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - startPeriod).count() >= 2000) {
                startPeriod = std::chrono::high_resolution_clock::now();
                m_CPUInterpreterHandler->FetchOpcode();
                m_IOControllerHandler->SetScreenTexture(m_CPUInterpreterHandler->GetVideoBuffer());
                m_IOControllerHandler->RenderScreen();            
            }
            while (SDL_PollEvent(&keyEvent)) {
                if (keyEvent.type == SDL_QUIT) {
                    running = false;
                }
            }
            
        }
    }

    VinNES::~VinNES() {}
}
