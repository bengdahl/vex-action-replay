#ifndef ACTION_REPLAY_H
#define ACTION_REPLAY_H
// BEGIN ACTION_REPLAY API

#include <fstream>
#include <cstdio>
#include <string>
#include <cstring>

#ifndef INPUT_RECORD_PATH
#define INPUT_RECORD_PATH "controller_input.csv"
#endif

#ifndef INPUT_DELAY
#define INPUT_DELAY 30
#endif

class ActionReplay: private vex::controller {
    private:
        
    public:
        int emuStep = 0;
        int recordStep = 0;
        bool emulated = false;
        std::string inputFilePath;
        vex::brain arBrain;
        ActionReplay(vex::brain arBrain, std::string inputPath) : vex::controller(), inputFilePath(inputPath), arBrain(arBrain) { };
        vex::controller::lcd Screen;
    
        int32_t emuA1;
        int32_t Axis1Value(){ return emulated ? emuA1 : Axis1.value(); };
        int32_t Axis1Position(vex::percentUnits units) { return floor((100.0/127.0)*Axis1Value()); };
    
        int32_t emuA2;
        int32_t Axis2Value(){ return emulated ? emuA2 : Axis2.value(); };
        int32_t Axis2Position(vex::percentUnits units) { return floor((100.0/127.0)*Axis2Value()); };
    
        int32_t emuA3;
        int32_t Axis3Value(){ return emulated ? emuA3 : Axis3.value(); };
        int32_t Axis3Position(vex::percentUnits units) { return floor((100.0/127.0)*Axis3Value()); };
    
        int32_t emuA4;
        int32_t Axis4Value(){ return emulated ? emuA4 : Axis4.value(); };
        int32_t Axis4Position(vex::percentUnits units) { return floor((100.0/127.0)*Axis4Value()); };
    
        bool emuL1;
        bool ButtonL1Pressed() { return emulated ? emuL1 : ButtonL1.pressing(); }; 
        bool emuL2;
        bool ButtonL2Pressed() { return emulated ? emuL2 : ButtonL2.pressing(); };
        bool emuR1;
        bool ButtonR1Pressed() { return emulated ? emuR1 : ButtonR1.pressing(); }; 
        bool emuR2;
        bool ButtonR2Pressed() { return emulated ? emuR2 : ButtonR2.pressing(); };
        bool emuL;
        bool ButtonLeftPressed() { return emulated ? emuL : ButtonLeft.pressing(); }; 
        bool emuR;
        bool ButtonRightPressed() { return emulated ? emuR : ButtonRight.pressing(); };
        bool emuU;
        bool ButtonUpPressed() { return emulated ? emuU : ButtonUp.pressing(); }; 
        bool emuD;
        bool ButtonDownPressed() { return emulated ? emuD : ButtonDown.pressing(); };
        bool emuA;
        bool ButtonAPressed() { return emulated ? emuA : ButtonA.pressing(); }; 
        bool emuY;
        bool ButtonYPressed() { return emulated ? emuY : ButtonY.pressing(); };
        bool emuX;
        bool ButtonXPressed() { return emulated ? emuX : ButtonX.pressing(); }; 
        bool emuB;
        bool ButtonBPressed() { return emulated ? emuB : ButtonB.pressing(); };
    
		void setInputFile(std::string fileName){
			inputFilePath = fileName;
		}
	
        int startEmulation(){
            return startEmulation(INPUT_DELAY);
        }
        int startEmulation(int delay){
            emulated = true;
            emuStep = 0;
            while (true) {
                bool ok = updateEmulation();
                emuStep++;
                if (!ok) break;
                vex::task::sleep(3*delay);
                vex::task::yield();
            }
            emulated = false;
            return 0;
        }
    
        bool updateEmulation() {
            std::ifstream inputFile;
            inputFile.open(inputFilePath);
            if (!inputFile.good()) return false;
            std::string str;
            for (int i=0;i<emuStep;i++){
                std::getline(inputFile, str);
                if (!inputFile.good()) return false;
                if (inputFile.eof()) return false;
            }
            std::getline(inputFile,str);
            
            char* cstr = new char [str.length()+1];
            strcpy(cstr,str.c_str());
            int32_t inputs[16];
            char* pch;
            pch = strtok(cstr,", ");
            for(int i=0;i<16&&pch!=NULL;i++){
                inputs[i] = atoi(pch);
                pch = strtok(NULL,", ");
            }
            emuA1 = inputs[0];
            emuA2 = inputs[1];
            emuA3 = inputs[2];
            emuA4 = inputs[3];
            emuA = (bool)inputs[4];
            emuB = (bool)inputs[5];
            emuX = (bool)inputs[6];
            emuY = (bool)inputs[7];
            emuU = (bool)inputs[8];
            emuD = (bool)inputs[9];
            emuL = (bool)inputs[10];
            emuR = (bool)inputs[11];
            emuL1 = (bool)inputs[12];
            emuL2 = (bool)inputs[13];
            emuR1 = (bool)inputs[14];
            emuR2 = (bool)inputs[15];
            return true;
        }
        
        void capture_input(){
            char buffer[100];
            //                        A1  A2  A3  A4  A  B  X  Y  U  D  L  R L1 L2 R1 R2
            int l = sprintf(buffer, "%ld,%ld,%ld,%ld,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", 
                                Axis1Value(), Axis2Value(),
                                Axis3Value(), Axis4Value(),

                                ButtonAPressed(), ButtonBPressed(),
                                ButtonXPressed(), ButtonYPressed(),

                                ButtonUpPressed(), ButtonDownPressed(), 
                                ButtonLeftPressed(), ButtonRightPressed(),

                                ButtonL1Pressed(), ButtonL2Pressed(),
                                ButtonR1Pressed(), ButtonR2Pressed()
                           );
            Screen.clearLine();
            Screen.print("%s", buffer);
            arBrain.SDcard.appendfile(inputFilePath.c_str(), (uint8_t*)buffer, l);
            recordStep++;
        }
};

// END ACTION_REPLAY API
#endif