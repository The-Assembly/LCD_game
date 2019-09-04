#include <SPI.h>  // Including Necessary libraries for the OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Bounce2.h>

Adafruit_SSD1306 display(128, 64, &Wire, 4);  //Initializing the Display

// The setup variables needed to run the game

int resolution[2] = {128, 64}, ball[2] = {20, (resolution[1] / 2)}; // Screen Resolution
const int PIXEL_SIZE = 8, WALL_WIDTH = 4, PADDLE_WIDTH = 4, BALL_SIZE = 4;
int SPEED = 3; // These are constants that help the compiler run faster
int playerScore = 0, aiScore = 0, playerPos = 0, aiPos = 0; // This stores the positions of the player and AI
char ballDirectionHori = 'R', ballDirectionVerti = 'S'; // This controls the ball direction
boolean inProgress = true; // To begin the game

Bounce debouncer = Bounce(); // Digitally debouncing buttons to remove false triggering
Bounce easy = Bounce();
Bounce hard = Bounce();

const int buttonPin = 2; // Defining the button input pins
const int easyPin = 4;
const int hardPin = 3;
int buttonCount = 0; // Variables used for button counting
int even = 0;

int easymode = 0;
int hardmode = 0;

void setup()   {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Begin the display
  display.display(); // Display what's on the buffer
  display.clearDisplay(); // Clear the buffer to begin the game

  pinMode(buttonPin, INPUT); // Define the button pins as inputs to the system
  pinMode(easyPin, INPUT);
  pinMode(hardPin, INPUT);

  debouncer.attach(buttonPin, INPUT_PULLUP); // Debouncing function by the library
  easy.attach(easyPin, INPUT_PULLUP);
  hard.attach(hardPin, INPUT_PULLUP);
  debouncer.interval(25); // Stable interval of 25ms
  easy.interval(5);
  hard.interval(5);

  Serial.begin(9600); // Start the Serial Monitor
}

void loop() {
  debouncer.update(); // Begin checking for the button inputs
  easy.update();
  hard.update();

  if (debouncer.fell()) { // If the button is falling (Transitionign from High to Low after the click)
    buttonCount++; // Increase the counter
  }

  even = buttonCount % 2; // Check if the button input is an even number

  // Welcome Screen
  if (buttonCount == 0) { // If there is no button inputs yet
    display.display(); // Initialize Display
    display.clearDisplay(); // Clear the Display buffer
    display.setCursor(2, 0); // Position where the first character pixel will appear
    display.setTextSize(3); // Set the text size
    display.setTextColor(WHITE); // Set the text colour
    display.println("WELCOME"); // Print a message on the display
    display.setCursor(38, 25); // Move the cursor down and to the center
    display.setTextSize(2); // Different size
    display.setTextColor(WHITE); // Set colour
    display.println("CLICK     START"); // Print message
  }
  else if (buttonCount == 1) { // If the button has been clicked once - Difficulty Menu
    display.display();
    display.clearDisplay();
    display.setCursor(30, 0);
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.println("CHOOSE");
    display.setCursor(6, 20);
    display.println("DIFFICULTY");
    display.setCursor(6, 40);
    display.println("EASY  HARD");
    if (easy.fell()) {
      easymode++; // Increment easy mode button to trigger the game to start 
      buttonCount++; // Increment counter to go back to an even number
    }
    if (hard.fell()) {
      hardmode++; // Increment easy mode button to trigger the game to start 
      buttonCount++; // Increment counter to go back to an even number
    }
  }
  if (easymode >= 1) { // If the easy mode button is clicked from the difficulty menu, this will start easy mode
    if (even == 0) {
      display.clearDisplay();
      if (aiScore > 4 || playerScore > 4) { // Sets the maximum score to be reached (in this case it's first to 5)
        // check game state
        inProgress = false; // Ends the game
      }

      if (inProgress) {
        eraseScore(); // Clear the previous score
        eraseBall(ball[0], ball[1]); // Initialize the balls first position

        if (ballDirectionVerti == 'U') {
          // Move ball up diagonally
          ball[1] = ball[1] - SPEED;
        }

        if (ballDirectionVerti == 'D') {
          // Move ball down diagonally
          ball[1] = ball[1] + SPEED;
        }

        if (ball[1] <= 0) {
          // Bounce the ball off the top
          ballDirectionVerti = 'D';
        }
        if (ball[1] >= resolution[1]) {
          // Bounce the ball off the bottom
          ballDirectionVerti = 'U';
        }

        if (ballDirectionHori == 'R') {
          ball[0] = ball[0] + SPEED; // Move ball
          if (ball[0] >= (resolution[0] - 6)) {
            // Ball is at the AI edge of the screen
            if ((aiPos + 12) >= ball[1] && (aiPos - 12) <= ball[1]) { // Ball hits AI paddle
              if (ball[1] > (aiPos + 4)) {
                // Deflect ball down
                ballDirectionVerti = 'D';
              }
              else if (ball[1] < (aiPos - 4)) {
                // Deflect ball up
                ballDirectionVerti = 'U';
              }
              else {
                // Deflect ball straight
                ballDirectionVerti = 'S';
              }
              // Change ball direction
              ballDirectionHori = 'L';
            }
            else {
              // GOAL!
              ball[0] = 6; // Move ball to other side of screen
              ballDirectionVerti = 'S'; // Reset ball to straight travel
              ball[1] = resolution[1] / 2; // Move ball to middle of screen
              ++playerScore; // Increase player score
            }
          }
        }

        if (ballDirectionHori == 'L') {
          ball[0] = ball[0] - SPEED; // Move ball
          if (ball[0] <= 6) {
            // Ball is at the player edge of the screen
            if ((playerPos + 12) >= ball[1] && (playerPos - 12) <= ball[1]) {
              // Ball hits player paddle
              if (ball[1] > (playerPos + 4)) {
                // Deflect ball down
                ballDirectionVerti = 'D';
              }
              else if (ball[1] < (playerPos - 4)) {
                // Deflect ball up
                ballDirectionVerti = 'U';
              }
              else {
                // Deflect ball straight
                ballDirectionVerti = 'S';
              }
              ballDirectionHori = 'R';
            }
            else {
              ball[0] = resolution[0] - 6; // Move ball to other side of screen
              ballDirectionVerti = 'S'; // Reset ball to straight travel
              ball[1] = resolution[1] / 2; // Move ball to middle of screen
              ++aiScore; // Increase AI score
            }
          }
        }

        drawBall(ball[0], ball[1]);

        erasePlayerPaddle(playerPos);
        playerPos = analogRead(A0); // Read player potentiometer (Player's control stick)
        playerPos = map(playerPos, 0, 1023, 8, 54); // Convert value from 0 - 1023 to 8 - 54
        drawPlayerPaddle(playerPos); // Move the paddle to the player's chosen position
        moveAi(); // Run the AI function to play

        drawNet(); // Draw the middle line
        drawScore(); // Update the Score
      }
      else {
        // Somebody has won
        display.clearDisplay();
        display.setTextSize(3);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        if (aiScore > playerScore) {    // Figure out who
          display.println("YOU    LOSE!");
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(0, 50);
          display.println("Reset to replay");
        }
        else if (playerScore > aiScore) {
          display.println("YOU    WIN!");
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(0, 50);
          display.println("Press Reset to replay");
        }
      }
    }
    else {
      display.clearDisplay();
      display.setCursor(12, 20);
      display.setTextSize(3);
      display.setTextColor(WHITE);
      display.println("PAUSED");
    }
  }
  if (hardmode >= 1) { // If the hard mode button is clicked from the difficulty menu, this will start hard mode
    SPEED = 4;
    if (even == 0) {
      display.clearDisplay();
      if (aiScore > 2 || playerScore > 2) { // Sets the maximum score to be reached (in this case it's first to 3)
        // check game state
        inProgress = false; // Ends the game
      }

      if (inProgress) {
        eraseScore(); // Clear the previous score
        eraseBall(ball[0], ball[1]); // Initialize the balls first position

        if (ballDirectionVerti == 'U') {
          // Move ball up diagonally
          ball[1] = ball[1] - SPEED;
        }

        if (ballDirectionVerti == 'D') {
          // Move ball down diagonally
          ball[1] = ball[1] + SPEED;
        }

        if (ball[1] <= 0) {
          // Bounce the ball off the top
          ballDirectionVerti = 'D';
        }
        if (ball[1] >= resolution[1]) {
          // Bounce the ball off the bottom
          ballDirectionVerti = 'U';
        }

        if (ballDirectionHori == 'R') {
          ball[0] = ball[0] + SPEED; // Move ball
          if (ball[0] >= (resolution[0] - 6)) {
            // Ball is at the AI edge of the screen
            if ((aiPos + 12) >= ball[1] && (aiPos - 12) <= ball[1]) { // Ball hits AI paddle
              if (ball[1] > (aiPos + 4)) {
                // Deflect ball down
                ballDirectionVerti = 'D';
              }
              else if (ball[1] < (aiPos - 4)) {
                // Deflect ball up
                ballDirectionVerti = 'U';
              }
              else {
                // Deflect ball straight
                ballDirectionVerti = 'S';
              }
              // Change ball direction
              ballDirectionHori = 'L';
            }
            else {
              // GOAL!
              ball[0] = 6; // Move ball to other side of screen
              ballDirectionVerti = 'S'; // Reset ball to straight travel
              ball[1] = resolution[1] / 2; // Move ball to middle of screen
              ++playerScore; // Increase player score
            }
          }
        }

        if (ballDirectionHori == 'L') {
          ball[0] = ball[0] - SPEED; // Move ball
          if (ball[0] <= 6) {
            // Ball is at the player edge of the screen
            if ((playerPos + 12) >= ball[1] && (playerPos - 12) <= ball[1]) {
              // Ball hits player paddle
              if (ball[1] > (playerPos + 4)) {
                // Deflect ball down
                ballDirectionVerti = 'D';
              }
              else if (ball[1] < (playerPos - 4)) {
                // Deflect ball up
                ballDirectionVerti = 'U';
              }
              else {
                // Deflect ball straight
                ballDirectionVerti = 'S';
              }
              ballDirectionHori = 'R';
            }
            else {
              ball[0] = resolution[0] - 6; // Move ball to other side of screen
              ballDirectionVerti = 'S'; // Reset ball to straight travel
              ball[1] = resolution[1] / 2; // Move ball to middle of screen
              ++aiScore; // Increase AI score
            }
          }
        }

        drawBall(ball[0], ball[1]);

        erasePlayerPaddle(playerPos);
        playerPos = analogRead(A0); // Read player potentiometer (Player's control stick)
        playerPos = map(playerPos, 0, 1023, 8, 54); // Convert value from 0 - 1023 to 8 - 54
        drawPlayerPaddle(playerPos); // Move the paddle to the player's chosen position
        moveAih(); // Run the AI function to play

        drawNet(); // Draw the middle line
        drawScore(); // Update the Score
      }
      else {
        // Somebody has won
        display.clearDisplay();
        display.setTextSize(3);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        if (aiScore > playerScore) {    // Figure out who
          display.println("YOU    LOSE!");
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(0, 50);
          display.println("Reset to replay");
        }
        else if (playerScore > aiScore) {
          display.println("YOU    WIN!");
          display.setTextSize(1);
          display.setTextColor(WHITE);
          display.setCursor(0, 50);
          display.println("Press Reset to replay");
        }
      }
    }
    else {
      display.clearDisplay();
      display.setCursor(12, 20);
      display.setTextSize(3);
      display.setTextColor(WHITE);
      display.println("PAUSED");
    }
  }
  display.display();
}


void moveAi() {
  // Move the AI paddle
  eraseAiPaddle(aiPos);
  if (ball[1] > aiPos) {
    ++aiPos;
    //aiPos += 2;
  }
  else if (ball[1] < aiPos) {
    --aiPos;
    //aiPos -= 2;
  }
  drawAiPaddle(aiPos);
}

void moveAih() {
  // Move the AI paddle
  eraseAiPaddle(aiPos);
  if (ball[1] > aiPos) {
    //++aiPos;
    aiPos += 2;
  }
  else if (ball[1] < aiPos) {
    //--aiPos;
    aiPos -= 2;
  }
  drawAiPaddle(aiPos);
}

void drawScore() {
  // Draw AI and player scores
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(45, 0);
  display.println(playerScore);

  display.setCursor(75, 0);
  display.println(aiScore);
}

void eraseScore() {
  // Erase AI and player scores
  display.setTextSize(2);
  display.setTextColor(BLACK);
  display.setCursor(45, 0);
  display.println(playerScore);

  display.setCursor(75, 0);
  display.println(aiScore);
}

void drawNet() {
  for (int i = 0; i < (resolution[1] / WALL_WIDTH); ++i) {
    drawPixel(((resolution[0] / 2) - 1), i * (WALL_WIDTH) + (WALL_WIDTH * i), WALL_WIDTH);
  }
}

void drawPixel(int posX, int posY, int dimensions) {
  // draw group of pixels
  for (int x = 0; x < dimensions; ++x) {
    for (int y = 0; y < dimensions; ++y) {
      display.drawPixel((posX + x), (posY + y), WHITE);
    }
  }
}

void erasePixel(int posX, int posY, int dimensions) {
  // erase group of pixels
  for (int x = 0; x < dimensions; ++x) {
    for (int y = 0; y < dimensions; ++y) {
      display.drawPixel((posX + x), (posY + y), BLACK);
    }
  }
}

void erasePlayerPaddle(int row) {
  erasePixel(0, row - (PADDLE_WIDTH * 2), PADDLE_WIDTH);
  erasePixel(0, row - PADDLE_WIDTH, PADDLE_WIDTH);
  erasePixel(0, row, PADDLE_WIDTH);
  erasePixel(0, row + PADDLE_WIDTH, PADDLE_WIDTH);
  erasePixel(0, row + (PADDLE_WIDTH + 2), PADDLE_WIDTH);
}

void drawPlayerPaddle(int row) {
  drawPixel(0, row - (PADDLE_WIDTH * 2), PADDLE_WIDTH);
  drawPixel(0, row - PADDLE_WIDTH, PADDLE_WIDTH);
  drawPixel(0, row, PADDLE_WIDTH);
  drawPixel(0, row + PADDLE_WIDTH, PADDLE_WIDTH);
  drawPixel(0, row + (PADDLE_WIDTH + 2), PADDLE_WIDTH);
}

void drawAiPaddle(int row) {
  int column = resolution[0] - PADDLE_WIDTH;
  drawPixel(column, row - (PADDLE_WIDTH * 2), PADDLE_WIDTH);
  drawPixel(column, row - PADDLE_WIDTH, PADDLE_WIDTH);
  drawPixel(column, row, PADDLE_WIDTH);
  drawPixel(column, row + PADDLE_WIDTH, PADDLE_WIDTH);
  drawPixel(column, row + (PADDLE_WIDTH * 2), PADDLE_WIDTH);
}

void eraseAiPaddle(int row) {
  int column = resolution[0] - PADDLE_WIDTH;
  erasePixel(column, row - (PADDLE_WIDTH * 2), PADDLE_WIDTH);
  erasePixel(column, row - PADDLE_WIDTH, PADDLE_WIDTH);
  erasePixel(column, row, PADDLE_WIDTH);
  erasePixel(column, row + PADDLE_WIDTH, PADDLE_WIDTH);
  erasePixel(column, row + (PADDLE_WIDTH * 2), PADDLE_WIDTH);
}

void drawBall(int x, int y) {
  display.drawCircle(x, y, BALL_SIZE, WHITE);
}

void eraseBall(int x, int y) {
  display.drawCircle(x, y, BALL_SIZE, BLACK);
}
