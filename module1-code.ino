#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();
// Instantiate the sprite object and link it to the display driver configuration
TFT_eSprite spr = TFT_eSprite(&tft); // Off-screen RAM buffer used for double-buffering

// Bubble structure
struct Bubble {
  float x;
  float y;
  float vx;         // Horizontal drift
  float vy;         // Vertical rise velocity
  int16_t radius;
  uint16_t color;
  bool active;
};

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

// Update bubble positions and deactivate when fully off-screen
void updateBubbles() {
  unsigned long now = millis();

  // Spawn new bubble at random intervals
  if (now - lastBubbleSpawn >= nextSpawnInterval) {
    spawnBubble();
    lastBubbleSpawn = now;
    nextSpawnInterval = random(300, 500);
  }

  for (int i = 0; i < MAX_BUBBLES; i++) {
    if (bubbles[i].active) {
      // Move bubble along xy axis
      bubbles[i].x += bubbles[i].vx;
      bubbles[i].y += bubbles[i].vy;

      // Deactivate when bubble moves off the frame
      bool offTop   = (bubbles[i].y + bubbles[i].radius < 0);
      bool offLeft  = (bubbles[i].x + bubbles[i].radius < 0);
      bool offRight = (bubbles[i].x - bubbles[i].radius > tft.width());

      if (offTop || offLeft || offRight) {
        bubbles[i].active = false;
      }
    }
  }
}

// Render active bubbles to sprite buffer
void drawBubblesToSprite() {
  for (int i = 0; i < MAX_BUBBLES; i++) {
    if (bubbles[i].active) {
      // Draw filled circle into the invisible off-screen RAM buffer instead of physical screen
      spr.fillCircle((int)bubbles[i].x, (int)bubbles[i].y, bubbles[i].radius, bubbles[i].color);
    }
  }
}

// Full-screen background transition using alphaBlend()
void transitionBackground(uint16_t startColor, uint16_t endColor, int steps, int stepDelayMs) {
  for (int i = 0; i <= steps; i++) {
    // Scale current step to an 8-bit alpha value (0 = 0% endColor, 255 = 100% endColor)
    uint8_t alpha = (i * 255) / steps;

    uint16_t currentColor = tft.alphaBlend(alpha, endColor, startColor);

    // 1. Clear off-screen  buffer with new solid background color (prevents physical screen flash)
    spr.fillSprite(currentColor);

    // 2. Update physics and render floating bubbles into the  buffer
    updateBubbles();
    drawBubblesToSprite();

    // 3. Transfer the completed frame (instead of updating instantly)
    spr.pushSprite(0, 0);

    delay(stepDelayMs);
  }
}

void setup() {
  tft.init();
  tft.setRotation(1);

  // Allocate memory in ESP32 RAM for full-screen off-screen buffer (Width x Height x 2 bytes for RGB565)
  spr.createSprite(tft.width(), tft.height());

  randomSeed(analogRead(0));

  for (int i = 0; i < MAX_BUBBLES; i++) {
    bubbles[i].active = false;
  }
}

void loop() {
  const int steps = 80;
  const int stepDelay = 20;

  // 1. Transition: TFT_NAVY -> TFT_BLUE
  transitionBackground(TFT_NAVY, TFT_BLUE, steps, stepDelay);
  delay(5);

  // 2. Transition: TFT_BLUE -> TFT_SKYBLUE
  transitionBackground(TFT_BLUE, TFT_SKYBLUE, steps, stepDelay);
  delay(5);

  // 3. Transition: TFT_SKYBLUE -> TFT_ORANGE
  transitionBackground(TFT_SKYBLUE, TFT_ORANGE, steps, stepDelay);
  delay(10);

  // 4. Transition: TFT_ORANGE -> TFT_PURPLE
  transitionBackground(TFT_ORANGE, TFT_PURPLE, steps, stepDelay);
  delay(20);

  // 5. Transition: TFT_PURPLE -> TFT_PINK 
  transitionBackground(TFT_PURPLE, TFT_PINK, steps, stepDelay);
  delay(10);

  // 6. Transition: TFT_PINK -> TFT_RED  
  transitionBackground(TFT_PINK, TFT_RED, steps, stepDelay);
  delay(5);

  // 7. Transition: TFT_RED -> TFT_NAVY (loop back)
  transitionBackground(TFT_RED, TFT_NAVY, steps, stepDelay);
  delay(5);
}