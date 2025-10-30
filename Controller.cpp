//
// Created by markk on 2025-06-09.
//
#include <iostream>
#include <windows.h>
#include <xinput.h>
#include <vector>
#include <conio.h>

class Xbox{
  private:
    XINPUT_STATE state;
    DWORD controllerIndex;
    bool connected;

  public:
    Xbox(DWORD index = 0) : controllerIndex(index), connected(false){
      ZeroMemory(&state,sizeof(XINPUT_STATE));
      checkConnection();
    }

    bool checkConnection(){
      DWORD result = XInputGetState(controllerIndex, &state);
      connected = (result == ERROR_SUCCESS);
      return connected;
    }

    bool update(){
      if (!checkConnection()) {
        return false;
      }
      DWORD result = XInputGetState(controllerIndex, &state);
      return (result == ERROR_SUCCESS);
    }

    bool isConnected() const {return connected;}

    float getLeftStickX() const {
        if (!connected) return 0.0f;
        return state.Gamepad.sThumbLX / 32767.0f;
    }

    float getLeftStickY() const {
        if (!connected) return 0.0f;
        return state.Gamepad.sThumbLY / 32767.0f;
    }

    float getRightStickX() const {
        if (!connected) return 0.0f;
        return state.Gamepad.sThumbRX / 32767.0f;
    }

    float getRightStickY() const {
        if (!connected) return 0.0f;
        return state.Gamepad.sThumbRY / 32767.0f;
    }
    // Trigger functions (returns 0.0 to 1.0)
    float getLeftTrigger() const {
        if (!connected) return 0.0f;
        return (state.Gamepad.bLeftTrigger / 255.0f) * -1;
    }

    float getRightTrigger() const {
        if (!connected) return 0.0f;
        return state.Gamepad.bRightTrigger / 255.0f;
    }
    // Button functions
    bool isButtonPressed(WORD button) const {
        if (!connected) return false;
        return (state.Gamepad.wButtons & button) != 0;
    }
    // The buttons
    bool isAPressed() const { return isButtonPressed(XINPUT_GAMEPAD_A); }
    bool isBPressed() const { return isButtonPressed(XINPUT_GAMEPAD_B); }
    bool isXPressed() const { return isButtonPressed(XINPUT_GAMEPAD_X); }
    bool isYPressed() const { return isButtonPressed(XINPUT_GAMEPAD_Y); }
    bool isStartPressed() const { return isButtonPressed(XINPUT_GAMEPAD_START); }
    bool isBackPressed() const { return isButtonPressed(XINPUT_GAMEPAD_BACK); }
    bool isLeftBumperPressed() const { return isButtonPressed(XINPUT_GAMEPAD_LEFT_SHOULDER); }
    bool isRightBumperPressed() const { return isButtonPressed(XINPUT_GAMEPAD_RIGHT_SHOULDER); }
    bool isLeftStickPressed() const { return isButtonPressed(XINPUT_GAMEPAD_LEFT_THUMB); }
    bool isRightStickPressed() const { return isButtonPressed(XINPUT_GAMEPAD_RIGHT_THUMB); }
    // D-pad functions
    bool isDPadUpPressed() const { return isButtonPressed(XINPUT_GAMEPAD_DPAD_UP); }
    bool isDPadDownPressed() const { return isButtonPressed(XINPUT_GAMEPAD_DPAD_DOWN); }
    bool isDPadLeftPressed() const { return isButtonPressed(XINPUT_GAMEPAD_DPAD_LEFT); }
    bool isDPadRightPressed() const { return isButtonPressed(XINPUT_GAMEPAD_DPAD_RIGHT); }

    // Vibrations
    void setVibration(float leftMotor, float rightMotor) {
        if (!connected) return;
        XINPUT_VIBRATION vibration;
        vibration.wLeftMotorSpeed = (WORD)(leftMotor * 65535);
        vibration.wRightMotorSpeed = (WORD)(rightMotor * 65535);
        XInputSetState(controllerIndex, &vibration);
    }
    // Deadzone
    float applyDeadzone(float value, float deadzone = 0.1f) const {
        if (std::abs(value) < deadzone) {
            return 0.0f;
        }
        float sign = (value > 0) ? 1.0f : -1.0f;
        return sign * ((std::abs(value) - deadzone) / (1.0f - deadzone));
    }

    // Print Everything
    void printStatus() const {
        if (!connected) {
            std::cout << "\rController " << controllerIndex << " not connected!" << std::endl;
            return;
        }
        // Print analog stick values with deadzone applied
        float leftX = applyDeadzone(getLeftStickX());
        float leftY = applyDeadzone(getLeftStickY());
        float rightX = applyDeadzone(getRightStickX());
        float rightY = applyDeadzone(getRightStickY());
        std::cout << "\rLeft Stick: (" << leftX << ", " << leftY << ") ";
        std::cout << "Right Stick: (" << rightX << ", " << rightY << ") ";
        std::cout << "Triggers: L=" << getLeftTrigger() << " R=" << getRightTrigger() << " ";
        // Print pressed buttons
        if (isAPressed()) std::cout << "[A] ";
        if (isBPressed()) std::cout << "[B] ";
        if (isXPressed()) std::cout << "[X] ";
        if (isYPressed()) std::cout << "[Y] ";
        if (isStartPressed()) std::cout << "[START] ";
        if (isBackPressed()) std::cout << "[BACK] ";
        if (isLeftBumperPressed()) std::cout << "[LB] ";
        if (isRightBumperPressed()) std::cout << "[RB] ";
        if (isLeftStickPressed()) std::cout << "[L3] ";
        if (isRightStickPressed()) std::cout << "[R3] ";
        if (isDPadUpPressed()) std::cout << "[DU]";
        if (isDPadDownPressed()) std::cout << "[DD]";
        if (isDPadLeftPressed()) std::cout << "[DL]";
        if (isDPadRightPressed()) std::cout << "[DR]";
        std::cout << "                    " << std::flush;
    }
};

class Serial{
    private:
        HANDLE hSerial = INVALID_HANDLE_VALUE;
        DCB dcbSerial = {0};
    
    public:
        Serial() {}

        void open(std::string s){
            std::string port = "\\\\.\\" + s;
            hSerial = CreateFileA(
                port.c_str(),
                GENERIC_READ | GENERIC_WRITE,  // Add READ access too
                0,
                NULL,
                OPEN_EXISTING,
                0,
                NULL
            );

            if (hSerial == INVALID_HANDLE_VALUE) {
                DWORD error = GetLastError();
                std::cerr << "Failed to open " << s << " Error code: " << error << std::endl;
                return; // Don't continue if port didn't open
            }
            std::cout << "Successfully opened " << s << std::endl;
        }

        void config(){
            dcbSerial.DCBlength = sizeof(dcbSerial);
            if (!GetCommState(hSerial, &dcbSerial)) {
                std::cerr << "GetCommState failed.\n";
                return;
            }

            dcbSerial.BaudRate = CBR_9600;   // Match Arduino's Serial.begin(9600);
            dcbSerial.ByteSize = 8;          // 8 bits
            dcbSerial.StopBits = ONESTOPBIT;
            dcbSerial.Parity   = NOPARITY;
            if (!SetCommState(hSerial, &dcbSerial)) {
                std::cerr << "SetCommState failed.\n";
            }
        }

        void send(std::string val){
            DWORD bytesWritten;
            WriteFile(hSerial, val.c_str(), val.length(), &bytesWritten, NULL);
        }

        void close(){
            if (hSerial != INVALID_HANDLE_VALUE) {
                CloseHandle(hSerial);
            }
        }

        void clearConsole(){
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            CONSOLE_SCREEN_BUFFER_INFO csbi;
            DWORD count;
            DWORD cellCount;
            COORD homeCoords = {0, 0};

            if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) exit(1);

            cellCount = csbi.dwSize.X * csbi.dwSize.Y;

            FillConsoleOutputCharacter(hConsole, ' ', cellCount, homeCoords, &count);
            FillConsoleOutputAttribute(hConsole, csbi.wAttributes, cellCount, homeCoords, &count);
            SetConsoleCursorPosition(hConsole, homeCoords);
        }

        std::string show_ports(){
            char devices[65536];
            DWORD charsReturned = QueryDosDeviceA(NULL, devices, sizeof(devices));
            std::vector<std::string> ports;

            char* currentDevice = devices;
            while (*currentDevice) {
                std::string deviceName(currentDevice);
                if (deviceName.rfind("COM", 0) == 0) {
                    ports.push_back(deviceName);
                }
                currentDevice += deviceName.size() + 1;
            }
            if (ports.empty()) {
                std::cout << "No serial ports found.\n";
                return "";
            }

            int selectedIndex = 0;

            while (true) {
                clearConsole();
                std::cout << "XInput Controller Test - Press Ctrl+C to exit" << std::endl;
                std::cout << "Connect your Xbox controller..." << std::endl << std::endl;
                for (size_t i = 0; i < ports.size(); ++i) {
                    if ((int)i == selectedIndex) {
                        std::cout << "> " << ports[i] << " <" << '\n';
                    } else {
                        std::cout << "  " << ports[i] << '\n';
                    }
                }
                int key = _getch();
                if (key == 224) {  // Special keys (arrow keys)
                int arrow = _getch();
                if (arrow == 72) {         // Up arrow
                    selectedIndex = (selectedIndex - 1 + ports.size()) % ports.size();
                } else if (arrow == 80) {  // Down arrow
                    selectedIndex = (selectedIndex + 1) % ports.size();
                }
                } else if (key == 13) {  // Enter
                    std::cout << "\nYou selected: " << ports[selectedIndex] << "\n";
                    break;
                }
            }
            return ports[selectedIndex];
        }

        ~Serial() {
            close();
        }

};

int main() {
    Xbox xbox(0); // Controller 0 (first controller)

    std::cout << "XInput Controller Test - Press Ctrl+C to exit" << std::endl;
    std::cout << "Connect your Xbox controller..." << std::endl;

    Serial ser;
    std::string user_port = ser.show_ports();
    ser.open(user_port);
    ser.config();
    Sleep(2000);

    bool running = true;
    while (running) {
        if (xbox.update()) {
            xbox.printStatus();
            // Send data to Arduino
            ser.send("Trig:" + std::to_string(xbox.getLeftTrigger() + xbox.getRightTrigger()) + 
                     " LX:" + std::to_string(xbox.getLeftStickX()) +
                     " LY:" + std::to_string(xbox.getLeftStickY()) +
                     " RX:" + std::to_string(xbox.getRightStickX()) +
                     " RY:" + std::to_string(xbox.getRightStickY()) +
                     " L3:" + std::to_string(xbox.isLeftStickPressed()) +
                     " R3:" + std::to_string(xbox.isRightStickPressed()) +
                     " A:" + std::to_string(xbox.isAPressed()) +
                     " B:" + std::to_string(xbox.isBPressed()) +
                     " X:" + std::to_string(xbox.isYPressed()) +
                     " Y:" + std::to_string(xbox.isXPressed()) +
                     " DD:" + std::to_string(xbox.isDPadDownPressed()) +
                     " DU:" + std::to_string(xbox.isDPadUpPressed()) +
                     " DL:" + std::to_string(xbox.isDPadLeftPressed()) +
                     " DR:" + std::to_string(xbox.isDPadRightPressed()) +
                     " LB:" + std::to_string(xbox.isLeftBumperPressed()) +
                     " RB:" + std::to_string(xbox.isRightBumperPressed()) +
                     "\n");
            // Example: Exit when back button is pressed
            if (xbox.isBackPressed()) {
                std::cout << "Back button pressed - exiting..." << std::endl;
                running = false;
            }

            // Example: Vibration when A button is pressed
            if (xbox.isAPressed()) {
                xbox.setVibration(0.1f, 0.9f); // 50% vibration
            } else {
                xbox.setVibration(0.0f, 0.0f); // Stop vibration
            }
        } else {
            std::cout << "Waiting for controller connection..." << std::endl;
        }

        Sleep(100); // 100ms delay
    }
    ser.close();
    return 0;
}