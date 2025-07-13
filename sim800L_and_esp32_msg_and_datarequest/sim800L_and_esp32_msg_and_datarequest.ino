#include <HardwareSerial.h>

HardwareSerial sim800(1); // UART1

#define SIM800_TX 16
#define SIM800_RX 17

String phoneNumber = "+91**********";//replace with phone number
String initialMessage = "Hello from ESP32 + SIM800L!";
String dataMessage = "Here is the data you requested.";

bool awaitingSMSBody = false;
String incomingSMS = "";
String senderNumber = "";

void sendATCommand(const String &cmd, int waitTime = 1000) {
  sim800.println(cmd);
  Serial.print("Sent: ");
  Serial.println(cmd);
  delay(waitTime);
  while (sim800.available()) {
    Serial.write(sim800.read());
  }
}

bool sendSMS(String number, String text) {
  sendATCommand("AT+CMGF=1", 500); // Set SMS mode to text

  sim800.print("AT+CMGS=\"");
  sim800.print(number);
  sim800.println("\"");

  // Wait for '>' prompt
  unsigned long timeout = millis();
  while (!sim800.available()) {
    if (millis() - timeout > 5000) {
      Serial.println("Timeout waiting for > prompt.");
      return false;
    }
  }

  while (sim800.available()) {
    char c = sim800.read();
    Serial.write(c);
    if (c == '>') break;
  }

  sim800.print(text);
  sim800.write(26); // CTRL+Z

  Serial.println("\nSending SMS:");
  Serial.println(text);

  // Wait for confirmation
  delay(5000);
  while (sim800.available()) {
    Serial.write(sim800.read());
  }

  return true;
}

void parseIncomingSMS(String line) {
  if (line.startsWith("+CMT:")) {
    int firstQuote = line.indexOf('"');
    int secondQuote = line.indexOf('"', firstQuote + 1);
    senderNumber = line.substring(firstQuote + 1, secondQuote);
    awaitingSMSBody = true;
    Serial.print("From: ");
    Serial.println(senderNumber);
  } else if (awaitingSMSBody) {
    incomingSMS = line;
    incomingSMS.trim();
    Serial.print("Received SMS Text: ");
    Serial.println(incomingSMS);
    awaitingSMSBody = false;

    String lower = incomingSMS;
    lower.toLowerCase();

    if (lower.indexOf("data") != -1) {
      Serial.println("Keyword 'data' found. Sending data message...");
      if (!sendSMS(senderNumber, dataMessage)) {
        Serial.println("Failed to send data message.");
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  sim800.begin(9600, SERIAL_8N1, SIM800_RX, SIM800_TX);
  delay(3000);

  Serial.println("Initializing SIM800L...");

  sendATCommand("AT");
  sendATCommand("AT+CPIN?");
  sendATCommand("AT+CREG?");
  sendATCommand("AT+CSQ");
  sendATCommand("AT+CLIP=1");           // Caller ID
  sendATCommand("AT+CNMI=2,2,0,0,0");   // Show new SMS directly

  sendSMS(phoneNumber, initialMessage); //  send welcome message
}

void loop() {
  static String line = "";
  while (sim800.available()) {
    char c = sim800.read();
    Serial.write(c);

    if (c == '\n') {
      line.trim();
      if (line.length() > 0) {
        parseIncomingSMS(line);
      }
      line = "";
    } else if (c != '\r') {
      line += c;
    }
  }
}
