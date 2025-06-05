// const String Command = "LD 3 \r\n"
//                        "AND 2 \r\n"
//                        "OUT 8 \r\n";

const String Command = "LD 3 \r\n"
                       "AND 2 \r\n"
                       "OUT 8 \r\n"
                       "AND 4 \r\n"
                       "AND 5 \r\n"
                       "OUT 8 \r\n";

int Acc = 0;
int posCommand = 0;
bool runflag = false;  //if power on auto start . runflag should be true
String TermString = "";

String cmdArray[10];  //translated commands
int cmdArg[10];    //argument of translated commands
//string cmdArg2[10];    //argument of translated commands

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  for (int i = 2; i <= 7; i++) {
    pinMode(i, INPUT_PULLUP);
  }
  for (int i = 8; i <= 11; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }

//Translate Command strings and store to arrays
  String command = Command;
  posCommand = 0;
  while(command.indexOf("\r\n") != -1) {
    int index = command.indexOf("\r\n");
  //上２行 こう書いてもいい？
  //while(int index = command.indexOf("\r\n") != -1) {
    String line = command.substring(0, index);
    setCommand2Array(line); //ignore error commannd 
    command = command.substring(index + 2);  //
  }
  cmdArray[posCommand] = "xxxx";  //　exit code
  posCommand = 0;
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    if (c == "\n") {
      doTermCommand(TermString);
      TermString = "";
    } else {
      TermString += c;
    }
  }
  if (runflag) {
    doCommand();
  }
}

void doTermCommand(String str) {
  str.trim();
  if (str == "R") {
    runflag = true;
    posCommand = 0;
    Acc = 0;
  } else if (str == "S") {
    runflag = false;
  }
}

String doCommand() {

  String cmd = cmdArray[posCommand];
  int arg = cmdArg[posCommand];
  //  int arg2 = cmdArg[posCommand]; // if you want
  posCommand++;
  if (cmd == "LD") {
    Acc = digitalRead(arg);  // Hi level mean Sw On.    if Hi level mean Sw Off..Should be reverse the logic.
  } else if (cmd == "AND") {
    Acc = Acc && digitalRead(arg);
  } else if (cmd == "OR") {
    Acc = Acc || digitalRead(arg);
  } else if (cmd == "NOT") {
    Acc = !digitalRead(arg);
  } else if (cmd == "NAND") {
    Acc = !(Acc && digitalRead(arg));
  } else if (cmd == "NOR") {
    Acc = !(Acc || digitalRead(arg));
  } else if (cmd == "OUT") {
    digitalWrite(arg, Acc ? HIGH : LOW);  //Hi mean On
  } else if (cmd == "NOUT") {
    digitalWrite(arg, Acc ? LOW : HIGH);
  } else if (cmd == "xxxx") {
    posCommand = 0;
    Acc = 0;
    return;
  }
}

bool setCommand2Array(String line) {  // if error return true
  line.trim();
  int i = line.indexOf(' ');
  if (i > 0) {
    String cmd = line.substring(0, i);
    String arg = line.substring(i + 1);

    if (cmd == "LD" || cmd == "AND" || cmd == "OR" || cmd == "NOT" || cmd == "NAND" || cmd == "NOR" || cmd == "OUT" || cmd == "NOUT") {
      //std commands will have an arg.
      int pin = arg.toInt();
      //if(pin==-1){//error todo }
      cmdArray[posCommand] = cmd;
      cmdArg[posCommand++] = pin;
      return false;
      //  } else if (cmd == "timer") {  // add something new
      //  } else if (cmd == "pulse") {
    } else {
      Serial.print("There is Error in Program. \r\n Line ");
      Serial.println(posCommand);
      cmdArray[posCommand++] = "error";
      delay(2000);
      return true;
    }
  }
}
