// otestuje funkcnost ramene
void test()
{
    rkMotorsSetSpeed(100, 100);
    delay(5000);
    rkMotorsSetSpeed(0, 0);
    rkServosSetPosition(3, -90);
    delay(1000);
    rkServosSetPosition(3, 90);
    delay(1000);
    rkMotorsSetSpeed(-50, -50);
    while (true)
    {
        if (rkButtonIsPressed(BTN_RIGHT))
        {
            break;
        }
    }
    delay(500);
    rkMotorsSetSpeed(0, 0);
    rkMotorsSetSpeed(100, 100);
    delay(5000);
    rkMotorsSetSpeed(0, 0);
}
// nastavi pozici ramene a klepet na chytani kostek
void arm_down()
{
    rkServosSetPosition(2, 90);
    printf("arm in pick mode, angle:%f\n", rkServosGetPosition(1));
   
}
// nastavi pozici ramene na jizdu a zavre klepeta
void arm_up()
{
    
    rkServosSetPosition(2, 0);
    printf("arm in drive mode, angle:%f\n", rkServosGetPosition(1));
}
// nastavi pozici ramene na vylozeni kostky a otevre klepeta
void arm_back()
{
    rkServosSetPosition(2, -90);
    delay(500);
    rkServosDisable(2);
    delay(500);
    printf("arm in delivery mode, angle:%f\n", rkServosGetPosition(1));
    
}

void puck_eject()
{
    rkServosSetPosition(2, -10);
    delay(1000);
}