#include <DallasTemperature.h>
#include <EEPROM.h>
#include <SPI.h>
#include <Ethernet.h>
#include <OneWire.h>
#include <SD.h>
#include <Adafruit_MCP23017.h>

//#define DEBUG

#ifdef DEBUG
	#define DB_SERIAL_BEGIN(x) Serial.begin(x)
	#define DB_SPRINT(x) Serial.print(x)
	#define DB_SPRINTLN(x) Serial.println(x)
	#define DB_LCDPRINT(x, y, p) lcd.setCursor(x,y), lcd.print(p)
	#define WARN_SERIAL_BEGIN(x) Serial.begin(x)
	#define WARN_SPRINT(x) Serial.print(x)
	#define WARN_SPRINTLN(x) Serial.println(x)
	#define WARN_LCDPRINT(x, y, p) lcd.setCursor(x,y), lcd.print(p)
#else
	#define DB_SERIAL_BEGIN(x)
	#define DB_SPRINT(x)
	#define DB_SPRINTLN(x)
	#define DB_LCDPRINT(x, y, p)
#endif

#ifdef WARN
	#define WARN_SERIAL_BEGIN(x) Serial.begin(x)
	#define WARN_SPRINT(x) Serial.print(x)
	#define WARN_SPRINTLN(x) Serial.println(x)
	#define WARN_LCDPRINT(x, y, p) lcd.setCursor(x,y), lcd.print(p)
#else
	#define WARN_SERIAL_BEGIN(x)
	#define WARN_SPRINT(x)
	#define WARN_SPRINTLN(x)
	#define WARN_LCDPRINT(x, y, p)
#endif

//buffer size for processing HTML commands
#define MAXBUFF 40

//pins
#define DS18B20 49 //DS18B20 Pin
#define MICROSD 4 //Micro SD Card Pin

#define RELAY1 0			//Relay Numbers
#define RELAY2 1
#define RELAY3 2
#define RELAY4 3
#define RELAY5 4
#define RELAY6 5
#define RELAY7 6
#define RELAY8 7

#define LCDBL 36			//LCD Backling Pin
#define LEDR 42				//RGB Status LED pins
#define LEDG 44
#define LEDB 45

LiquidCrystal lcd(33, 30, 37, 34, 35, 32);//LCD Pins

//Generic HTML Commands
#define HTMLCONTENT F("Content-Type: text/html")
#define CLOSECONNECTION F("Connection: close")
#define DOCTYPEHTML F("<!DOCTYPE HTML>")

//EEPROM ADDRESSES
#define CRC32ADDRESS 0
#define USERADDRESS 4
#define NETWORKADDRESS 45

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
byte port[] = {8,0,8,0};
EthernetServer server(port);

String readString;
byte i;
byte dsAddress[8];
OneWire ds(DS18B20);
unsigned long checksum;

struct Network {
	boolean dhcp;
	byte ip[4];
	byte gateway[4];
	byte subnet[4];
	byte dns[4];
	byte port[4];
};

struct User {
	boolean enabled;
	char userName[20];
	char password[20];
};

User user;
Network networkConfig;
boolean loggedIn = false;

void setup()
{
	DB_SERIAL_BEGIN(38400);
	WARN_SERIAL_BEGIN(38400);

	//Check EEPROM
	if(EEPROM.get(CRC32ADDRESS, checksum) != eeprom_crc()){
		WARN_SPRINTLN(F("EEPROM ERROR: Values reset to default"));
		WARN_SPRINTLN(F("Username: admin\nPassword: password"));
		reset();
	}else{
		DB_SPRINTLN("EEPROM CRC Check Successful");
	}

	//Start the SD Card... No point continuing if there is no SD card detected
	while(!SD.begin(4)){
		WARN_SPRINTLN(F("SD Failed"));
	}

	//search for connected DS18B20's
	ds.reset_search();

	//set the MAC address according to the first DS18B20
	if(!ds.search(dsAddress)){
		DB_SPRINTLN("Not found. Using default MAC address.");
	}else{
		DB_SPRINTLN("success. Setting MAC address");
		mac[1] = dsAddress[3];	//Offset array to skip DS18B20 family code, and skip mac[0]
		mac[2] = dsAddress[4];
		mac[3] = dsAddress[5];
		mac[4] = dsAddress[6];
		mac[5] = dsAddress[7];
	}

	//Start Ethernet connection
	while(Ethernet.begin(mac) == 0){
		DB_SPRINTLN("No Ethernet");
	}
	WARN_SPRINTLN(Ethernet.localIP());

	//Start the Ethernet server.
	server.begin();
}

void loop()
{
	String s;
	EthernetClient client = server.available();

	if(client){
		DB_SPRINTLN("new client");

		//a HTTP request ends with a blank line
		boolean currentLineIsBlank = true;
		while(client.connected()){
			if(client.available()){
				char c = client.read();

				s+=c;

				if(c == '\n' && currentLineIsBlank){
					interpret(client, s);
				}
				if (c == '\n') {
					// you're starting a new line
					currentLineIsBlank = true;
				} else if (c != '\r') {
					// you've gotten a character on the current line
					currentLineIsBlank = false;
				}
			}
		}
	}
}

void interpret(EthernetClient &client, String &request){
	String message;
	String method = request.substring(0, request.indexOf(' '));

	DB_SPRINT("Method: ");
	DB_SPRINTLN(method);
	DB_SPRINTLN(request);

	if(method.equals("GET") && request.indexOf('?') == -1){
	    publish(client, getFileName(request));
	}else if(method.equals("GET") && request.indexOf('?') > -1){
		String tmp = request.substring(request.indexOf('?'));
		message = tmp.substring(0, tmp.indexOf(' '));
		process(client, method, message);
	}else if(method.equals("POST")){
		while(client.available()){
			message += (char)client.read();
		}
		DB_SPRINT("Post Message: ");
		DB_SPRINTLN(message);
		process(client, method, message);
	}

	// give the web browser time to receive the data
	delay(1);
	client.stop();
	DB_SPRINTLN("client disconnected");
}

void publish(EthernetClient &client, String fileName){
	String returnType;
	const String htmlcont = HTMLCONTENT;
	const String closeConnec = CLOSECONNECTION;


	if(fileName.endsWith(".htm") || fileName.endsWith(".html")){
		returnType = HTMLCONTENT;
	}else if(fileName.endsWith(".css")){
		returnType = F("Content-Type: text/css");
	}else if(fileName.endsWith(".js")){
		returnType = F("Content-Type: text/javascript");
	}else if(fileName.endsWith(".svg") || fileName.endsWith(".xml")){
		returnType = F("Content-Type: image/svg+xml");
	}else{
		returnType = HTMLCONTENT;
	}

	DB_SPRINT("File Name: ");
	DB_SPRINTLN(fileName);

	File current = SD.open(fileName);
	if(current){
		DB_SPRINTLN("Serving");
		client.println(F("HTTP/1.1 200 OK"));
		client.println(returnType);
		client.println(closeConnec);  // the connection will be closed after completion of the response
		client.println();//10

		while(current.available()){
			client.write(current.read());
		}
		current.close();
		DB_SPRINTLN("Served");
	}else{
		DB_SPRINTLN("Failed to get file");
		client.println(F("HTTP/1.1 404 Not Found"));
		client.println(HTMLCONTENT);
		client.println(closeConnec);  // the connection will be closed after completion of the response
		client.println();
		client.println(DOCTYPEHTML);
		client.println(F("<html>"));
		client.println(F("<h1> Error 404 - File Not Found :( </h1>"));
		client.println(F("</html>"));
	}

	// give the web browser time to receive the data
	delay(1);
	client.stop();
	DB_SPRINTLN("client disconnected");
}

String getFileName(String &request){
	char req[MAXBUFF];
	(request.substring(request.indexOf('/'))).toCharArray(req, MAXBUFF);
	String fileName;

	for(int i = 0; i < strlen(req); i++){
		if(req[i] != '\n' && req[i] != '\r' && req[i] != ' ' && req[i] != '?'){
			fileName += req[i];
		}else{
			break;
		}
	}
	if(fileName.equals("/")){
		fileName = "index.php";
	}
	return fileName;
}

void process(EthernetClient &client, String &method, String &message){
	client.println(F("HTTP/1.1 200 OK"));
	client.println(HTMLCONTENT);
	client.println(CLOSECONNECTION);
	client.println();
	client.println(DOCTYPEHTML);
	client.println(F("<html>"));
	client.print("<h1> ");
	client.print(method);
	client.println(F(" TEST OK </h1>"));
	client.print("<p> ");
	client.print(message);
	client.println(" </p>");
	client.println(F("</html>"));
}

void reset(){
	User defUser{
		true,
		F("admin"),
		F("password")
	};

	Network defNetworkConfig{
		true,
		{192, 168, 1, 101},
		{192, 168, 1, 1},
		{255, 255, 255, 0},
		{192, 168, 1, 1},
		{8, 0, 8, 0}
	};

	EEPROM.put(USERADDRESS, defUser);
	EEPROM.put(NETWORKADDRESS, defNetworkConfig);
	EEPROM.put(CRC32ADDRESS, eeprom_crc());
}

template<typename T> void eeprom_update(int address, T &data){
	if(address > 3){
		EEPROM.put(address, *data);
		EEPROM.put(CRC32ADDRESS, eeprom_crc());
	}
}

unsigned long eeprom_crc() {

  const unsigned long crc_table[16] = {
    0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
    0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
    0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
  };

  unsigned long crc = ~0L;

  for (int index = USERADDRESS; index < EEPROM.length()  ; ++index) {
    crc = crc_table[(crc ^ EEPROM[index]) & 0x0f] ^ (crc >> 4);
    crc = crc_table[(crc ^ (EEPROM[index] >> 4)) & 0x0f] ^ (crc >> 4);
    crc = ~crc;
  }
  return crc;
}