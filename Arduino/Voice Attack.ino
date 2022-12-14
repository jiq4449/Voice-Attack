//DISCLAIMER: The following code has been slightly edited to better encompass its function of serial communication with Unity

/*
  WM Harris 470.12-2 demo Serial with FFT library  4.6.22
  
  FFT_03 Example Edited to run more frequently
  and translate into dominant frequency to send to (not too close) speaker.
  Example of use of the FFT libray to compute FFT for a signal sampled through the ADC.
        Copyright (C) 2018 Enrique Condés and Ragnar Ranøyen Homb

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
// arduinoFFT - Version: 1.5.6
#include <arduinoFFT.h>//use app Library Mgr to hunt and add this

arduinoFFT FFT = arduinoFFT(); /* Create FFT object */
/*
  These values can be changed in order to evaluate the functions
*/
#define CHANNEL A1

const uint16_t samples = 256;//128; //64; //This value MUST ALWAYS be a power of 2
const double samplingFrequency = 5000; //Hz, must be less than 10000 due to ADC

unsigned int sampling_period_us;
unsigned long microseconds;

/*
  These are the input and output vectors
  Input vectors receive computed results from FFT
*/
double vReal[samples];
double vImag[samples];

#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03

void setup()
{
  sampling_period_us = round(1000000 * (1.0 / samplingFrequency));
  Serial.begin(115200);
  //Serial.println("Ready");
}

void loop()
{
  /*SAMPLING*/
  for (int i = 0; i < samples; i++)
  {
    microseconds = micros();    //Overflows after around 70 minutes!

    vReal[i] = analogRead(CHANNEL);
    vImag[i] = 0;
    while (micros() < (microseconds + sampling_period_us)) {
      //empty loop
    }
  }

  FFT.Windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);  /* Weigh data */
  FFT.Compute(vReal, vImag, samples, FFT_FORWARD); /* Compute FFT */
  FFT.ComplexToMagnitude(vReal, vImag, samples); /* Compute magnitudes */
  
  double peakFreq = FFT.MajorPeak(vReal, samples, samplingFrequency); /* peak freq? */
  
  //calculate a sort-of max magnitude from 1/4, 1/3, and 1/2 thru array of magnitudes
  long sortaMag = max(vReal[int(samples * .33)], vReal[int(samples * .5)]);
  sortaMag = max(sortaMag, vReal[int(samples * .25)]);
  
  if (sortaMag > 150) { //If this sample was loud enough
    PrintToSerial(peakFreq);
  }
  else
  {
    PrintToSerial(0);
  }
  //delay(2);
}

void PrintToSerial(double peakFrequency)
{
  Serial.println(peakFrequency);
}