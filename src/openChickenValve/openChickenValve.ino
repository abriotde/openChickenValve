int CLOSE_BUTTTON = 4; // pin close button
int OPEN_BUTTON = 5;   // pin open button
int PIN_DOOR_CLOSED = 6;
int PIN_DOOR_OPEN = 7;
int MotorPin1 = 8;  // pin IN1 L293D
int MotorPin2 = 9;  // pin IN2 L293D
int MotorPin3 = 10; // pin IN3 L293D
int MotorPin4 = 11; // pin IN4 L293D
int PIN_LIGHT = A0; // pin LDR

int THRESHOLD_DAY = 350;   // threshold of photoresistor for is_daytime
int THRESHOLD_NIGHT = 200; // threshold of photoresistor for is_nighttime

int light_level = 0;
int delayTime = 10;            // delay for 10ms
int Tempo_light_level = 10000; // light delay for 10 seconds = 10000ms
boolean is_door_closed = false;
boolean is_door_open = false;
boolean is_door_detected_closed = false;
boolean is_door_detected_open = false;
boolean state_door_detected_closed = false, state_door_detected_open = false;
boolean mem_h = false, mem_b = false, mem_door_detected_closed = false, mem_door_detected_open = false;
boolean mem_movment = false;
boolean mem_light = false;
boolean mem_init = false;
boolean light_detected = false;
boolean is_daytime = true; // is_daytime 1 = day, 0 = night
boolean is_init = false;
boolean activeTempo = false;

// Temps à l'activation de la tempo
unsigned long tempoDepart = 0;

/* void init()
{
    door_init();
} */
void setup()
{
  Serial.begin(9600);
  pinMode(CLOSE_BUTTTON, INPUT_PULLUP);
  pinMode(OPEN_BUTTON, INPUT_PULLUP);
  pinMode(PIN_DOOR_CLOSED, INPUT_PULLUP);
  pinMode(PIN_DOOR_OPEN, INPUT_PULLUP); 
  pinMode(MotorPin1, OUTPUT);            // A-
  pinMode(MotorPin2, OUTPUT);            // A+
  pinMode(MotorPin3, OUTPUT);            // B-
  pinMode(MotorPin4, OUTPUT);            // B+
  
  door_init();
  init();
}


void loop()
{
    light_level = analogRead(PIN_LIGHT);
    if (is_init)
    {
        Serial.println(light_level);
        if (light_level >= THRESHOLD_DAY)
        {
            light_detected = true;
        }else if (light_level <= THRESHOLD_NIGHT)
        {
            light_detected = false;
        }
        if (light_detected != mem_light)
        {
            activeTempo = true;
            tempoDepart = millis();
            Serial.println("Light on");
        }
        if (light_detected && activeTempo && ((millis() - tempoDepart) >= Tempo_light_level))
        {
            is_daytime = true;
            Serial.println("It is day time.");
            activeTempo = false;
            if (!is_door_detected_closed && !is_door_open)
            {
                door_open();
            }
        }
        mem_light = light_detected;

        if (!light_detected && activeTempo && (millis() - tempoDepart) >= Tempo_light_level)
        {
            is_daytime = false;
            Serial.println("It is night time.");
            activeTempo = false;
            if (!is_door_detected_closed && !is_door_closed)
            {
                door_close();
            }
        }
        mem_light = light_detected;
    }

    state_door_detected_closed = !digitalRead(CLOSE_BUTTTON);
    state_door_detected_open = !digitalRead(OPEN_BUTTON); 
    is_door_detected_closed = !digitalRead(PIN_DOOR_CLOSED);
    is_door_detected_open = !digitalRead(PIN_DOOR_OPEN);

    if (is_door_detected_closed != mem_door_detected_closed) // Changement d'état du fin de course haut (front montant ou descendant)
    {
        if (is_door_detected_closed)
        {
            Serial.println("Door closed.");
        }
        if (!is_door_detected_closed)
        {
            Serial.println("Door not closed");
        }
    }
    if (is_door_detected_open != mem_door_detected_open) // Changement d'état du fin de course bas (front montant ou descendant)
    {
        if (is_door_detected_open)
        {
            Serial.println("Door open.");
        }
        if (!is_door_detected_open)
        {
            Serial.println("Door not open.");
        }
    }
    mem_door_detected_open = is_door_detected_open;

    if (state_door_detected_closed != mem_h) // state change of button hight
    {
        Serial.println("Button hight");                                                                        
        if (state_door_detected_closed && !state_door_detected_open && !is_door_detected_closed && !is_door_closed) // button hight but not low
        {
            door_close(); // Lancer la fonction sens normal
        }
    }
    mem_h = state_door_detected_closed;        // Mémorisation du nouvel état du bouton haut
    if (state_door_detected_open != mem_b) // state change of button low
    {
        if (state_door_detected_open && !state_door_detected_closed && !is_door_detected_closed && !is_door_open) // button low but not hight
        {
            door_open();
        }
    }
    mem_b = state_door_detected_open; // Mémorisation du nouvel état du bouton bas
}

void door_init()
{
    delay(5000);
    while (!is_door_detected_closed)
    {
        digitalWrite(MotorPin1, HIGH);
        digitalWrite(MotorPin2, HIGH);
        digitalWrite(MotorPin3, LOW);
        digitalWrite(MotorPin4, LOW);
        delay(delayTime);

        digitalWrite(MotorPin1, LOW);
        digitalWrite(MotorPin2, HIGH);
        digitalWrite(MotorPin3, HIGH);
        digitalWrite(MotorPin4, LOW);
        delay(delayTime);

        digitalWrite(MotorPin1, LOW);
        digitalWrite(MotorPin2, LOW);
        digitalWrite(MotorPin3, HIGH);
        digitalWrite(MotorPin4, HIGH);
        delay(delayTime);

        digitalWrite(MotorPin1, HIGH);
        digitalWrite(MotorPin2, LOW);
        digitalWrite(MotorPin3, LOW);
        digitalWrite(MotorPin4, HIGH);
        delay(delayTime);
        Serial.println("Close the door");
        is_door_detected_closed = !digitalRead(PIN_DOOR_CLOSED);
        state_door_detected_open = !digitalRead(OPEN_BUTTON); // Inverse de la lecture sur entrée BP bas
        if (is_door_detected_closed)
        {
            Serial.println("Door closed");
            Stop();
            is_door_closed = true;
            is_door_open = false;
            is_init = true;
            break;
        }
    }
}

void door_close()
{
    while (!is_door_detected_closed)
    {
        digitalWrite(MotorPin1, HIGH);
        digitalWrite(MotorPin2, HIGH);
        digitalWrite(MotorPin3, LOW);
        digitalWrite(MotorPin4, LOW);
        delay(delayTime);

        digitalWrite(MotorPin1, LOW);
        digitalWrite(MotorPin2, HIGH);
        digitalWrite(MotorPin3, HIGH);
        digitalWrite(MotorPin4, LOW);
        delay(delayTime);

        digitalWrite(MotorPin1, LOW);
        digitalWrite(MotorPin2, LOW);
        digitalWrite(MotorPin3, HIGH);
        digitalWrite(MotorPin4, HIGH);
        delay(delayTime);

        digitalWrite(MotorPin1, HIGH);
        digitalWrite(MotorPin2, LOW);
        digitalWrite(MotorPin3, LOW);
        digitalWrite(MotorPin4, HIGH);
        delay(delayTime);
        Serial.println("Close the door");

        is_door_detected_closed = !digitalRead(PIN_DOOR_CLOSED);
        state_door_detected_open = !digitalRead(OPEN_BUTTON); // Inverse de la lecture sur entrée BP bas
        if (is_door_detected_closed || state_door_detected_open)
        {
            is_door_closed = true;
            is_door_open = false;
            Serial.println("Door is closed");
            Stop();
            break;
        }
    }
}

void door_open()
{
    while (!is_door_detected_closed)
    {
        digitalWrite(MotorPin1, LOW);
        digitalWrite(MotorPin2, LOW);
        digitalWrite(MotorPin3, HIGH);
        digitalWrite(MotorPin4, HIGH);
        delay(delayTime);

        digitalWrite(MotorPin1, LOW);
        digitalWrite(MotorPin2, HIGH);
        digitalWrite(MotorPin3, HIGH);
        digitalWrite(MotorPin4, LOW);
        delay(delayTime);

        digitalWrite(MotorPin1, HIGH);
        digitalWrite(MotorPin2, HIGH);
        digitalWrite(MotorPin3, LOW);
        digitalWrite(MotorPin4, LOW);
        delay(delayTime);

        digitalWrite(MotorPin1, HIGH);
        digitalWrite(MotorPin2, LOW);
        digitalWrite(MotorPin3, LOW);
        digitalWrite(MotorPin4, HIGH);
        delay(delayTime);
        Serial.println("Open the door");

        is_door_detected_closed = !digitalRead(PIN_DOOR_OPEN);
        state_door_detected_closed = !digitalRead(CLOSE_BUTTTON); // Inverse de la lecture sur entrée BP haut
        if (is_door_detected_closed || state_door_detected_closed)
        {
            is_door_closed = false;
            is_door_open = true;
            Serial.println("Door is open");
            Stop();
            break;
        }
    }
}

void Stop()
{
    digitalWrite(MotorPin1, LOW);
    digitalWrite(MotorPin2, LOW);
    digitalWrite(MotorPin3, LOW);
    digitalWrite(MotorPin4, LOW);
    activeTempo = 0;
}

/*

timestamp_lastwakeup = 0;
timestamp_lastsleep = 0;
daytime_last =0;
assert(DAYTIME_MIN>MAX_DAYTIME_DIFF);
state=get_state();
while(true){
    if(state==closed){
        if(daytime_last==0) daytime_last=DAYTIME_MAX;
        int nighttime_last=CYCLETIME-daytime_last;
wait_morning(nighttime_last-MAX_DAYTIME_DIFF,nighttime_last+MAX_DAYTIME_DIFF);
        timestamp_lastwakeup=gettimestamp();
if(timestamp_lastsleep>0){
daytime_last=CYCLETIME-(timestamp_lastwakeup-timestamp_lastsleep);
}
        door_open();
        state=open;
    }elseif(state==open){
        if(daytime_last==0) daytime_last=DAYTIME_MIN;
        wait(daytime_last-MAX_DAYTIME_DIFF);
        wait_evening(0,daytime_last+2*MAX_DAYTIME_DIFF);
        timestamp_lastsleep=gettimestamp();
if(timestamp_lastwakeup>0){
    daytime_last=timestamp_lastsleep-timestamp_lastwakeup;
}
        door_close();
        state=closed;
    }
}

*/
