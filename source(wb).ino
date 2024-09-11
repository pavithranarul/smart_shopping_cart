#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>

#define greenled A3
#define buzzer A2
#define redled A1
#define SS_PIN 10  // SS pin for RC522 RFID reader
#define RST_PIN 9  // RST pin for RC522 RFID reader

MFRC522 mfrc522(SS_PIN, RST_PIN);

int p1 = 0, p2 = 0, p3 = 0;  // Counters for each product
double total = 0;             // Total price
int count_prod = 0;           // Total number of products in the cart

void setup ()
{
  pinMode(greenled, OUTPUT);  // Green LED
  pinMode(buzzer, OUTPUT);    // Buzzer
  pinMode(redled, OUTPUT);    // Red LED

  Serial.begin(9600);         // Initialize Serial Monitor
  SPI.begin();                // Initialize SPI bus
  mfrc522.PCD_Init();         // Initialize RFID reader

  Serial.println("AUTOMATIC BILL SHOPPING CART");
  delay(2000);
  Serial.println("WELCOME TO SUPER MARKET");
  delay(2000);
  Serial.println("Please Add Item");
}

void loop()
{
  // Check if a new card is present
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    String tagUID = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      tagUID += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""); // Add leading zero for single digit
      tagUID += String(mfrc522.uid.uidByte[i], HEX);              // Convert byte to hex
    }
    tagUID.toUpperCase();  // Convert UID to uppercase for easier comparison
    Serial.print("Tag UID: ");
    Serial.println(tagUID);

    // Product detection based on RFID tag UID
    if (tagUID == "265D42D9") {  // Butter Tag
      handleProduct(p1, 10.00, "Butter");
    }
    else if (tagUID == "C607DF2B") {  // Milk Tag
      handleProduct(p2, 20.00, "Milk");
    }
    else if (tagUID == "4C3327F9") {  // Tea Tag
      handleProduct(p3, 25.00, "Tea");
    }

    // Halt PICC to prevent continuous reading of the same card
    mfrc522.PICC_HaltA();
  }
}

// Function to add/remove a product based on the count
void handleProduct(int &productCount, double price, const char* productName) {
  if (productCount < 2) {  // Add product if it's been added less than 3 times
    productCount++;
    total += price;
    count_prod++;
    Serial.print(productName);
    Serial.println(" Added");
    Serial.print("Price: ");
    Serial.println(price);
    Serial.print("Total Price: ");
    Serial.println(total);
    feedbackSuccess();
  }
  else {  // If product is added 3rd time, remove it
    productCount--;  // Decrease product count
    total -= price;  // Adjust total price
    count_prod--;    // Decrease total product count
    Serial.print(productName);
    Serial.println(" Removed (after adding 3rd time)");
    Serial.print("Total Price: ");
    Serial.println(total);
    feedbackSuccess();
  }
}

// Success feedback function
void feedbackSuccess() {
  digitalWrite(redled, LOW);   // Turn off red LED
  digitalWrite(greenled, HIGH);  // Turn on green LED
  digitalWrite(buzzer, HIGH);    // Turn on buzzer
  delay(500);                    // Delay to allow feedback
  digitalWrite(buzzer, LOW);     // Turn off buzzer
  digitalWrite(greenled, LOW);   // Turn off green LED
}

// Error feedback function
void feedbackError() {
  digitalWrite(redled, HIGH);    // Turn on red LED
  digitalWrite(buzzer, HIGH);    // Turn on buzzer
  delay(500);                    // Delay to allow feedback
  digitalWrite(buzzer, LOW);     // Turn off buzzer
  digitalWrite(redled, LOW);     // Turn off red LED
}
