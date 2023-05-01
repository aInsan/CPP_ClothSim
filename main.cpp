#include "raylib.h" // Graphics library to display sim
#include "raymath.h" // Math for raylib e.g. Vector2 arithmetic
#include <iostream> // For debugging

using namespace std;

// For display
const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1000;

// Define constants for later
const int CLOTH_WIDTH = 30; // number of points in grid
const int CLOTH_HEIGHT = CLOTH_WIDTH;
const float CLOTH_SIZE = SCREEN_HEIGHT / CLOTH_WIDTH; // Makes sure the cloth fits and is large on screen no matter the dimesions
const float GRAVITY = 0.5f; // to pull points down

// Point class with a position vector, veloctiy vector and 2 properties for interactions with other points
class Point
{
public:
    Vector2 position;
    Vector2 velocity;
    bool isAnchored;
    bool isRemoved;
};
// Cloth Initialization function
void InitCloth(Point cloth[][CLOTH_WIDTH])
{
    // Iterate through all points in the cloth array
    for (int i = 0; i < CLOTH_WIDTH; i++)
    {
        for (int j = 0; j < CLOTH_HEIGHT; j++)
        {
            // For each point place it at a certain point 
            cloth[i][j].position = { CLOTH_SIZE * i + 150, CLOTH_SIZE * j };
            cloth[i][j].velocity = { 0.0f, 0.0f }; // and set its veloctity vector to zero
            cloth[i][j].isAnchored = false; //Make all points not anchored and not removed
            cloth[i][j].isRemoved = false;
        }
    }
    // Set top-left, top-middle, and top-right points to be an anchor
    cloth[0][0].isAnchored = true;
    cloth[CLOTH_WIDTH / 2][0].isAnchored = true;
    cloth[CLOTH_WIDTH - 1][0].isAnchored = true;
}

// Function that makes sure the points do not stretch furthur than they can
void ApplyConstraints(Point cloth[][CLOTH_WIDTH])
{
    // Max length for each point is the CLOTH_SIZE
    const float restLength = CLOTH_SIZE;

    // Apply constraints along horizontal and vertical directions
    for (int i = 0; i < CLOTH_WIDTH; i++)
    {
        for (int j = 0; j < CLOTH_HEIGHT; j++)
        {
            // Pick a point
            Point& p1 = cloth[i][j];

            // If not a point at the edge
            if (i < CLOTH_WIDTH - 1)
            {
                //Get the point to the right
                Point& p2 = cloth[i + 1][j];
                // Find the difference in their position
                Vector2 delta = Vector2Subtract(p2.position, p1.position);
                float distance = Vector2Length(delta);

                //if that difference is longer than restLength, and both points are not removed
                if ((distance > restLength) && (!p1.isRemoved && !p2.isRemoved))
                {
                    //If one of the two points is an anchor only move the other to compensate
                    if (p1.isAnchored)
                    {
                        Vector2 correction = Vector2Scale(delta, (distance - restLength) / distance);
                        p2.position = Vector2Subtract(p2.position, correction);
                    }
                    else if (p2.isAnchored)
                    {
                        Vector2 correction = Vector2Scale(delta, (distance - restLength) / distance);
                        p1.position = Vector2Add(p1.position, correction);
                    }
                    else
                    {
                        //Otherwise move both points so their distance stays within bounds
                        Vector2 correction = Vector2Scale(delta, (distance - restLength) / (2 * distance));
                        p1.position = Vector2Add(p1.position, correction);
                        p2.position = Vector2Subtract(p2.position, correction);
                    }
                }
            }

            // Same as before but for p2 in under p1 instead of to the right
            if (j < CLOTH_HEIGHT - 1)
            {
                Point& p2 = cloth[i][j + 1];
                Vector2 delta = Vector2Subtract(p2.position, p1.position);
                float distance = Vector2Length(delta);
                if ((distance > restLength) && (!p1.isRemoved && !p2.isRemoved))
                {
                    if (p1.isAnchored)
                    {
                        Vector2 correction = Vector2Scale(delta, (distance - restLength) / distance);
                        p2.position = Vector2Subtract(p2.position, correction);
                    }
                    else if (p2.isAnchored)
                    {
                        Vector2 correction = Vector2Scale(delta, (distance - restLength) / distance);
                        p1.position = Vector2Add(p1.position, correction);
                    }
                    else
                    {
                        Vector2 correction = Vector2Scale(delta, (distance - restLength) / (2 * distance));
                        p1.position = Vector2Add(p1.position, correction);
                        p2.position = Vector2Subtract(p2.position, correction);
                    }
                }
            }
        }
    }
}

// Function that updates the points' properties
// Has the deltaTime parameter that it uses to always run the program at the same speed
void UpdateCloth(float deltaTime, Point cloth[][CLOTH_WIDTH])
{
    for (int i = 0; i < CLOTH_WIDTH; i++)
    {
        for (int j = 0; j < CLOTH_HEIGHT; j++)
        {
            //If the point is not anchored or off screen
            if (!cloth[i][j].isAnchored && cloth[i][j].position.y < SCREEN_HEIGHT)
            {
                //If left-click is held
                if (IsMouseButtonDown(0))
                {
                    // Check if the mouse is very close to any points
                    if (CheckCollisionPointCircle(GetMousePosition(), cloth[i][j].position, CLOTH_SIZE * 2 / 5 /*small radius around point*/))
                    {
                        //if so, remove that point (this will simulate tears)
                        cloth[i][j].isRemoved = true;
                    }
                }
                //Apply the constraints to the points 
                // Run the ApplyConstraints function line-34
                ApplyConstraints(cloth);
                // Apply gravity
                cloth[i][j].velocity = Vector2Add(cloth[i][j].velocity, {0.0f, GRAVITY});
                //Move the points based on their velocities
                cloth[i][j].position = Vector2Add(cloth[i][j].position, Vector2Scale(cloth[i][j].velocity, deltaTime));
            }
        }
    }
}

//Function to display the simulation
void DrawCloth(Point cloth[][CLOTH_WIDTH])
{
    for (int i = 0; i < CLOTH_WIDTH - 1; i++)
    {
        for (int j = 0; j < CLOTH_HEIGHT - 1; j++)
        {
            //For each point in cloth,
                //find 3 more points forming a square, 
            Vector2 p1 = cloth[i][j].position;
            Vector2 p2 = cloth[i + 1][j].position;
            Vector2 p3 = cloth[i + 1][j + 1].position;
            Vector2 p4 = cloth[i][j + 1].position;
            // Draw a small circle at each point
            if (!cloth[i][j].isRemoved) { DrawCircleV(p1, 2, BLUE); }
            // Since the for loop never reaches the edge, if this point is the last in the for loop, draw a circle at the ajacent point
            if ((i == CLOTH_WIDTH - 2) && (j == CLOTH_HEIGHT - 2) && !cloth[i + 1][j + 1].isRemoved) { DrawCircleV(p3, 2, BLUE); }
            if (i == CLOTH_WIDTH - 2 && !cloth[i + 1][j].isRemoved) { DrawCircleV(p2, 2, BLUE); }
            if (j == CLOTH_HEIGHT - 2 && !cloth[i][j + 1].isRemoved) { DrawCircleV(p4, 2, BLUE); }
            // Draw the wire frame for the square, forming a grid if any points are not removed
            if (!cloth[i][j].isRemoved && !cloth[i + 1][j].isRemoved)
                DrawLineV(p1, p2, BLACK);
            if (!cloth[i + 1][j].isRemoved && !cloth[i + 1][j + 1].isRemoved)
                DrawLineV(p2, p3, BLACK);
            if (!cloth[i + 1][j + 1].isRemoved && !cloth[i][j + 1].isRemoved)
                DrawLineV(p3, p4, BLACK);
            if (!cloth[i][j + 1].isRemoved && !cloth[i][j].isRemoved)
                DrawLineV(p4, p1, BLACK);
        }
    }
        //Draw a red circle at the anchor points
        DrawCircleV(cloth[0][0].position, 5.0f, RED);
        DrawCircleV(cloth[CLOTH_WIDTH / 2][0].position, 5.0f, RED);
        DrawCircleV(cloth[CLOTH_WIDTH - 1][0].position, 5.0f, RED);

}

int main()
{
    // Creating a 2d array of points of dimensions CLOTH_WIDTH x CLOTH_HEIGHT
    Point cloth[CLOTH_WIDTH][CLOTH_HEIGHT];
    // Empty Vector2 array that will store the anchor points
    Vector2 anchorPoints[3];
    //Set up the window using raylib
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Cloth Simulation");
    //Set a FPS cap
    SetTargetFPS(144);

    // Initialize the cloth line-34
    InitCloth(cloth);

    //While the eqc key isn't presses and the x button isn't clicked draw the simulation
    while (!WindowShouldClose())
    {
        //If f5 is pressed reset the simulation
        if (IsKeyPressed(KEY_R)) {
            InitCloth(cloth); // Initialize function line-34
        }
        if (IsKeyPressed(KEY_LEFT_SHIFT)) {
            ToggleFullscreen();
        }
        //get the framtime to insure the physics does not run at different speeds at different FPS
        float deltaTime = GetFrameTime();

        //Update the cloth sim line-135
        UpdateCloth(deltaTime, cloth);

        //raylib function to setup drawing to the screen
        BeginDrawing();

        //every frame set the screen to white so you can't see last frame
        ClearBackground(RAYWHITE);

        //FPS COUNTER
        DrawText(TextFormat("FPS: %i", (int)round(1.0f / deltaTime)), 10, 10, 20, BLACK);
        //Tell users how to reset the simulation
        DrawText("'R' to reset simulation.", (SCREEN_WIDTH / 2) + 180, SCREEN_HEIGHT / 2, 60, Color{ 25, 25, 25,25 });

        // Draw the cloth line-169
        DrawCloth(cloth);

        //raylib function that marks the end of the drawing to screen
        EndDrawing();
    }

    // Once esc is presses or X is clicked, close the game window
    CloseWindow();

    // Exit with success code 0
    return 0;
}