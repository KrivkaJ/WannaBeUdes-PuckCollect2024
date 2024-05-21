const int speed = 100;
const float ticksToMm = 3.62; // prepocet z tiku v enkoderech na mm
const float wheel_diameter = 162;

//jizda rovne o zadanou vzdalenost 
void forward(int mm)
{
    rkMotorsDrive(mm * ticksToMm, mm * ticksToMm, speed);
}

//zatoceni na miste o zadany pocet stupnu
// + doprava - doleva
void turn(int degrees)
{
    rkMotorsDrive(3.141 * wheel_diameter * degrees / 360 * ticksToMm, -3.141 * wheel_diameter * degrees / 360 * ticksToMm, speed);
}

//oblouk
// tue=doprava, false=doleva
void curve(int radius, int degrees, byte new_state, bool right){
    int sR = radius/radius * 40;
    int sL = (radius + wheel_diameter) / radius * 40;
    printf("sR: %i, sL: %i\n", sR, sL);
    if (right)
    {
    rkMotorsDriveRightAsync((ticksToMm * radius * PI * degrees)/180, sR, [&, new_state](){state = new_state;});
    rkMotorsDriveLeftAsync((ticksToMm * (radius + wheel_diameter) * PI * degrees)/180, sL);
    }
    else{
    rkMotorsDriveLeftAsync((ticksToMm * radius * PI * degrees)/180, sR, [&, new_state](){state = new_state;});
    rkMotorsDriveRightAsync((ticksToMm * (radius + wheel_diameter) * PI * degrees)/180, sL);
    }
}

//oblouk dozadu 
void curve_back(int radius, bool right)
{
    int sR = radius / radius * 40;
    int sL = (radius + wheel_diameter) / radius * 40;
    printf("sR: %i, sL: %i\n", sR, sL);
    if (right)
    {
        rkMotorsDriveRightAsync(-ticksToMm * radius * PI, sR);
        rkMotorsDriveLeftAsync(-ticksToMm * (radius + wheel_diameter) * PI, sL);
    }
    else
    {
        rkMotorsDriveLeftAsync(-ticksToMm * radius * PI, sR);
        rkMotorsDriveRightAsync(-ticksToMm * (radius + wheel_diameter) * PI, sL);
    }
}

// robot couva dokud nenarazi na zadni tlacitko
void back_button()
{
    rkMotorsSetSpeed(-100, -100);
    while (true)
    {
        if (rkButtonIsPressed(BTN_RIGHT))
        {
            rkMotorsSetSpeed(-100, -20);
        }
        if (rkButtonIsPressed(BTN_LEFT))
        {
            rkMotorsSetSpeed(-20, -100);
        }
        if (rkButtonIsPressed(BTN_LEFT) && rkButtonIsPressed(BTN_RIGHT))
        {
            break;
        }
        
    }
    delay(500);
    rkMotorsSetSpeed(0, 0);
}

// otocka robota ode zdi o 90 stupnu doleva
void turn_by_wall()
{
    rkServosSetPosition(3, -90);
    delay(1000);
    rkServosSetPosition(3, 90);
    delay(100);
    rkServosSetPosition(3, -80);
    delay(500);
    rkServosSetPosition(3, 90);
    back_button();
}

uint16_t red, green, blue, clear[2], clear_avg;

bool go_for_puck(){
    rkMotorsSetSpeed(speed, speed);
    delay(1000);
    do{
        clear_avg = 0;
        for (byte i = 0; i < 2; i++)
        {
            tcs.getRawData(&red, &green, &blue, &clear[i]);
            printf("clear: %hu, US: %hhu\n", clear[i], rkUltraMeasure(1));
            delay(50);
        }
        for (byte i = 0; i < 2; i++)
        {
            clear_avg += clear[i];
        }
        clear_avg /= 2;
    }while((clear_avg > 1000) && (rkUltraMeasure(1) > 40));
    rkMotorsSetSpeed(0, 0);
    if (clear_avg < 1000)
    {
        printf("found by rgb senzor\n");
        return true;
    }
    else
    {
        printf("stoped by US\n");
        return false;
    }
    
}


SortColor pos = NEUTRAL;
void set_sorting_mechanism(SortColor color){
    if (color == SortColor::RED)
    {
        rkServosSetPosition(1, 40);
        delay(3500);
        rkServosSetPosition(1, 0);
        pos = RED;
    }
    if (color == SortColor::BLUE)
    {
        rkServosSetPosition(1, -40);
        delay(3500);
        rkServosSetPosition(1, 0);
        pos = BLUE;
    }
    if (color == SortColor::NEUTRAL)
    {
        if (pos = SortColor::RED)
        {
            rkServosSetPosition(1, 40);
            delay(3500);
            rkServosSetPosition(1, 0);
        }
        if (pos = SortColor::BLUE)
        {
            rkServosSetPosition(1, -40);
            delay(3500);
            rkServosSetPosition(1, 0);
        }
        pos = NEUTRAL;
    }
    printf("pos: %i\n", rkServosGetPosition(1));
}

void sort_puck(){
    SortColor color = get_puck_color();
    set_sorting_mechanism(color);
    forward(100);
    set_sorting_mechanism(SortColor::NEUTRAL);
}