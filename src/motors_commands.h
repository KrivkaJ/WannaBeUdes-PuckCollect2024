const int speed = 100;
const float ticksToMm = 3.62; // prepocet z tiku v enkoderech na mm
const float wheel_diameter = 285;

//jizda rovne o zadanou vzdalenost 
void forward(int mm)
{
    rkMotorsDrive(mm * ticksToMm*0.92, mm * ticksToMm, speed);

    /*
    bool st = false; 

    rkMotorsDriveRightAsync(ticksToMm * mm, 100, [&](){st = true;});
    rkMotorsDriveLeftAsync(ticksToMm * mm*0.93, 90);
    while (true)
    {
        if (st)
        {
            break;
        }
    }
    rkMotorsSetSpeed(0, 0);
    */
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

uint16_t red, green, blue, clear, sum;

bool go_for_puck(){
    rkMotorsSetSpeed(99, 100);
    delay(200);
    do{
        tcs.getRawData(&red, &green, &blue, &clear);
        //printf("clear: %hu, US: %hhu\n", clear, rkUltraMeasure(1));
        delay(50);
        sum = red + green + blue;
        printf("clear: %hu, red: %hu, green: %hu, blue: %hu, sum: %hu\n", clear, red, green, blue, red+green+blue);
        update_sensors();
        if (linie > 0)
        {
            if ((rkUltraMeasure(2) < 400) || (rkUltraMeasure(2) == 0)) //nalezeni soupere
            {   
                printf("nalezeni soupere\n");
                break;
            }
        }
    }while((clear > 600) && ((rkUltraMeasure(1) > 300) || (rkUltraMeasure(1) == 0))); // (clear_avg > 1000) && 
    rkMotorsSetSpeed(0, 0);
    if ((clear <= 600) && (rkUltraMeasure(1) > 300))
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

/*
SortColor pos = NEUTRAL;
void set_sorting_mechanism(SortColor col){
    if (col == SortColor::RED)
    {
        rkServosSetPosition(1, 90);
        delay(1200);
        rkServosSetPosition(1, 0);
        pos = RED;
    }
    if (col == SortColor::BLUE)
    {
        rkServosSetPosition(1, -90);
        delay(1200); 
        rkServosSetPosition(1, 0);
        pos = BLUE;
    }
    printf("pos1: %i\n", pos);
    if (col == SortColor::NEUTRAL)
    {
        if (pos = SortColor::RED)
        {
            rkServosSetPosition(1, -90);
            delay(1200); 
            rkServosSetPosition(1, 0);
        }
        if (pos = SortColor::BLUE)
        {
            rkServosSetPosition(1, 90);
            delay(1200);
            rkServosSetPosition(1, 0);
        }
        pos = NEUTRAL;
    }
    printf("pos2: %i\n", pos);
}
*/
void sort_puck(){
    SortColor color = get_puck_color();
    byte i = 1;
    if (start_zone_red){
        i=1;
    }
    else
    {
        i=-1;
    }
    
    if(color == SortColor::RED){
        rkServosSetPosition(1, 90*i);
        delay(1200);
        rkServosSetPosition(1, 0);
        forward(120);
        delay(1000);
        rkServosSetPosition(1, -90*i);
        delay(1200); 
        rkServosSetPosition(1, 0);
    }
    if(color == SortColor::BLUE){
        rkServosSetPosition(1, -90*i);
        delay(1200);
        rkServosSetPosition(1, 0);
        forward(120);
        delay(1000);
        rkServosSetPosition(1, 90*i);
        delay(1200); 
        rkServosSetPosition(1, 0);
    }
}