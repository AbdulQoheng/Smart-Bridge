#include <Servo.h>
Servo myservo1; // jembatan
Servo myservo2; // jembatan
Servo myservo3; // jalan
Servo myservo4; // jalan
int pos = 90;
int pos1 = 00;

#define echoPin1 13  // deklarasi pin 13 dengan nama echoPin (untuk pin echo
#define trigPin1 12  // deklarasi pin 12 dengan nama trigPin (untuk pin trig)
#define echoPin2 11  // deklarasi pin 11 dengan nama echoPin (untuk pin echo
#define trigPin2 10  // deklarasi pin 10 dengan nama trigPin (untuk pin trig)

int ldr = A0;
int relay = 7;
int relay_on = 0;
int relay_off = 1;
int habistutupjalan = 0;
int bazer = 9;

int infra = A1;

int encoder_pin = 2; // pulse output from the module
unsigned int rpm; // rpj reading
volatile byte pulses; // number of pulses
unsigned long timeold;
// number of pulses per revolution
// based on your encoder disc
unsigned int pulsesperturn = 20;
void counter()
{
  //Update count
  pulses++;
}

int posisi = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  myservo1.attach(6);
  myservo2.attach(5);
  myservo3.attach(4);
  myservo4.attach(3);

  pinMode(trigPin1, OUTPUT); // menjadikan output
  pinMode(echoPin1, INPUT); // menjadikan input
  pinMode(trigPin2, OUTPUT); // menjadikan output
  pinMode(echoPin2, INPUT); // menjadikan input

  pinMode(ldr, INPUT);
  pinMode(relay, OUTPUT);
  pinMode(infra, INPUT);
  pinMode(bazer, OUTPUT);

  pinMode(encoder_pin, INPUT);
  //Interrupt 0 is digital pin 2
  //Triggers on Falling Edge (change from HIGH to LOW)
  attachInterrupt(0, counter, FALLING);
  // Initialize
  pulses = 0;
  rpm = 0;
  timeold = 0;

}

void loop() {
  // put your main code here, to run repeatedlyr:
  int kecepatan = angin() ;

  Serial.print("Kecepatan Angin = ");
  Serial.print(kecepatan);
  Serial.println(" KM/h");

  double air = jarakair();
  Serial.print("Jarak air dengan jembatan = ");
  Serial.print(air);
  Serial.println(" cm");

  double sisi1 = jarakkapal1();
  Serial.print("Jarak kapal1 dengan jembatan = ");
  Serial.print(sisi1);
  Serial.println(" cm");


  boolean bawah = bawahkapal();
  Serial.print("Dibawah jembatan = ");
  if (bawah == true) {
    Serial.print("terdeteksi");
  } else {
    Serial.print("tidak terdeteksi");
  }
  Serial.println(" kapal");
  delay(500);
  Serial.println(bawah);

  lampujalan();
  //  kondisi air dan angin
  if (kecepatan >= 60 || air <= 5) {
    if (posisi == 0) {
      alarem();
      posisi = 1;
    }
    habistutupjalan = 1;
    tutupjalan();
    delay(2000);

  } else {
    if (sisi1 <= 40) {
      if (posisi == 0) {
        alarem();
        posisi = 1;
      }
      tutupjalan();
      delay(1000);
      bukajembatan();
      delay(5000);
    } else if ( bawah == false && posisi == 1 ) {
      tutupjembatan();
      delay(1000);
      bukajalan();
      delay(2000);
      posisi = 0;
      habistutupjalan = 0;
    }

  }
}

void alarem() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(bazer, HIGH);
    delay(1000);
    digitalWrite(bazer, LOW);
    delay(100);
  }

}
int angin() {
  if (millis() - timeold >= 1000) {
    //Don't process interrupts during calculations
    detachInterrupt(0);
    rpm = (60 * 1000 / pulsesperturn ) / (millis() - timeold) * pulses;
    timeold = millis();
    pulses = 0;
    int kecepatan1 = (rpm * 81 * 60) / 1000; // kecepatan km/h
    //      Serial.print("RPM = ");
    //      Serial.println(rpm);
    //Restart the interrupt processing
    attachInterrupt(0, counter, FALLING);
    return kecepatan1;
  }
}

int jarakair() {
  digitalWrite(trigPin1, HIGH); // menyalakan trigPin
  delay(10); // dengan waktu 10 ms
  digitalWrite(trigPin1, LOW); // mematikan trigPin
  double data = pulseIn(echoPin1, HIGH); // menginisialisasi dengan data yang di baca dari echo
  int jarak = 0.0343 * (data / 2); // membuat jarak dari data
  return jarak;

}

double jarakkapal1() {
  digitalWrite(trigPin2, HIGH); // menyalakan trigPin
  delay(10); // dengan waktu 10 ms
  digitalWrite(trigPin2, LOW); // mematikan trigPin
  double data = pulseIn(echoPin2, HIGH); // menginisialisasi dengan data yang di baca dari echo
  double jarak = 0.0343 * (data / 2); // membuat jarak dari data

  return jarak;
}

boolean bawahkapal() {
  int val = digitalRead(infra);
  if (val == LOW) {
    return true;
  } else {
    return false;
  }
}

void lampujalan() {
  int val = digitalRead(ldr);
  if (val == 1) {
    digitalWrite(relay, relay_on);
  } else {
    digitalWrite(relay, relay_off);
  }

}

void bukajembatan() {
  for (pos1; pos1 > 00; pos1 -= 1) {
    myservo3.write(pos1);
    myservo4.write(pos1);
    delay(1);
  }
}

void tutupjembatan() {
  for (pos1; pos1 < 55; pos1 += 1) {
    myservo3.write(pos1);
    myservo4.write(pos1);
    delay(30);
  }
}

void bukajalan() {
  for (pos; pos > 90; pos -= 1) {
    myservo1.write(pos);
    myservo2.write(pos);
    delay(10);
  }
}

void tutupjalan() {
  for (pos; pos < 180; pos += 1) {
    myservo1.write(pos);
    myservo2.write(pos);
    delay(10);
  }
}
