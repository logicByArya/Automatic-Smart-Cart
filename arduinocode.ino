/***********************************************************************************************
 *              Auto CART
***********************************************************************************************/
#include<Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define buz 7
#define sw1 2
#define sw2 3
#define sw3 4
#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN); 
LiquidCrystal_I2C lcd(0x27,20,4);

/*******************************************************************************************/
void rfid_read();
void main_menu();
void StoreItems();
void addItemToCart();
void buzer(int);
void addingProcedure(int);
int key();
void list_cart();
/*******************************************************************************************/
String ID="";   
String content= "";

struct cart
{
  String UUID, ITEM;
  float PRICE;
} STOCK[10];

struct list
{
  String ITEM;
  float PRICE;
  int qty;
} CART[10];

int TotalItems = 7;
int items = 0;
 
   int pt,ot,dt=0;
/*******************************************************************************************/
void setup() 
{
  Serial.begin(9600);                     
  SPI.begin();                             
  mfrc522.PCD_Init();  

  pinMode(sw1, INPUT);
  pinMode(sw2, INPUT);
  pinMode(sw3, INPUT);
  pinMode(buz, OUTPUT);

  lcd.init();
  lcd.clear();
  lcd.backlight();

  main_menu();
  StoreItems();
}

void loop() 
{
  rfid_read();
  addItemToCart();
  list_cart();
  delay(100);
} 
/*******************************************************************************************/
void rfid_read()
{
  content = "";

  if (!mfrc522.PICC_IsNewCardPresent()) 
  {
    content = "NA";
    return;
  }                                      
  if (!mfrc522.PICC_ReadCardSerial()) 
  {
    content = "NA";
    return;
  }
  
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
}

/*******************************************************************************************/
void main_menu()
{
  lcd.clear();
  lcd.setCursor(0,1);  
  lcd.print("   Automatic CART");

  lcd.setCursor(0,2); 
  lcd.print("Place your Item");
}

/*******************************************************************************************/
void StoreItems()
{
  STOCK[0].UUID = " 05 3a b0 c0 08 b0 c1";
  STOCK[0].ITEM = "Rice";
  STOCK[0].PRICE = 58.99;

  STOCK[1].UUID = " 05 36 0d 61 26 b0 c1";
  STOCK[1].ITEM = "Sugar";
  STOCK[1].PRICE = 34.49;

  STOCK[2].UUID = " 05 3c 54 b8 38 b0 c1";
  STOCK[2].ITEM = "Banana";
  STOCK[2].PRICE = 29.99;

  STOCK[3].UUID = " 05 33 04 19 38 b0 c1";
  STOCK[3].ITEM = "Milk";
  STOCK[3].PRICE = 18.50;

  STOCK[4].UUID = " 05 32 7a 97 38 b0 c1";
  STOCK[4].ITEM = "Bread";
  STOCK[4].PRICE = 29.75;

  STOCK[5].UUID = " ba c3 d9 63";
  STOCK[5].ITEM = "Eggs";
  STOCK[5].PRICE = 38.99;

  STOCK[6].UUID = " 79 f0 b4 05";
  STOCK[6].ITEM = "Apples";
  STOCK[6].PRICE = 50.99;
}

/*******************************************************************************************/
void addItemToCart()
{     
  if(content != "NA")
  {  
    if(content != ID)
    {
      buzer(1);
      ID = content;

      for(int i=0;i<TotalItems;i++)
      {
        if(STOCK[i].UUID == content)
        {
          addingProcedure(i);   
          main_menu();  
          ID = "";
        }
      }
    }
  }
}

/*******************************************************************************************/
void buzer(int n)
{
  for(int i=0;i<n;i++)
  {
    digitalWrite(buz,HIGH);
    delay(50);
    digitalWrite(buz,LOW);
    delay(50);
  }
}

/*******************************************************************************************/
int key()
{
  if(digitalRead(sw1)==0) { delay(60); return 1; }
  if(digitalRead(sw2)==0) { delay(60); return 2; }
  if(digitalRead(sw3)==0) { delay(60); return 3; }
  return 4;
}

/*******************************************************************************************/
void addingProcedure(int i)
{
    int k;
    
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("   Automatic CART");  
    lcd.setCursor(0, 0);
    lcd.print(STOCK[i].ITEM); 
    lcd.setCursor(12, 0); 
    lcd.print(STOCK[i].PRICE); 
    lcd.setCursor(0, 3);
    lcd.print("S1->Add 2->Ignore");  
    lcd.setCursor(0, 2);
    lcd.print("Choose your option"); 

    pt = ot = millis() / 100;
    dt = 0;

    while (1)
    {
        pt = millis() / 100;
        if (pt - ot > 5)
        {
            dt++;
            ot = pt;
            lcd.setCursor(18, 1);
            lcd.print(dt);  
            if (dt > 10)  
            {
                buzer(1);
                return;
            }
        }

        k = key();  

        if (k == 2)  
        {
            buzer(1);
            return;
        }

        if (k == 1)  
        {
            int f = 0;
            for (int k = 0; k < items; k++)
            {
                if (CART[k].ITEM == STOCK[i].ITEM)
                {
                    f = 1;
                    CART[k].qty++;  
                }
            }

            if (f == 0)  
            {
                CART[items].ITEM = STOCK[i].ITEM;
                CART[items].PRICE = STOCK[i].PRICE;
                CART[items].qty = 1;            
                items++;
            }

            buzer(1);
            lcd.clear();
            lcd.print("CART UPDATED");
            delay(1000);
            return;
        }
    }
}

/*******************************************************************************************
 *       LIST CART 
*******************************************************************************************/
void list_cart()
{
    if (key() != 3) return;  

    if (items == 0)
    {
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print("cart EMPTY");
        buzer(3);
        delay(1000);
        main_menu();
        return;
    }

    int cursor = 0;          
    int top = 0;        
    int visible = 3;         
    int LCDline[3] = {0, 3, 2};   

    bool refresh = true;
   

    while (1)
    {
        if (refresh)
        {
            float totalprice=0;
            for (int i = 0; i < items; i++)
            {
              totalprice += CART[i].PRICE * CART[i].qty;

            }
            lcd.clear();
            lcd.setCursor(0,1);
            lcd.print("Total:");
            lcd.print(items);
            lcd.print("items");
            lcd.setCursor(13,1);
            lcd.print(totalprice,2);
               

            for (int i = 0; i < visible; i++)
            {
                int index = top + i;
                if (index >= items) break;

                lcd.setCursor(0, LCDline[i]);

                if (index == cursor) lcd.print(">"); 
                else lcd.print(" ");

                lcd.print(CART[index].ITEM);
                lcd.print(" ");
                lcd.print(CART[index].PRICE,2);
                lcd.print("  ");
                lcd.print(CART[index].qty);
            }
            

          

            refresh = false;
        }

        int k = key();

        if (k == 2)  //  2 for moving down
        {
            cursor++;
            if (cursor >= items) cursor = 0;

            // Scroll logic
            if (cursor < top)
                top = cursor;
            else if (cursor >= top + visible)
                top = cursor - (visible - 1);

            buzer(1);
            refresh = true;
        }

        else if (k == 1)   
        {
            main_menu();
            buzer(1);
            return;
        } 
            else if (k == 3)   // REMOVE the item
        {
        CART[cursor].qty--;

        if (CART[cursor].qty == 0)
        {
            for(int r = cursor; r < items - 1; r++)
            {
                CART[r] = CART[r+1];
            }
            items--;
            if (cursor >= items) cursor = items - 1;
            if (cursor < 0) cursor = 0;
        }

        buzer(2);
        refresh = true;
      
        delay(150);
      }
    }
}