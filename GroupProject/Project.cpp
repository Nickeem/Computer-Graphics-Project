﻿/* 2023 Computer Graphics Project
* Contributors:
FAITH BROWNE
NICKEEM PAYNE-DEACON
CRYSTAL CHOW 
ROMAINE CARTER
TYRESE MAYNARD 
JOSHUA CODDINGTON
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
GLuint screenWidth = 1000, screenHeight = 800;
GLfloat scale_size = 1.f;

//Initial location of camera
glm::vec3 camLocation(0.0f, 0.0f, 1500.0f);

// Camera
Camera camera(camLocation);
float lastX = screenWidth / 2.0f;
float lastY = screenHeight / 2.0f;
bool mouseMoved = true;

float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int modes);
//void mouseClickedCallback(GLFWwindow* window, int button, int  action, int mode);
void moveMouseCallback(GLFWwindow* window, double xpos, double ypos);
void clickDragCallback(GLFWwindow* window, int button, int  action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// misc functions
unsigned int loadCubemap(vector<std::string>);


void init_Resources()
{
    // Initialize the resources - set window, etc.
    /*if (!glfwInit())
    {
        cout << "\nFailed to Initialize GLFW...";
        exit(EXIT_FAILURE);
    } */

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


    //Create the window 
    window = glfwCreateWindow(screenWidth, screenHeight, "COMP3420 Project- GROUP 5", 0, 0);

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
    glfwSetKeyCallback(window, keyboardCallback);

    //----------------------------------------------------
        // Registering the call-back functions for the mouse
        //----------------------------------------------------
    // glfwSetMouseButtonCallback(window, mouseClickedCallback);
    glfwSetMouseButtonCallback(window, clickDragCallback);
    glfwSetCursorPosCallback(window, moveMouseCallback);
    glfwSetScrollCallback(window, scroll_callback);

    // Options - Hide the mouse pointer
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Setup OpenGL options
    glEnable(GL_DEPTH_TEST);


}
//end of initresources 


//Main Function 
int main()
{
    init_Resources();

    GLfloat skullAngle = 0;

    glm::mat4 View;

    //Compile shaders 
    Shader objshader("modelverte.glsl","modelfrag.glsl");
    // Shaders for skybox
    Shader shader("cubemaps.vs", "cubemaps.frag");
    Shader skyboxShader("skybox.vs", "skybox.frag");


    //load the obj file 
    Model skull((GLchar*)"skull.obj");

    GLuint viewID = glGetUniformLocation(objshader.Program, "view");


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
    //vertex data for skybox
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

    // skbox faces  using images
    vector<std::string> faces
    {
        "assets/right.jpg",
        "assets/left.jpg",
        "assets/top.jpg",
        "assets/bottom.jpg",
        "assets/front.jpg",
        "assets/back.jpg"
    };
    unsigned int cubemapTexture = loadCubemap(faces);

    skyboxShader.Use();
    skyboxShader.setInt("skybox", 0);
    

    



    //Keep displaying the window until we have shut it down
    while (!glfwWindowShouldClose(window))
    {
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

        // =======================================================================
        // Step 4. create the View matrix
        // =======================================================================
        View = camera.GetViewMatrix();

        objshader.Use();
        glUniformMatrix4fv(viewID, 1,
            GL_FALSE, glm::value_ptr(View));

        // =======================================================================
        // Create the model matrix 
        // =======================================================================
        //display shader
        objshader.Use();
        glm::mat4 Model = glm::mat4(1);

        //Modify the model matrix with scaling, translation, rotation, etc
        Model = glm::scale(Model, glm::vec3(scale_size));
        // translate skull further away from camera
        Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, -1500.5f));

        
        Model = glm::rotate(Model, skullAngle, glm::vec3(0.0f, 1.0f, 0.0f));

        // =======================================================================
        // Step 6. Pass the Model matrix,  to the skull shader as "model"
        // =======================================================================
        objshader.setMat4("model", Model);


        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.Use();
        View = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", View);
        skyboxShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default


        objshader.Use();
        glUniformMatrix4fv(viewID, 1,
            GL_FALSE, glm::value_ptr(View));


        //render the model 
        skull.Draw(objshader); 

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    //Release ALL the other resources
    glfwTerminate();
    exit(EXIT_SUCCESS);
}




void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int modes)
{
    // If ESC is pressed, close the window
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
        return;
    }

    if ((key == GLFW_KEY_HOME || key == GLFW_KEY_H) && action == GLFW_PRESS)
        camera = glm::vec3(0.0f, 0.0f, 1500.0f);
    
    // move camera
    if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
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
        // Make sure that the button is held down
        glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GL_TRUE);

    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        cout << "\nEnd Draging Mouse...\n";

        cout << "\nMouse has moved from X : " << startX << " to " << endX;
        cout << "\nMouse has moved from Y : " << startY << " to " << endY;
    }
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
