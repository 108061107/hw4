#include "mbed.h"
#include "bbcar.h"
#include "bbcar_rpc.h"
#include <iostream>
#include <cmath>

using namespace std;

Ticker servo_ticker;
PwmOut pin5(D5), pin6(D6);
BufferedSerial pc(STDIO_UART_TX, STDIO_UART_RX);
BufferedSerial xbee(D1, D0);
DigitalInOut pin11(D11);
BufferedSerial openmv_uart(D10,D9); // tx, rx


BBCar car(pin5, pin6, servo_ticker);

int main(){
    pc.set_baud(9600);
    xbee.set_baud(9600);
    openmv_uart.set_baud(9600);

    char data[30];
    int j = 0;
    while(1){
      if(openmv_uart.readable()){
            char recv[1];
            openmv_uart.read(recv, sizeof(recv));
            data[j] = recv[0];
            j++;
            if (j == 29) break;
      }
    }
    /*
    for (int i = 0; i < 30; i++) printf("%c", data[i]);
    printf("\n");
    char buffer1[50];
    sprintf(buffer1, "%s\r\n", data);
    xbee.write(buffer1, sizeof(buffer1));
    */

    float data_tx_R = (data[5]-'0') + (data[7]-'0')/10.0 + (data[8]-'0')/100.0 + (data[9]-'0')/1000.0;    // 3.177
    float data_tz_R = (data[16]-'0') + (data[18]-'0')/10.0 + (data[19]-'0')/100.0 + (data[20]-'0')/1000.0;
    float data_tx_L = (data[6]-'0') + (data[8]-'0')/10.0 + (data[9]-'0')/100.0 + (data[10]-'0')/1000.0;
    float data_tz_L = (data[17]-'0') + (data[19]-'0')/10.0 + (data[20]-'0')/100.0 + (data[21]-'0')/1000.0;
    float angle_R = atan(data_tx_R/data_tz_R);
    float angle_L = atan(data_tx_L/data_tz_L);
    float side_R = sqrt(data_tx_R*data_tx_R+data_tz_R*data_tz_R);
    float side_L = sqrt(data_tx_L*data_tx_L+data_tz_L*data_tz_L);
    float distance_R = side_R*sin(angle_R)*6.2;
    float distance_L = side_L*sin(angle_L)*6.2;
    float speed = 19.5;

    if (data[0] == 'R') {
        // printf("%f %f\n", data_tx_R, data_tz_R);
        car.turn(-80, -0.1);
        ThisThread::sleep_for(1800ms);
        car.goStraight(-200);
        wait_us(850*distance_R/speed*1000);
        car.turn(120, -0.1);
        ThisThread::sleep_for(1050ms);
        car.stop();
        ThisThread::sleep_for(2s);
    }
    else if (data[0] == 'L') {
        // printf("%f %f\n", data_tx_L, data_tz_L);
        car.turn(120, -0.1);
        ThisThread::sleep_for(1050ms);
        car.goStraight(-200);
        wait_us(850*distance_L/speed*1000);
        car.turn(-80, -0.1);
        ThisThread::sleep_for(1800ms);
        car.stop();
        ThisThread::sleep_for(2s);
    }

    parallax_ping ping(pin11);
    int i = 0;
    while(i < 10) {
        char buffer[20];
        printf("Ping = %.1f\n", float(ping));
        sprintf(buffer, "Ping = %.1f\r\n", float(ping));
        xbee.write(buffer, sizeof(buffer));
        ThisThread::sleep_for(1s);
        i++;
   }

    
}