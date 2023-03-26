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


//ANY INCLUDES GO HERE 
#include "shader.h";
#include "model.h";



GLFWwindow* window;

// properties
GLuint screenWidth = 800, screenHeight = 800;
GLfloat scale_size = 1.f;

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

    // =======================================================================
    //  Set the projection matrix
    // =======================================================================
    glm::mat4 projection = glm::perspective(45.0f, // Field of Vision
        (GLfloat)screenWidth / (GLfloat)screenHeight, // Aspect Ratio
        1.0f,
        10000.0f // camera position
    ); 

    objshader.Use();
    glUniformMatrix4fv(glGetUniformLocation(objshader.Program, "projection"),
        1, GL_FALSE, glm::value_ptr(projection));



    //Keep displaying the window until we have shut it down
    while (!glfwWindowShouldClose(window))
    {

        // Clear buffers
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        // =======================================================================
        // Step 4. create the View matrix
        // =======================================================================
        View = glm::lookAt
        (
            glm::vec3(500, 500, 1300), // Camera position in World Space
            glm::vec3(0, 0, 0),  // and looking at the origin
            glm::vec3(0, 1, 0)   // Head is up (set to 0,-1,0 to look upside-down)
        );

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
