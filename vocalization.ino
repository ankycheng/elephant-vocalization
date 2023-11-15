// WAV files for this and other Tutorials are here:
// http://www.pjrc.com/teensy/td_libs_AudioDataFiles.html

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Bounce.h>

AudioPlaySdWav playSdWav1;
AudioOutputI2S i2s1;
AudioOutputAnalogStereo dacs1;
AudioConnection patchCord1(playSdWav1, 0, i2s1, 0);
AudioConnection patchCord2(playSdWav1, 1, i2s1, 1);
AudioControlSGTL5000 sgtl5000_1;

// Use these with the Teensy Audio Shield
#define SDCARD_CS_PIN 10
#define SDCARD_MOSI_PIN 7 // Teensy 4 ignores this, uses pin 11
#define SDCARD_SCK_PIN 14 // Teensy 4 ignores this, uses pin 13

#define TRACKCOUNT 5
String trackNames[TRACKCOUNT] = {
    "Contact-Rumble-B2504224-48ElephantVoices.wav",
    "Roar-C1401746-48ElephantVoices.wav",
    "Rumble-Roar-Rumble-C270002448ElephantVoices.wav",
    "Trumpet-Blast-B3820203ElephantVoices.wav",
    "chirps-ElemotionFoundation.wav"};

float brightnessList[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
int buttonList[5] = {9, 5, 4, 3, 2};

Bounce button0 = Bounce(33, 30);
Bounce button1 = Bounce(34, 30);
Bounce button2 = Bounce(35, 30);
Bounce button3 = Bounce(36, 30);
Bounce button4 = Bounce(37, 30);
Bounce button5 = Bounce(38, 30);

float volData = 0.0;
int currentTrack = -1;
float fadeInSpd = 2.0;
float fadeOutSpd = 3.0;

void setup()
{
    Serial.begin(9600);
    pinMode(33, INPUT_PULLUP);
    pinMode(34, INPUT_PULLUP);
    pinMode(35, INPUT_PULLUP);
    pinMode(36, INPUT_PULLUP);
    pinMode(37, INPUT_PULLUP);
    pinMode(38, INPUT_PULLUP);

    pinMode(9, OUTPUT);
    pinMode(5, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(3, OUTPUT);
    pinMode(2, OUTPUT);
    pinMode(1, OUTPUT);

    AudioMemory(8);
    sgtl5000_1.enable();
    sgtl5000_1.volume(0.6);
    SPI.setMOSI(SDCARD_MOSI_PIN);
    SPI.setSCK(SDCARD_SCK_PIN);
    if (!(SD.begin(SDCARD_CS_PIN)))
    {
        while (1)
        {
            Serial.println("Unable to access the SD card");
            delay(500);
        }
    }
    delay(1000);
}

void loop()
{
    // update volume
    float newVolData = analogRead(A0) / 1023.0;
    if (abs(newVolData - volData) > 0.1)
    {
        sgtl5000_1.volume(newVolData);
        volData = newVolData;
    }

    if (playSdWav1.isPlaying() == 0)
    {
        currentTrack = -1;
    }

    // fade LEDs
    for (int i = 0; i < 5; i++)
    {
        // if not current playing: fade out
        float currentBrightness = brightnessList[i];

        if (i != currentTrack)
        {
            if (currentBrightness > 0.0)
            {
                brightnessList[i] -= fadeOutSpd;
            }
            if (currentBrightness < fadeOutSpd + 1.0)
            {
                brightnessList[i] = 0.0;
            }
            //  else {

            //   brightnessList[i] = currentBrightness > 0.0 ? brightnessList[i] -= fadeOutSpd : brightnessList[i];
            //   brightnessList[i] = currentBrightness < 5.0 ? 0.0 : brightnessList[i];
            // }
        }
        // if it's current playing: fade in
        else
        {
            if (currentBrightness < 255.0)
            {
                brightnessList[i] += fadeInSpd;
            }
        }
        analogWrite(buttonList[i], brightnessList[i]);
    }

    // Play next sound track
    button0.update();
    button1.update();
    button2.update();
    button3.update();
    button4.update();

    if (button0.fallingEdge())
    {
        playSound(0);
        currentTrack = 0;
    }

    if (button1.fallingEdge())
    {
        playSound(1);
        currentTrack = 1;
    }

    if (button2.fallingEdge())
    {
        Serial.println("button2 clicked");
        playSound(2);
        currentTrack = 2;
    }

    if (button3.fallingEdge())
    {
        Serial.println("button3 clicked");
        playSound(3);
        currentTrack = 3;
    }

    if (button4.fallingEdge())
    {
        Serial.println("button4 clicked");
        playSound(4);
        currentTrack = 4;
    }

    // for debugging
    // if (currentTrack == -1)
    // {
    //     Serial.print(" Proccessor Usage : ");
    //     Serial.print(AudioProcessorUsageMax());
    //     Serial.print(" Is playing? : ");
    //     Serial.print(playSdWav1.isPlaying());
    //     Serial.print(" Current track: ");
    //     Serial.println(currentTrack);
    // }

    // if (currentTrack != -1)
    // {
    //     Serial.print(" Proccessor Usage : ");
    //     Serial.print(AudioProcessorUsageMax());
    //     Serial.print(" Is playing? : ");
    //     Serial.print(playSdWav1.isPlaying());
    //     Serial.print(" Current track: ");
    //     Serial.println(currentTrack);
    // }

    delay(30);
}

void playSound(int TrackNumber)
{
    // Serial.print("playing: ");
    // Serial.println(TrackNumber);
    playSdWav1.stop();
    playSdWav1.play(trackNames[TrackNumber].c_str());
}
