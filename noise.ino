int PeakToPeak = 0;
long PeakToPeakAvg = 0;
int ShockDetect = 0;
int ShockCount = 0;
int NoisePeakCount = 0;
int NoiseAvgCount = 0;
int NoiseAndShockCount = 0;
int index = 0;

#define SHOCK_THRESHOLD 100
#define NOISE_AVERAGE_THRESHOLD 3
#define NOISE_PEAK_THRESHOLD 40
#define NOISE_PEAK_COUNT_THRESHOLD 3
#define NOISE_CHECK_INTERVAL_1M 60000
#define NOISE_CHECK_INTERVAL_5M 60000
#define NOISE_CHECK_INTERVAL_1H 3600000
#define NOISE_CHECK_INTERVAL_1D 86400000
#define TIME_MAX 4294897296
#define SAMPLE_WINDOW 50

unsigned long LastCheckMinute = 0;
unsigned long LastCheckHour = 0;
unsigned long LastCheckDay = 0;

float MaxNoiseMin = 0;
unsigned int MaxNoiseWarnHour = 0;
unsigned int MaxNoiseWarnDay = 0;
unsigned int MaxNoiseWarnMonth = 0;

float AvgNoiseMin = 0;
unsigned int AvgNoiseWarnHour = 0;
unsigned int AvgNoiseWarnDay = 0;
unsigned int AvgNoiseWarnMonth = 0;

#define LED_PIN 8
#define WAVE_PIN 7
#define SOUND_PIN A0

int LEDCount = 0;


void setup() {
  Serial.begin(9600);
  
  pinMode(WAVE_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  Serial.println(F("\nInitializing..."));
  if (!cc3000.begin()) {
    Serial.println(F("Couldn't begin()! Check your wiring?"));
    while(1);
  }
  
  
  Serial.print(F("\nAttempting to connect to ")); Serial.println(WLAN_SSID);
  if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    Serial.println(F("Failed!"));
    while(1);
  }
  Serial.println(F("Connected!"));
  
  Serial.println(F("Request DHCP"));
  while (!cc3000.checkDHCP()) {
    delay(3000); 
  }  
 
  while (! displayConnectionDetails()) {
    delay(1000);
  }

  ip = 0;
  Serial.print(WEBSITE); Serial.print(F(" -> "));
  while (ip == 0) {
    if (! cc3000.getHostByName(WEBSITE, &ip)) {
      Serial.println(F("Couldn't resolve!"));
    }
    delay(1500);
  }
  cc3000.printIPdotsRev(ip);
}

void loop() {
  checkPeakToPeak();
  
  // 소음 최대 측정
  if(NOISE_PEAK_THRESHOLD < PeakToPeak) {
    turnOnLed();
    NoisePeakCount++;
  }
  if(SHOCK_THRESHOLD < ShockDetect) {
    turnOnLed();
    ShockCount++;
  }
  
  Serial.print("Peak noise = ");
  Serial.print(NoisePeakCount);
  Serial.print(", Avg = ");
  Serial.print(PeakToPeakAvg);
  Serial.print(", ShockCount = ");
  Serial.println(ShockCount);
  
  checkLed();
  
  unsigned long current = millis();
  if(current - LastCheckMinute > NOISE_CHECK_INTERVAL_1M) {
    int isNoiseDetected = 0;
    MaxNoiseMin = NoisePeakCount;
    AvgNoiseMin = (float)PeakToPeakAvg / (float)(index + 1);
    
    if(NOISE_PEAK_COUNT_THRESHOLD <= MaxNoiseMin) {
      turnOnLed();
      AvgNoiseWarnHour++;
      isNoiseDetected = 1;
    }
    else if(NOISE_AVERAGE_THRESHOLD <= AvgNoiseMin) {
      turnOnLed();
      AvgNoiseWarnHour++;
      isNoiseDetected = 1;
    }
    else if(ShockCount > 0) {
      turnOnLed();
      AvgNoiseWarnHour++;
      isNoiseDetected = 1;
    }
    
    LastCheckMinute = current;
    
    if(current - LastCheckHour > NOISE_CHECK_INTERVAL_1H) {
      MaxNoiseWarnDay += MaxNoiseWarnHour;
      AvgNoiseWarnDay += AvgNoiseWarnHour;
      MaxNoiseWarnHour = 0;
      AvgNoiseWarnHour = 0;
      LastCheckHour = current;
    }
    if(current - LastCheckDay > NOISE_CHECK_INTERVAL_1D) {
      MaxNoiseWarnMonth += MaxNoiseWarnDay;
      AvgNoiseWarnMonth += AvgNoiseWarnDay;
      MaxNoiseWarnDay = 0;
      AvgNoiseWarnDay = 0;
      LastCheckDay = current;
    }
    


void showWarning() {
  digitalWrite(LED_PIN, HIGH);
  LastWarningTime = millis();
}



void resetAll() {
  LastCheckMinute = 0;
  LastCheckHour = 0;
  LastCheckDay = 0;
  
  MaxNoiseWarnMonth = 0;
  AvgNoiseWarnMonth = 0;
}



 
