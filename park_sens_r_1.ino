// Pin tanımlamaları
const int TRIGGER_PIN = 7;
const int ECHO_PIN = 6;
const int BUZZER_PIN = 11;
const int RED_LED = 3;
const int YELLOW_LED = 9;
const int GREEN_LED = 10;

// Mesafe sabitleri (cm)
const int MESAFE_KISA = 30;
const int MESAFE_ORTA = 40;
const int MESAFE_UZUN = 50;
const int GUVENLI_MESAFE = 60;
const int MAX_MESAFE = 400; // HC-SR04 için maksimum ölçüm mesafesi

// Global değişkenler
unsigned long sonOlcumZamani = 0;
const unsigned long OLCUM_ARALIGI = 100; // ms

void setup() {
    pinMode(RED_LED, OUTPUT);
    pinMode(YELLOW_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    pinMode(TRIGGER_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    Serial.begin(9600);
}

int mesafeOlc() {
    digitalWrite(TRIGGER_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN, LOW);
    
    unsigned long zaman = pulseIn(ECHO_PIN, HIGH, 30000); // Timeout eklendi
    
    // Hatalı okuma kontrolü
    if (zaman == 0) {
        return MAX_MESAFE;
    }
    
    int mesafe = zaman * 0.034 / 2;
    
    // Mesafe sınırlarını kontrol et
    if (mesafe <= 0 || mesafe > MAX_MESAFE) {
        return MAX_MESAFE;
    }
    
    return mesafe;
}

void uyarBuzzer(int frekans, int tekrar, int bekleme) {
    for (int i = 0; i < tekrar; i++) {
        tone(BUZZER_PIN, frekans, bekleme);
        delay(bekleme * 2); // Ses çalma ve susma için toplam süre
    }
    noTone(BUZZER_PIN);
}

void tumLedleriSondur() {
    digitalWrite(RED_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(GREEN_LED, LOW);
}

void guvenliDurum() {
    tumLedleriSondur();
    digitalWrite(GREEN_LED, HIGH);
    noTone(BUZZER_PIN);
}

void kisaMesafeUyarisi() {
    tumLedleriSondur();
    digitalWrite(RED_LED, HIGH);
    tone(BUZZER_PIN, 250);
}

void ortaMesafeUyarisi() {
    tumLedleriSondur();
    digitalWrite(YELLOW_LED, HIGH);
    uyarBuzzer(450, 3, 150);
}

void uzunMesafeUyarisi() {
    tumLedleriSondur();
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(GREEN_LED, HIGH);
    uyarBuzzer(300, 2, 300);
}

void loop() {
    unsigned long simdikiZaman = millis();
    
    // Belirli aralıklarla ölçüm yap
    if (simdikiZaman - sonOlcumZamani >= OLCUM_ARALIGI) {
        int mesafe = mesafeOlc();
        sonOlcumZamani = simdikiZaman;
        
        Serial.print("Mesafe: ");
        Serial.print(mesafe);
        Serial.println(" cm");
        
        // Mesafeye bağlı kontroller
        if (mesafe < MESAFE_KISA) {
            kisaMesafeUyarisi();
        } 
        else if (mesafe < MESAFE_ORTA) {
            ortaMesafeUyarisi();
        } 
        else if (mesafe <= MESAFE_UZUN) {
            uzunMesafeUyarisi();
        } 
        else if (mesafe <= GUVENLI_MESAFE) {
            guvenliDurum();
        } 
        else {
            tumLedleriSondur();
            noTone(BUZZER_PIN);
        }
    }
}