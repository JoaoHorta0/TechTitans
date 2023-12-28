
//---- AB&T EasyCAT shield application example V.2_0 -------------------------------------------  

#define CUSTOM
#include "TechTitansTest1.h"

#include "EasyCAT.h"                // EasyCAT library to interface the LAN9252
#include <SPI.h>                    // SPI library

 
EasyCAT EASYCAT;                    // EasyCAT istantiation

                                    // The constructor allow us to choose the pin used for the EasyCAT SPI chip select 
                                    // Without any parameter pin 9 will be used 
                   
                                    // We can choose between:
                                    // 8, 9, 10, A5, 6, 7                                    

                                    // On the EasyCAT board the SPI chip select is selected through a bank of jumpers              

                                    // (The EasyCAT board REV_A allows only pins 8, 9, 10 through 0 ohm resistors)

 //EasyCAT EASYCAT(8);              // example:                                  
                                    // pin 8 will be used as SPI chip select
                                    // The chip select chosen by the firmware must match the setting on the board  

#define QUBE_NUMBER 7 //up to 7                                    
#define MODULE_NUMBER 15 //up to 15

//---- pins declaration ------------------------------------------------------------------------------

const int Ana0 = A0;                // analog input 0
const int Ana1 = A1;                // analog input 1

const int BitOut0 = A2;             // digital output bit 0
const int BitOut1 = A3;             // digital output bit 1
const int BitOut2 = A4;             // digital output bit 2
const int BitOut3 = A5;             // digital output bit 3

const int BitOut4 = 8;             // digital output bit 4
const int BitOut5 = 4;             // digital output bit 5

const int BitIn0 = 3;               // digital input  bit 0
const int BitIn1 = 5;               // digital input  bit 1
const int BitIn2 = 6;               // digital input  bit 2
const int BitIn3 = 7;               // digital input  bit 3



//---- global variables ---------------------------------------------------------------------------


UWORD ContaUp;                      // used for sawthoot test generation
UWORD ContaDown;                    //

UWORD Analog0;
UWORD Analog1;

UWORD Test;
UWORD TestOut;

char IncomingData = 0;

unsigned long Millis = 0;
unsigned long PreviousMillis = 0;
unsigned long PreviousSaw = 0;
unsigned long PreviousCycle = 0;

static unsigned long lastMillis = 0;
const unsigned long interval = 100;


//---- Qube/Module Connection Variables ---------------------------------------------------------------------------

int QubesConnectedBIN[8];
int QubesConnectedDEC = 0;//170;

int ModulesConnectedBIN[16];
int ModulesConnectedDEC_1stByte = 0;
int ModulesConnectedDEC_2ndByte = 0;

bool disconnectQM = false;
bool disconnectQube = false;
int disconnectQubeNumber = 0;
bool disconnectModule = false;
int disconnectModuleNumber = 0;


//---- setup ---------------------------------------------------------------------------------------
 
void setup()
{
  Serial.begin(9600);                                             // serial line initialization
                                                                  //(used only for debug)
           
  Serial.print ("\nEasyCAT - Generic EtherCAT slave\n");          // print the banner

  pinMode(BitOut0, OUTPUT);                                       // digital output pins setting
  pinMode(BitOut1, OUTPUT);                                       // 
  pinMode(BitOut2, OUTPUT);                                       //
  pinMode(BitOut3, OUTPUT);                                       //
  
  pinMode(BitOut4, OUTPUT);                                       //
  pinMode(BitOut5, OUTPUT);                                       //

  

  pinMode(BitIn0, INPUT_PULLUP);                                  // digital input pins setting
  pinMode(BitIn1, INPUT_PULLUP);                                  // 
  pinMode(BitIn2, INPUT_PULLUP);                                  //
  pinMode(BitIn3, INPUT_PULLUP);                                  //

  ContaUp.Word = 0x0000;                                          //
  ContaDown.Word = 0x0000;                                        //

  
  TestOut.Word = 0x007D;
  
  Test.Word = 0x0000;
 
                                                                  //---- initialize the EasyCAT board -----
                                                                  
  if (EASYCAT.Init() == true)                                     // initialization
  {                                                               // succesfully completed
    Serial.print ("initialized");                                 //
  }                                                               //
  
  else                                                            // initialization failed   
  {                                                               // the EasyCAT board was not recognized
    Serial.print ("initialization failed");                       //     
                                                                  // The most common reason is that the SPI 
                                                                  // chip select choosen on the board doesn't 
                                                                  // match the one choosen by the firmware
                                                                  
    pinMode(13, OUTPUT);                                          // stay in loop for ever
                                                                  // with the Arduino led blinking
    while(1)                                                      //
    {                                                             //   
      digitalWrite (13, LOW);                                     // 
      delay(500);                                                 //   
      digitalWrite (13, HIGH);                                    //  
      delay(500);                                                 // 
    }                                                             // 
  } 
  
  PreviousMillis = millis();
}


//---- main loop ----------------------------------------------------------------------------------------
 
void loop()                                             // In the main loop we must call ciclically the 
{                                                       // EasyCAT task and our application
                                                        //
                                                        // This allows the bidirectional exachange of the data
                                                        // between the EtherCAT master and our application
                                                        //
                                                        // The EasyCAT cycle and the Master cycle are asynchronous
                                                        //   

  EASYCAT.MainTask();                                   // execute the EasyCAT task

  QubeToQubeConnection(disconnectQubeNumber, disconnectQube);
  ModuleToModuleConnection(disconnectModuleNumber, disconnectModule);
  HMItoTWINCAT();
  TWINCATtoHMI();
  Application();                                        // user applications

}



//---- user application ------------------------------------------------------------------------------

void Application ()                                        

{
  
    
  Millis = millis();                                    // As an example for this application 
  if (Millis - PreviousMillis >= 10)                    // we choose a cycle time of 10 mS 
  {                                                     // 
    PreviousMillis = Millis;                            //
 
                                                        // --- analog inputs management ---
      
    Analog0.Word = analogRead(Ana0);                    // read analog input 0
    Analog0.Word >>= 2;                                 // normalize it on 8 bits
    EASYCAT.BufferIn.Byte[0] = Analog0.Byte[0];         // and put the result into
                                                        // input Byte 0 

    Analog1.Word = analogRead(Ana1);                    // read analog input 1
    Analog1.Word >>= 2;                                 // normalize it on 8 bits 
    EASYCAT.BufferIn.Byte[1] = Analog1.Byte[0];         // and put the result into
                                                        // input Byte 1   

                                                        // --- four output bits management ----
                                                        //                          
    if (EASYCAT.BufferOut.Byte[0] & (1<<0))             // the four output bits are mapped to the 
      digitalWrite (BitOut0, HIGH);                     // lower nibble of output Byte 0
    else                                                // 
      digitalWrite (BitOut0, LOW);                      // we read each bit and write it
                                                        // to the corrisponding pin
    if (EASYCAT.BufferOut.Byte[0] & (1<<1))             // 
      digitalWrite (BitOut1, HIGH);                     //
    else                                                //
      digitalWrite (BitOut1, LOW);                      // 
                                                        //
    if (EASYCAT.BufferOut.Byte[0] & (1<<2))             //   
      digitalWrite (BitOut2, HIGH);                     //
    else                                                //
      digitalWrite (BitOut2, LOW);                      // 
                                                        //  
    if (EASYCAT.BufferOut.Byte[0] & (1<<3))             // 
      digitalWrite (BitOut3, HIGH);                     //
    else                                                //
      digitalWrite (BitOut3, LOW);                      // 





 /*  if (EASYCAT.BufferOut.Byte[1] & (1<<0))             // 
      digitalWrite (BitOut4, HIGH);                     //
    else                                                //
      digitalWrite (BitOut4, LOW);   
      
    if (EASYCAT.BufferOut.Byte[1] & (1<<1))             // 
      digitalWrite (BitOut5, HIGH);                     //
    else                                                //
      digitalWrite (BitOut5, LOW);   */


                                                        //--- four input bits management ---
                                                        //  
    if (digitalRead(BitIn0))                            // the four input pins are mapped to the
      EASYCAT.BufferIn.Byte[6] |= (1<<0);               // lower nibble of input Byte 6
    else                                                //
      EASYCAT.BufferIn.Byte[6] &= ~(1<<0);              // we read each pin and write it 
                                                        // to the corresponding bit
    if (digitalRead(BitIn1))                            //
      EASYCAT.BufferIn.Byte[6] |= (1<<1);               //  
    else                                                //
      EASYCAT.BufferIn.Byte[6] &= ~(1<<1);              //
                                                        //
    if (digitalRead(BitIn2))                            //
      EASYCAT.BufferIn.Byte[6] |= (1<<2);               //   
    else                                                //
      EASYCAT.BufferIn.Byte[6] &= ~(1<<2);              //  
                                                        //  
    if (digitalRead(BitIn3))                            //
      EASYCAT.BufferIn.Byte[6] |= (1<<3);               // 
    else                                                //
      EASYCAT.BufferIn.Byte[6] &= ~(1<<3);              //

                                                        // --- test sawtooth generation --- 
                                                        //
    Millis = millis();                                  // each 100 mS
    if (Millis - PreviousSaw >= 100)                    // 
    {                                                   // 
      PreviousSaw = Millis;                             //
                                                        //
      ContaUp.Word++;                                   // we increment the variable ContaUp  
      ContaDown.Word--;                                 // and decrement ContaDown
    }                                                   //

                                                        // we use these variables to create sawtooth,
                                                        // with different slopes and periods, for
                                                        // test pourpose, in input Bytes 2,3,4,5,30,31
                                                        
    EASYCAT.BufferIn.Byte[2] = ContaUp.Byte[0];         // slow rising slope
    EASYCAT.BufferIn.Byte[3] = ContaUp.Byte[1];         // extremly slow rising slope
    
    EASYCAT.BufferIn.Byte[4] = ContaDown.Byte[0];       // slow falling slope
    EASYCAT.BufferIn.Byte[5] = ContaDown.Byte[1];       // extremly slow falling slope
    
    EASYCAT.BufferIn.Byte[6] = Test.Byte[0];  //Arduino -> Twinny
      
    EASYCAT.BufferIn.Byte[7] = ContaUp.Byte[0] << 2;   // medium speed rising slope
    EASYCAT.BufferIn.Byte[8] = ContaDown.Byte[0] << 2; // medium speed falling slope    

    TestOut.Byte[0] =   EASYCAT.BufferOut.Byte[0];  // Twinny -> Arduino

/*
    if(Millis - lastMillis >= interval){
    Serial.print(TestOut.Word);
    Serial.print(",");
    Serial.print(Test.Word);
    Serial.print(",");
    Serial.print(ContaUp.Byte[0]);
    Serial.print(",");
    Serial.print(ContaDown.Byte[0]);
    Serial.print(",");
    Serial.print(Analog0.Byte[0]);
    Serial.print(",");
    Serial.println(Analog1.Word);

    lastMillis = Millis;
    }*/
  }   
}

void TWINCATtoHMI()
{

   Millis = millis(); 
  
    if(Millis - lastMillis >= interval){
    Serial.print(TestOut.Word);
    Serial.print(",");
    Serial.print(Test.Word);
    Serial.print(",");
    Serial.print(ContaUp.Byte[0]);
    Serial.print(",");
    Serial.print(ContaDown.Byte[0]);
    Serial.print(",");
    Serial.print(Analog0.Byte[0]);
    Serial.print(",");
    Serial.print(Analog1.Word);
    
    Serial.print("/");
    for (int i = 0; i < 8; i++) {
    Serial.print(QubesConnectedBIN[i]);
    if(i != 7){Serial.print(",");}
    }
    Serial.print("/");

    for (int i = 0; i < 16; i++) {
    Serial.print(ModulesConnectedBIN[i]);
    if(i != 15){Serial.print(",");}
    }
    Serial.println("/");

    lastMillis = Millis;
    }
  
}

void HMItoTWINCAT()
{
    if(Serial.available() > 0)
  {
    IncomingData = Serial.read();
    Serial.println(IncomingData);
    if(IncomingData != ','){
          
            Disconnect();
        
        if(IncomingData == '1')
        {
            Test.Word = 0x00FF;
            EASYCAT.BufferIn.Byte[6] = Test.Byte[0];
          }
        if(IncomingData == '2')
        {
           Test.Word = 0x0000;
           EASYCAT.BufferIn.Byte[6] = Test.Byte[0];
          }
          
          if(IncomingData == '3')
        {
           digitalWrite (BitOut4, HIGH);  
          }
          if(IncomingData == '4')
        {
           digitalWrite (BitOut4, LOW);  
          }
          
          if(IncomingData == '5')
        {
           digitalWrite (BitOut5, HIGH);  
          }
          if(IncomingData == '6')
        {
           digitalWrite (BitOut5, LOW);  
          }
    }
  }
}



void Disconnect(){
  
  if(IncomingData == 'D'){disconnectQM = true;}
        else if((IncomingData == 'Q') && (disconnectQM == true)){disconnectQube = true;}
        else if((IncomingData == 'M') && (disconnectQM == true)){disconnectModule = true;}
        
        else if((disconnectQM == true) && (disconnectQube == true)){disconnectQubeNumber = IncomingData - 97;
                                                                   //Serial.println(disconnectQubeNumber);
                                                                   QubeToQubeConnection(disconnectQubeNumber, disconnectQube);
                                                                   disconnectQM = false;
                                                                   disconnectQube = false;}
        
        else if((disconnectQM == true) && (disconnectModule == true)){disconnectModuleNumber = IncomingData - 97;
                                                                    //Serial.println(disconnectModuleNumber);
                                                                    ModuleToModuleConnection(disconnectModuleNumber, disconnectModule);
                                                                    disconnectQM = false;
                                                                    disconnectModule = false;}
        else{disconnectQM = false;
             disconnectQube = false;
             disconnectModule = false;}
  
}


void QubeToQubeConnection(int NUMBER, bool disconnectQ){ //(int NUMBER, int Pvalue, bool disconnectQ){

QubesConnectedDEC = EASYCAT.BufferOut.Byte[31];

  //int QubesConnectedBIN[8];
  //int QubesConnectedDEC = Pvalue;//0;
  //Serial.print(ModulesConnectedDEC);
  //Serial.print("\n");
  // Convert decimal to binary and store each bit in the array
  
  for (int i = 7; i >= 0; i--) {
    QubesConnectedBIN[i] = QubesConnectedDEC % 2;
    QubesConnectedDEC /= 2;
  }
  
  if(disconnectQ == false){
    if(QubesConnectedBIN[7-NUMBER] == 0){QubesConnectedBIN[7-NUMBER] = 1;}
  }
  else if(disconnectQ == true){
    if(QubesConnectedBIN[7-NUMBER] == 1){QubesConnectedBIN[7-NUMBER] = 0;}
  }
  
  for (int i = 0; i < 8; i++) {
    QubesConnectedDEC = QubesConnectedDEC * 2 + QubesConnectedBIN[i];
  }

  // Serial.print("\n");
  //Serial.print("Qubes1stByte: ");
  //Serial.println(QubesConnectedDEC);
  
  EASYCAT.BufferIn.Byte[31] = QubesConnectedDEC;
}




void ModuleToModuleConnection(int NUMBER, bool disconnectM){ //(int NUMBER, int Pvalue, bool disconnectM){

ModulesConnectedDEC_1stByte = EASYCAT.BufferOut.Byte[29];
ModulesConnectedDEC_2ndByte = EASYCAT.BufferOut.Byte[30];

  /*
  int ModulesConnectedBIN[16];
  int ModulesConnectedDEC_1stByte = 0;
  int ModulesConnectedDEC_2ndByte = Pvalue;
Serial.print(ModulesConnectedDEC_1stByte);
Serial.println(ModulesConnectedDEC_2ndByte);*/

  // Convert decimal to binary and store each bit in the array
  for (int i = 7; i >= 0; i--) {
    ModulesConnectedBIN[i] = ModulesConnectedDEC_1stByte % 2;
    ModulesConnectedDEC_1stByte /= 2;
  }
  for (int i = 15; i >= 8; i--) {
    ModulesConnectedBIN[i] = ModulesConnectedDEC_2ndByte % 2;
    ModulesConnectedDEC_2ndByte /= 2;
  }
  
  if(disconnectM == false){
    if(ModulesConnectedBIN[15-NUMBER] == 0){ModulesConnectedBIN[15-NUMBER] = 1;}
  }
  else if(disconnectM == true){
    if(ModulesConnectedBIN[15-NUMBER] == 1){ModulesConnectedBIN[15-NUMBER] = 0;}
  }

  for (int i = 0; i < 8; i++) {ModulesConnectedDEC_1stByte = ModulesConnectedDEC_1stByte * 2 + ModulesConnectedBIN[i];}
  for (int i = 8; i < 16; i++) {ModulesConnectedDEC_2ndByte = ModulesConnectedDEC_2ndByte * 2 + ModulesConnectedBIN[i];}

  //Serial.print("\n");
  //Serial.print("Module1stByte: ");
  //Serial.println(ModulesConnectedDEC_1stByte);
  //Serial.print("Module2ndByte: ");
  //Serial.println(ModulesConnectedDEC_2ndByte);
  //Serial.print("\n");

  EASYCAT.BufferIn.Byte[29] = ModulesConnectedDEC_1stByte;
  EASYCAT.BufferIn.Byte[30] = ModulesConnectedDEC_2ndByte;

}




/*
void QubeToQubeConnection(){

QubesConnectedDEC = EASYCAT.BufferOut.Byte[31];
//Serial.print(ModulesConnectedDEC);
  //Serial.print("\n");
  // Convert decimal to binary and store each bit in the array
  for (int i = 7; i >= 0; i--) {
    QubesConnectedBIN[i] = QubesConnectedDEC % 2;
    QubesConnectedDEC /= 2;
  }
  if(QubesConnectedBIN[7-QUBE_NUMBER] == 0){
    QubesConnectedBIN[7-QUBE_NUMBER] = 1;}
  
  for (int i = 0; i < 8; i++) {
    QubesConnectedDEC = QubesConnectedDEC * 2 + QubesConnectedBIN[i];
  }

 // Serial.print("\n");
  //Serial.print(ModulesConnectedDEC);
  EASYCAT.BufferIn.Byte[31] = QubesConnectedDEC;

}


void ModuleToModuleConnection(){

ModulesConnectedDEC_1stByte = EASYCAT.BufferOut.Byte[29];
ModulesConnectedDEC_2ndByte = EASYCAT.BufferOut.Byte[30];

//Serial.print(ModulesConnectedDEC_1stByte);
//Serial.println(ModulesConnectedDEC_2ndByte);

  // Convert decimal to binary and store each bit in the array
  for (int i = 7; i >= 0; i--) {
    ModulesConnectedBIN[i] = ModulesConnectedDEC_1stByte % 2;
    ModulesConnectedDEC_1stByte /= 2;
  }
  for (int i = 15; i >= 8; i--) {
    ModulesConnectedBIN[i] = ModulesConnectedDEC_2ndByte % 2;
    ModulesConnectedDEC_2ndByte /= 2;
  }
  
  if(ModulesConnectedBIN[15-MODULE_NUMBER] == 0){
    ModulesConnectedBIN[15-MODULE_NUMBER] = 1;}

  for (int i = 0; i < 8; i++) {ModulesConnectedDEC_1stByte = ModulesConnectedDEC_1stByte * 2 + ModulesConnectedBIN[i];}
  for (int i = 8; i < 16; i++) {ModulesConnectedDEC_2ndByte = ModulesConnectedDEC_2ndByte * 2 + ModulesConnectedBIN[i];}

//Serial.print("\n");
//Serial.print(ModulesConnectedDEC_1stByte);
//Serial.println(ModulesConnectedDEC_2ndByte);
//Serial.print("\n");

  EASYCAT.BufferIn.Byte[29] = ModulesConnectedDEC_1stByte;
  EASYCAT.BufferIn.Byte[30] = ModulesConnectedDEC_2ndByte;

}*/
  
