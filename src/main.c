#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h> //ver 3.4
#define WIDTH 800
#define HEIGHT 800


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow *window);
unsigned int createShaderProgram(const char * vertexSrc,const char * fragSrc);



const char *vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "out vec2 position;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                 "   position=vec2(aPos.x,aPos.y);\n"
                                 "}\0";

const char *mandelbrotShaderSource = "\n"
                                     "#version 330 core\n"
                                     "out vec4 FragColor;\n"
                                     "in vec2 position;\n"
                                     "uniform vec2 offset;\n"
                                     "uniform float scale;\n"
                                     "uniform float maxIterations;\n"
                                     "uniform int colour;\n"
                                     "vec2 sqrtImg(vec2 pos){\n"
                                     "    vec2 outpos;\n"
                                     "    outpos.x = pos.x*pos.x - pos.y*pos.y;\n"
                                     "    outpos.y = 2.0f*pos.x*pos.y;\n"
                                     "    return outpos;\n"
                                     "}\n"
                                     "vec4 iteratebw(vec2 pos,float maxIterations){\n"
                                     "    vec2 z = vec2(0,0);\n"
                                     "    for(int i=0;i<maxIterations;i++){\n"
                                     "       z=sqrtImg(z)+pos;\n"
                                     "       if(length(z)>2.0f){ \n"
                                     "           return vec4(1.0f,1.0f,1.0f,1.0f)*(float(i)/maxIterations);\n"
                                     "       }\n"
                                     "    }\n"
                                     "    return vec4(0.0f,0.0f,0.0f,1.0f);\n"
                                     "}\n"
                                     "vec4 iterate(vec2 pos,float maxIterations){\n"
                                     "    vec2 z = vec2(0,0);\n"
                                     "    for(int i=0;i<maxIterations;i++){\n"
                                     "       z=sqrtImg(z)+pos;\n"
                                     "       if(length(z)>2.0f){ \n"
                                     "           float val = float(i)/maxIterations;\n"
                                     "           float r = abs(val * 6.0f - 3.0f) - 1.0f;\n"
                                     "           float g = 2.0f - abs(val * 6.0f - 2.0f);\n"
                                     "           float b = 2.0f - abs(val * 6.0f - 4.0f);\n"
                                     "           return vec4(b,g,r,1.0f);"
                                     "       }\n"
                                     "    }\n"
                                     "    return vec4(0.0f,0.0f,0.0f,1.0f);\n"
                                     "}\n"
                                     "void main()\n"
                                     "{\n"
                                     "    vec2 coord = vec2(position.x*scale+(1*offset.x),position.y*scale+(1*offset.y));\n"
                                     "    if (colour==1){\n"
                                     "      FragColor = iterate(coord,500.0f);\n"
                                     "    }\n"
                                     "    else {\n"
                                     "      FragColor = iteratebw(coord,500.0f);\n"
                                     "    }\n"
                                     "}";

//uniforms
float offsetX = 0,offsetY=0;
float scale=1.0f;
int colour=1;

int main()
{

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Mandelbrot Set    ", NULL, NULL);
    if (window == NULL)
    {
        printf("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD");
        return -1;
    }
    glViewport(0, 0, WIDTH, HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window,key_callback);

    GLuint mandelProgram = createShaderProgram(vertexShaderSource,mandelbrotShaderSource);



    // ------------------------------------------------------------------
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
            -1.0f, -1.0f, 0.0f, // left
            -1.0f, 1.0f, 0.0f, // right
            1.0f,  -1.0f, 0.0f,  // top
            1.0f, 1.0f, 0.0f, // left
            1.0f, -1.0f, 0.0f, // right
            -1.0f,  1.0f, 0.0f,  // top
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);


    while(!glfwWindowShouldClose(window))
    {

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(mandelProgram);

        int offsetLocation = glGetUniformLocation(mandelProgram, "offset");
        int scaleLocation = glGetUniformLocation(mandelProgram, "scale");
        int colourLocation = glGetUniformLocation(mandelProgram, "colour");
        glUniform2f(offsetLocation, offsetX, offsetY);
        glUniform1f(scaleLocation,scale);
        glUniform1i(colourLocation,colour);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if(action == GLFW_RELEASE) return; //only handle press events
    if(key == GLFW_KEY_SPACE) colour = !colour;
}
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);
    if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        offsetX-=scale*0.01f;
    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        offsetX+=scale*0.01f;
    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        offsetY+=scale*0.01f;
    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        offsetY-=scale*0.01f;
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        scale-=scale*0.01f;
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        scale+=scale*0.01f;
}
unsigned int createShaderProgram(const char * vertexSrc,const char * fragSrc){
    int  success; //check shader compiles
    char infoLog[512];


    unsigned int vertexShader;//shader compilation
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSrc, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n");
    }

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragSrc, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n");
        printf("%s", infoLog);
    }


    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n");
        printf("%s", infoLog);
    }

    glDeleteShader(vertexShader); //cleanup
    glDeleteShader(fragmentShader);
    return shaderProgram;
}
