#include "CPUInterpreter.h"

namespace CPUInterpreter{
    CPUInterpreter::CPUInterpreter()
    {
        size_t romSize;

        memset(m_RAM, 0, ramSize);
        memset(m_VideoBuffer, 0, videoBufferSize);
        memset(&m_CpuRegisters, 0, sizeof(m_CpuRegisters));

        FILE *romFile = fopen("spaceinvaders.ch8", "rb");
        fseek(romFile, 0, SEEK_END);
        romSize = ftell(romFile);
        fseek(romFile, 0, SEEK_SET);
        std::cout << "File size is " << romSize;
        m_romData = new uint8_t[romSize];
        fread(m_romData, sizeof(uint8_t), romSize, romFile);
        fclose(romFile);
        m_CpuRegisters.PC = 0x200;
        m_CpuRegisters.SP = 0x0;
        memcpy(&m_RAM[0x50], Utils::CHIP8Fonts, CHIP8FontsSize);
        memcpy(&m_RAM[m_CpuRegisters.PC], m_romData, romSize);
    }

    CPUInterpreter::~CPUInterpreter() {
        delete m_romData;
    }

    Utils::ErrorCode CPUInterpreter::UpdateKeyState(SDL_KeyboardEvent keyEvent) {
        m_keyEvent = keyEvent;
        std::cout << "UPDATE KEY STATE " << m_keyEvent.keysym.sym << "\n";
        return Utils::ErrorCode::RETURN_OK;
    }

    Utils::ErrorCode CPUInterpreter::FetchOpcode() {
        uint16_t opcode = m_RAM[m_CpuRegisters.PC] << 8 | m_RAM[m_CpuRegisters.PC+1];
        switch (opcode & 0xF000)
        {
        case 0x0000:
            switch(opcode & 0xFF) {
                case 0xE0: //CLR
                    memset(m_VideoBuffer, 0, videoBufferSize);
                    m_CpuRegisters.PC += 2;
                    // std::cout << "CLR" << std::endl;
                    break;
                case 0xEE: //RET
                    m_CpuRegisters.PC = m_CpuRegisters.S[m_CpuRegisters.SP--];
                    // std::cout << "RET" << std::endl;
                    break;
            }
            break;
        case 0x1000: // JP NNN | 1NNN
            m_CpuRegisters.PC = opcode & 0x0FFF;
            //printf("JP %03X\n", m_CpuRegisters.PC);
            break;
        case 0x2000: // CALL addr | 2NNN
            //printf("CALL %03X\n", opcode & 0x0FFF);
            m_CpuRegisters.S[++m_CpuRegisters.SP] = m_CpuRegisters.PC + 2;
            m_CpuRegisters.PC = opcode & 0x0FFF;
            break;
        case 0x3000: // SE Vx, byte | 3XKK
            //printf("SE V%01X, %02X\n", (opcode >> 8) & 0x0F, opcode & 0x00FF);
            if(m_CpuRegisters.V[(opcode >> 8) & 0x0F] == (opcode & 0x00FF))
                m_CpuRegisters.PC += 2;
            m_CpuRegisters.PC += 2;
            break;
        case 0x4000: // SNE Vx, byte | 4XKK
            //printf("SNE V%01X, %02X\n", (opcode >> 8) & 0x0F, opcode & 0x00FF);
            if(m_CpuRegisters.V[(opcode >> 8) & 0x0F] != (opcode & 0x00FF))
                m_CpuRegisters.PC += 2;
            m_CpuRegisters.PC += 2;
            break;
        case 0X5000: // SE Vx, Vy | 5XY0
            //printf("SE V%01X, V%01X\n", (opcode >> 8) & 0x0F, (opcode >> 4) & 0x0F);
            if(m_CpuRegisters.V[(opcode >> 8) & 0x0F] == m_CpuRegisters.V[(opcode >> 4) & 0x0F])
                m_CpuRegisters.PC += 2;
            m_CpuRegisters.PC += 2;
            break;
        case 0x6000: // LD Vx, byte | 6XKK
            m_CpuRegisters.V[(opcode >> 8) & 0x0F] = opcode & 0x00FF;
            //printf("ADDRESS %04X | LD V%01X, %02X\n", m_CpuRegisters.PC, (opcode >> 8) & 0x0F, opcode & 0x00FF);
            m_CpuRegisters.PC += 2;
            break;
        case 0x7000: // ADD Vx, byte | 7XKK
            m_CpuRegisters.V[(opcode >> 8) & 0x0F] = m_CpuRegisters.V[(opcode >> 8) & 0x0F] + (opcode & 0x00FF);
            //printf("ADDRESS %04X | ADD V%01X, %02X\n", m_CpuRegisters.PC, (opcode >> 8) & 0x0F, (opcode & 0x00FF));
            m_CpuRegisters.PC += 2;
            break;
        case 0x8000: 
            switch(opcode & 0x0F) {
                case 0x0: // LD Vx, Vy | 8XY0
                    //printf("LD V%01X, V%01X\n", (opcode >> 8) & 0x0F, (opcode >> 4) & 0x0F);
                    m_CpuRegisters.V[(opcode >> 8) & 0x0F] = m_CpuRegisters.V[(opcode >> 4) & 0x0F];
                    m_CpuRegisters.PC += 2;
                    break;
                case 0x1: // OR Vx, Vy | 8XY1
                    //printf("OR V%01X, V%01X\n", (opcode >> 8) & 0x0F, (opcode >> 4) & 0x0F);
                    m_CpuRegisters.V[(opcode >> 8) & 0x0F] |= m_CpuRegisters.V[(opcode >> 4) & 0x0F];
                    m_CpuRegisters.PC += 2;
                    break;
                case 0x2: // AND Vx, Vy | 8XY2
                    //printf("AND V%01X, V%01X\n", (opcode >> 8) & 0x0F, (opcode >> 4) & 0x0F);
                    m_CpuRegisters.V[(opcode >> 8) & 0x0F] &= m_CpuRegisters.V[(opcode >> 4) & 0x0F];
                    m_CpuRegisters.PC += 2;
                    break;
                case 0x3: // XOR Vx, Vy | 8XY3
                    //printf("XOR V%01X, V%01X\n", (opcode >> 8) & 0x0F, (opcode >> 4) & 0x0F);
                    m_CpuRegisters.V[(opcode >> 8) & 0x0F] ^= m_CpuRegisters.V[(opcode >> 4) & 0x0F];
                    m_CpuRegisters.PC += 2;
                    break;
                case 0x4: // ADD Vx, Vy | 8XY4
                    //printf("ADD V%01X, V%01X\n", (opcode >> 8) & 0x0F, (opcode >> 4) & 0x0F);
                    m_CpuRegisters.V[0xF] = 0;
                    if(m_CpuRegisters.V[(opcode >> 8) & 0x0F] + m_CpuRegisters.V[(opcode >> 4) & 0x0F] > sizeof(uint8_t))
                        m_CpuRegisters.V[0xF] = 1;
                    m_CpuRegisters.V[(opcode >> 8) & 0x0F] += m_CpuRegisters.V[(opcode >> 4) & 0x0F];
                    m_CpuRegisters.V[(opcode >> 8) & 0x0F] &= 0xFF;
                    m_CpuRegisters.PC += 2;
                    break;
                case 0x5: // SUB Vx, Vy | 8XY5
                    //printf("SUB V%01X, V%01X\n", (opcode >> 8) & 0x0F, (opcode >> 4) & 0x0F);
                    m_CpuRegisters.V[0xF] = 0;
                    if(m_CpuRegisters.V[(opcode >> 8) & 0x0F] > m_CpuRegisters.V[(opcode >> 4) & 0x0F])
                        m_CpuRegisters.V[0xF] = 1;
                    m_CpuRegisters.V[(opcode >> 8) & 0x0F] -= m_CpuRegisters.V[(opcode >> 4) & 0x0F];
                    m_CpuRegisters.PC += 2;
                    break;
                case 0x6: // SHR Vx{, Vy} | 8XY6
                    //printf("SHR V%01X, V%01X\n", (opcode >> 8) & 0x0F, (opcode >> 4) & 0x0F);
                    m_CpuRegisters.V[0xF] = 0;
                    if((m_CpuRegisters.V[(opcode >> 8) & 0x0F] & 0b1) == 1)
                        m_CpuRegisters.V[0xF] = 1;
                    m_CpuRegisters.V[(opcode >> 8) & 0x0F] /= 2;
                    m_CpuRegisters.PC += 2;
                    break;
                case 0x7: // SUBN Vx, Vy | 8XY7
                    //printf("SUBN V%01X, V%01X\n", (opcode >> 8) & 0x0F, (opcode >> 4) & 0x0F);
                    m_CpuRegisters.V[0xF] = 0;
                    if(m_CpuRegisters.V[(opcode >> 4) & 0x0F] > m_CpuRegisters.V[(opcode >> 8) & 0x0F])
                        m_CpuRegisters.V[0xF] = 1;
                    m_CpuRegisters.V[(opcode >> 8) & 0x0F] = m_CpuRegisters.V[(opcode >> 4) & 0x0F] - m_CpuRegisters.V[(opcode >> 8) & 0x0F];
                    m_CpuRegisters.PC += 2;
                    break;
                case 0xE: // SHL Vx{, Vy} | 8XYE
                    //printf("SHL V%01X, V%01X\n", (opcode >> 8) & 0x0F, (opcode >> 4) & 0x0F);
                    m_CpuRegisters.V[0xF] = 0;
                    if((m_CpuRegisters.V[(opcode >> 8) & 0x0F] & 0x80) == 0x80)
                        m_CpuRegisters.V[0xF] = 1;
                    m_CpuRegisters.V[(opcode >> 8) & 0x0F] *= 2;
                    m_CpuRegisters.PC += 2;
                    break;
            }
            break;
        case 0x9000: // SNE Vx, Vy | 9XY0
            if(m_CpuRegisters.V[(opcode >> 8) & 0x0F] != m_CpuRegisters.V[(opcode >> 4) & 0x0F])
                m_CpuRegisters.PC += 2;
            m_CpuRegisters.PC += 2;
            break;
        case 0xA000: // LD I, addr | ANNN
            m_CpuRegisters.I = opcode & 0x0FFF;
            //printf("PC %04X |LD I, %03X\n", m_CpuRegisters.PC, opcode & 0x0FFF);
            m_CpuRegisters.PC += 2;
            break;
        case 0xD000: // DRW Vx, Vy, nibble | DXYN
        {
            //printf("DRW V%01X, V%01X, %01X\n", (opcode >> 8) & 0x0F, (opcode >> 4) & 0x0F, opcode & 0x0F);
            m_CpuRegisters.V[0x0F] = 0;
            for(int blit = 0; blit < (opcode & 0x000F); blit++) {
                for(int bit = 0; bit < 8; bit++) {
                    if(m_VideoBuffer[bit + m_CpuRegisters.V[((opcode >> 8) & 0x0F)] + videoWidth*(m_CpuRegisters.V[(opcode >> 4) & 0x0F] + blit)]) m_CpuRegisters.V[0x0F] = 1;
                    m_VideoBuffer[bit + m_CpuRegisters.V[((opcode >> 8) & 0x0F)] + videoWidth*(m_CpuRegisters.V[(opcode >> 4) & 0x0F] + blit)] ^= (m_RAM[(m_CpuRegisters.I) + blit] & (0x80 >> bit)) ? 1 : 0;
                }
            }
            m_CpuRegisters.PC += 2;
            break;
        }
        case 0xE000:
        {
            switch(opcode & 0xFF) {
                case 0x9E: //SKP Vx | EX9E
                {   
                    SDL_PumpEvents();
                    if(SDL_GetKeyboardState(NULL)[Utils::keyButtons[m_CpuRegisters.V[(opcode >> 8) & 0x0F]]] != 0) {
                            m_CpuRegisters.PC += 2;
                    }
                    m_CpuRegisters.PC += 2;
                    break;
                }
                case 0xA1: //SKNP Vx | EXA1
                {                       
                    SDL_PumpEvents();
                    //printf("PC %04X | SKNP V%01X\n", m_CpuRegisters.PC, (opcode >> 8)&0x0F);
                    if((!SDL_GetKeyboardState(NULL)[m_CpuRegisters.V[Utils::keyButtons[m_CpuRegisters.V[(opcode >> 8) & 0x0F]]]])) {
                            m_CpuRegisters.PC += 2;
                    }
                    // m_CpuRegisters.PC += 2;
                    break;
                }
            }
            break;
        }
        case 0xF000:
        {   
            switch(opcode & 0xFF)
            {
                case 0x07: // LD Vx, DT | FX07
                {
                    //printf("LD V%01X, DT\n", (opcode >> 8)&0x0F);
                    m_CpuRegisters.V[(opcode >> 8) & 0x0F] = m_CpuRegisters.DT;
                    m_CpuRegisters.PC += 2;
                    break;
                }
                case 0x0A: //LD Vx, K | FX0A
                {   
                    SDL_Event event;
                    //printf("LD Vx%01X, K\n", (opcode >> 8)&0x0F);
                    while(m_keyEvent.state != SDL_PRESSED) {
                        SDL_PollEvent(&event);
                        m_keyEvent = event.key;
                    };
                    for(auto counter = 0; counter < 16; counter++)
                        if(Utils::keyButtons[counter] == m_keyEvent.keysym.sym)
                            m_CpuRegisters.V[(opcode >> 8)&0x0F] = counter;
                    m_CpuRegisters.PC += 2;
                    break;
                }
                case 0x15: // LD DT, Vx | FX15
                {
                    //printf("LD DT, V%01X\n", (opcode >> 8)&0x0F);
                    m_CpuRegisters.DT = m_CpuRegisters.V[(opcode >> 8) & 0x0F];
                    m_CpuRegisters.PC += 2;
                    break;
                }
                case 0x18: // LD ST, Vx | FX18
                {
                    //printf("LD ST, V%01X\n", (opcode >> 8)&0x0F);
                    m_CpuRegisters.ST = m_CpuRegisters.V[(opcode >> 8) & 0x0F];
                    m_CpuRegisters.PC += 2;
                    break;
                }
                case 0x1E: // ADD I, Vx | FX1E
                {
                    //printf("ADD I, V%01X\n", (opcode >> 8)&0x0F);
                    m_RAM[m_CpuRegisters.I] += m_CpuRegisters.V[(opcode >> 8) & 0x0F];
                    m_CpuRegisters.PC += 2;
                    break;
                }
                case 0x33: // LD B, Vx | FX33
                {
                    //printf("LD B, V%01X\n", (opcode >> 8)&0x0F);
                    uint8_t V = m_CpuRegisters.V[(opcode >> 8) & 0x0F];
                    m_RAM[m_CpuRegisters.I] = V - V%100;
                    m_RAM[m_CpuRegisters.I + 1] = V%100 - V%10;
                    m_RAM[m_CpuRegisters.I + 2] = V%10;
                    //printf("BCD %d %1d %1d %1d", V, m_RAM[m_CpuRegisters.I], m_RAM[m_CpuRegisters.I + 1], m_RAM[m_CpuRegisters.I + 2]);
                    m_CpuRegisters.PC += 2;
                    break;
                }
                case 0x55: // LD [I], Vx | FX55
                {
                    //printf("LD [I], V%01X\n", (opcode >> 8)&0x0F);
                    for(auto counter = 0; counter <= (opcode >> 8)&0x0F; counter++)
                        m_RAM[m_CpuRegisters.I + counter] = m_CpuRegisters.V[counter];
                    m_CpuRegisters.PC += 2;
                    break;
                }
                case 0x65: // LD Vx, [I] | FX55
                {
                    //printf("LD V%01X, [I]\n", (opcode >> 8)&0x0F);
                    for(auto counter = 0; counter <= (opcode >> 8)&0x0F; counter++)
                         m_CpuRegisters.V[counter] = m_RAM[m_CpuRegisters.I + counter];
                    m_CpuRegisters.PC += 2;
                    break;
                }
                default:
                    //printf("Unknow opcode %04X detected\n", opcode);
                    SDL_Delay(2000);
                    return Utils::ErrorCode::RETURN_UNHANDLED_ERROR;
                    break;
            }
            break;
        }
        default:
            //printf("Unknow opcode %04X detected\n", opcode);
            SDL_Delay(2000);
            return Utils::ErrorCode::RETURN_UNHANDLED_ERROR;
        }
        return Utils::ErrorCode::RETURN_OK;
    }


    uint8_t* CPUInterpreter::GetVideoBuffer() {
        return m_VideoBuffer;
    }

    void CPUInterpreter::tickDelayTimer() {
        if(m_CpuRegisters.DT == 0) return;
        m_CpuRegisters.DT--;
    }
    void CPUInterpreter::tickSoundTimer() {
        if(m_CpuRegisters.ST == 0) return;
        printf("\a");
        m_CpuRegisters.ST--;
    }

};