# ESP32-SIM800L-SMS-Responder
This project uses an ESP32 with a SIM800L GSM module to create an **SMS-based responder system**. It listens for incoming SMS messages and automatically replies with predefined responses when specific keywords (like "data") are detected.

✅ Sends a welcome SMS on startup
- ✅ Listens for incoming SMS messages
- ✅ Extracts sender’s number and message content
- ✅ Sends an automatic reply when keyword "data" is detected
- ✅ Uses Hardware Serial (UART1) for communication with SIM800L
- ✅ Debug output to Serial Monitor
