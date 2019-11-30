#include <DallasTemperature.h>
#include <EEPROM.h>
#include <SPI.h>
#include <Ethernet.h>
#include <OneWire.h>
#include <SD.h>
#include <Adafruit_MCP23017.h>
#include <LiquidCrystal.h>
#include "Definitions.h"

String readString;
byte i;
byte dsAddress[8];
unsigned long checksum;
boolean loggedIn = false;

struct Network {
	boolean dhcp;
	byte ip[4];
	byte gateway[4];
	byte subnet[4];
	byte dns[4];
};

struct User {
	boolean enabled;
	char userName[20];
	char password[20];
};

LiquidCrystal lcd(RS, ENABLE, D0, D1, D2, D3);//LCD Pins

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

EthernetServer server(PORT);

OneWire ds(DS18B20);

User user;
Network networkConfig;

boolean relayValues[] = {0, 1, 0, 1, 0, 0, 0, 0};

void setup()
{
	Serial.begin(38400);

	//Check EEPROM
	if(EEPROM.get(CRC32ADDRESS, checksum) != eeprom_crc()){
		Serial.println(F("EEPROM ERROR: Values reset to default"));
		Serial.println(F("Username: admin\nPassword: password"));
		reset();
	}else{
		Serial.println("EEPROM CRC Check Successful");
	}

	//Start the SD Card... No point continuing if there is no SD card detected
	while(!SD.begin(4)){
		Serial.println(F("SD Failed"));
	}

	//search for connected DS18B20's
	ds.reset_search();

	//set the MAC address according to the first DS18B20
	if(!ds.search(dsAddress)){
		Serial.println("Not found. Using default MAC address.");
	}else{
		Serial.println("success. Setting MAC address");
		mac[1] = dsAddress[3];	//Offset array to skip DS18B20 family code, and skip mac[0]
		mac[2] = dsAddress[4];
		mac[3] = dsAddress[5];
		mac[4] = dsAddress[6];
		mac[5] = dsAddress[7];
	}

	//Start Ethernet connection
	while(Ethernet.begin(mac) == 0){
		Serial.println("No Ethernet");
	}
	Serial.println(Ethernet.localIP());

	//Start the Ethernet server.
	server.begin();
}

void loop()
{
	String s;
	EthernetClient client = server.available();

	if(client){
		Serial.println("new client");

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

	Serial.print("Method: ");
	Serial.println(method);
	Serial.println(request);

	if(method.equals("GET") && request.indexOf('?') == -1){
	    publish(client, getFileName(request));
	}else if(method.equals("GET") && request.indexOf('?') > -1){
		String tmp = request.substring(request.indexOf('?'));
		message = tmp.substring(0, tmp.indexOf(' '));
		if(message.indexOf('?_') > -1){
			publish(client, getFileName(request));
		}else{
			process(client, method, message);
		}
		
	}else if(method.equals("POST")){
		while(client.available()){
			message += (char)client.read();
		}
		Serial.print("Post Message: ");
		Serial.println(message);
		process(client, method, message);
	}

	// give the web browser time to receive the data
	delay(1);
	client.stop();
	Serial.println("client disconnected");
}

void publish(EthernetClient &client, String fileName){
	String returnType;

	if(fileName.endsWith(".htm") || fileName.endsWith(".html")){
		returnType = F("Content-Type: text/html");
		serveFile(client, fileName, returnType);
	}else if(fileName.endsWith(".css")){
		returnType = F("Content-Type: text/css");
		serveFile(client, fileName, returnType);
	}else if(fileName.endsWith(".js")){
		returnType = F("Content-Type: text/javascript");
		serveFile(client, fileName, returnType);
	}else if(fileName.endsWith(".svg") || fileName.endsWith(".xml")){
		returnType = F("Content-Type: image/svg+xml");
		serveFile(client, fileName, returnType);
	}else if(fileName.endsWith(".val")){
		getValue(client, fileName);
	}else{
		returnType = F("Content-Type: text/html");
		serveFile(client, fileName, returnType);
	}
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
		fileName = "index.htm";
	}
	return fileName;
}

void serveFile(EthernetClient &client, String &fileName, String returnType){
	const String htmlcont = F("Content-Type: text/html");
	const String closeConnec = F("Connection: close");

	Serial.print("File Name: ");
	Serial.println(fileName);

	File current = SD.open(fileName);
	if(current){
		Serial.println("Serving");
		client.println(F("HTTP/1.1 200 OK"));
		client.println(returnType);
		client.println(closeConnec);  // the connection will be closed after completion of the response
		client.println();//10

		while(current.available()){
			client.write(current.read());
		}
		current.close();
		Serial.println("Served");
	}else{
		Serial.println("Failed to get file");
		client.println(F("HTTP/1.1 404 Not Found"));
		client.println(F("Content-Type: text/html"));
		client.println(closeConnec);  // the connection will be closed after completion of the response
		client.println();
		client.println(F("<!DOCTYPE HTML>"));
		client.println(F("<html>"));
		client.println(F("<h1> Error 404 - File Not Found :( </h1>"));
		client.println(F("</html>"));
	}
}

void getValue(EthernetClient &client, String &fileName){
	client.println(F("HTTP/1.1 200 OK"));
	client.println("Content-Type: text/html");
	client.println("Connection: close");
	client.println();
	String output;
	for(int i = 0; i < 8; i++){
		output = "<div id=relay" + String(i+1) + ">" + String(relayValues[i]) + "</div>";
		Serial.println(output);
		client.println(output);
	}
}

void process(EthernetClient &client, String &method, String &message){
	client.println(F("HTTP/1.1 200 OK"));
	client.println("Content-Type: text/html");
	client.println("Connection: close");
	client.println();
	client.println("<!DOCTYPE HTML>");
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