/* VibeChk source code
Software: David Badiei
Hardware designer: Nathan Vuong
Documentation: Favian Zuniga
*/

//Include LCD libraries
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//Define button and buzzer macros
#define button 2
#define buzzer 13

//Define variables
LiquidCrystal_I2C lcd(0x27, 16, 2);
int buttonState = LOW;
long timePast[4] = { 0,0,0,0};
bool upordown = false;
char obstacles[2][14];
long randomNumber;
int score = -1;

//Define functions
void gameLaunchpage();
void countdown();
void gameLoop();
void addObstacle();
void scrollLeft();
void printObstacles();
void updateScore();
void gameOver();

void setup() 
{
	//Set buzzer
	pinMode(buzzer, OUTPUT);

	//Initialize LCD and turn on backlight
	lcd.init();
	lcd.init();
	lcd.backlight();

	//Seed a random number
	randomSeed(analogRead(0));
}

void loop() 
{
	//Start the game
	gameLaunchpage();
}

void gameLaunchpage()
{
	//Create the launch page and wait for a button press
	lcd.setCursor(0, 0);
	lcd.print("====VIBECHK!====");
	lcd.setCursor(2, 1);
	lcd.print("Press button");
	buttonState = digitalRead(button);
	if (buttonState == HIGH)
	{
		countdown();
	}
}

void countdown()
{
	//Clear the obstacle variable
	for (int x = 0; x < 2; x++)
	{
		for (int y = 0; y < 14; y++)
		{
			obstacles[x][y] = ' ';
		}
	}
	
	//Create a countdown for 3 seconds and then start the game proper
	lcd.clear();
	lcd.print("   Get Ready!");
	lcd.setCursor(7, 1);
	lcd.print(3);
	delay(1000);
	lcd.setCursor(7, 1);
	lcd.print(2);
	delay(1000);
	lcd.setCursor(7, 1);
	lcd.print(1);
	delay(1000);
	buttonState = LOW;
	gameLoop();
}

void gameLoop()
{
	//Clear the screen, draw character, calculate current time, set the score to zero and add the first obstacle
	lcd.clear();
	lcd.setCursor(1, 0);
	lcd.print(">");
	timePast[0] = millis();
	timePast[2] = millis();
	updateScore();
	addObstacle();
	while (1)
	{
		//Get button state and get current time
		buttonState = digitalRead(button);
		timePast[1] = millis();
		if (buttonState == HIGH)
		{
			if (upordown == false)
			{
				//Change the player to the opposite row
				lcd.setCursor(1, 0);
				lcd.print(" ");
				lcd.setCursor(1, 1);
				lcd.print(">");
				upordown = true;
				delay(150);
			}
			else
			{
				//Change the player to the opposite row
				lcd.setCursor(1, 1);
				lcd.print(" ");
				lcd.setCursor(1, 0);
				lcd.print(">");
				upordown = false;
				delay(150);
			}
		}
		
		//Every 750 miliseconds add an obstacle
		if (timePast[1] - timePast[0] >= 750)
		{
			addObstacle();
		}
		
		//Every 100 miliseconds scroll the obstacles left
		if (timePast[1] - timePast[2] >= 100)
		{
			scrollLeft();
		}

		//Every 200 miliseconds silence the buzzer
		if (timePast[1] - timePast[3] >= 200)
		{
			digitalWrite(buzzer, LOW);
		}
	}
}

void addObstacle()
{
	//Generate random number
	randomNumber = random(3);

	//If random number is zero generate obstacle on first row
	if (randomNumber == 0)
	{
		obstacles[0][13] = 'O';
		obstacles[1][13] = ' ';
	}

	//If random number is one generate obstacle on second row
	else if (randomNumber == 1)
	{
		obstacles[0][13] = ' ';
		obstacles[1][13] = 'O';
	}

	//If random number is anything else generate nothing
	else
	{
		obstacles[0][13] = ' ';
		obstacles[1][13] = ' ';
	}

	//Reprint obstacles
	printObstacles();

	//Get new time
	timePast[0] = millis();
}

void scrollLeft()
{
	//Shift both rows of obstacles
	for (int i = 0; i <= 12; i++)
	{
		obstacles[0][i] = obstacles[0][i+1];
		obstacles[1][i] = obstacles[1][i+1];
		if (obstacles[0][i] == 'O')
		{
			obstacles[0][i + 1] = ' ';
		}
		if (obstacles[1][i] == 'O')
		{
			obstacles[1][i + 1] = ' ';
		}
	}

	//Check if enemy dodged an obstacle
	if (obstacles[0][0] == 'O' && upordown != false)
	{
		updateScore();
	}
	else if (obstacles[1][0] == 'O' && upordown == false)
	{
		updateScore();
	}

	//Check for collision
	if (obstacles[0][0] == 'O' && upordown == false)
	{
		buttonState = LOW;
		gameOver();
	}
	else if (obstacles[1][0] == 'O' && upordown == true)
	{
		buttonState = LOW;
		gameOver();
	}

	//Reprint obstacles
	printObstacles();

	//Get new time
	timePast[2] = millis();
}

void printObstacles()
{
	//Print both rows of obstacles
	lcd.setCursor(2, 0);
	lcd.print(obstacles[0]);
	lcd.setCursor(2, 1);
	lcd.print(obstacles[1]);
}

void updateScore()
{
	//Increment score variable
	score++;

	//If score is 100 set it back to zero
	if (score == 100)
	{
		score = 0;
	}

	//Draw score
	lcd.setCursor(0, 0);
	lcd.print(score / 10);
	lcd.setCursor(0, 1);
	lcd.print(score % 10);

	//Enable buzzer
	digitalWrite(buzzer, HIGH);

	//Get new time
	timePast[3] = millis();
}

void gameOver()
{
	//Clear screen and draw game over screen
	lcd.clear();
	digitalWrite(buzzer, HIGH);
	lcd.setCursor(0,0);
	lcd.print("GameOver Scre:");
	lcd.print(score);
	lcd.setCursor(0, 1);
	lcd.print("Press button");

	//Wait for keypress and then launch the countdown again when pressed
	while (1)
	{
		buttonState = digitalRead(button);
		if (buttonState == HIGH)
		{
			score = -1;
			buttonState = LOW;
			upordown = false;
			digitalWrite(buzzer, LOW);
			countdown();
		}
	}
}