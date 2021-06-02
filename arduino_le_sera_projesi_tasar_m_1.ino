// Bu arduino genel olarak SERA İZLEMEDE hava kalitesi değerlendirmesinden sorumludur.
#include <LiquidCrystal.h>  // Bu, LCD Ekranı doğru şekilde kullanmak içindir

//Bunlar LCD ekrana bağlanmak için kullanılır
int regSel = 7; // Bu, LCD sinyali görüntülemesi
int enable = 6; //Bu, genel olarak LCD'yi etkinleştirecektir.
int sig1 = 5; // Sinyal 1
int sig2 = 4; // Sinyal 2
int sig3 = 3; // Sinyal 3
int sig4 = 2; // Sinyal 4

// Bunlar ısıtma ve soğutma elemanlarımızın çalıştırılması için kullanılır
int motor1 = 8; // Isıtıcıyı saat yönünde kontrol eder
int motor2 = 9; //Isıtıcıyı saat yönünün tersine kontrol eder
int motor3 = 10; // Saat yönünde soğutucuyu kontrol eder
int motor4 = 11; // Saat yönünün tersine soğutucuyu kontrol eder

// Bu yapay ışığı kontrol edecek
int light = 12; // led pin

// Bunlar algılamak için kullanılan gerçek sensörlerdir
char temp = A0; // Sıcaklık Sensörü
char lumos = A1; // Foto Direnç

//Kendi kullanımımız için statik beyanlar
float temperature; //Sıcaklık Degerinin tutulduğu yer
float intensity; //ışık yoğunluğunun tutuldugu yer
int flag = 0; //karşılama mesajını belirleyecek

// LCD Nesnesının tanımlandıgı yer
LiquidCrystal lcd(regSel, enable, sig1, sig2, sig3, sig4);

// Fonksiyon bildirimleri
float readTemperature(char); // sıcaklığı belirler
float readLuminousity(char); //ışığı belirler
void authenticate(); // fonksiyonlara izin verilip verilmeyeceğine burada karar verilecek

void setup() {
    // Veri yazacağımız pinler
    pinMode(motor1, OUTPUT);
    pinMode(motor2, OUTPUT);
    pinMode(motor3, OUTPUT);
    pinMode(motor4, OUTPUT);
    pinMode(light, OUTPUT);

    // sıcaklığın ve ışığın okuyacağı pinler
    pinMode(temp, INPUT);
    pinMode(lumos, INPUT);

    //Hata ayıklamak için seri monitör
    Serial.begin(1156200); //iletişim için 1156200 baud hızı
    //Tüm verileri çalıştırmak için LCD;
  	Serial.flush();
    lcd.begin(16, 2); // 16*2 lcd ızgarası
}

void loop() {
  	lcd.clear();
	temperature = readTemperature(temp); // temp okur
    lcd.setCursor(3, 0); // lcd konumuna karar verir
    lcd.print("Temp: "); //sağlanan dizeyi yazdırır 
    lcd.setCursor(9, 0); 
    lcd.print(temperature);
    if (temperature < 20) { // 20  altındaki tropikal bir sera için soğuk
    	lcd.setCursor(4, 1);
    	lcd.print("Cok Soguk"); 
        digitalWrite(8, HIGH); //ısıtıcıyı etkinleştirir
        digitalWrite(10, LOW); // soğutucuyu devre dışı bırakır
    } else if (temperature > 27) { // 27'nin yukarısı tropikal bir sera için sıcaktır
        lcd.setCursor(4, 1); 
    	lcd.print("Cok Sicak");
        digitalWrite(8, LOW); // ısıtıcıyı devre dışı bırakır
        digitalWrite(10, HIGH); //soğutucuyu etkinleştirir
    } else {
      	lcd.setCursor(4, 1); 
    	lcd.print("Muhtesem");
        digitalWrite(8, LOW); //ısıtıcı ve soğutucuyu devre dışı bırakır
        digitalWrite(10, LOW);
    }
    digitalWrite(9, LOW); // saat yönünün tersine hareketi engeller
    digitalWrite(11, LOW);
    delay(150);
    lcd.clear();
    intensity = readLuminousity(lumos); // ışık okunur
    lcd.setCursor(1, 0);
   	lcd.print("Yogunluk: "); 
    lcd.setCursor(12, 0);
   	lcd.print(intensity); 
    if (intensity < 100) { // Dışarısı çok karanlıksa
    	lcd.setCursor(4, 1);
    	lcd.print("Cok Karanlik");
        digitalWrite(light, HIGH); // aydınlatmayı etkinleştir
    } else  if (intensity > 180) { // dışarısı çok aydınlık
        lcd.setCursor(4, 1);
    	lcd.print("Cok Parlak");
    } else {
      	lcd.setCursor(4, 1);
    	lcd.print("Muhtesem");
    }
    delay(150);
    digitalWrite(light, LOW); //aydınlatmayı kapatıyoruz
}

// Bu işlev sıcaklığı okur ve santigrat dereceye dönüştürür.
float readTemperature(char pin) {
    float volts = analogRead(pin); 
    float value = map(((volts - 20) * 3.04), 0, 1023, -40, 125); // bu -40C ila 125C arasında uygun bir aralık elde ederiz
    return value;
}

// Bu işlev ışığı okur ve parlaklığa dönüştürür
float readLuminousity(char pin) {
    float lum = analogRead(pin); 
    float value = map(lum, 0, 700, 0, 255);
    return value;
}

/**************
* Bu işlev, cihaz kimlik doğrulamasının kilidini açmak için kullanılabilir
* ancak seri iletişim devre dışıdır
***************/
// Bu işlev, karşılama mesajını değiştirecek
void authenticate() {
  lcd.setCursor(1,0);
  if (char(Serial.read()) == 'w')
  lcd.print('y');
  else
  lcd.print('n');
  
}
