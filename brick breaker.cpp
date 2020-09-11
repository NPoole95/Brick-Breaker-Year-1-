// GamesAssignment2.cpp: A program using the TL-Engine
//Nicholas Poole

#include <TL-Engine.h>	// TL-Engine include file and namespace
using namespace tle;

enum boxSide { leftSide, rightSide, frontSide, backSide, noSide };
boxSide SphereToBox(float pointX, float pointY, float pointZ, float sphereoldX, float sphereOldZ, float cubeXLength, float cubeYLength, float cubeZLength, float cubeXPos, float cubeYPos, float cubeZPos, float sphereRadius);

void main()
{
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine(kTLX);
	myEngine->StartWindowed();

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder("./Media");

	/**** Set up your scene here ****/
	IMesh* floorMesh;
	IMesh* skyBoxMesh;
	IMesh* marbleMesh;
	IMesh* arrowMesh;
	IMesh* blockMesh;
	IMesh* barrierMesh;
	IMesh* dummyMesh;


	IModel* floor;
	IModel* skyBox;
	IModel* marble;
	IModel* arrow;
	IModel* block[20];
	IModel* barrier[20];
	IModel* dummyMarble;
	IModel* cameraDummy;



	floorMesh = myEngine->LoadMesh("Floor.x");
	skyBoxMesh = myEngine->LoadMesh("SkyBox_Hell.x");
	marbleMesh = myEngine->LoadMesh("Marble.x");
	arrowMesh = myEngine->LoadMesh("Arrow.x");
	blockMesh = myEngine->LoadMesh("Block.x");
	barrierMesh = myEngine->LoadMesh("Barrier.x");
	dummyMesh = myEngine->LoadMesh("dummy.x");


	skyBox = skyBoxMesh->CreateModel(0, -1000, 0);
	marble = marbleMesh->CreateModel(0, 2, 0);
	dummyMarble = dummyMesh->CreateModel(0, 2, 0);
	cameraDummy = dummyMesh->CreateModel(0, 0, 60);
	arrow = arrowMesh->CreateModel(0, 2, -10);
	floor = floorMesh->CreateModel();
	floor->SetSkin("rough_mud.jpg");

	// Initializes camera and attatches it to dummy model
	ICamera* myCamera;
	myCamera = myEngine->CreateCamera(kManual);
	myCamera->AttachToParent(cameraDummy);
	myCamera->Move(0, 30, -120);
	myCamera->RotateX(10);

	const float barrierLength = 16.0f; // length (z) of barriers
	const float barrierHeight = 20.0f; // Height (Y) of barriers
	const float barrierWidth = 4.0f; //width (x) of barriers

	// speeds
	float frameTime = 0.0f;
	const float kGameSpeed = 30.0f; //Game speed constant
	float blockSpeed;
	float blockSinkSpeed;
	float kBallSpeed;
	float kRotationSpeed;

	
	float blockUnderFloor = -6.0f; // limit at which the block is fully under the floor and no longer visable
	const float kBallQty = 1.0f; //Number of balls (marbles)
	const int kBlockQty = 20; // Number of blocks 

	const float rotationMaxLimit = 50.0f;  //Maximum boundary for cannon rotation
	const float rotationMinLimit = -50.0f; //Minimum boundary for cannon rotation
	const float pi = 3.142;     // Mathmatical value for Pi
	const float marbleMaxZLimit = 200.0f; // maximum distance marble can move in z axis before game is reset
	const float marbleMinZLimit = -25.0f; // minimum distance marble can move in z axis before game is reset
	const float blockBoundary = 10.0f; //Limit blocks can reach before triggering a defeat
	float rotationLimit = 0.00f;// limit for rotation of cannon, to be incremented later
	float zMoveVector = 0.0f;          // Calculated Z movement vector of sphere
	float xMoveVector = 0.0f;          // Calculated X movement vector of sphere
	float cubeXLength = 10.0f;  // X Length (width) of target cubes
	float cubeYLength = 10.0f;  // Y Length (height) of target cubes
	float cubeZLength = 10.0f;  // Z Length (depth) of target cubes
	float sphereRadius = 2.0f;  // Radius of marble being fired
	float oldX = marble->GetX(); // old x pos of marble used in complex collision detection
	float oldZ = marble->GetZ(); // old Z pos of marble used in complex collision detection

	float barrierLeftX = -65.0F; // X position of left side barriers
	float barrierRightX = 65.0F; // X position of right side barriers
	float barrierY = 5.0F;       // Y position of barriers    
	const int kbarrierAmount = 20;      // Number of barriers 2 
	float barrierGap = 18.0f;         // Gap between barriers
	float startZ = 0.0f; // Starting Z position for barriers

	const float cameraMaxRotationLimit = 60.0f;  //maximum limit for camera forwards rotation
	const float cameraMinRotationLimit = -10.0f; //maximum limit for camera backwards rotation
	float cameraRotation = 0.0f; // camera rotation variable

	bool ballEnabled = true; // A boolean used in conjunction with a short timer to avoid the ball colliding with multiple blocks at the same time.
	const float ballTimerMax = 0.1f;
	float ballTimer = ballTimerMax;

	int blocksDestroyed = 0;  //Number of collisions
	int maxBlocksDestroyed = 20;     //Maximum number of blocks destroyed before game ends


	const EKeyCode kRotateRightKey = Key_Z; //Rotates the cannon to the right
	const EKeyCode kRotateLeftKey = Key_X;  //Rotates the cannon to the left
	const EKeyCode kQuitKey = Key_Escape;   //Closes the game
	const EKeyCode kFiringStateKey = Key_Space; //Changes the game to the firing state
	const EKeyCode kResetKey = Key_R; //Resets the balls position
	const EKeyCode kcameraForward = Key_Up; // rotates the camera forwards
	const EKeyCode kcameraBackwards = Key_Down; // rotates the camera backwards
	const EKeyCode kCameraReset = Key_C; //Resets the camera to origional position

	const float blockPosX[kBlockQty]{ -54.0f, -42.0f, -30.0f, -18.0f, -6.0f, 6.0f, 18.0f, 30.0f, 42.0f, 54.0f,-54.0f, -42.0f, -30.0f, -18.0f, -6.0f, 6.0f, 18.0f, 30.0f, 42.0f, 54.0f };     // Array containing X position of blocks
	const float blockPosY[kBlockQty]{ 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f };				   // Array containing Y position of blocks
	const float blockPosZ[kBlockQty]{ 120.0f,120.0f,120.0f,120.0f,120.0f,120.0f,120.0f,120.0f,120.0f,120.0f,132.0f,132.0f,132.0f,132.0f,132.0f,132.0f,132.0f,132.0f,132.0f,132.0f };       // Array containing Z position of blocks

	float barrierZ[kbarrierAmount]{ 18.0f, 36.0f, 54.0f, 72.0f, 90.0f, 108.0f, 126.0f, 144.0f, 162.0f, 180.0f, 18.0f, 36.0f, 54.0f, 72.0f, 90.0f, 108.0f, 126.0f, 144.0f, 162.0f, 180.0f };
	float barrierX[kbarrierAmount]{ 65.0f, 65.0f ,65.0f ,65.0f ,65.0f ,65.0f, 65.0f ,65.0f ,65.0f ,65.0f ,-65.0f ,-65.0f ,-65.0f ,-65.0f ,-65.0f, -65.0f ,-65.0f ,-65.0f ,-65.0f ,-65.0f };

	//bool blockHitIndicator[kBlockQty]{ false,false, false, false, false, false, false, false, false, false };      // Array containing bools indicating if a block has been hit


	enum blockState { Healthy, Damaged, Destroyed }; //Game states of blocks
	blockState currentBlockState[kBlockQty] = { Healthy,Healthy,Healthy,Healthy,Healthy,Healthy,Healthy,Healthy,Healthy,Healthy }; // initializes block health states
	enum gameState { Ready, Firing, Contact, Over }; //gamestates controlling cannon and marble
	gameState currentGameState = Ready;  //Initializes the cannon ready to be fired
	arrow->AttachToParent(dummyMarble);   // Attatches arrow to marble to show cannon direction



	for (int i = 0; i < kBlockQty; i++)          // For loop to generate blocks using array positions for X,Y,Z coords
	{
		block[i] = blockMesh->CreateModel(blockPosX[i], blockPosY[i], blockPosZ[i]);
		block[i]->SetSkin("glass_blue.jpg");

	}

	for (int i = 0; i < kbarrierAmount; i++)   // For loop to generate barriers 
	{

		barrier[i] = barrierMesh->CreateModel(barrierX[i], barrierY, barrierZ[i]);// generates x,y,z coords for barriers (left x pos, barrier Y pos, startZ (multiplied by number of blocks and adds barrier gap)


		if (barrierZ[i] < 100.0f) // checks position of barriers, making 4 furthest barriers appear with yellow and black stripes
		{
			barrier[i]->SetSkin("barrier1a.bmp");
		}

	}



	frameTime = myEngine->Timer();
	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// update timers
		frameTime = myEngine->Timer();
		blockSpeed = -0.1 * kGameSpeed * frameTime; //Speed at which blocks move down the screen
		blockSinkSpeed = -kGameSpeed * frameTime; // speed at which blocks sink after being destroyed
		kBallSpeed = 4 * kGameSpeed * frameTime; //Speed at which the ball(marble) moves
		kRotationSpeed = kGameSpeed * frameTime; // Speed at which toe cannon rotates

		ballTimer -= frameTime;
		if (ballTimer <= 0.0f) // set the ball to be enabled again if the timer has expired
		{
			ballEnabled = true;
		}
		else if (ballEnabled == false)
		{
			int i = 1;
		}

		// Draw the scene
		myEngine->DrawScene();

		/**** Update your scene each frame here ****/

		//Collisions
		for (int i = 0; i < kBlockQty; i++)
		{
			if (currentBlockState[i] != Destroyed && currentGameState != Over)  // checks to make sure collisions are not tested on already destroyed blocks
			{
				if (block[i]->GetZ() <= blockBoundary) //Checks to see if the blocks have reached the cannon, causing a defeat
				{
					currentGameState = Over;
					marble->SetSkin("glass_Red.jpg"); //Sets marble to red to show defeat and end of game
					blockSpeed = 0.0f;

				}
				else
				{
					block[i]->MoveZ(blockSpeed); // Causes the blocks to slowly move towards the cannon. ignoring blocks that have been destroyed.
				}
				if (ballEnabled)
				{
					boxSide collision = SphereToBox(marble->GetX(), marble->GetY(), marble->GetZ(), oldX, oldZ, cubeXLength, cubeYLength, cubeZLength, block[i]->GetX(), block[i]->GetY(), block[i]->GetZ(), sphereRadius);
					if (collision == frontSide || collision == backSide)
					{
						zMoveVector = -zMoveVector; //causes the marble to rebound off of blocks 

						if (currentBlockState[i] == Healthy)
						{
							currentBlockState[i] = Damaged;
							block[i]->SetSkin("glass_red.jpg"); // Sets the block to red to show it is damaged

						}
						else if (currentBlockState[i] == Damaged)
						{

							currentBlockState[i] = Destroyed;
							blocksDestroyed++; //increases counter for no. of blocks destroyed

						}
						// disable the ball after a collision to stop multiple boxes being hit at once
						ballEnabled = false;
						ballTimer = ballTimerMax;
					}
					else if (collision == leftSide || collision == rightSide)
					{
						xMoveVector = -xMoveVector; //causes the marble to rebound off of blocks 

						if (currentBlockState[i] == Healthy)
						{
							currentBlockState[i] = Damaged;
							block[i]->SetSkin("glass_red.jpg"); // Sets the block to red to show it is damaged

						}
						else if (currentBlockState[i] == Damaged)
						{
							currentBlockState[i] = Destroyed;
							blocksDestroyed++; //increases counter for no. of blocks destroyed
						}
						// disable the ball after a collision to stop multiple boxes being hit at once
						ballEnabled = false;
						ballTimer = ballTimerMax;
					}
				}
			}
			else if (currentBlockState[i] == Destroyed)
			{
				if (block[i]->GetY() >= blockUnderFloor) //checks if block is not under the floor
				{
					block[i]->MoveY(blockSinkSpeed); // moves the block slowly into the floor
				}
			}
		}

		oldX = marble->GetX(); //sets marbles current x pos for use in collisions
		oldZ = marble->GetZ(); //sets marbles current x pos for use in collisions

		for (int i = 0; i < kbarrierAmount; i++)
		{
			if (SphereToBox(marble->GetX(), marble->GetY(), marble->GetZ(), oldX, oldZ, barrierWidth, barrierHeight, barrierLength, barrierX[i], barrierY, barrierZ[i], sphereRadius)) // calculates sphere to box collisions for barriers
			{
				xMoveVector = -xMoveVector; // causes the x movement vector to be reversed, making the marble bounce off of the barrier

			}
		}

		if (myEngine->KeyHeld(kcameraForward))
		{
			if (cameraRotation < cameraMaxRotationLimit) //Tests limit of camera rotation to keep it within boundaries
			{
				cameraDummy->RotateX(kRotationSpeed); //rotates camera forwards
				cameraRotation += kRotationSpeed;     //increments rotation counter
			}
		}
		if (myEngine->KeyHeld(kcameraBackwards))
		{
			if (cameraRotation > cameraMinRotationLimit) //Tests limit of camera rotation to keep it within boundaries
			{
				cameraDummy->RotateX(-kRotationSpeed); //rotates camera forwards
				cameraRotation -= kRotationSpeed;	   //decrements camera rotation counter
			}
		}
		if (myEngine->KeyHit(kCameraReset))
		{
			cameraDummy->ResetOrientation();   //Resets camera angle to origional position
			cameraRotation = 0;                //Resets camera rotation variable to 0

		}

		if (blocksDestroyed == maxBlocksDestroyed)
		{
			currentGameState = Over;  // Ends game once all blocks are destroyed (player Victory)
			marble->SetSkin("glass_green.jpg"); //Sets marble to green to show victory and end of game
		}


		if (currentGameState == Ready)
		{
			marble->SetX(0.0f);
			marble->SetZ(0.0f); //Resets marble position
			if (myEngine->KeyHeld(kRotateRightKey))
			{
				if (rotationLimit < rotationMaxLimit) // Tests limit of cannon rotation to keep it within boundaries
				{
					marble->RotateLocalY(kRotationSpeed); //Turns the marble to the right
					dummyMarble->RotateLocalY(kRotationSpeed); //rotates arrow and marble dummy to the right
					rotationLimit += kRotationSpeed; //increments cannon rotation limit
				}

			}
			if (myEngine->KeyHeld(kRotateLeftKey))
			{
				if (rotationLimit > rotationMinLimit) // Tests limit of cannon rotation to keep it within boundaries
				{
					marble->RotateLocalY(-kRotationSpeed); //Turns the marble to the left
					dummyMarble->RotateLocalY(-kRotationSpeed); //rotates arrow and marble dummy to the left
					rotationLimit -= kRotationSpeed; //decrements cannon rotation limit
				}
			}
			if (myEngine->KeyHit(kFiringStateKey))
			{
				currentGameState = Firing; //Changes the game to the firing state
				zMoveVector = cos(rotationLimit * pi / 180); //calculates marble Z movement vector
				xMoveVector = sin(rotationLimit * pi / 180); //calculates marble X movement vector
				
			}

		}

		if (currentGameState == Firing)
		{
			marble->Move(xMoveVector * kBallSpeed, 0.0f, zMoveVector * kBallSpeed); //Starts marble movement
			if (myEngine->KeyHeld(kRotateRightKey))
			{
				if (rotationLimit < rotationMaxLimit) // Tests limit of cannon rotation to keep it within boundaries
				{
					marble->RotateLocalY(kRotationSpeed); //Turns the cannon to the right
					dummyMarble->RotateLocalY(kRotationSpeed); //rotates arrow and marble dummy to the right
					rotationLimit += kRotationSpeed; //increments cannon rotation limit
				}

			}
			if (myEngine->KeyHeld(kRotateLeftKey))
			{
				if (rotationLimit > rotationMinLimit) // Tests limit of cannon rotation to keep it within boundaries
				{
					marble->RotateLocalY(-kRotationSpeed); //Turns the cannon to the left
					dummyMarble->RotateLocalY(-kRotationSpeed); //rotates arrow and marble dummy to the left
					rotationLimit -= kRotationSpeed; //decrements cannon rotation limit
				}
			}
		}

		if (currentGameState == Contact)
		{

		}
		if (currentGameState == Over)
		{

		}
		if (myEngine->KeyHit(kQuitKey))
		{
			myEngine->Stop(); // stops the game and ends the game loop.
		}
		if (marble->GetZ() > marbleMaxZLimit) // resets marble position when marble exceeds max Z limit
		{
			currentGameState = Ready;
		}
		if (marble->GetZ() < marbleMinZLimit)   // resets marble position when marble exceeds min Z limit
		{
			currentGameState = Ready;
		}

		if (currentGameState != Over)
		{

			if (myEngine->KeyHit(kResetKey))
			{
				currentGameState = Ready;
			}
		}

	}


	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}

boxSide SphereToBox(float pointX, float pointY, float pointZ, float sphereoldX, float sphereOldZ, float cubeXLength, float cubeYLength, float cubeZLength, float cubeXPos, float cubeYPos, float cubeZPos, float sphereRadius)
{
	float minX = cubeXPos - (cubeXLength / 2.0f);
	float maxX = cubeXPos + (cubeXLength / 2.0f);
	float minY = cubeYPos - (cubeYLength / 2.0f);
	float maxY = cubeYPos + (cubeYLength / 2.0f);
	float minZ = cubeZPos - (cubeZLength / 2.0f);
	float maxZ = cubeZPos + (cubeZLength / 2.0f);

	boxSide result = noSide;

	if ((pointX > minX - sphereRadius && pointX < maxX + sphereRadius) &&
		(pointY > minY - sphereRadius && pointY < maxY + sphereRadius) &&
		(pointZ > minZ - sphereRadius && pointZ < maxZ + sphereRadius))

	{
		if (sphereoldX < minX) result = leftSide;
		else if (sphereoldX > maxX) result = rightSide;
		else if (sphereOldZ < minZ) result = frontSide;
		else if (sphereOldZ > maxZ)  result = backSide;
	}

	return (result);

}