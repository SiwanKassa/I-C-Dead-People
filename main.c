/**
* @mainpage ZumoBot Project
* @brief    You can make your own ZumoBot with various sensors.
* @details  <br><br>
    <p>
    <B>General</B><br>
    You will use Pololu Zumo Shields for your robot project with CY8CKIT-059(PSoC 5LP) from Cypress semiconductor.This 
    library has basic methods of various sensors and communications so that you can make what you want with them. <br> 
    <br><br>
    </p>
    
    <p>
    <B>Sensors</B><br>
    &nbsp;Included: <br>
        &nbsp;&nbsp;&nbsp;&nbsp;LSM303D: Accelerometer & Magnetometer<br>
        &nbsp;&nbsp;&nbsp;&nbsp;L3GD20H: Gyroscope<br>
        &nbsp;&nbsp;&nbsp;&nbsp;Reflectance sensor<br>
        &nbsp;&nbsp;&nbsp;&nbsp;Motors
    &nbsp;Wii nunchuck<br>
    &nbsp;TSOP-2236: IR Receiver<br>
    &nbsp;HC-SR04: Ultrasonic sensor<br>
    &nbsp;APDS-9301: Ambient light sensor<br>
    &nbsp;IR LED <br><br><br>
    </p>
    
    <p>
    <B>Communication</B><br>
    I2C, UART, Serial<br>
    </p>
*/

#include <project.h>
#include <stdio.h>
#include "Systick.h"
#include "Motor.h"
#include "Ultra.h"
#include "Nunchuk.h"
#include "Reflectance.h"
#include "I2C_made.h"
#include "Gyro.h"
#include "Accel_magnet.h"
#include "IR.h"
#include "Ambient.h"
#include "Beep.h"
#include <time.h>
#include <sys/time.h>
int rread(void);

/**
 * @file    main.c
 * @brief   
 * @details  ** Enable global interrupt since Zumo library uses interrupts. **<br>&nbsp;&nbsp;&nbsp;CyGlobalIntEnable;<br>
*/

#if 0
//battery level//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
    Systick_Start();
    
    ADC_Battery_Start();        

    int16 adcresult =0;
    float volts = 0.0;

    printf("\nBoot\n");

    //BatteryLed_Write(1); // Switch led on 
    BatteryLed_Write(0); // Switch led off 
    //uint8 button;
    //button = SW1_Read(); // read SW1 on pSoC board
    // SW1_Read() returns zero when button is pressed
    // SW1_Read() returns one when button is not pressed

    for(;;)
    {
        
        ADC_Battery_StartConvert();
        if(ADC_Battery_IsEndConversion(ADC_Battery_WAIT_FOR_RESULT)) {   // wait for get ADC converted value
            adcresult = ADC_Battery_GetResult16(); // get the ADC value (0 - 4095)
            // convert value to Volts
            // you need to implement the conversion
            volts = ((float)adcresult*5)/4095*1.5;
            // Print both ADC results and converted value
            printf("%d %f\r\n",adcresult, volts);
        }
        CyDelay(500);
        
    }
 }   
#endif

#if 0
// button
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
    Systick_Start();
    
    printf("\nBoot\n");

    //BatteryLed_Write(1); // Switch led on 
    BatteryLed_Write(0); // Switch led off 
    
    //uint8 button;
    //button = SW1_Read(); // read SW1 on pSoC board
    // SW1_Read() returns zero when button is pressed
    // SW1_Read() returns one when button is not pressed
    
    bool led = false;
    
    for(;;)
    {
        // toggle led state when button is pressed
        if(SW1_Read() == 0) {
            led = !led;
            BatteryLed_Write(led);
            ShieldLed_Write(led);
            if(led) printf("Led is ON\n");
            else printf("Led is OFF\n");
            Beep(1000, 150);
            while(SW1_Read() == 0) CyDelay(10); // wait while button is being pressed
        }        
    }
 }   
#endif


#if 0
//ultrasonic sensor//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
    Systick_Start();
    Ultra_Start();                          // Ultra Sonic Start function
    while(1) {
        int d = Ultra_GetDistance();
        //If you want to print out the value  
        printf("distance = %d\r\n", d);
        CyDelay(200);
    }
}   
#endif


#if 0
//IR receiver//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
    IR_Start();
    
    uint32_t IR_val; 
    
    printf("\n\nIR test\n");
    
    IR_flush(); // clear IR receive buffer
    printf("Buffer cleared\n");
    
    IR_wait(); // wait for IR command
    printf("IR command received\n");
    
    // print received IR pulses and their lengths
    for(;;)
    {
        if(IR_get(&IR_val)) {
            int l = IR_val & IR_SIGNAL_MASK; // get pulse length
            int b = 0;
            if((IR_val & IR_SIGNAL_HIGH) != 0) b = 1; // get pulse state (0/1)
            printf("%d %d\r\n",b, l);
            //printf("%d %lu\r\n",IR_val & IR_SIGNAL_HIGH ? 1 : 0, (unsigned long) (IR_val & IR_SIGNAL_MASK));
        }
    }    
 }   
#endif
void motor_turnRight (uint8 speed,uint32 delay) {
    MotorDirLeft_Write (0);
    MotorDirRight_Write (1);
    PWM_WriteCompare1 (speed);
    PWM_WriteCompare2 (speed);
    CyDelay (delay);
}
void motor_turnLeft (uint8 speed,uint32 delay) {
    MotorDirLeft_Write (1);
    MotorDirRight_Write (0);
    PWM_WriteCompare1 (speed);
    PWM_WriteCompare2 (speed);
    CyDelay (delay);
}

#if 1
//reflectance//
int main()
{
    struct sensors_ ref;
    struct sensors_ dig;
    int firstLine = 0;
    int firstLinePassed =0;
    int secondLine = 0;
    int secondLinePassed =0;
    int outOfBounds = 0;
    bool StartSignal = true;

    Systick_Start();

    CyGlobalIntEnable; 
    UART_1_Start();
    IR_Start();
  
    reflectance_start();
    CyDelay(2);
    reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000); // set center sensor threshold to 11000 and others to 9000
    motor_start();
    //motor_forward(0,0);
    MotorDirLeft_Write (0);
    MotorDirRight_Write (0);
    
    //Robot reaches starting line and stops
    while(StartSignal == true)
    {
        reflectance_read(&ref);
        reflectance_digital(&dig);
        // Stops robot at starting line
        if (dig.l1 == 1 && dig.l2 == 1 && dig.l3 == 1 && dig.r1 == 1 && dig.r2 == 1 && dig.r3 == 1){
            motor_forward (0,0);
            IR_wait();
            StartSignal = false;
            motor_forward(75,100);
        }// Moves robot forward until it reaches the startingline
        else {
            motor_forward (75,1);
        }
    }
    ShieldLed_Write (0);

    for(;;)
    {
        // read raw sensor values
        reflectance_read(&ref);
        //printf("%5d %5d %5d %5d %5d %5d\r\n", ref.l3, ref.l2, ref.l1, ref.r1, ref.r2, ref.r3);       // print out each period of reflectance sensors
        
        // read digital values that are based on threshold. 0 = white, 1 = black
        // when blackness value is over threshold the sensors reads 1, otherwise 0
        reflectance_digital(&dig);      //print out 0 or 1 according to results of reflectance period
        //printf("%5d %5d %5d %5d %5d %5d \r\n", dig.l3, dig.l2, dig.l1, dig.r1, dig.r2, dig.r3);        //print out 0 or 1 according to results of reflectance period
        BatteryLed_Write (0);
        
        
        //{1 1 1 1 1 1} stop : supposed to stop if has encountered 3 black lines
        if (dig.l1 == 1 && dig.l2 == 1 && dig.l3 == 1 && dig.r1 == 1 && dig.r2 == 1 && dig.r3 == 1){
            firstLine++;
            if (firstLine >= 1 && firstLinePassed >= 1){
                secondLine++;
            }
            if (secondLinePassed >= 1){
                motor_stop();
                ShieldLed_Write (1);
            }
            
        }
        //{0 0 0 0 0 0} stop or turn if track is lost depending on direction of travel
        else if (dig.l1 == 0 && dig.l2 == 0 && dig.l3 == 0 && dig.r1 == 0 && dig.r2 == 0 && dig.r3 == 0){
            if (outOfBounds == 1){
                motor_turnLeft(150,2);
                BatteryLed_Write (1);
            }
            if (outOfBounds == 2){
                motor_turnRight(150,2);
                BatteryLed_Write (1);
            }
            if (outOfBounds == 0) {
                motor_forward (0,0);
                BatteryLed_Write (1);
            }
        }
        
        //{0 0 1 1 0 0} forward : also checks how many lines have been passed
        else if (dig.l1 == 1 && dig.r1 == 1){
            motor_forward(250,2);
           
        }
        //{0 1 1 0 0 0} soft left  
        else if (dig.l2 == 1 && dig.l1 == 1){
            motor_turn(150,250,2);
            outOfBounds = 1;
        }
        //{0 0 0 1 1 0} soft right 
        else if (dig.r2 == 1 && dig.r1 == 1){
            motor_turn(250,150,2);
            outOfBounds = 2;
        }
        //{1 1 0 0 0 0} left  
        else if (dig.l3 == 1 && dig.l2 == 1){
            motor_turn(80,210,2);
            outOfBounds = 1;
        }
        //{0 0 0 0 1 1} right 
        else if (dig.r3 == 1 && dig.r2 == 1){
            motor_turn(210,80,2);
            outOfBounds = 2;
        }
        //{0 0 0 0 0 1} hard left 
        else if (dig.l3 == 1){
            //motor_turnLeft(150,1);
            motor_turn (1,250,2);
            outOfBounds = 1;
        }
        //{1 0 0 0 0 0} hard right
        else if(dig.r3 == 1){
            //motor_turnRight(150,1);
            motor_turn (250,1,2);
            outOfBounds = 2;
        }
        // Uses the sidemost sensor to check if line has been passed
        if (dig.r3 == 0 && dig.l3)
        {
            if (firstLine >= 1){
                ShieldLed_Write (1);
                firstLinePassed++;
            }
            if (secondLine >= 1) {
                ShieldLed_Write (0);
                secondLinePassed++;
            } 
        }
      CyDelay(1);  
    }
    
}   
#endif


#if 0
//motor//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();

    motor_start();              // motor start

    motor_forward(100,2000);     // moving forward
    motor_turn(200,50,2000);     // turn
    motor_turn(50,200,2000);     // turn
    motor_backward(100,2000);    // movinb backward
       
    motor_stop();               // motor stop
    
    for(;;)
    {

    }
}
#endif


/* [] END OF FILE */
