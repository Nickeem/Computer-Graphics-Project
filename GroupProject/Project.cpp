﻿/* 2023 Computer Graphics Project
* Contributors:
FAITH BROWNE
NICKEEM PAYNE-DEACON
CRYSTAL CHOW 
ROMAINE CARTER
TYRESE MAYNARD 
JOSHUA CODRINGTON
*/

/* 
*********Installed******** :
Install-Package glew_dynamic
Install-Package glfw
Install-Package GLMathematics
Install-Package freeimage -version 3.16.0
Install-Package nupengl.core
Install-Package Soil
Install-Package Assimp -version 3.0.0
*/

/*
Instructions:
Move camera with arrow keys
Mouse can be used to change what you are looking at
Pressing H returns the camera to the initial position

To move gun:
    Press I - to move up
    Press K - to move down
    Press J - to move left
    Press L - to move right
To rotate gun
    Press A - to rotate left
    Press D - to rotate right
To change size of gun
    Press 1 - to make gun smaller
    Press 2 - to make gun bigger
To stop Rain
    Press SPACE
Press ESC to close program

*/

#include <GL/glew.h>  //include glew
#include <GLFW/glfw3.h> //include glfw 


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/* REFERENCES GO HERE:
1. The skull obj- https://www.turbosquid.com/3d-models/free-skull-3d-model/474120#
*/

//ANY INCLUDES GO HERE 
#include "shader.h";
#include "model.h";
#include "camera.h";



GLFWwindow* window;

// properties
GLuint screenWidth = 1500, screenHeight = 900;


//Initial location of camera
glm::vec3 camLocation(0.0f, 0.0f, 1500.0f);

// Camera
Camera camera(camLocation);
float lastX = screenWidth / 2.0f;
float lastY = screenHeight / 2.0f;
bool mouseMoved = true;

float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;


// gun size 
GLfloat gunSize = 60.0f;
const GLfloat GUN_SIZE_CHANGE_FACTOR = 0.05f;
//  gun position
GLfloat gunPositionZ = 0.1f;
GLfloat gunPositionX = 0.0f;
GLfloat gunPositionY = 0.0f;
// gun angle
GLfloat gunAngle = -0.9f;
const GLfloat GUN_ANGLE_CHANGE_FACTOR = 0.008;
// gun movement
const GLfloat GUN_MOVEMENT_CHANGE = 0.05f; // change how much the gun moves (X,Y) when keys are pressed



// properties of rain
const GLfloat RAIN_HEIGHT = 1000.0f;
const GLfloat RAIN_SURFACE = -1000.f;
const int MAX_RAIN_WIDTH = 4000;
const int MAX_RAIN_Z = 4000;
const int MAX_RAIN_SPEED = 10000;
const unsigned int RAIN_DROPS = 1000;
GLboolean stopRain = false;

GLfloat skullAngle = -0.9f;


void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int modes);
void inputCallback(GLFWwindow* window);
// void mouseClickedCallback(GLFWwindow* window, int button, int  action, int mode);
void moveMouseCallback(GLFWwindow* window, double xpos, double ypos);
void clickDragCallback(GLFWwindow* window, int button, int  action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void windowSize_callback(GLFWwindow* window, int width, int height);

// misc functions
unsigned int loadCubemap(vector<std::string>);


void init_Resources()
{
    // Initialize the resources - set window, etc.
    if (!glfwInit())
    {
        cout << "\nFailed to Initialize GLFW...";
        exit(EXIT_FAILURE);
    } 

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


    //Create the window 
    window = glfwCreateWindow(screenWidth, screenHeight, "COMP3420 Project - GROUP 5", 0, 0);

    //If window fails creation, then shut down the display window 
    if (!window)
    {
        cout << "\nFailed to open display window...";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    //if succeded then make display active 
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;

    //initialize GLEW 
    if (glewInit() != GLEW_OK)
    {
        cout << "Failed to initialize GLEW";
        exit(EXIT_FAILURE);
    }

    //----------------------------------------------------
    // Registering the call-back function for the keyboard
    //----------------------------------------------------
    //glfwSetKeyCallback(window, keyboardCallback);

    //----------------------------------------------------
        // Registering the call-back functions for the mouse
        //----------------------------------------------------
    // glfwSetMouseButtonCallback(window, mouseClickedCallback);
    glfwSetKeyCallback(window, keyboardCallback);
    glfwSetMouseButtonCallback(window, clickDragCallback);
    glfwSetCursorPosCallback(window, moveMouseCallback);
    glfwSetScrollCallback(window, scroll_callback);

    // Options - Hide the mouse pointer
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetWindowSizeCallback(window, windowSize_callback);

    // Setup OpenGL options
    glEnable(GL_DEPTH_TEST);


}
//end of initresources 


//Main Function 
int main()
{
    srand(static_cast<unsigned int>(glfwGetTime())); // initialize random seed
    init_Resources();

    

    glm::mat4 View;

    //Compile object shaders 
    Shader objshader("modelverte.glsl","modelfrag.glsl");
    Shader rainShader("shaders/rain.vs", "shaders/rain.frag");

    // Shaders for Background
    Shader shader("cubemaps.vs", "cubemaps.frag");
    Shader matrixbgShader("skybox.vs", "skybox.frag");

    GLuint viewID = glGetUniformLocation(objshader.Program, "view");
    GLuint viewID_rain = glGetUniformLocation(rainShader.Program, "view");


    //load the obj files
    Model gun((GLchar*)"assets/fi-ex.obj"); // gun model
    Model Rain((GLchar*)"assets/rain.obj"); // rain model

    
    //rain model
    glm::mat4* rainModelMatrices;
    rainModelMatrices = new glm::mat4[RAIN_DROPS];
    GLfloat rainPositions[RAIN_DROPS][3]; // two dimentional array that stores x,y coordinates of rain drops
    GLfloat rainSpeeds[RAIN_DROPS]; // array to store speed of each rain drop


    for (unsigned int i = 0; i < RAIN_DROPS; i++) {
        GLfloat x = (float)(rand() % MAX_RAIN_WIDTH);
        GLfloat z = (float)(rand() % MAX_RAIN_Z);
        GLfloat speed = (float)(rand() % MAX_RAIN_SPEED) / 10000;
        if (rand() % 2 == 0)
            x *= -1; // 50% chance of making number negative
        if (rand() % 2 == 0)
            z *= -1; // 50% chance of making number negative
        cout << "x: " << x << endl; // check range of x values
        rainPositions[i][0] = x; // assign random x value of rain drop
        rainPositions[i][1] = RAIN_HEIGHT; // assign y position of raindrop 
        rainPositions[i][2] = z; // assign z position of raindrop 
        rainSpeeds[i] = speed; // randomly assign speed of rain to rainDrop at position i
    }


    

    


    // vertex data for cubemap
    float cubeVertices[] = {
        // positions          // texture Coords
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    //vertex data for Matrix Background
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    // cube Vertex Array Object
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    // skybox Vertex Array Object
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // matrix background faces  using images
    vector<std::string> faces
    {
        "assets/matrix.jpg",
        "assets/matrix.jpg",
        "assets/matrix.jpg",
        "assets/matrix.jpg",
        "assets/matrix.jpg",
        "assets/matrix.jpg"
    };
    unsigned int cubemapTexture = loadCubemap(faces);

    matrixbgShader.Use();
    matrixbgShader.setInt("skybox", 0);
    

    //Keep displaying the window until we have shut it down
    while (!glfwWindowShouldClose(window))
    {
        inputCallback(window);

        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Clear buffers
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // =======================================================================
        //  Set the projection matrix
        // =======================================================================
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), // Field of Vision
            (GLfloat)screenWidth / (GLfloat)screenHeight, // Aspect Ratio
            1.0f,
            10000.0f // camera position
        );

        objshader.Use();
        objshader.setMat4("projection", projection);

        rainShader.Use();
        rainShader.setMat4("projection", projection);

        // =======================================================================
        // Step 4. create the View matrix
        // =======================================================================
        View = camera.GetViewMatrix();

        objshader.Use();
        objshader.setMat4("view", View);
       // glUniformMatrix4fv(glGetUniformLocation(objshader.Program, "view"), 1,
         //   GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));

        rainShader.Use();
        rainShader.setMat4("view", View);
        //glUniformMatrix4fv(glGetUniformLocation(rainShader.Program, "view"), 1,
          //  GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));
        

        // =======================================================================
        // Create the model matrix  for gun
        // =======================================================================
        
        //display shader
        objshader.Use();
        glm::mat4 Model = glm::mat4(1);

        //Modify the model matrix with scaling, translation, rotation, etc
        Model = glm::scale(Model, glm::vec3(gunSize));
        // translate gun further away from camera
        Model = glm::translate(Model, glm::vec3(gunPositionX, gunPositionY, gunPositionZ));
        gunAngle += 0.0001;
        if (gunAngle > 360) gunAngle = 0.001;
        Model = glm::rotate(Model, gunAngle, glm::vec3(0.0f, 1.0f, 0.0f));

        // =======================================================================
        // Pass the Model matrix,  to the gun shader as "model"
        // =======================================================================
        objshader.setMat4("model", Model);
        gun.Draw(objshader);



       // TEST FOR EDGE DETECTION

       /*
       //if gun touch the side 
    lastX+=gunPositionX; 
    if(lastX > 750 || lastX <-750) 
    gunPositionX *= -1; 

    lastY+=gunPoisitionZ; 
    if(lastY >750 || last Y <-750)
    gunPositionX*= -1; 

    //test for if rain touches gun
//for loop for rain drops 
for (unsigned int i=0; i <RAIN_DROPS;i++)
{
    xdif = gunPosittionZ - rainPositions[i];
    ydif = gunPositionX - rainrainPositions[i];
        if ( sqrt((xdif*xdif) + (ydif*ydif)) < 750)    //if rain and gun touch then
            {
               ??
            }
}



    //increment location of gun 
    lastX += gunPositionX; 
    lastY += gunPositionZ; 
    
    
       
*/


        // =======================================================================
        // Create the model matrix  for rain including physics
        // =======================================================================
        int changeY;
        rainShader.Use();
        for (unsigned int i = 0; i < RAIN_DROPS; i++) {
            glm::mat4 model = glm::mat4(1.0f);

            
            if (!stopRain) {
                rainPositions[i][1] -= rainSpeeds[i]; // make rain fall to surface
                if (rainPositions[i][1] < RAIN_SURFACE || rainPositions[i][1] > RAIN_HEIGHT)
                    rainSpeeds[i] *= -1;
            }

            model = glm::translate(model, glm::vec3(rainPositions[i][0], rainPositions[i][1], rainPositions[i][2]));
            model = glm::scale(model, glm::vec3(100.f, 10.f, 100.f));
            // add list of matrices
            rainModelMatrices[i] = model;
        }

        // configure instanced array
        // -------------------------
        unsigned int buffer;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, RAIN_DROPS * sizeof(glm::mat4), &rainModelMatrices[0], GL_STATIC_DRAW);

        for (unsigned int i = 0; i < Rain.meshes.size(); i++)
        {
            unsigned int VAO = Rain.meshes[i].VAO;
            glBindVertexArray(VAO);
            // set attribute pointers for matrix (4 times vec4)
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
            glEnableVertexAttribArray(5);
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
            glEnableVertexAttribArray(6);
            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

            glVertexAttribDivisor(3, 1);
            glVertexAttribDivisor(4, 1);
            glVertexAttribDivisor(5, 1);
            glVertexAttribDivisor(6, 1);

            glBindVertexArray(0);
        }


        /*glm::mat4 rainModel = glm::mat4(1);
        rainModel = glm::scale(rainModel, glm::vec3(100.f));
        rainHeight -= rainSpeed;
        if (rainHeight < -1.0f)
            rainHeight = RAINHeight;
        rainModel = glm::translate(rainModel, glm::vec3(0.0f, rainHeight, rainPositionZ));
        rainShader.setMat4("model", rainModel);
        Rain.Draw(rainShader);*/
        for (unsigned int i = 0; i < Rain.meshes.size(); i++)
        {
            glBindVertexArray(Rain.meshes[i].VAO);
            glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(Rain.meshes[i].indices.size()), GL_UNSIGNED_INT, 0, RAIN_DROPS);
            glBindVertexArray(0);
        }



        // draw matrix background as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        matrixbgShader.Use();
        View = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        matrixbgShader.setMat4("view", View);
        matrixbgShader.setMat4("projection", projection);
        // matrix bg cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

       

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    //Release ALL the other resources
    glfwTerminate();
    exit(EXIT_SUCCESS);
}




void inputCallback(GLFWwindow* window)
{
    // If ESC is pressed, close the window
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        cout << "ESC Key Pressed\n";
        glfwSetWindowShouldClose(window, GL_TRUE);
        return;
    }

    if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
    {
        cout << "HOME/H Key Pressed\n";
        camera = glm::vec3(0.0f, 0.0f, 1500.0f);
    }
    
    // move camera
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        cout << "UP Arrow Key Pressed\n";
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        cout << "DOWN Arrow Key Pressed\n";
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        cout << "LEFT Arrow Key Pressed\n";
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        cout << "RIGHT Arrow Key Pressed\n";
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }

    // Rotate Object
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cout << "A Key Pressed\n";
        gunAngle -= GUN_ANGLE_CHANGE_FACTOR;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        cout << "D Key Pressed\n";
        gunAngle += GUN_ANGLE_CHANGE_FACTOR;
    }
    // move Object
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        gunPositionX -= GUN_MOVEMENT_CHANGE;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        gunPositionX += GUN_MOVEMENT_CHANGE;
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        gunPositionY += GUN_MOVEMENT_CHANGE;
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        gunPositionY -= GUN_MOVEMENT_CHANGE;

    // change size of object
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        gunSize -= GUN_SIZE_CHANGE_FACTOR;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        gunSize += GUN_SIZE_CHANGE_FACTOR;  
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int modes)
{
    // stop or start rain
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        stopRain = !stopRain;
    }

}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}


void clickDragCallback(GLFWwindow* window, int button, int  action, int mode)
{
    static double startX = 0, startY = 0, endX = 0, endY = 0;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        cout << "Left Button Clicked\n";
        // Make sure that the button is held down
        glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GL_TRUE);

        // Gets starting X & Y positions of mouse pointer 
        glfwGetCursorPos(window, &startX, &startY);

        cout << "\nBegin Dragging Mouse... ";

    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        cout << "\nEnd Draging Mouse...\n";

        // Gets ending X & Y positions of mouse pointer 
        glfwGetCursorPos(window, &endX, &endY);

        cout << "\nMouse has moved from X : " << startX << " to " << endX;
        cout << "\nMouse has moved from Y : " << startY << " to " << endY << "\n";
    }


    if (button == GLFW_MOUSE_BUTTON_RIGHT &&
        action == GLFW_PRESS)
        cout << "\nRight Button Clicked\n";

    if (button == GLFW_MOUSE_BUTTON_MIDDLE &&
        action == GLFW_PRESS)
        cout << "\nMiddle Button Clicked\n";
}

void moveMouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    //if (mouseHold) {

        if (mouseMoved)
        {

            lastX = xpos;
            lastY = ypos;
            mouseMoved = false;
        }

        GLfloat xoffset = xpos - lastX;
        GLfloat yoffset = lastY - ypos;

        lastX = xpos;
        lastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);
    //}
    
}

void windowSize_callback(GLFWwindow* window, int width,int height)
{
    glViewport(0, 0, width, height);
}

// misc
unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = SOIL_load_image(faces[i].c_str(), &width, &height, 0, SOIL_LOAD_RGB);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
