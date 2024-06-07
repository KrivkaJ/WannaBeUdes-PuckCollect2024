#include <Arduino.h>
#include "robotka.h"
#include "Adafruit_TCS34725.h"

byte state = 1;
byte puck_count = 0;
bool start_zone_red = 0;
byte linie = 0;

// Funkce setup se zavolá vždy po startu robota.
float g_US1 = 0;
float g_US2 = 0;
byte k = 0;

// pokud se nepovede neco inicializovat (RGB senzor), program se zasekne v teto funkci
void trap()
{
    Serial.println("trap\n");
    while(1);
}

static const uint8_t TCS_led_pin = 2;
static const uint8_t TCS_SDA_pin = 21;
static const uint8_t TCS_SCL_pin = 22;

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_1X);

#include "arm_commands.h"
#include "sensors_commands.h"
#include "motors_commands.h"

void setup() {
    rkConfig cfg;
    // Upravte nastavení, například:
    // cfg.motor_max_power_pct = 30; // limit výkonu motorů na 30%
    rkSetup(cfg);

    Serial.begin(115200);
    Serial.println("start");

    // inicializace RGB
    pinMode(TCS_SDA_pin, PULLUP);
    pinMode(TCS_SCL_pin, PULLUP);
    Wire1.begin(TCS_SDA_pin, TCS_SCL_pin, 100000);
    if (!tcs.begin(TCS34725_ADDRESS, &Wire1)) {
        Serial.println("Can not connect to the RGB sensor");
    //    trap();
    }
    pinMode(TCS_led_pin, GPIO_MODE_OUTPUT);
    digitalWrite(TCS_led_pin, 1);
    Serial.println("Starting main loop\n");

    /////////////////////////////////////////////////////////////////////////
    // STARTOVACI SEKVENCE
    /////////////////////////////////////////////////////////////////////////
    
    rkServosSetPosition(3, -17);
    rkServosSetPosition(2, 90);
    for (byte i = 0; i < 8; i++) {
        rkSmartLedsRGB(i, 0, 0, 0);
    }
    
    

    // start tlacitko, vyber startovni zony (barvy)
    while (true)
    {
        if (rkButtonIsPressed(BTN_LEFT))
        {
            start_zone_red = true;
        }
        if (rkButtonIsPressed(BTN_RIGHT))
        {
            start_zone_red = false;
        }
        if (rkButtonIsPressed(BTN_UP))
        {
            break;
        }
        if (rkButtonIsPressed(BTN_DOWN))
        {
            while (true)
            {
                if (rkButtonIsPressed(BTN_LEFT))
                {
                    rkServosSetPosition(1, 90);
                }
                if (rkButtonIsPressed(BTN_RIGHT))
                {
                    rkServosSetPosition(1, -90);
                }
                if (!rkButtonIsPressed(BTN_RIGHT) && !rkButtonIsPressed(BTN_LEFT))
                {
                    rkServosSetPosition(1, 0);
                }
                
            }
            
        }
        if (start_zone_red)
        {
            rkLedRed(true);
            rkLedGreen(false);
        }
        else
        {
            rkLedGreen(true);
            rkLedRed(false);
        }
    }
    

    for (byte i = 0; i < 8; i++) {
        rkSmartLedsRGB(i, 255, 255, 255);
    }
    delay(1000);

    /*
    int last_time = millis();
    while (true)
    {
        tcs.getRawData(&red, &green, &blue, &clear);
        printf("clear: %hu, red: %hu, green: %hu, blue: %hu, sum: %hu\n", clear, red, green, blue, red+green+blue);
        printf("time: %d\n", millis() - last_time);
        last_time = millis();
    }
    */
    //start tlacitko pro kalibraci klepet

/*
    while(true){
        if(rkButtonIsPressed(BTN_UP)){
                break;
        }
    }
    */
    
printf("batery percent: %u\n", rkBatteryPercent());
    /////////////////////////////////////
    while (true)
    {
        // printf("state= %u \n", state);
        delay(20);
        switch (state)
        {
        case 1:
            state = 2;
            // hledani puku
            if (go_for_puck())
            {
                sort_puck();
                puck_count++;
            }            
            
            if (rkUltraMeasure(1) > 300)
            {
                state = 1; //opakování
            }

            // soupeř je nalezen vpravo, robot se zastaví na 5 sekund 
            // a poté zkontroluje pomocí senzoru před sebou jestli zde stále není robot
            // pokud ano, robot se zastaví na 5 sekund a poté pokračuje
            if (((rkUltraMeasure(2) < 400) || (rkUltraMeasure(2) == 0)) && (linie > 0))
            {
                rkMotorsSetSpeed(0, 0);
                int time = millis();
                while (millis() - time < 5000){}
                if ((rkUltraMeasure(1) < 300) || (rkUltraMeasure(2) == 0))
                {
                    int time = millis();
                    while (millis() - time < 5000){}
                }
            }
            

            if ((rkUltraMeasure(1) <= 300) && (linie == 1) && rkUltraMeasure(1) != 0) // puck_count > 10 ||
            {
                printf("navrat do startovni zony\n");
                state = 7; // návrat do startovní zóny a vylození puků
            }
            else if (rkUltraMeasure(1) < 300 && rkUltraMeasure(1) != 0)
            {
                printf("start nove linie\n");
                state = 3; // otoceni a start nové linie
            }
            update_sensors();
            break;
        case 3: // otoceni a start nové linie
            state = 4;
            linie % 2 == 0 ? turn(-90) : turn(90);
            forward(300);
            linie % 2 == 0 ? turn(-90) : turn(90);
            back_button();
            linie++;
            printf("linie: %u\n", linie);
            state = 1;
            break;
        case 5:
            state = 6;
            
            state = 7;
            break;
        case 7:
            state = 8;
            // návrat do startovní zóny
            turn(90);
            back_button();
            forward(350);
            turn(-180);
            state = 9; //vyklopeni puku
            break;
        case 9: //vyklopeni puku
            state = 10;
            puck_eject();
            forward(-200);
            // konec šmitec
            break;  
        //////////////////////////////////////////
        // zde zacina bordel
        //////////////////////////////////////////
        
        case 11:
            state = 12;
            // musi se dopocitat
            curve(190, 90, 13, true);
            break;
        case 13:
            state = 14;
            forward(100);
            // musi se dopocitat
            curve(140, 160, 15, false);
            break;
        case 15:
            state = 16;
            forward(9999);
            state = 17;
            break;
        case 69:
            state = 70;
            while (true)
            {
                update_sensors();
            }
            
            //klepeta_close();
            //klepeta_open();
        }
    }
    for (byte i = 0; i < 8; i++)
    {
        rkSmartLedsRGB(i, 255, 255, 255);
    }
    while(true){
       // go_for_brick();
    }
    for (byte i = 0; i < 8; i++)
    {
        rkSmartLedsRGB(i, 255, 255, 255);
    }
    
}
void loop() {
    // precteni a poslani RGB
    float r, g, b;
    tcs.getRGB(&r, &g, &b);
    Serial.print(uint8_t(r)); Serial.print(", "); Serial.print(uint8_t(g)); Serial.print(", "); Serial.print(uint8_t(b));Serial.print(" || "); 
    uint16_t red, green, blue, clear;
    tcs.getRawData(&red, &green, &blue, &clear);
    Serial.print(red); Serial.print(", "); Serial.print(green); Serial.print(", "); Serial.print(blue); Serial.print(", "); 
    Serial.print(clear); Serial.print("\n "); 
    delay(1000);
}