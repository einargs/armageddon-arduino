#define CMD_SEPARATOR ':'
#define LINE_TERMINATOR ';'

typedef String CmdSpace; // Command space
CmdSpace LIGHT_CMD = "LT"; // LED control.
CmdSpace MOTOR_CMD = "MC"; // Motor control.
CmdSpace SERIAL_CMD = "SR"; // Serial control.

String inputString = "";

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  inputString.reserve(200); // reserve 200 bytes for inputString
  Serial.println("starting");
}

// the loop function runs over and over again forever
void loop() {
  while (Serial.available()) {
    char inChar = (char) Serial.read();
    
    if (inChar == LINE_TERMINATOR) {
      onSerialLine(inputString);
      inputString = "";
    } else {
      inputString += inChar;
    }
  }
}

void onSerialLine(String line) {
  line.trim(); // mutates
  handleCmdString(line);  
  Serial.println("Recieved: " + line);
}

//
String getCmdChunk(String cmdString, int index) {
  int startIndex = 0;
  int indexCount = 0;
  while (true) {
    int nextSeperatorIndex = cmdString.indexOf(CMD_SEPARATOR, startIndex);
    boolean isEnd = (nextSeperatorIndex == -1);

    if (index == indexCount) {
      if (isEnd) {
        return cmdString.substring(startIndex);
      } else {
        return cmdString.substring(startIndex, nextSeperatorIndex);
      }
    } else if (isEnd) {
      return "";
    } else {
      indexCount++;
      startIndex = nextSeperatorIndex+1;
    }
  }
}
CmdSpace getCmdSpace(String cmdString) {
  return getCmdChunk(cmdString, 0);
}
void handleCmdString(String cmdString) {
  String cmdSpace = getCmdSpace(cmdString);
  if (cmdSpace.equals(LIGHT_CMD)) {
    handleLightCmd(cmdString);
  } else if (cmdSpace.equals(SERIAL_CMD)) {
    handleSerialCmd(cmdString);
  }
}


// LIGHT command space
// Command format: "LT:[ledId]:[state]"
// ledId: integer|"builtin"
// state: "on":"off"
int getLedPin(String cmdString) {
  String ledId = getCmdChunk(cmdString, 1);
  if (ledId.equals("builtin")) {
    return LED_BUILTIN;
  } else {
    return ledId.toInt();
  }
}
String getLedState(String cmdString) {
  return getCmdChunk(cmdString, 2);
}
void handleLightCmd(String cmdString) {
  boolean setToOn = getLedState(cmdString);
  int ledPin = getLedPin(cmdString);
  String state = getLedState(cmdString);
  if (state.equals("on")) {
    digitalWrite(ledPin, HIGH);
  } else if (state.equals("off")) {
    digitalWrite(ledPin, LOW);
  }
}


// SERIAL command space
// Command format: "SR:[msg]"
// msg: string
String getSerialMessage(String cmdString) {
  return getCmdChunk(cmdString, 1);
}
void handleSerialCmd(String cmdString) {
  String message = getSerialMessage(cmdString);
  Serial.println(message);
}


// MOTOR command space
// Command format: "MC:[motorId]:[position]"
// motorId: string
// position: integer
