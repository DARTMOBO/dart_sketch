///////////////////////////
// DART_SKETCH   v1.82   //
// Massimiliano Marchese //
// Piero Pappalardo      //
// www.dartmobo.com      //
///////////////////////////

#define note_off 0                   // 1 = enabled // 0 = disabled // send NOTE-OFF messages on button release -  if NOTE Type has been selected
#define main_encoder 1               // 1 = enabled // 0 = disabled // MAIN ENCODER_ 
#define capacitivesensor_active 1    // 1 = enabled // 0 = disabled // CAPACITIVE SENSORS_
#define shifter_active  1            // 1 = enabled // 0 = disabled // SHIFT REGISTERS_ // if enabled, Matrix_pads must be disabled
#define Matrix_Pads 0                // 1 = enabled // 0 = disabled // max7219 chips
#define LED_pattern 3                // 0 = dart one // 1 = kombat // 2 = NB-boards // 3 = Kombat-NB // - Led animation pattern used by buttons and pots
#define DMX_active   0               // 1 = enabled // 0 = disabled // enable-disable also from _DART_Dmx_out.cpp !!!!!!!!!
#define pullups_active 1             // 1 = enabled // 0 = disabled // pullup resistors
#define page_active 1                // 1 = enabled // 0 = disabled // page_switch
#define stratos 0                    // 1 = enabled // 0 = disabled // Stratos sketch version.
#define touch_version 1              // 1 = 680k //  2 = 10m //     resistor settings for touch sensing circuit
#define mouse_block 1                // 1 = enabled // 0 = disabled // mouse messages are stopped after 2 seconds of repeated activity
#define arrows_block 0               // 1 = enabled // 0 = disabled // arrow key messages are stopped after 2 seconds of repeated activity
#define page_LEDs 0                  // 1 = page LEDs active
#define LED_rings 0                  // 1 = LED rings active
#define encoders_generic 0           // 1 = enabled 
#define MIDI_IN_block 0              // 1 = MIDI IN blocked
#define MIDI_OUT_block 0             // 1 = MIDI out blocked
#define MIDI_thru 0                  // 1 = MIDI Thru active
#define autosend 0

//---------------------------------------------


#if defined (__AVR_ATmega32U4__)  
#include "_DART_MIDI.h"
#include <Mouse.h>
#include <Keyboard.h>
midiEventPacket_t rx;
#endif

#if (capacitivesensor_active == 1)

#include "_DART_Touch_Sensor.h"
#if (stratos == 1 )
CapacitiveSensor   cs_4_2[1] = {CapacitiveSensor(8,9)}; // stratos
#endif
#if (stratos == 0 )
CapacitiveSensor   cs_4_2[2] = {CapacitiveSensor(8,7),CapacitiveSensor(8,9)};
#endif
 
#endif


#if (DMX_active == 1 && stratos == 0)
#include "_DART_DMX_Out.h"
#endif

#include "_DART_EEPROM.h"

#if (shifter_active == 1 && stratos == 0)
#include "_DART_Shifter.h"
#endif

// #if (Matrix_Pads == 1 && stratos == 0)
 #include "LedControl.h"
// #endif





  byte do_; // serve per diminuire la velocità i lettura encoder, daltando tot cicli.
///////////////////////////////////////////////////////////////////////
const byte max_modifiers = 60;
const int maxbeam = 520;
const int minbeam = 290;

///////////////////////////////////////////////////////////////////////
#if (LED_pattern == 0) 
   const byte encledtable[16]= { 4,5,6,7, 12,13,14,15, 3,2,1,0, 11,10,9,8,};  // dart one 
 #endif
 
 // const byte encledtable[16]= { 4,5,6,7, 12,13,14,15, 3,2,1,16, 11,10,9,8,};  // dart one - darietto
  
 
//  const byte encledtable[16] = { 15,15,14,14,13,13,12,12,11,11,10,10,9,9,8,8,};  // vector

 // const byte encledtable[16] = { 8,8,9,9,10,10,11,11,12,12,13,13,14,14,15,15};  // vector inverted

 #if (LED_pattern == 1) 
  const byte encledtable[16] = { 12,13,14,14,15,0,7,7,1,6,2,2,5,3,4,4};  // kombat
#endif

/*
#if (LED_pattern == 3) 
  const byte encledtable[16] = { 1,4,3,3,2,8,7,7,6,5,12,12,11,10,9,9};  // kombat - nb // 1,4,3,2, 8,7,6,5, 12 ,11,10,9
#endif
*/

#if (LED_pattern == 3) 
  const byte encledtable[16] = { 0,3,2,2,1,7,6,6,5,4,11,11,10,9,8,8};  // kombat - nb // 1,4,3,2, 8,7,6,5, 12 ,11,10,9
#endif

 #if (LED_pattern == 2) 
   const byte encledtable[16]= { 
   15,14,13,12,
  7,6,5,4,
   8,9,10,11,
   0,1,2,3
   };  // NB-boards
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////


 const PROGMEM  byte qwertymod[33] = { // da 0 a 24 ci sono i modificatori - da 25 a 32 controlli mouse
   
 0,   // niente 
 194, // f1
 195, // f2
 196, // f3
 
 197, // f4
 198, // f5 
 199, // f6
 200, // f7
 
 201, // f8
 202, // f9
 203, // f10
 204, // f11
 
 205, // f12
 215, // right arrow
 216, // left
 217, // down
 
 218, // up
 128, // left crtl
 129, // left shift
 130, // left alt
 
 179, // tab
 176, // return
 177, // esc
 212, // delete
 };


   const byte matrix_remap[] = {1, 6, 2, 5, 7,11, 0, 4, 3,10, 8,9 ,12, 3, 0, 8};

// const PROGMEM byte matrix_remap[] = {1, 6, 2, 5, 7,11, 0, 4, 3,10, 8,9 ,12, 3, 0, 8};

const PROGMEM byte  input_remap[]  = {6,8,4,2,7,1,5,3,};
 
byte remapper(byte input) 
{return  (( pgm_read_byte(input_remap + ((input)-(((input)/8)*8)) ) +(((input)/8)*8))) -1;}


//|||||||||||||||||||||||||||||||||
// const byte select[] = {4,5,6}; // pins connected to the 4051 input select lines

//|||||||||||||||||||||||||||||||||
byte  eeprom_preset_active ; // activate auxiliary preset at startup;
byte channel;
byte plexer;
byte chan;
const int upper_val = 750;
const byte lower_val = 255;
int valore; // analog 0-1024 value for 4051 analog readings.
volatile byte potOut;
//||||||||||||||||||||||||||||||
byte mousex;
byte mousey;
// byte mouserall;
//int mouseyacceleration; // 
//int mousexacceleration; // 
byte mouse_wheel_speed_counter;
/////////////////////////////////////////////////////////////
volatile byte encoder_mempos[2];             // da editor si scegli quale sarÃƒÆ’Ã‚Â  la memoryposition dell'encoder - la cosa rende piÃƒÆ’Ã‚Â¹ semplice creare un nuovo layout editor - masta mettere mode su encoder1
volatile byte touch_mempos[2];
byte V_touch_regulator[2] = {1,1};
byte mouse_mempos ;
byte PADS_mempos;
byte distance_mempos;
byte page_mempos;
 byte general_mempos = 0;
/////////////////////////////////////////////////////////////////////
byte page = max_modifiers ;

byte cycletimer;   /// conteggio di tempo in base ai cicli - serve per regolare effetti visivi led - e per il timing del virtualtouch

boolean openeditor;
byte editorcounter;
byte memoryposition;
byte type;
byte incomingByte;
byte note;
byte velocity;
byte action=2;

volatile byte old_cmd; // midi out doubles-filter, buffer variables 
volatile byte old_pitch;
volatile byte old_velocity;

byte padDecay[4];
byte padNum;
volatile  unsigned int padVal;  // 
volatile  unsigned int padVal2; // 
//////////////////////////////////////////////////////////////////////////////////   touch sensor
 byte limit_touch = 250;

volatile byte  readingsXen[2][3] ;      // the readings from the analog input
// byte readingsXenn = 250;
// volatile byte readingsXen2[3];
// volatile byte readingsXenn2;
//const byte Xenlimit = 6;
// const byte Xendivider = 2;
// volatile byte lettura;

#if (stratos == 0 )
const byte decaysensor_value = 2;
#endif
#if (stratos == 1 )
const byte decaysensor_value = 100;
#endif
byte decaysensor[2]; //= decaysensor_value;
byte indexXen = 0;                  // the index of the current reading
// unsigned int totalXen[2];                  // the running total
byte averageXen[2];                // the average
byte lower_Xen[2] = {200,200};           
byte higher_Xen[2] = {40,40}; 

//////////////////////////////////////////////////////////////////////////////////////


 byte buttonefx = 0; 
 byte buttonefxd = 0;
 byte buttonefxu = 0;
 byte shifterwrite = 1; // led refresh
 int encled[2]; //  used for encoder led animation

//////////////////////////////////////////////////////////////////////////////////////
#if (shifter_active == 1 && stratos == 0)
#define SER_Pin 10 //SER_IN
#define RCLK_Pin 11 //L_CLOCK
#define SRCLK_Pin 12 //CLOCK
#define NUM_REGISTERS 4 // how many registers are in the chain
Shifter shifter(SER_Pin, RCLK_Pin, SRCLK_Pin, NUM_REGISTERS);
#endif 

#if (Matrix_Pads == 1 && stratos == 0)
byte max_units = 12;
LedControl lc=LedControl(10,11,12,max_units); 
// LedControl(int dataPin, int clkPin, int csPin, int numDevices) 

//byte order_line[8] = {5,6,0,1,3,4,7,2};
 // const byte order_row[8] = {6,1,5,3,7,2,4,0};

 void set_unit(byte number_of_unit){ 
  lc.shutdown(number_of_unit,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(number_of_unit,1); // luminosita' da 1 a 15
  /* and clear the display */
  lc.clearDisplay(number_of_unit); 
   }
   
#endif

//////////////////////////////////////////////////////////////////////////////////////
//byte contoencoder;
byte numero2;
byte lastEncoded[2] ; // adesso queste variabili contendono la scala scritta da editor...
int encodervaluepot[2] ; 
byte encodervaluepot_buffer[2]; // serve per spegnere le note giÃƒÂ  suonate in encoder - scale mode
volatile byte MSB[2] ; 
volatile byte LSB[2] ;
byte encoder_block[2]= {64,64} ; // serve per bloccare l'attivitÃƒÂ  dell'encoder quando viene toccato ma tenuto fermo - per registrare una scala.
////////////////////////////////////////////////////////////////////////////////  
volatile byte lastbutton[64] ; // used to record the previous state of a button - debounce
volatile byte lightable[65] // ho provato a scendere a 64 - ma si creavano stranissimi problemi di conflitto memoria con buttonled_efx - messaggi midi disordinati etc etc
               //la vecchia lightable era fissa.

 = { //40,
1,40,3,40,2,40,0,40,
5,40,7,40,6,40,4,40,
9,40,11,40,10,40,8,40,
13,40,15,40,14,40,12,40,
17,40,19,40,18,40,16,40,
21,40,23,40,22,40,20,40,
};





 byte  valuetable[max_modifiers*2] ;
 byte  typetable[max_modifiers*2] ;
 byte  minvalue[max_modifiers] ;
 byte  maxvalue[max_modifiers] ;
 byte  modetable[max_modifiers] ; // 
 byte  qwertyvalue[max_modifiers];
 byte  dmxtable[max_modifiers];

 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
volatile byte bit_status[4][max_modifiers/4]; // bit_status e' un multiarray che contiene le vecchie tabelle di riferimento per far funzionare il toggle, 
                                              // il feedback e i led

 // 1 - ledstatus 1 e 2
 // 2 - feedback_bit1
 // 3 - feedback_bit2
 // 4 - bit_toggle 1 e 2

 byte feedop2 (byte input) { // used in midi feedback bit operations
 return input/8; }

 byte bit_write(byte array_, byte posizione, byte stato)
 {  bitWrite(bit_status[array_-1][feedop2(posizione)],posizione-(feedop2(posizione)*8),stato); }

  byte bit_read( byte array_,byte posizione)
  { return bitRead(bit_status[array_-1][feedop2(posizione)],posizione-(feedop2(posizione)*8));  }
 //  byte scrivi_bool (boolean numero) {  Serial.print(numero);  }
  
///////////////////////////////////////////////////////////////////////////
byte pagestate = 0;
/////////////////////////////////////////////////////////////////////////// 
// ------------------------------------------------------------- scale play
// void scale_play
byte at; // serve per lo scale player
byte beam_scala_buffer;
byte beam_counter;
int scala[4]; // scala[4] è un array che cotiene 4 scale... 
              // scalaSpinner0-page1, scalaSpinner1-page1, ScalaSpinner0-page2, scalaSpinner1-page2
int scala_learn; // viene suonato semre quello che c'è in questa variavie, ma in scala fissa viene caricato il contenuto da scale[4]
byte scala_array[6];
const byte scala_lenght = 6;

byte scala_counter;
byte scala_reset;



 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////7
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////

#if (Matrix_Pads == 1 && stratos == 0)
void single_h(byte number_of_unit,byte sprite, byte invert) { // quale pad, quale simbolo, inversione immagine
  
  byte sprite_pos = remapper(sprite); // in quale posizione di memoria è memorizzato il simbolo? // fare il remapper della numerazione di "led" (cioè lightable-1)

if (invert == 0 )
{
lc.setRow(number_of_unit,6, valuetable[sprite_pos]);
lc.setRow(number_of_unit,1, maxvalue[sprite_pos]);
lc.setRow(number_of_unit,5, minvalue[sprite_pos]);
lc.setRow(number_of_unit,3, modetable[sprite_pos]);

lc.setRow(number_of_unit,7, dmxtable[sprite_pos]);
lc.setRow(number_of_unit,2, qwertyvalue[sprite_pos]);
lc.setRow(number_of_unit,4, typetable[sprite_pos]);
lc.setRow(number_of_unit,0, lightable[sprite_pos]);
}
else 
{
  lc.setRow(number_of_unit,6, ~valuetable[sprite_pos]);
lc.setRow(number_of_unit,1, ~maxvalue[sprite_pos]);
lc.setRow(number_of_unit,5, ~minvalue[sprite_pos]);
lc.setRow(number_of_unit,3, ~modetable[sprite_pos]);

lc.setRow(number_of_unit,7, ~dmxtable[sprite_pos]);
lc.setRow(number_of_unit,2, ~qwertyvalue[sprite_pos]);
lc.setRow(number_of_unit,4, ~typetable[sprite_pos]);
lc.setRow(number_of_unit,0, ~lightable[sprite_pos]);
  }
}
#endif




#if (autosend == 1)
void autosend_()
{
for(byte c=0; c<48; c++)
{
  if (c == 25)
  {
    Serial.print("value: "); Serial.println(valuetable[c]);
    Serial.print("mode: "); Serial.println(modetable[c]);
    Serial.print("position: "); Serial.println(c);
    Serial.println("-----");
    }
    
  }
  delay(1000);

  
  }
#endif
