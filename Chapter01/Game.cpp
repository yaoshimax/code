// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Game.h"
#include <random>

const int thickness = 15;
const float leftPaddleH = 100.0f;
const float rightPaddleH = 100.0f;
const int ballCount = 5;

Game::Game()
:mWindow(nullptr)
,mRenderer(nullptr)
,mTicksCount(0)
,mIsRunning(true)
,mLeftPaddleDir(0)
,mRightPaddleDir(0)
{
	
}

bool Game::Initialize()
{
	// Initialize SDL
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (sdlResult != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}
	
	// Create an SDL Window
	mWindow = SDL_CreateWindow(
		"Game Programming in C++ (Chapter 1)", // Window title
		100,	// Top left x-coordinate of window
		100,	// Top left y-coordinate of window
		1024,	// Width of window
		768,	// Height of window
		0		// Flags (0 for no flags set)
	);

	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}
	
	//// Create SDL renderer
	mRenderer = SDL_CreateRenderer(
		mWindow, // Window to create renderer for
		-1,		 // Usually -1
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	if (!mRenderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}
	//
	mLeftPaddlePos.x = 10.0f;
	mLeftPaddlePos.y = 768.0f/2.0f;
	mRightPaddlePos.x = 1024.0f-10.0f-thickness;
	mRightPaddlePos.y = 768.0f / 2.0f;

	std::random_device seed_gen;
	std::mt19937 engine(seed_gen());
	std::uniform_real_distribution<> distribution(-50.0, 50.0);

	for(int i=0; i<ballCount; i++)
	{
		Ball ball;
		ball.pos.x = 1024.0f / 2.0f + static_cast<float>(distribution(engine));
		ball.pos.y = 768.0f/2.0f + static_cast<float>(distribution(engine));
		ball.vel.x = -200.0f + static_cast<float>(distribution(engine));
		ball.vel.y = 235.0f + static_cast<float>(distribution(engine));
		mBalls.push_back(ball);
	}
	return true;
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			// If we get an SDL_QUIT event, end loop
			case SDL_QUIT:
				mIsRunning = false;
				break;
		}
	}
	
	// Get state of keyboard
	const Uint8* state = SDL_GetKeyboardState(NULL);
	// If escape is pressed, also end loop
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}
	
	// Update left paddle direction based on W/S keys
	mLeftPaddleDir = 0;
	if (state[SDL_SCANCODE_W])
	{
		mLeftPaddleDir -= 1;
	}
	if (state[SDL_SCANCODE_S])
	{
		mLeftPaddleDir += 1;
	}

	// Update right paddle direction based on I/K keys
	mRightPaddleDir = 0;
	if (state[SDL_SCANCODE_I])
	{
		mRightPaddleDir -= 1;
	}
	if (state[SDL_SCANCODE_K])
	{
		mRightPaddleDir += 1;
	}

}

void Game::UpdateGame()
{
	// Wait until 16ms has elapsed since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	// Delta time is the difference in ticks from last frame
	// (converted to seconds)
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	
	// Clamp maximum delta time value
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}

	// Update tick counts (for next frame)
	mTicksCount = SDL_GetTicks();
	
	// Update left paddle position based on direction
	if (mLeftPaddleDir != 0)
	{
		mLeftPaddlePos.y += mLeftPaddleDir * 300.0f * deltaTime;
		// Make sure paddle doesn't move off screen!
		if (mLeftPaddlePos.y < (leftPaddleH/2.0f + thickness))
		{
			mLeftPaddlePos.y = leftPaddleH/2.0f + thickness;
		}
		else if (mLeftPaddlePos.y > (768.0f - leftPaddleH/2.0f - thickness))
		{
			mLeftPaddlePos.y = 768.0f - leftPaddleH/2.0f - thickness;
		}
	}
	
	// Update right paddle position based on direction
	if (mRightPaddleDir != 0)
	{
		mRightPaddlePos.y += mRightPaddleDir * 300.0f * deltaTime;
		// Make sure paddle doesn't move off screen!
		if (mRightPaddlePos.y < (rightPaddleH / 2.0f + thickness))
		{
			mRightPaddlePos.y = rightPaddleH / 2.0f + thickness;
		}
		else if (mRightPaddlePos.y > (768.0f - rightPaddleH / 2.0f - thickness))
		{
			mRightPaddlePos.y = 768.0f - rightPaddleH / 2.0f - thickness;
		}
	}
	
	// Update ball position based on ball velocity
	for(Ball& ball: mBalls){
		ball.pos.x += ball.vel.x * deltaTime;
		ball.pos.y += ball.vel.y * deltaTime;
		// Bounce if needed
		// Did we intersect with the paddle?
		float leftdiff = mLeftPaddlePos.y - ball.pos.y;
		float rightdiff = mRightPaddlePos.y - ball.pos.y;

		// Take absolute value of difference
		leftdiff = (leftdiff > 0.0f) ? leftdiff : -leftdiff;
		rightdiff = (rightdiff > 0.0f) ? rightdiff : -rightdiff;
		if (
			// Our y-difference is small enough to the left panel
			leftdiff <= leftPaddleH / 2.0f &&
			// We are in the correct x-position
			ball.pos.x <= 25.0f && ball.pos.x >= 20.0f &&
			// The ball is moving to the left
			ball.vel.x < 0.0f)
		{
			ball.vel.x *= -1.0f;
		}
		else if (
			// Our y-difference is small enough to the right panel
			rightdiff <= rightPaddleH / 2.0f &&
			// We are in the correct x-position
			ball.pos.x <= 1024.0f - 20.0f && ball.pos.x >= 1024.0f - 25.0f &&
			// The ball is moving to the right
			ball.vel.x > 0.0f)
		{
			ball.vel.x *= -1.0f;
		}
		// Did the ball go off the screen? (if so, end game)
		else if (ball.pos.x <= 0.0f || ball.pos.x >= 1024.0f)
		{
			ball.out = true;
		}
		// Did the ball collide with the top wall?
		if (ball.pos.y <= thickness && ball.vel.y < 0.0f)
		{
			ball.vel.y *= -1;
		}
		// Did the ball collide with the bottom wall?
		else if (ball.pos.y >= (768 - thickness) &&
			ball.vel.y > 0.0f)
		{
			ball.vel.y *= -1;
		}
	}
	mBalls.erase(std::remove_if(mBalls.begin(), mBalls.end(), [](Ball b) {return b.out; }), mBalls.end());
	if (mBalls.size() <= 0) {
		mIsRunning = false;
	}
}

void Game::GenerateOutput()
{
	// Set draw color to blue
	SDL_SetRenderDrawColor(
		mRenderer,
		0,		// R
		0,		// G 
		255,	// B
		255		// A
	);

	// Clear back buffer
	SDL_RenderClear(mRenderer);

	// Draw walls
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);

	// Draw top wall
	SDL_Rect wall{
		0,			// Top left x
		0,			// Top left y
		1024,		// Width
		thickness	// Height
	};
	SDL_RenderFillRect(mRenderer, &wall);

	// Draw bottom wall
	wall.y = 768 - thickness;
	SDL_RenderFillRect(mRenderer, &wall);

	// Draw left paddle
	SDL_Rect leftPaddle{
		static_cast<int>(mLeftPaddlePos.x),
		static_cast<int>(mLeftPaddlePos.y - leftPaddleH / 2),
		thickness,
		static_cast<int>(leftPaddleH)
	};
	SDL_RenderFillRect(mRenderer, &leftPaddle);

	// Draw right paddle
	SDL_Rect rightPaddle{
		static_cast<int>(mRightPaddlePos.x),
		static_cast<int>(mRightPaddlePos.y - rightPaddleH / 2),
		thickness,
		static_cast<int>(rightPaddleH)
	};
	SDL_RenderFillRect(mRenderer, &rightPaddle);

	// Draw ball
	for (Ball ball : mBalls) {
		SDL_Rect ballRect{
			static_cast<int>(ball.pos.x - thickness / 2),
			static_cast<int>(ball.pos.y - thickness / 2),
			thickness,
			thickness
		};
		SDL_RenderFillRect(mRenderer, &ballRect);
	}
	
	// Swap front buffer and back buffer
	SDL_RenderPresent(mRenderer);
}

void Game::Shutdown()
{
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}
