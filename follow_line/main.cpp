#include "mbed.h"
#include "bbcar.h"
#include "bbcar_rpc.h"
#include <iostream>

using namespace std;

Ticker servo_ticker;
PwmOut pin5(D5), pin6(D6);
BufferedSerial xbee(D1, D0);
BufferedSerial openmv_uart(D10,D9);
// BufferedSerial pc(USBTX, USBRX);
BufferedSerial pc(STDIO_UART_TX, STDIO_UART_RX);

BBCar car(pin5, pin6, servo_ticker);

int main() 
{
   openmv_uart.set_baud(9600);
   while (1) {
      if (openmv_uart.readable()) {
         char recv[1];
         openmv_uart.read(recv, sizeof(recv));

         if (recv[0] == 'y') car.goStraight(-200);
         else if (recv[0] == 'n') car.stop();
      }
   }
}