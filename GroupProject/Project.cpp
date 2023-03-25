/* 2023 Computer Graphics Project
* Contributors:
FAITH BROWNE
NICKEEM-DEACON PAYNE
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
Install-Package Soil
Install-Package Assimp -version 3.0.0
*/
#include <GL/glew.h>  //include glew
#include <GLFW/glfw3.h> //include glfw 
#include <iostream>
#include <cstdlib>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace std;

//ANY INCLUDES GO HERE 
#include "model.h";
#include "shader.h";


GLFWwindow* window;
void init_Resources()
{
    // Initialize the resources - set window, etc.
    if (!glfwInit())
    {
        cout << "\nFailed to Initialize GLFW...";
        exit(EXIT_FAILURE);
    }
    //Create the window 
    window = glfwCreateWindow(800, 800, "COMP3420 Project- GROUP 5", 0, 0);

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

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);//background colour set to black 
}
//end of initresources 


//Main Function 
int main(int argc, char* argv[])
{
    init_Resources();
    //Compile shaders 
    Shader objshader("modelverte.glsl","modelfrag.glsl");

    //load the obj file 
    Model skull((GLchar *)"skull.obj");



    //Keep displaying the window until we have shut it down
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        

        //display shader
        objshader.Use();

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
