


void encodery() {
    Serial.printf("L: %f, R: %f\n", rkMotorsGetPositionLeft(), rkMotorsGetPositionRight());
    // rkMotorsGetPositionLeft();
    // rkMotorsGetPositionRight();
}

void update_sensors() {
    g_US1 = rkUltraMeasure(1);
    g_US2 = rkUltraMeasure(2);
    printf(" g_US1: %f, g_US2: %f \n", g_US1, g_US2);
    //std::cout << " " << std::endl;
}

/*
RGB rgb_get(){
    float r, g, b;
    tcs.getRGB(&r, &g, &b);
    int red[3], green[3], blue[3];
    for (size_t i = 0; i < 3; i++)
    {
        tcs.getRGB(&r, &g, &b);
        red[i] = r;
        green[i] = g;
        blue[i] = b;
    }
    int sum_red = 0, sum_green = 0, sum_blue = 0;
    for (size_t i = 0; i < 3; i++)
    {
        sum_red += red[i];
        sum_green += green[i];
        sum_blue += blue[i];
    }
    byte red_avg = sum_red / 3;
    byte green_avg = sum_green / 3;
    byte blue_avg = sum_blue / 3;
    while (true)
    {
        if (rkButtonIsPressed(BTN_RIGHT))
        {
            break;
        }
    }
    delay(500);
    rkMotorsSetSpeed(0, 0);

    if (red_avg > green_avg && red_avg > blue_avg)
    {
        rgb_value = RED;
        printf("RED\n");
        return RED;
    }
    else if (blue_avg > red_avg && blue_avg > green_avg)
    {
        rgb_value = BLUE;
        printf("BLUE\n");
        return BLUE;
    }
    else{
        rgb_value = GREEN;
        printf("GREEN\n");
        return GREEN;
    }
}
*/


enum SortColor
{
    RED,
    BLUE,
    NEUTRAL
};
 SortColor get_puck_color(){
    float r, g, b;
        tcs.getRGB(&r, &g, &b);
        int red[3], green[3], blue[3];
        for (size_t i = 0; i < 3; i++)
        {
            tcs.getRGB(&r, &g, &b);
            red[i] = r;
            green[i] = g;
            blue[i] = b;
        }
        int sum_red = 0, sum_green = 0, sum_blue = 0;
        for (size_t i = 0; i < 3; i++)
        {
            sum_red += red[i];
            sum_green += green[i];
            sum_blue += blue[i];
        }
        byte red_avg = sum_red / 3;
        byte green_avg = sum_green / 3;
        byte blue_avg = sum_blue / 3;

        if (red_avg > green_avg && red_avg > blue_avg)
        {
            printf("RED\n");
            return RED;
        }
        else if (blue_avg > red_avg && blue_avg > green_avg)
        {
            printf("BLUE\n");
            return BLUE;
        }
        else{
            printf("GREEN\n");
            return NEUTRAL;
        }

}