#include <SoftwareSerial.h>

// تعيين المداخل التي سيتم استخدامها للاتصال بالبلوتوث
SoftwareSerial BluetoothSerial(0, 1); // RX, TX

// تعريف مداخل H-Bridge
const int IN1 = 2; // اتجاه المحرك 1
const int IN2 = 3;
const int ENA = 9; // سرعة المحرك 1 (PWM)

const int IN3 = 4; // اتجاه المحرك 2
const int IN4 = 5;
const int ENB = 10; // سرعة المحرك 2 (PWM)

const int buzzerPin = 8;  // توصيل البازر إلى الدبوس 8

// متغيرات لتخزين البيانات
int speed = 0;       // السرعة
char direction = 's'; // الاتجاه (افتراضيًا: توقف)

void setup() {
  // إعداد المداخل كمخرجات
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);

  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(buzzerPin, OUTPUT);  // تحديد البازر كمخرج

  // بدء الاتصال التسلسلي
  Serial.begin(9600);          // الاتصال مع Serial Monitor
  BluetoothSerial.begin(9600); // الاتصال مع البلوتوث
  Serial.println("Bluetooth Connected!");
}

void loop() {
  // إذا كانت هناك بيانات واردة من البلوتوث
  if (BluetoothSerial.available()) {
    // قراءة البيانات المرسلة من البلوتوث
    char incomingByte = BluetoothSerial.read();

    // إذا كانت البيانات رقمًا (0 إلى 5 للسرعة)
    if (incomingByte >= '1' && incomingByte <= '8') {
      speed = (incomingByte - '0') * 10; // تحويل السرعة إلى نطاق 0-100
      Serial.print("Speed: ");
      Serial.println(speed);
    } else { // إذا كانت البيانات حرفًا للاتجاه
      direction = incomingByte;

      // تنفيذ العمليات بناءً على الاتجاه
      controlMotors(direction, speed);

      // طباعة الاتجاه على Serial Monitor
      switch (direction) {
        case 'u':
          Serial.println("Forward");
          break;
        case 'd':
          Serial.println("Backward");
          break;
        case 'l':
          Serial.println("Left");
          break;
        case 'r':
          Serial.println("Right");
          break;
        case 's':
          Serial.println("Stop");
          break;
        case 'k':
          Serial.println("shoot");
          break;
        case 'a':
          Serial.println("Alarm");
          playSiren();  // تشغيل صوت صفارة الشرطة
          break;
        case 'm':
          Serial.println("Mute");
          noTone(buzzerPin); // إيقاف الصوت
          break;
        default:
          Serial.println("Unknown command");
          break;
      }
    }
  }
}

// دالة التحكم في المحركات
void controlMotors(char dir, int spd) {
  int motorSpeed = map(spd, 0, 80, 0, 255); // تحويل السرعة إلى نطاق 0-255

  switch (dir) {
    case 'u': // للأمام
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      analogWrite(ENA, motorSpeed);

      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
      analogWrite(ENB, motorSpeed);
      break;

    case 'd': // للخلف
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      analogWrite(ENA, motorSpeed);

      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
      analogWrite(ENB, motorSpeed);
      break;

    case 'l': // لليسار (محرك واحد يعمل)
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      analogWrite(ENA, motorSpeed);

      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW);
      analogWrite(ENB, 0); // إيقاف المحرك الآخر
      break;

    case 'r': // لليمين (محرك واحد يعمل)
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      analogWrite(ENA, 0); // إيقاف المحرك الآخر

      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
      analogWrite(ENB, motorSpeed);
      break;

    case 's': // توقف
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      analogWrite(ENA, 0);

      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW);
      analogWrite(ENB, 0);
      break;
      
    case 'k': // توقف
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      analogWrite(ENA, 255);

      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
      analogWrite(ENB, 255);
      delay(500);
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      analogWrite(ENA, 255);

      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
      analogWrite(ENB, 255);
      delay(500);
      break;
      
    default: // أي حالة أخرى - توقف
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      analogWrite(ENA, 0);

      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW);
      analogWrite(ENB, 0);
      break;
  }
}

// دالة لتشغيل صفارة الشرطة
void playSiren() {
  // ترددات صفارة الشرطة
  tone(buzzerPin, 1000);  // أول تردد
  delay(200);             // تأخير بين الترددات
  tone(buzzerPin, 1500);  // تردد ثاني
  delay(200);             // تأخير بين الترددات
  tone(buzzerPin, 1000);  // تردد أول
  delay(200);             // تأخير بين الترددات
  tone(buzzerPin, 1500);  // تردد ثاني
  delay(200);             // تأخير بين الترددات
  noTone(buzzerPin);      // إيقاف الصوت
}
