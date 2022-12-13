#ifndef CPU_INTERPRETER_H_
#define CPU_INTERPRETER_H_

#include <memory>
#include <iostream>
#include <cstring>

#include "Utils/Utils.h"
#include "SDL2/SDL.h"

const size_t ramSize = 4096;
const size_t videoWidth = 64;
const size_t videoHeigth = 32;
const size_t videoBufferSize = 64*32;

namespace CPUInterpreter {

    class CPUInterpreter {
      public:
      CPUInterpreter();
      ~CPUInterpreter();  
      Utils::ErrorCode FetchOpcode();
      uint8_t* GetVideoBuffer();
      void tickDelayTimer();

      private:
      uint8_t m_RAM[ramSize];
      uint8_t m_VideoBuffer[videoBufferSize];
      struct {
          uint16_t PC; //Program Counter
          uint16_t I; //Index Register
          uint16_t SP; // Stack Pointer
          uint8_t DT; // Delay Timer
          uint8_t ST; // Sound Timer
          uint8_t V[0x10]; // Variable Registers
          uint16_t S[0x10]; // Stack
      } m_CpuRegisters;
      uint8_t* m_romData;
    };
}

using CPUInterpreterPtr = std::shared_ptr<CPUInterpreter::CPUInterpreter>;

#endif