#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
 
/* define port */
WiFiClient espClient;
PubSubClient client(espClient);
/* WIFI settings */
const char* ssid = "Wifiname"; //Enter your wifi ssid name
const char* password = "Password"; //Enter your Wifi Password
const char* mqttusername = "username"; //MQTT USER CREDENTIALS from Device Onboarding Page
const char* mqttpassword = "password"; //MQTT USER CREDENTIALS from Device Onboarding Page
String DEVICE_SERIAL = "SNUltrasonic01"; //Serailno of the device from Device Operation page
//Backend credentials
const char* mqtt_server = "mqtt.iot.ideamart.io"; //mqtt.exampledomain.com
char* EVENT_TOPIC = "lolin/nodemcuv3/ultrasonic/common"; //EVENT Topic from Device Onboarding Page
String SUB_TOPIC_STRING = "+/" + DEVICE_SERIAL + "/lolin/nodemcuv3/ultrasonic/sub"; //Action Topic from Device Onboarding Page
String PUB_TOPIC_STRING = "/" + DEVICE_SERIAL + "/lolin/nodemcuv3/ultrasonic/pub"; //Action Topic Response from Device Onboarding Page
char msg[1024];

//====================================================Connecting to Wifi
void setup_wifi() {
	delay(10);
	// We start by connecting to a WiFi network
	Serial.println();
	Serial.print("Connecting to ");
	Serial.println(ssid);
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
		}
	randomSeed(micros());
	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
	//=====================================================MQTT
	}
//receiving a message
void callback(char* topic, byte* payload,long length) {
  Serial.print("Message arrived [");
  Serial.print(SUB_TOPIC_STRING);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    msg[i] = (char)payload[i];
  }
  //do_actions(msg);
}
void reconnect() {
	// Loop until we're reconnected
	while (!client.connected()) {
		Serial.print("Attempting MQTT connection...");
		// Create a random client ID
		String clientId = "ESP8266Client-";
		clientId += String(random(0xffff), HEX);
		// Attempt to connect
		if (client.connect(clientId.c_str(), "mqttusername", "mqttpassword")) {
			Serial.println("connected");
			//subscribe to the topic
			const char* SUB_TOPIC = SUB_TOPIC_STRING.c_str();
			client.subscribe(SUB_TOPIC);
			} else {
			Serial.print("failed, rc=");
			Serial.print(client.state());
			Serial.println(" try again in 5 seconds");
			// Wait 5 seconds before retrying
			delay(5000);
				}
			}
		}

void publish_message(const char* message) {
	Serial.println(message); //print to the console
	client.publish(EVENT_TOPIC, message); //post to the Event topic on the server
		}
void readSensorValues(){
		int h = 40 ;
		String message = "{\"eventName\":\"getDistance\",\"status\":\"none\",\"evenparam\":{\"distance\":\"" + 	String(h) + "\"},\"id\":\""+ DEVICE_SERIAL +"\"}";		
      		char* msg = (char*)message.c_str();
		publish_message(msg); //send the event to backend
		}

void send_event() {
	//Should call publish_message() function to send events. Pass your message as parameter
	publish_message(msg);
}

void setup() {
	// put your setup code here, to run once:
	Serial.begin(9600); //initialise serial port
	setup_wifi(); //initialise wifi
	client.setServer(mqtt_server, 1883); //establish mqtt server connection
	Serial.println("Distance from Ultrasonic Sensor\n\n"); // any message to print to the serial console
	}
void loop() {
	// put your main code here, to run repeatedly:
	if (WiFi.status() != WL_CONNECTED) {
		setup_wifi();
		}
		if (WiFi.status() == WL_CONNECTED && !client.connected()) {
			reconnect();
			}
	
	readSensorValues();  //Read vlaues from sensors
	delay(10000); // every 10 seconds
	client.loop(); //repeat
	}
