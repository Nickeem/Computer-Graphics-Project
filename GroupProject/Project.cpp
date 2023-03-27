/* 2023 Computer Graphics Project
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
//void moveMouseCallback(GLFWwindow* window, double xpos, double ypos);
void clickDragCallback(GLFWwindow* window, int button, int  action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


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
    //glfwSetCursorPosCallback(window, moveMouseCallback);
    glfwSetScrollCallback(window, scroll_callback);

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

    //load the obj file 
    Model skull((GLchar*)"skull.obj");

    GLuint viewID = glGetUniformLocation(objshader.Program, "view");

    

    



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
        glUniformMatrix4fv(glGetUniformLocation(objshader.Program, "projection"),
            1, GL_FALSE, glm::value_ptr(projection));

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
        Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, 0.0f));

        
        Model = glm::rotate(Model, skullAngle, glm::vec3(0.0f, 1.0f, 0.0f));





        // =======================================================================
        // Step 6. Pass the Model matrix, "planetModel", to the shader as "model"
        // =======================================================================
        glUniformMatrix4fv(glGetUniformLocation(objshader.Program, "model"), 1,
            GL_FALSE, glm::value_ptr(Model));

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


    // move camera
    const float cameraSpeed = 100.f; // adjust accordingly
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

    while (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        // Make sure that the button is held down
        glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GL_TRUE);


        // ------Do things here -------
        //e.g. Catch starting XY location of the mouse pointer
        glfwGetCursorPos(window, &startX, &startY);

        cout << "\n\nBegin Dragging Mouse... ";
        if (action != GLFW_RELEASE) {
            break;
        }

    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        cout << "\nEnd Draging Mouse...\n";

        // ------Do other things here -------
        //e.g. Catch ending XY location of the mouse pointer
        glfwGetCursorPos(window, &endX, &endY);
        GLfloat xoffset = startX - lastX;
        GLfloat yoffset = lastY - startY;
        camera.ProcessMouseMovement(xoffset, yoffset);

        cout << "\nMouse has moved from X : " << startX << " to " << endX;
        cout << "\nMouse has moved from Y : " << startY << " to " << endY;
    }
}
