//Controlador de reles e temperatura através do bluetooth
//Desenvolvido por Maikon Oliveira

#include <LiquidCrystal_I2C.h> 
#include <WiFi.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <Firebase_ESP_Client.h>

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define intervalo 500

// Define SSID e senha para rede de internet.
#define WIFI_SSID "Maikon"
#define WIFI_PASSWORD "infinity356"

// Define API Key e id do banco de dados.
#define API_KEY "AIzaSyAhxG6RsUstAO9CRZd4k38yP1UV8jswH30"
#define FIREBASE_ID "ihouse-fc98b"

// Define e-mail e senha para autenticação do firebase.
#define USER_EMAIL "esp32@gmail.com"
#define USER_PASS "IHouseesp32"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

String uid;

LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(4, DHT11);

unsigned long timerAnterior = 0;
const long interval = intervalo;

String doc_path_reles = "IHouse/reles";
String doc_path_clima = "IHouse/clima";

int state1 = 0;
int state2 = 0;
int state3 = 0;
int state4 = 0;
int state5 = 0;
int state6 = 0;
int state7 = 0;
int state8 = 0;
int aux=0;
int count=0;

float u = 0;
float t = 0;
float somaTemp=0;
float somaUmid=0;

float tempVet[100];
float umidVet[100];

void setup() {
  Serial.begin(115200);
  lcd.init();       
  lcd.backlight();
  
  // Inicializa conexão wi-fi.
  initWifi();

  config.api_key = API_KEY;

  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASS;

  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(2048);
  
  config.token_status_callback = tokenStatusCallback;  
  
  Firebase.begin(&config, &auth);
  
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  // Imprime na serial o UID de usuario.
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.print(uid);
  
  dht.begin();
  pinMode(32, OUTPUT);
  pinMode(33, OUTPUT);
  pinMode(25, OUTPUT);
  pinMode(26, OUTPUT);
  pinMode(27, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(32, HIGH);
  digitalWrite(33, HIGH);
  digitalWrite(25, HIGH);
  digitalWrite(26, HIGH);
  digitalWrite(27, HIGH);
  digitalWrite(14, HIGH);
  digitalWrite(12, HIGH);
  digitalWrite(13, HIGH);
}

void loop() {

 //Realiza leitura dos dados de temperatura e umidade.
 sensor();

//Realiza leitura dos dados dos relés.
 String fbdo = getRele();
 
 //Liga ou desliga os relés
 String rele01 = getValue("rele01", fbdo);
 if(rele01 == "A" &&  state1 == 0) rele(1);
 else if(rele01 == "B" &&  state1 == 1) rele(1);
 String rele02 = getValue("rele02", fbdo);
 if(rele02 == "A" &&  state2 == 0) rele(2);
 else if(rele02 == "B" &&  state2 == 1) rele(2);
 String rele03 = getValue("rele03", fbdo);
 if(rele03 == "A" &&  state3 == 0) rele(3);
 else if(rele03 == "B" &&  state3 == 1) rele(3);
 String rele04 = getValue("rele04", fbdo);
 if(rele04 == "A" &&  state4 == 0) rele(4);
 else if(rele04 == "B" &&  state4 == 1) rele(4);
  String rele05 = getValue("rele05", fbdo);
 if(rele05 == "A" &&  state5 == 0) rele(5);
 else if(rele05 == "B" &&  state5 == 1) rele(5);
  String rele06 = getValue("rele06", fbdo);
 if(rele06 == "A" &&  state6 == 0) rele(6);
 else if(rele06 == "B" &&  state6 == 1) rele(6);
  String rele07 = getValue("rele07", fbdo);
 if(rele07 == "A" &&  state7 == 0) rele(7);
 else if(rele07 == "B" &&  state7 == 1) rele(7);
  String rele08 = getValue("rele08", fbdo);
 if(rele08 == "A" &&  state8 == 0) rele(8);
 else if(rele08 == "B" &&  state8 == 1) rele(8);
 
 //Busca o tempo de execução;
 unsigned long timer = millis();
 estado(timer);   
}

// Inicializa Wi-Fi
void initWifi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();
}

// Realiza a leitura e impressão da temperatura e umidade atual
void sensor(){

  FirebaseJson content;
  u = dht.readHumidity();
  t = dht.readTemperature();
  //Verifica se a leitura do sensor é valida!
  if (isnan(t) || isnan(u)) 
  {
    Serial.println("Falha ao ler o sensor!");
  } 
  else
  {
    tempVet[count] = t;
    umidVet[count] = u;
    count++;
    if(count == 100){
      for(int i=0;i<100;i++){
        somaTemp = tempVet[i] + somaTemp;
        somaUmid = umidVet[i] + somaUmid;
      }
      String temp=convertFloatToString(somaTemp/100);  
      String umid=convertFloatToString(somaUmid/100);
      content.set("fields/temperatura/stringValue", temp.c_str());
      content.set("fields/umidade/stringValue", umid.c_str());
      if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_ID, "",doc_path_clima.c_str(), content.raw(), "temperatura,umidade")){
        //Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
      }else{
        Serial.println(fbdo.errorReason());
      }
      lcd.setCursor(0, 0);
      lcd.print("T:");
      lcd.print(somaTemp/100,1);
      lcd.print("C");
      lcd.print("  ");
      lcd.print("U:");
      lcd.print(somaUmid/100,1);
      lcd.print("%");
      count=0;
      somaTemp=0;
      somaUmid=0;
    }
  }
}
// Converte valor lido do sensor para string.
String convertFloatToString(float value){
  char charValue[10];
  String conver;
  dtostrf(value,1,2,charValue);
  conver = String(charValue);
  return conver;
}
// Busca o estado dos relés no banco de dados.
String getRele(){  
  if (Firebase.Firestore.getDocument(&fbdo, FIREBASE_ID, "", doc_path_reles.c_str())) {
    //Serial.printf("ok\n%s\n", fbdo.payload().c_str());
    return fbdo.payload().c_str();
  }else{
    //Serial.println(fbdo.errorReason());
    return "ERRO";
  }
}

// Busca o valor de cada relé (mask) na String fbdo.
String getValue(String mask, String fbdo){
  FirebaseJson payload;
  payload.setJsonData(fbdo);
  FirebaseJsonData jsonData;
  payload.get(jsonData, "fields/"+mask+"/stringValue", true);
  //Serial.print("Value: ");
  //Serial.print(jsonData.stringValue);
  //Serial.print("\n");
  return jsonData.stringValue; 
}

//Realiza o acionamento dos relés através de uma variavel
void rele(int N){
 if(N==1){
   if(state1==0){
    digitalWrite(32, LOW);
    state1=1;
   }else{
    digitalWrite(32, HIGH);
    state1=0;
   }
  }else if(N==2){
    if(state2==0){
      digitalWrite(33, LOW);
      state2=1;
    }else{
      digitalWrite(33, HIGH);
      state2=0;
    }
  }else if(N==3){
    if(state3==0){
      digitalWrite(25, LOW);
      state3=1;
    }else{
      digitalWrite(25, HIGH);
      state3=0;
    }
  }else if(N==4){
    if(state4==0){
      digitalWrite(26, LOW);
      state4=1;
    }else{
      digitalWrite(26, HIGH);
      state4=0;
    }
  }else if(N==5){
    if(state5==0){
      digitalWrite(27, LOW);
      state5=1;
    }else{
      digitalWrite(27, HIGH);
      state5=0;
    }
  }else if(N==6){
    if(state6==0){
      digitalWrite(14, LOW);
      state6=1;
    }else{
      digitalWrite(14, HIGH);
      state6=0;
    }
  }else if(N==7){
    if(state7==0){
      digitalWrite(12, LOW);
      state7=1;
    }else{
      digitalWrite(12, HIGH);
      state7=0;
    }
  }else if(N==8){
    if(state8==0){
      digitalWrite(13, LOW);
      state8=1;
    }else{
      digitalWrite(13, HIGH);
      state8=0;
    }
  }
}
//Realiza a impressão na tela dos estados dos relés
void estado(unsigned long timerAtual){
  if (timerAtual - timerAnterior > intervalo && aux == 0) {  
    timerAnterior = timerAtual;
    lcd.setCursor(3, 1);
    lcd.print("Rele 01: ");
    lcd.print(state1);
    aux=1;
  }else if (timerAtual - timerAnterior > intervalo && aux == 1) {  
    timerAnterior = timerAtual;
    lcd.setCursor(3, 1);
    lcd.print("Rele 02: ");
    lcd.print(state2);
    aux=2;
  }else if (timerAtual - timerAnterior > intervalo && aux == 2) {  
    timerAnterior = timerAtual;
    lcd.setCursor(3, 1);
    lcd.print("Rele 03: ");
    lcd.print(state3);
    aux=3;
  }else if (timerAtual - timerAnterior > intervalo && aux == 3) {  
    timerAnterior = timerAtual;
    lcd.setCursor(3, 1);
    lcd.print("Rele 04: ");
    lcd.print(state4);
    aux=4;
  }else if (timerAtual - timerAnterior > intervalo && aux == 4) {  
    timerAnterior = timerAtual;
    lcd.setCursor(3, 1);
    lcd.print("Rele 05: ");
    lcd.print(state5);
    aux=5;
  }else if (timerAtual - timerAnterior > intervalo && aux == 5) {  
    timerAnterior = timerAtual;
    lcd.setCursor(3, 1);
    lcd.print("Rele 06: ");
    lcd.print(state6);
    aux=6;
  }else if (timerAtual - timerAnterior > intervalo && aux == 6) {  
    timerAnterior = timerAtual;
    lcd.setCursor(3, 1);
    lcd.print("Rele 07: ");
    lcd.print(state7);
    aux=7;
  }else if (timerAtual - timerAnterior > intervalo && aux == 7) {  
    timerAnterior = timerAtual;
    lcd.setCursor(3, 1);
    lcd.print("Rele 08: ");
    lcd.print(state8);
    aux=0;
  }
}
