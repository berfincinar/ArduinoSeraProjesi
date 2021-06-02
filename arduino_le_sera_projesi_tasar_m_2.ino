// Bu arduino esas olarak su seviyelerini ve gaz sevıyesını kontrol etmekten ve kimlik doğrulamasını sağlamaktan sorumludur.
#include <Servo.h>  // Bu, pompalama için servo motor kullanmamıza izin verecektir.
#include <Keypad.h> // Bu, cihaza tuş takımı tabanlı kimlik doğrulamaya izin verecektir

// Bunlar genel tuş takımı yapılandırmasıdır
byte rows = 4; 
byte cols = 4; // 4*4 tuş takımı kurulumu
char keys[4][4] = { // Bunlar, tuş takımındaki her tuşu 2D dizi olarak temsil eder
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};
byte rowPins[4] = {9, 8, 7, 6}; // Satır
byte colPins[4] = {5, 4, 3, 2}; // Sütun Bılgılerı

// Bu, gerçek bir tuş takımının tanımlaması burda gerçekleşiyor
Keypad kp = Keypad(makeKeymap(keys), rowPins, colPins, rows, cols);

// Bu, servo motor kontrolü yapılıyor
Servo svr;

// Bunlar, arduino tarafından kullanılacak çeşitli pinleri temsil eder.
char gas = A0; // gaz sensörü değerini okumak için kullanılır
int echo = 10; // mesafe sensörü okumasını okumak için kullanılır
int trig = 11; //nabız göndermek için kullanılır
int serv = 12; // servo açısını kontrol eder
int buzz = 13; // buzziness gönderir 

// Statik bildirimler
String password= "0000"; // ana anahtarı içerir
String pressed=""; // girilen anahtarı içerir

float distance; // ultrasonik sensörden gelen mesafe okumasını depolar
float gaser; // gaz sensöründen gelen gazlı değeri depolar

int flag = 0; // kimlik doğrulama için tetikleyici

// İşlev bildirimleri
float measureDistance(int, int); // mesafeyi ölçer
float measureSmoke(int); // dumanı ölçer
void checkKeyPad(); // yetkilendirmeyi zorunlu kılar
void setup() {
    Serial.begin(1156200); // tetikleme sinyali gönderir
  	Serial.flush();  
  
  	pinMode(trig, OUTPUT); //  titreşim gönderir
  	pinMode(echo, INPUT); // echo sinyalini okur
    pinMode(gas, INPUT); //gaz değerini okur
    pinMode(buzz, OUTPUT); //piezo degerini gönderir

    svr.attach(serv); //motoru kontrol eden bağlantı noktasına ekler
}

void loop()
{
  	//mesafe sensörnun çalışma fonksıyonu ,  su deposunun çalışma alanı
    distance = 0.01723 * measureDistance(trig, echo); // 340 * 100 * 0.00001 / 2
  	Serial.print("Mesafe: ");
  	Serial.print(distance);
  	Serial.println();
    if (distance > 200) { // sadece mesafe 200'den büyükse su pompalamamız gerekir
    	for (int i = 0; i <= 180; i++) {
        	svr.write(i);
            delay(5); //pozisyona ulaşmak için 10 ms gecikme
        }
      delay(50);
        for (int i = 180; i >= 0; i--) {
            svr.write(i); //pompa orijinal konumuna geri dönmelidir
            delay(5);
        }
    } else {
        svr.write(0); // böylece pompa orijinal konumuna geri döner
    }
		// gaz sensorunun calısma alanı
    gaser = measureSmoke(gas); // gaz değerini okur
  	Serial.print("Gaz: ");
  	Serial.print(gaser);
  	Serial.println();
  
    if (gaser > 85) {
        for (int i = 200; i <= 600; i += 50) {
            tone(buzz, i, 10); // buzz ın çınlamasına(ses çıkarmasına)izin verilecek
            delay(5);
        }
    } else {
        digitalWrite(buzz, 0); // buzz çalışmayı durduracak
    }
  	svr.write(0);
  	delay(20);
}

//Cm cinsinden kat edilen mesafeyi bulur
float measureDistance(int triggerPin, int echoPin) {
    digitalWrite(triggerPin, LOW);
  	delayMicroseconds(2);
  	// Tetik pimini 10 mikrosaniye için YÜKSEK duruma ayarlar
  	digitalWrite(triggerPin, HIGH);
  	delayMicroseconds(10);
  	digitalWrite(triggerPin, LOW);
  	// echo pinini okur ve ses dalgası hareket süresini mikrosaniye cinsinden döndürür
  	return pulseIn(echoPin, HIGH);
}

// Havadaki duman miktarını bulur
float measureSmoke(int pin) {
    return analogRead(pin);
}

/*************
*Bu işlev kimlik doğrulama işlevini görüyor, ancak
*seri iletişim olmadığı için işlevsel değil
**************/

// Tuşa basma olayını dinler
void checkKeyPad() {
    char key = kp.getKey();
    if (int(key) != 0) {
        if (key == 'C') { // Bu, girdimiz değeri temizlememize izin verecek (clear)
     	    pressed = "";
      	    delay(100); // Herhangi bir önbelleği temizlemek için
        } else if (key == '*') { //Bu, kontrol için pwd göndermemize izin verecektir.
          if (pressed == password) {
                flag = 1; // kontrol döngüsünden çıkacak
            }
          Serial.write(char('w'));
        } else {
     	    pressed.concat(key);  // girilen verileri saklama olayı burada gerçekleşiyor
            Serial.write(char(key));
        }
    }
}