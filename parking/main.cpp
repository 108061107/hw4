#include "mbed.h"
#include "bbcar.h"
#include "bbcar_rpc.h"
#include <iostream>

using namespace std;

Ticker servo_ticker;
PwmOut pin5(D5), pin6(D6);
BufferedSerial xbee(D1, D0);
// BufferedSerial pc(USBTX, USBRX);
BufferedSerial pc(STDIO_UART_TX, STDIO_UART_RX);

BBCar car(pin5, pin6, servo_ticker);

void parking(Arguments *in, Reply *out);
RPCFunction rpcLED(&parking, "parking");
double d1, d2, dir;

int main() {
    char buf[256], outbuf[256];
    FILE *devin = fdopen(&xbee, "r");
    FILE *devout = fdopen(&xbee, "w");

    while (1) {
        memset(buf, 0, 256);
        for( int i = 0; ; i++ ) {
            char recv = fgetc(devin);
            if(recv == '\n') {
                printf("\r\n");
                break;
            }
            buf[i] = fputc(recv, devout);
        }
        RPC::call(buf, outbuf);
    }
}

void parking (Arguments *in, Reply *out)   {
    // In this scenario, when using RPC delimit the three arguments with a space.
    d1 = in->getArg<double>();
    d2 = in->getArg<double>();
    dir = in->getArg<char>();

    if (char(dir) == 'E') {
        car.goStraight(-200);
        ThisThread::sleep_for(int(1000*d2/20)ms);
        car.turn(100, -0.1);
        ThisThread::sleep_for(1500ms);
        car.goStraight(-200);
        ThisThread::sleep_for(int(1000*(d1+10)/20)ms);
        car.stop();
    }
    else if (char(dir) == 'W') {
        car.goStraight(-200);
        ThisThread::sleep_for(int(1000*d2/20)ms);
        car.turn(-80, -0.1);
        ThisThread::sleep_for(2000ms);
        car.goStraight(-200);
        ThisThread::sleep_for(int(1000*(d1+10)/20)ms);
        car.stop();
    }
}
