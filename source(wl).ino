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

int p1 = 0, p2 = 0, p3 = 0;
double total = 0;
int count_prod = 0;

void setup ()
{
  pinMode(6, INPUT_PULLUP);  // Push button for removing items
  pinMode(greenled, OUTPUT); // Green LED
  pinMode(buzzer, OUTPUT);   // Buzzer
  pinMode(redled, OUTPUT);   // Red LED

  Serial.begin(9600);        // Initialize Serial Monitor
  SPI.begin();               // Initialize SPI bus
  mfrc522.PCD_Init();        // Initialize RFID reader

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

    // Check if button is pressed (remove mode)
    int removeMode = digitalRead(6);

    // Product detection based on RFID tag UID
    if (tagUID == "265D42D9" && removeMode == 1)  // Butter Tag
    {
      Serial.println("Butter Added");
      Serial.println("Price: 10.00");
      p1++;
      total += 10.00;
      Serial.print("Total : ");
      Serial.print(total);
      count_prod++;
      feedbackSuccess();
    }
    else if (tagUID == "265D42D9" && removeMode == 0)  // Remove Butter
    {
      removeProduct(p1, 10.00, "Butter");
    }

    if (tagUID == "C607DF2B" && removeMode == 1)  // Milk Tag
    {
      Serial.println("Milk Added");
      Serial.println("Price: 20.00");
      p2++;
      total += 20.00;
      Serial.print("Total : ");
      Serial.print(total);
      count_prod++;
      feedbackSuccess();
    }
    else if (tagUID == "C607DF2B" && removeMode == 0)  // Remove Milk
    {
      removeProduct(p2, 20.00, "Milk");
    }

    if (tagUID == "4C3327F9" && removeMode == 1)  // Tea Tag
    {
      Serial.println("Tea Added");
      Serial.println("Price: 25.00");
      p3++;
      total += 25.00;
      Serial.print("Total : ");
      Serial.print(total);
      count_prod++;
      feedbackSuccess();
    }
    else if (tagUID == "4C3327F9" && removeMode == 0)  // Remove Tea
    {
      removeProduct(p3, 25.00, "Tea");
    }

    // Halt PICC to prevent continuous reading of the same card
    mfrc522.PICC_HaltA();
  }
}

// Success feedback function
void feedbackSuccess() {
  digitalWrite(redled, HIGH);
  digitalWrite(buzzer, HIGH);
  digitalWrite(greenled, LOW);
  delay(2000);
  digitalWrite(redled, LOW);
  digitalWrite(buzzer, LOW);
  digitalWrite(greenled, HIGH);
}

// Function to remove a product from the cart
void removeProduct(int &productCount, double price, const char* productName) {
  if (productCount > 0) {
    Serial.print(productName);
    Serial.println(" Removed");
    productCount--;
    total -= price;
    count_prod--;
    Serial.print("Total Price: ");
    Serial.println(total);
    feedbackSuccess();
  } else {
    Serial.print(productName);
    Serial.println(" not in cart!");
    feedbackError();
  }
}

// Error feedback function
void feedbackError() {
  digitalWrite(redled, HIGH);
  digitalWrite(buzzer, HIGH);
  delay(2000);
  digitalWrite(redled, LOW);
  digitalWrite(buzzer, LOW);
}
