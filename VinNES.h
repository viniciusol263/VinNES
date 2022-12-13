#ifndef _VINNES_H_
#define _VINNES_H

#include <iostream>
#include <memory>
#include <chrono>

#include "IOController/IOController.h"
#include "CPUInterpreter/CPUInterpreter.h"
#include "Utils/Utils.h"
#include "SDL2/SDL.h"


namespace VinNES {
    class VinNES {
        public:
        VinNES();
        ~VinNES();
        
        private:
        IOControllerPtr m_IOControllerHandler;
        CPUInterpreterPtr m_CPUInterpreterHandler;
        SDL_Keycode m_currentKey;
    };

}

#endif 
