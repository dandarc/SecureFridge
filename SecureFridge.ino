#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);

const int idLength = 4;
const int lengthDelay = 5000;


typedef struct ID {
    //byte array of RFID uid
    byte ID[idLength];
    //Bays are bitwise coded - use the spreadsheet to generate
    byte Bays;
};

//this is the list of valid RFID’s - copy from spreadsheet
const PROGMEM ID validIDs[]  = {
  {{1,1,1,1},63},
  {{2,2,2,2},63},
  {{3,3,3,3},32},
  {{4,4,4,4},3},
  {{5,5,5,5},4}
};

typedef struct t  {
    unsigned long tStart;
    unsigned long tTimeout;
};

t t_func1 = {0, 60000}; //Run every 60000ms, 1 minute

bool tCheck (struct t *t ) {
  if (millis() > t->tStart + t->tTimeout) return true;    
}

void tRun (struct t *t) {
    t->tStart = millis();
}

void setup() 
{
  SPI.begin();
  mfrc522.PCD_Init();
  mfrc522.PCD_SetRegisterBitMask(mfrc522.RFCfgReg, (0x07<<4));
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);
  pinMode(A0, OUTPUT);
  setAllBaysHigh();
}

void loop() 
{
  if ( ! mfrc522.PICC_IsNewCardPresent() && ! mfrc522.PICC_ReadCardSerial()) 
  {
    //Reset RFID --timer wasn't working because it only got called after a card is actually read I think
    if (tCheck(&t_func1)) {
        resetRFID();
        tRun(&t_func1);
    }
    return;
  }

  //since we have the ID's stored as a byte array, we don't need to do any fancy reading of the ID
  validateID(mfrc522.uid.uidByte);  
}

void validateID(byte theID[4]) {
  for (int i = 0; i < sizeof(validIDs)/sizeof(validIDs[0]); i++)
  {
    //memcmp is a way to compare two byte arrays directly
    if (memcmp (theID, validIDs[i].ID, idLength)==0)
    {
      setBaysLow(validIDs[i].Bays);
      delay(lengthDelay);
      setAllBaysHigh();
      resetRFID();
      break;
     }
  }
}

void setBaysLow(byte Bays)
{
  if (Bays & 1 > 0) digitalWrite(A0, LOW);
  if (Bays & 2 > 0) digitalWrite(A1, LOW);
  if (Bays & 4 > 0) digitalWrite(A2, LOW);
  if (Bays & 8 > 0) digitalWrite(A3, LOW);
  if (Bays & 16 > 0) digitalWrite(A4, LOW);
  if (Bays & 32 > 0) digitalWrite(A5, LOW);
}

void setAllBaysHigh() {
  digitalWrite(A1, HIGH);
  digitalWrite(A2, HIGH);
  digitalWrite(A3, HIGH);
  digitalWrite(A4, HIGH);
  digitalWrite(A5, HIGH);
  digitalWrite(A0, HIGH);
  resetRFID();
}

void resetRFID()
{
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  mfrc522.PCD_SetRegisterBitMask(mfrc522.RFCfgReg, (0x07<<4));
}
