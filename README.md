# coms3930-module-1: Generative Art

<img width="740" height="416" alt="IMG_8382 (1)" src="https://github.com/user-attachments/assets/0c662d2b-d1b8-433c-a57a-c12996b0dad9" />

The theme for Module 1 was Transitions and I decided to reference the pioneering artist Hilma af Klint, particularly her The Ten Largest Series. 

In the foreground, floating bubbles appear from the bottom of the screen based on randomised characteristics (speed, interval, colour, etc). Meanwhile, the solid background gradually transitions between different chosen colours from the TFT_eSPI library (TFT_eSPI.h), which is the only library used. 

Set up your ESP - TFT to be able to connect to the Arduino IDE in your computer according to its requirements. This was the software used to communicate with the hardware.   

## Code - Design Choices
### Bubbles
Changing the following lines of code allows you to personalise the characteristics of your bubbles. 
- ```const int MAX_BUBBLES```: the maximum number of bubbles in the screen;
- ```unsigned long nextSpawnInterval```: the interval between the spawn of bubbles;
- ```bubbles[i].vx```, ```bubbles[i].vy```: the floating speed;
- ```uint16_t palette[]```: the colour options for the bubbles;

```c++
const int MAX_BUBBLES = 40;
Bubble bubbles[MAX_BUBBLES];

unsigned long lastBubbleSpawn = 0;
unsigned long nextSpawnInterval = 400;

// Spawn a bubble slightly below the screen
void spawnBubble() {
  for (int i = 0; i < MAX_BUBBLES; i++) {
    if (!bubbles[i].active) {
      bubbles[i].radius = random(3, 40);
      bubbles[i].x = random(0, tft.width());
      bubbles[i].y = tft.height() + bubbles[i].radius; // Start below bottom edge
      
      // Floating speed: slight left/right drift, upward motion
      bubbles[i].vx = (random(-15, 15) / 10.0f); // -1.5 to +1.5 px/frame
      bubbles[i].vy = -(random(12, 20) / 10.0f); // -1.2 to -3.5 px/frame (upward)

      uint16_t palette[] = { TFT_WHITE, TFT_SKYBLUE, TFT_ORANGE, TFT_GOLD, TFT_PINK, TFT_BLACK };
      bubbles[i].color = palette[random(0, 5)];
      bubbles[i].active = true;
      break;
    }
  }
}
```

### Background
To define the main colours of your transition:
```c++
void transitionBackground(uint16_t startColor, uint16_t endColor, int steps, int stepDela
```
- ```startColor```: your initial colour;
- ```endColor```: your final colour;
