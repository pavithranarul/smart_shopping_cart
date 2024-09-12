#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>

#define greenled A3
#define buzzer A2
#define redled A1
#define SS_PIN 10  // SS pin for RC522 RFID reader
#define RST_PIN 9  // RST pin for RC522 RFID reader

MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 4); // I2C address 0x27, 16 column, 4 row LCD

int p1 = 0, p2 = 0, p3 = 0;
double total = 0;
int count_prod = 0;

void setup()
{
  pinMode(6, INPUT_PULLUP);  // Push button for removing items
  pinMode(greenled, OUTPUT); // Green LED
  pinMode(buzzer, OUTPUT);   // Buzzer
  pinMode(redled, OUTPUT);   // Red LED

  Serial.begin(9600);        // Initialize Serial Monitor
  SPI.begin();               // Initialize SPI bus
  mfrc522.PCD_Init();        // Initialize RFID reader

  lcd.init();                // Initialize the LCD
  lcd.backlight();           // Turn on the backlight

  lcd.setCursor(0, 0);       // Set the cursor to the first row
  lcd.print("AUTOMATIC BILL");
  lcd.setCursor(0, 1);
  lcd.print("SHOPPING CART");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WELCOME TO");
  lcd.setCursor(0, 1);
  lcd.print("SUPER MARKET");
  delay(2000);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Please Add Item");

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
      addProduct("Butter", 10.00, p1);
    }
    else if (tagUID == "265D42D9" && removeMode == 0)  // Remove Butter
    {
      removeProduct(p1, 10.00, "Butter");
    }

    if (tagUID == "C607DF2B" && removeMode == 1)  // Milk Tag
    {
      addProduct("Milk", 20.00, p2);
    }
    else if (tagUID == "C607DF2B" && removeMode == 0)  // Remove Milk
    {
      removeProduct(p2, 20.00, "Milk");
    }

    if (tagUID == "4C3327F9" && removeMode == 1)  // Tea Tag
    {
      addProduct("Tea", 25.00, p3);
    }
    else if (tagUID == "4C3327F9" && removeMode == 0)  // Remove Tea
    {
      removeProduct(p3, 25.00, "Tea");
    }

    // Halt PICC to prevent continuous reading of the same card
    mfrc522.PICC_HaltA();
  }
}

// Function to add a product to the cart
void addProduct(const char* productName, double price, int &productCount) {
  Serial.print(productName);
  Serial.println(" Added");
  Serial.print("Price: ");
  Serial.println(price);
  productCount++;
  total += price;
  Serial.print("Total: ");
  Serial.println(total);
  count_prod++;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(productName);
  lcd.setCursor(0, 1);
  lcd.print("Price: ");
  lcd.print(price);
  lcd.setCursor(0, 2);
  lcd.print("Total: ");
  lcd.print(total);

  feedbackSuccess();
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

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(productName);
    lcd.setCursor(0, 1);
    lcd.print("Removed");
    lcd.setCursor(0, 2);
    lcd.print("Total: ");
    lcd.print(total);

    feedbackSuccess();
  } else {
    Serial.print(productName);
    Serial.println(" not in cart!");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(productName);
    lcd.setCursor(0, 1);
    lcd.print("not in cart!");

    feedbackError();
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

// Error feedback function
void feedbackError() {
  digitalWrite(redled, HIGH);
  digitalWrite(buzzer, HIGH);
  delay(2000);
  digitalWrite(redled, LOW);
  digitalWrite(buzzer, LOW);
}
