/*
==================================================================================
cGame.cpp
==================================================================================
*/
#include "cGame.h"

cGame* cGame::pInstance = NULL;
static cTextureMgr* theTextureMgr = cTextureMgr::getInstance();
static cFontMgr* theFontMgr = cFontMgr::getInstance();
static cSoundMgr* theSoundMgr = cSoundMgr::getInstance();
static cButtonMgr* theButtonMgr = cButtonMgr::getInstance();


/*
=================================================================================
Constructor
=================================================================================
*/
cGame::cGame()
{

}
/*
=================================================================================
Singleton Design Pattern
=================================================================================
*/
cGame* cGame::getInstance()
{
	if (pInstance == NULL)
	{
		pInstance = new cGame();
	}
	return cGame::pInstance;
}


void cGame::initialise(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	// Get width and height of render context
	SDL_GetRendererOutputSize(theRenderer, &renderWidth, &renderHeight);
	this->m_lastTime = high_resolution_clock::now();
	// Clear the buffer with a black background
	SDL_SetRenderDrawColor(theRenderer, 0, 0, 0, 255);
	SDL_RenderPresent(theRenderer);

	theTextureMgr->setRenderer(theRenderer);
	theFontMgr->initFontLib();
	theSoundMgr->initMixer();
	theScore = 0;

	theAreaClicked = { 0, 0 };

	// Store the textures
	textureName = { "hurdle",  "bullet", "theMan", "theBackground", "theButton", "theTitleScreen", "theEndScreen" };
	texturesToUse = { "Images\\Hurdle.png", "Images\\bullet.png", "Images\\Running.png", "Images\\Track Start.png", "Images\\GameStart.png", "Images\\TitleScreen.png", "Images\\EndScreen.png" };

	for (int tCount = 0; tCount < textureName.size(); tCount++)
	{
		theTextureMgr->addTexture(textureName[tCount], texturesToUse[tCount]);
	}

	tempTextTexture = theTextureMgr->getTexture("hurdle");
	aRect = { 0, 0, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
	aColour = { 228, 213, 238, 255 };

	//Store the textures for the buttons
	btnNameList = { "start_btn", "exit_btn" };
	btnTexturesToUse = { "Images\\StartGame.png", "Images\\EndGame.png" };
	btnPos = { { 220, 575 }, { 600, 575 }, { 500, 600 } };
	for (int bCount = 0; bCount < btnNameList.size(); bCount++)
	{
		theTextureMgr->addTexture(btnNameList[bCount], btnTexturesToUse[bCount]);
	}

	for (int bCount = 0; bCount < btnNameList.size(); bCount++)
	{
		cButton * newBtn = new cButton();
		newBtn->setTexture(theTextureMgr->getTexture(btnNameList[bCount]));
		newBtn->setSpritePos(btnPos[bCount]);
		newBtn->setSpriteDimensions(theTextureMgr->getTexture(btnNameList[bCount])->getTWidth(), theTextureMgr->getTexture(btnNameList[bCount])->getTHeight());
		theButtonMgr->add(btnNameList[bCount], newBtn);
	}

	theGameState = TITLE;
	theBtnType = EXIT;

	// Create textures for Game Dialogue (text)
	fontList = { "digital", "Gameplay" };
	fontsToUse = { "Fonts/digital-7.ttf", "Fonts/Gameplay.ttf" };
	for (int fonts = 0; fonts < fontList.size(); fonts++)
	{
		theFontMgr->addFont(fontList[fonts], fontsToUse[fonts], 36);
	}
	gameTextList = { "Hurdle Game", "", "Left and right arrow keys change lanes", "OR use A and D keys to change lanes", "Hurdles" };

	theTextureMgr->addTexture("Title", theFontMgr->getFont("Gameplay")->createTextTexture(theRenderer, gameTextList[0], SOLID, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }));
	theTextureMgr->addTexture("Score", theFontMgr->getFont("Gameplay")->createTextTexture(theRenderer, gameTextList[1], SOLID, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }));
	theTextureMgr->addTexture("Instruction1", theFontMgr->getFont("Gameplay")->createTextTexture(theRenderer, gameTextList[2], SOLID, { 255, 255, 255, 0 }, { 0, 0, 0, 0 }));
	theTextureMgr->addTexture("Instruction2", theFontMgr->getFont("Gameplay")->createTextTexture(theRenderer, gameTextList[3], SOLID, { 255, 255, 255, 0 }, { 0, 0, 0, 0 }));
	theTextureMgr->addTexture("Hurdle", theFontMgr->getFont("Gameplay")->createTextTexture(theRenderer, gameTextList[4], SOLID, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }));



	// Load game sounds
	soundList = { "theme", "boing", "explosion", "beep" };
	soundTypes = { MUSIC, SFX, SFX, SFX };
	soundsToUse = { "Audio/RetroSynthGroove1.aiff", "Audio/Jump.wav", "Audio/explosion2.wav", "Audio/beep.wav"  };
	for (int sounds = 0; sounds < soundList.size(); sounds++)
	{
		theSoundMgr->add(soundList[sounds], soundsToUse[sounds], soundTypes[sounds]);
	}

	theSoundMgr->getSnd("theme")->play(-1);

	spriteTitle.setSpritePos({ 0, 0 });
	spriteTitle.setTexture(theTextureMgr->getTexture("theTitleScreen"));
	spriteTitle.setSpriteDimensions(theTextureMgr->getTexture("theTitleScreen")->getTWidth(), theTextureMgr->getTexture("theTitleScreen")->getTHeight());

	spriteBkgd.setSpritePos({ 0, 0 });
	spriteBkgd.setTexture(theTextureMgr->getTexture("theBackground"));
	spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("theBackground")->getTWidth(), theTextureMgr->getTexture("theBackground")->getTHeight());

	spriteEnd.setSpritePos({ 0, 0 });
	spriteEnd.setTexture(theTextureMgr->getTexture("theEndScreen"));
	spriteEnd.setSpriteDimensions(theTextureMgr->getTexture("theEndScreen")->getTWidth(), theTextureMgr->getTexture("theEndScreen")->getTHeight());

	
	// Create vector array of textures

	for (int astro = 0; astro <= 2; astro++)
	{

		theAsteroids.push_back(new cAsteroid);
		theAsteroids[astro]->setSpritePos({ 50 + (360 * astro), -100 + (-375 * astro) });
		theAsteroids[astro]->setSpriteTranslation({ 0, 200 });
		theAsteroids[astro]->setTexture(theTextureMgr->getTexture("hurdle"));
		theAsteroids[astro]->setSpriteDimensions(theTextureMgr->getTexture("hurdle")->getTWidth(), theTextureMgr->getTexture("hurdle")->getTHeight());
		theAsteroids[astro]->setAsteroidVelocity({ 3.0f, 3.0f });
		theAsteroids[astro]->setActive(true);
	}

	thePlayer.push_back(new cRocket);
	thePlayer[0]->setSpritePos({ 100, 500 });
	thePlayer[0]->setTexture(theTextureMgr->getTexture("theMan"));
	thePlayer[0]->setSpriteDimensions(theTextureMgr->getTexture("theMan")->getTWidth(), theTextureMgr->getTexture("theMan")->getTHeight());
	thePlayer[0]->setRocketVelocity({ 1, 1 });
	thePlayer[0]->setSpriteTranslation({ 1, 1 });

}

void cGame::run(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	loop = true;

	while (loop)
	{
		//We get the time that passed since the last frame
		double elapsedTime = this->getElapsedSeconds();

		loop = this->getInput(loop);
		this->update(elapsedTime);
		this->render(theSDLWND, theRenderer);
	}
}

void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	SDL_RenderClear(theRenderer);
	switch (theGameState)
	{
	case TITLE:
	{
		spriteTitle.render(theRenderer, NULL, NULL, spriteTitle.getSpriteScale());
		cTexture* tempTextTexture = theTextureMgr->getTexture("Instruction1");
		SDL_Rect pos = { 50, 625, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		FPoint scale = { 1, 1 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		
		theTextureMgr->addTexture("Instruction2", theFontMgr->getFont("Gameplay")->createTextTexture(theRenderer, msgScore.c_str(), SOLID, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }));
		tempTextTexture = theTextureMgr->getTexture("Instruction2");
		pos = { 50, 675, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		
		// Render Button
		theButtonMgr->getBtn("start_btn")->render(theRenderer, &theButtonMgr->getBtn("start_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("start_btn")->getSpritePos(), theButtonMgr->getBtn("start_btn")->getSpriteScale());
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());
		
	}
	break;

	case PLAYING:
	{
		SDL_RenderClear(theRenderer);
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
		// Render each asteroid in the vector array
		for (int draw = 0; draw < theAsteroids.size(); draw++)
		{
			theAsteroids[draw]->render(theRenderer, &theAsteroids[draw]->getSpriteDimensions(), &theAsteroids[draw]->getSpritePos(), theAsteroids[draw]->getSpriteRotAngle(), &theAsteroids[draw]->getSpriteCentre(), theAsteroids[draw]->getSpriteScale());
		}
		
		cTexture* tempTextTexture = theTextureMgr->getTexture("Title");
		SDL_Rect pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		FPoint scale = { 1, 1 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		// Render The Score
		theTextureMgr->addTexture("Hurdle", theFontMgr->getFont("Gameplay")->createTextTexture(theRenderer, gameTextList[4], SOLID, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }));
		tempTextTexture = theTextureMgr->getTexture("Hurdle");
		pos = { 700, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		theTextureMgr->addTexture("Score", theFontMgr->getFont("Gameplay")->createTextTexture(theRenderer, msgScore.c_str(), SOLID, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }));
		tempTextTexture = theTextureMgr->getTexture("Score");
		pos = { 900, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		// render the rocket
		thePlayer[0]->render(theRenderer, &thePlayer[0]->getSpriteDimensions(), &thePlayer[0]->getSpritePos(), thePlayer[0]->getSpriteRotAngle(), &thePlayer[0]->getSpriteCentre(), thePlayer[0]->getSpriteScale());
		SDL_RenderPresent(theRenderer);

		
		
	}
	break;
	case END:
	{
		SDL_RenderClear(theRenderer);
		spriteEnd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());

		theTextureMgr->addTexture("Score", theFontMgr->getFont("Gameplay")->createTextTexture(theRenderer, msgScore.c_str(), SOLID, { 255, 255, 0, 0 }, { 0, 0, 0, 0 }));
		tempTextTexture = theTextureMgr->getTexture("Score");
		pos = { 512, 300, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		theTextureMgr->addTexture("Hurdle", theFontMgr->getFont("Gameplay")->createTextTexture(theRenderer, gameTextList[4], SOLID, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }));
		tempTextTexture = theTextureMgr->getTexture("Hurdle");
		pos = { 425, 350, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);


		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());
	}
	break;

	case QUIT:
	{
		loop = false;
	}
	break;
	default:
		break;
	}
	SDL_RenderPresent(theRenderer);
}

void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer, double rotAngle, SDL_Point* spriteCentre)
{

	SDL_RenderPresent(theRenderer);
}

void cGame::update()
{

}

void cGame::update(double deltaTime)
{
	// Check Button clicked and change state
	if (theGameState == TITLE || theGameState == END)
	{
		theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, QUIT, theAreaClicked);
	}
	else if (theGameState == END)
	{
		theGameState = theButtonMgr->getBtn("title_btn")->update(theGameState, TITLE, theAreaClicked);
		theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, QUIT, theAreaClicked);
	}
	else
	{
		theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, TITLE, theAreaClicked);
	}
	theGameState = theButtonMgr->getBtn("start_btn")->update(theGameState, PLAYING, theAreaClicked);
	
	if (theGameState == PLAYING)
	{
		// Update the visibility and position of each asteriod
		vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin();
		while (asteroidIterator != theAsteroids.end())
		{
			if ((*asteroidIterator)->isActive() == false)
			{
				asteroidIterator = theAsteroids.erase(asteroidIterator);
			}
			else
			{
				if ((*asteroidIterator)->getSpritePos().y > 770)
				{
					theScore += 1;
					theTextureMgr->deleteTexture("Score");
					msgScore = gameTextList[1] + to_string(theScore);
					theSoundMgr->getSnd("boing")->play(0);

				}
				(*asteroidIterator)->update(deltaTime);
				++asteroidIterator;
			}
		}
		// Update the visibility and position of each bullet
		vector<cRocket*>::iterator bulletIterartor = thePlayer.begin();
		while (bulletIterartor != thePlayer.end())
		{
			if ((*bulletIterartor)->isActive() == false)
			{
				bulletIterartor = thePlayer.erase(bulletIterartor);
			}
			else
			{
				(*bulletIterartor)->update(deltaTime);
				++bulletIterartor;
			}
		}
		/*
		==============================================================
		| Check for collisions
		==============================================================
		*/
		for (vector<cRocket*>::iterator bulletIterartor = thePlayer.begin(); bulletIterartor != thePlayer.end(); ++bulletIterartor)
		{
			//(*bulletIterartor)->update(deltaTime);
			for (vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin(); asteroidIterator != theAsteroids.end(); ++asteroidIterator)
			{
				if ((*asteroidIterator)->collidedWith(&(*asteroidIterator)->getBoundingRect(), &(*bulletIterartor)->getBoundingRect()))
				{
					// if a collision set the bullet and asteroid to false
					(*asteroidIterator)->setActive(false);
					theSoundMgr->getSnd("explosion")->play(0);
					theAreaClicked = { 0, 0 };
					theGameState = END;
				}
			}
		}


		// Update the Rockets position
		thePlayer[0]->update(deltaTime);
	}
}

bool cGame::getInput(bool theLoop)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			theLoop = false;
		}

		switch (event.type)
		{
			case SDL_MOUSEBUTTONDOWN:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
				{
					theAreaClicked = { event.motion.x, event.motion.y };
					theSoundMgr->getSnd("beep")->play(0);
				}
				break;
				case SDL_BUTTON_RIGHT:
					break;
				default:
					break;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
				{
				}
				break;
				case SDL_BUTTON_RIGHT:
					break;
				default:
					break;
				}
				break;
			case SDL_MOUSEMOTION:
			break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					theLoop = false;
					break;
				
				case SDLK_RIGHT:
				{
					if (theGameState == PLAYING)
					{

						if (thePlayer[0]->getSpritePos().x < 600)
						{
							theSoundMgr->getSnd("beep")->play(0);
							thePlayer[0]->setSpriteTranslation({ 360, 0 });
						}
					}
				}
				break;
				case SDLK_d:
				{
					if (theGameState == PLAYING)
					{

						if (thePlayer[0]->getSpritePos().x < 600)
						{
							theSoundMgr->getSnd("beep")->play(0);
							thePlayer[0]->setSpriteTranslation({ 360, 0 });
						}
					}
				}
				break;

				case SDLK_LEFT:
				{
					if (theGameState == PLAYING)
					{

						if (thePlayer[0]->getSpritePos().x > 200)
						{
							theSoundMgr->getSnd("beep")->play(0);
							thePlayer[0]->setSpriteTranslation({ -360, 0 });
						}
					}
				}

				case SDLK_a:
				{
					if (theGameState == PLAYING)
					{

						if (thePlayer[0]->getSpritePos().x > 200)
						{
							theSoundMgr->getSnd("beep")->play(0);
							thePlayer[0]->setSpriteTranslation({ -360, 0 });
						}
					}
				}
				break;
				default:
					break;
				}

			default:
				break;
		}

	}
	return theLoop;
}

double cGame::getElapsedSeconds()
{
	this->m_CurrentTime = high_resolution_clock::now();
	this->deltaTime = (this->m_CurrentTime - this->m_lastTime);
	this->m_lastTime = this->m_CurrentTime;
	return deltaTime.count();
}

void cGame::cleanUp(SDL_Window* theSDLWND)
{
	// Delete our OpengL context
	SDL_GL_DeleteContext(theSDLWND);

	// Destroy the window
	SDL_DestroyWindow(theSDLWND);

	// Quit IMG system
	IMG_Quit();

	// Shutdown SDL 2
	SDL_Quit();
}

