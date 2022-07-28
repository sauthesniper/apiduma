#include <SoftwareSerial.h>
#include <Keypad.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
SoftwareSerial printer(68,69);
SoftwareSerial mySerial(10,11);
SoftwareSerial dispenser(66,67);
#define PN532_SCK  (52)
#define PN532_MOSI (51)
#define PN532_SS   (53)
#define PN532_MISO (50)
Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);
const byte ROWS = 4;const byte COLS = 4; 
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {30,32,34,36}; byte colPins[COLS] = {22,24,26,28}; 
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
String apn = "live.vodafone.com";   
String apn_username = "";
void send_to_serial(String command,int importance) {
  mySerial.println(command);char tmp[1000]; int tmpindex=0;
  if (importance==500)
  {
     while (!mySerial.available()) delay(15);
      while (mySerial.available()) 
      {
        char t=mySerial.read();
        tmp[++tmpindex]=t;
        delay(5);
        Serial.write(t);
      }
    while (!mySerial.available()) delay(15);
      while (mySerial.available()) 
      {
        char t=mySerial.read();
        tmp[++tmpindex]=t;
        delay(5);
        Serial.write(t);
      }
      return;
  }
if (importance==101)
    {
      while (tmp[tmpindex-1]!='O'&&tmp[tmpindex]!='K') 
        while (mySerial.available()) 
        {
          char t=mySerial.read();
          tmp[++tmpindex]=t;
          Serial.write(t);
        }
        return;
    }
if (importance==201){
     while (!mySerial.available()) delay(30);
      while (mySerial.available()) 
      {
        char t=mySerial.read();
        tmp[++tmpindex]=t;
        delay(10);
        Serial.write(t);
      }
      return;
  }
}
void readSerial()
{
    while (mySerial.available()) Serial.write(mySerial.read());
}
void configure_gprs() 
{
  Serial.println(" --- CONFIGURARE GPRS --- ");
  send_to_serial("AT+SAPBR=3,1,Contype,GPRS",101);
  send_to_serial("AT+SAPBR=3,1,APN,live.vodafone.com",101);
  send_to_serial("AT+SAPBR=1,1",101);
  send_to_serial("AT+SAPBR=2,1",101);
  send_to_serial("AT+HTTPINIT",101); 
  send_to_serial("AT+HTTPPARA=CID,1",101); 
}
void send_to_server( String data) 
{
  send_to_serial("AT+HTTPPARA=URL,http://identify-solutions.eu/API.php?syntax=arduino&session="+data,101); 
  send_to_serial("AT+HTTPACTION=0",500);
  mySerial.println("AT+HTTPREAD");delay(200);
    char srv_data[500]={0}; ;int srv_data_index=0,i=0;
    char message[200]={0},message_index=0,joke_name[25]={0},joke_name_index=0;
    while(mySerial.read()!='#') ;
    char nextChar=mySerial.read();
    if (nextChar=='X')
    {
      print_begin(); return;
    }
    if (nextChar=='Y')
    {
      char score=mySerial.read();
      print_end(score); return;
    }
    srv_data[srv_data_index]=nextChar; srv_data_index+=1;
    while (mySerial.available()) 
      { 
        char t=mySerial.read();
        srv_data[srv_data_index]=t;
        srv_data_index+=1;
      }
     for (i;srv_data[i];i++) Serial.write(srv_data[i]); i=0;
     for (i;srv_data[i]!='%';i++) 
      {
        joke_name[joke_name_index]=srv_data[i];
        joke_name_index+=1;
      } 
     Serial.println(joke_name);i++;
     for (i;srv_data[i]!='#';i++) {message[message_index]=srv_data[i];message_index+=1;} 
     Serial.println(message);
     print_ticket(joke_name,message); 
}
void feed(int lines)
{
printer.write(27);
printer.write(100);
printer.write(lines);
printer.write(10);
}

void cut()
{
feed(3);
printer.write(27);
printer.write(105);
printer.write(10);
}

void color(bool col)
{
printer.write(29);
printer.write(66);
printer.write(col);
printer.write(10);
}

void size(int siz)
{
printer.write(29);
printer.write(33);
printer.write(siz);
printer.write(10);
}
void begin()
{
printer.write(27);
printer.write(64);
printer.write(10);
}

void justify(int just)
{
printer.write(27);
printer.write(97);
printer.write(just);
printer.write(10);
}
void print_ticket(char *camera, char *mesaj)
{
  begin();
  justify(1);size(2);
  printer.println("APIDUMA TICKETS");
  justify(0);size(1);
  color(0);
  printer.print("CAMERA: ");
  printer.println(camera);
  printer.print("DUMA:");
  size(0);
  printer.println(mesaj);
  justify(1);size(2);
  printer.println("APIDUMA TICKETS");
  justify(0);cut();
}
void print_begin()
{
  begin();
  justify(1);size(2);
  printer.println("NUMBER HUNTER");
  justify(0);size(1);
  printer.println("SALUT,"); feed(1);
  printer.println("Acesta este NumberHunter,");
  printer.println("o cale mistica prin care tu vei face turulintregii cladiri, intr-un mod cat mai");
  printer.println("distractiv!");
  printer.println("REGULI: Regulile sunt simple. ");
  printer.println("Vei primi un bilet cu un citat sau o duma");
  printer.println("care te va inspira sa ne aduci un numar."); feed(1);
  printer.println("Biletul iti da cateva informatii despre o camera unde trebuie TU sa ajungi si sa cauti acel numar.");
  printer.println("Atentie, raspunsurile corecte valoareaza puncte!");
  printer.println("Fauritorii de lumi iti ureaza succes si multa grija! Capcanele pot fi la orice pas . Strabunii doritori de API-uri vor ucide orice plebeu care tradeaza regula de minim scor maxim +1.");
  justify(1);size(2);
  printer.println("NUMBER HUNTER");
  justify(0);cut();
}
void print_end(char scor)
{
  begin();
  justify(1);size(2);
  printer.println("NUMBER HUNTER");
  justify(0);size(1);
  printer.println("FELICITARI,"); feed(1);
  printer.println("Acesta a fost NumberHunter,");
  printer.println("Ai ajuns la capatul lumilor create");
  printer.println("de fauritorul de lumi.");
  printer.println(" ");color(1);
  printer.print("SCORUL TAU: ");printer.println(scor);
  printer.println(" ");color(0);
  printer.println("Te mai asteptam!");
  justify(1);size(2);feed(1);
  printer.println("NUMBER HUNTER");
  justify(0);cut();
}
int set_seed()
{
  int seed=1;
  mySerial.println("AT+CCLK?"); delay(100);
  for (int k=1;k<=28;k++) mySerial.read();
  char t=mySerial.read(); if (t!='0') seed=seed*(t-48);
  t=mySerial.read(); if (t!='0') seed=seed*(t-48);
  t=mySerial.read(); if (t!='0') seed=seed*(t-48);
  t=mySerial.read(); if (t!='0') seed=seed*(t-48);
  mySerial.read();
  t=mySerial.read(); if (t!='0') seed=seed*(t-48);
  t=mySerial.read(); if (t!='0') seed=seed*(t-48);
  mySerial.read();
  t=mySerial.read(); if (t!='0') seed=seed*(t-48);
  t=mySerial.read(); if (t!='0') seed=seed*(t-48);
  readSerial();
  return seed;
}
int randomint;
void setup() {
  Serial.begin(115200);
  printer.begin(9600);
  mySerial.begin(9600);
    configure_gprs();
    randomSeed(set_seed());
  randomint=random(100000,999999);

}
void loop() {
  String data="S";data.concat(randomint);
  String util="&ans="; data.concat(util);
  while (!Serial.available()) delay(10);
  char ans[20]={0},ans_index=0;
  for (int i=1;i<=10;i++) Serial.println();
  Serial.println("INTRODU CODUL URMATOAREI CAMERE: ");
  while (Serial.available()&&ans_index<15)
  {
    ans[ans_index]=Serial.read();
    ans_index+=1;
  } while(Serial.available()) Serial.read();
  data.concat(ans);
  Serial.println(data);
  send_to_server(data);
}