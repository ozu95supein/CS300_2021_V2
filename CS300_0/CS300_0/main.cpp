#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <GL/GL.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "OGLDebug.h"
#include "ShaderUtils.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "RenderableMeshObject.h"
#include "LightSourceObject.h"

static int     winID;
static GLsizei WIDTH = 1280;
static GLsizei HEIGHT = 720;
//arbitrary numbers for width and height
const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
glm::vec2 SHADOW_DIM(SHADOW_WIDTH, SHADOW_HEIGHT);

float PIValue = glm::atan(1) * 4;
GLuint CreateShader(GLenum eShaderType, const std::string& strShaderFile)
{
    GLuint       shader = glCreateShader(eShaderType);
    const char* strFileData = strShaderFile.c_str();
    glShaderSource(shader, 1, &strFileData, NULL);

    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
        auto dummyvar = infoLogLength + 1;
        GLchar* strInfoLog = new GLchar[dummyvar];
        glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

        const char* strShaderType = NULL;
        switch (eShaderType)
        {
        case GL_VERTEX_SHADER:
            strShaderType = "vertex";
            break;
        case GL_GEOMETRY_SHADER:
            strShaderType = "geometry";
            break;
        case GL_FRAGMENT_SHADER:
            strShaderType = "fragment";
            break;
        }

        fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
        delete[] strInfoLog;
    }

    return shader;
}
GLuint CreateProgram(const std::vector<GLuint>& shaderList)
{
    GLuint program = glCreateProgram();

    for (size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
        glAttachShader(program, shaderList[iLoop]);

    glLinkProgram(program);

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint infoLogLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
        auto dummyvar = infoLogLength + 1;
        GLchar* strInfoLog = new GLchar[dummyvar];
        glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
        fprintf(stderr, "Linker failure: %s\n", strInfoLog);
        delete[] strInfoLog;
    }

    for (size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
        glDetachShader(program, shaderList[iLoop]);

    return program;
}
GLuint InitializeProgram()
{
    GLuint theProgram = ShaderUtils::CreateShaderProgram("Vertex.vert", "Fragment.frag");
    return theProgram;
}
GLuint InitializeWhiteProgram()
{
    GLuint theProgram = ShaderUtils::CreateShaderProgram("White.vert", "White.frag");
    return theProgram;
}
GLuint InitializeRedProgram()
{
    GLuint theProgram = ShaderUtils::CreateShaderProgram("White.vert", "red.frag");
    return theProgram;
}
GLuint InitializeGreenProgram()
{
    GLuint theProgram = ShaderUtils::CreateShaderProgram("White.vert", "green.frag");
    return theProgram;
}
GLuint InitializeBlueProgram()
{
    GLuint theProgram = ShaderUtils::CreateShaderProgram("White.vert", "blue.frag");
    return theProgram;
}
GLuint InitializeNormalProgram()
{
    GLuint theProgram = ShaderUtils::CreateShaderProgram("NormalVertex.vert", "NormalFragment.frag");
    return theProgram;
}
GLuint InitializeSingleColorProgram()
{
    GLuint theProgram = ShaderUtils::CreateShaderProgram("White.vert", "SingleColor.frag");
    return theProgram;
}
GLuint InitializeDepthBufferProgram()
{
    GLuint theProgram = ShaderUtils::CreateShaderProgram("depth.vert", "depth.frag");
    return theProgram;
}
GLuint InitializeDepthPlaneProgram()
{
    GLuint theProgram = ShaderUtils::CreateShaderProgram("depthplane.vert", "depthplane.frag");
    return theProgram;
}
GLuint InitializeBasicColorMapProgram()
{
    GLuint theProgram = ShaderUtils::CreateShaderProgram("BasicColorMapShader.vert", "BasicColorMapShader.frag");
    return theProgram;
}
void CleanUpObjectAndBuffers(GLuint& vbo, GLuint& vao, Mesh& mesh)
{
    // Delete the VBOs
    glDeleteBuffers(1, &vbo);
    // Delete the VAO
    glDeleteVertexArrays(1, &vao);
    mesh.CleanupAndReset(); 
}
void ChangeSlices(GLuint& vbo, GLuint& vao, RenderableMeshObject & RenderableMesh, int new_slices, MeshType & t, GLuint & normal_vbo, GLuint & normal_vao, GLuint& average_normal_vbo, GLuint& average_normal_vao)
{
    Mesh& mesh = RenderableMesh.GetMesh();
    //reset the Mesh object and clean up the buffers
    CleanUpObjectAndBuffers(vbo, vao, mesh);
    CleanUpObjectAndBuffers(normal_vbo, normal_vao, mesh);
    CleanUpObjectAndBuffers(average_normal_vbo, average_normal_vao, mesh);
    //reconstruct mesh with new slices
    switch (t)
    {
    case MeshType::CYLINDER:
        mesh.ConstructCylinder(new_slices);
        break;
    case MeshType::CONE:
        mesh.ConstructCone(new_slices);
        break;
    case MeshType::SPHERE:
        mesh.ConstructSphere(new_slices);
        break;
    default:
        break;
    }
    
    RenderableMesh.Renderable_InitializeMeshBuffers(vbo, vao, mesh);
    mesh.GenerateNormalLines();
    mesh.GenerateAveragedNormalLines();
    RenderableMesh.Renderable_InitializeNormalBuffers(normal_vbo, normal_vao, mesh);
    RenderableMesh.Renderable_InitializeAveragedNormalBuffers(average_normal_vbo, average_normal_vao, mesh);
}
GLuint& makeTexture(GLuint& t)
{
    //make the colored texture array?
    glm::vec4 blue(0.0f, 0.0f, 1.0f, 1.0f);
    glm::vec4 cyan(0.0f, 1.0f, 1.0f, 1.0f);
    glm::vec4 green(0.0f, 1.0f, 0.0f, 1.0f);
    glm::vec4 yellow(1.0f, 1.0f, 0.0f, 1.0f);
    glm::vec4 red(1.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 purple(1.0f, 0.0f, 1.0f, 1.0f);
    glm::vec4 textureColorArray[] =
    {
        blue, cyan, green, yellow, red, purple,
        cyan, green, yellow, red, purple, blue,
        green, yellow, red, purple, blue, cyan,
        yellow, red, purple, blue, cyan, green,
        red, purple, blue, cyan, green, yellow,
        purple, blue, cyan, green, yellow, red
    };

    // Create texture
    glGenTextures(1, &t);
    glBindTexture(GL_TEXTURE_2D, t);

    int pixelFormat = GL_RGBA;

    // Give pixel data to opengl
    glTexImage2D(GL_TEXTURE_2D, 0, pixelFormat, 6, 6, 0, pixelFormat, GL_FLOAT, textureColorArray);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    return t;
}
GLuint makeNormalMapTexture(const std::string& filename)
{
    //load texture from filename
    SDL_Surface* normalMap = IMG_Load(filename.c_str());
    if (normalMap == nullptr)
    {
        std::cout << "Could not load texture: " + filename << std::endl;
        return 0;
    }
    GLuint texture;
    // Create texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // Select pixel format from surface
    int pixelFormat = GL_RGB;
    if (normalMap->format->BytesPerPixel == 4)
    {
        pixelFormat = GL_RGBA;
    }
    // Give pixel data to opengl
    glTexImage2D(GL_TEXTURE_2D, 0, pixelFormat, normalMap->w, normalMap->h, 0, pixelFormat, GL_UNSIGNED_BYTE, normalMap->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Free surface because OpenGL already has the data
    SDL_FreeSurface(normalMap);

    return texture;
}
#undef main
int main(int argc, char* args[])
{
    //init SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "Could not initialize SDL: " << SDL_GetError() << std::endl;
        exit(1);
    }
    //Create THe window witth the parameters
    SDL_Window* window = SDL_CreateWindow("CS300", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
    if (window == nullptr)
    {
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }
    //ASK ABOUT THIS
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    //CREATE the Context, ask about this
    SDL_GLContext context_ = SDL_GL_CreateContext(window);
    if (context_ == nullptr)
    {
        SDL_DestroyWindow(window);
        std::cout << "SDL_GL_CreateContext Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }
    glewExperimental = true;
    if (glewInit() != GLEW_OK)
    {
        SDL_GL_DeleteContext(context_);
        SDL_DestroyWindow(window);
        std::cout << "GLEW Error: Failed to init" << std::endl;
        SDL_Quit();
        exit(1);
    }

#if _DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
#endif
    // print GPU data
    std::cout << "GL_VENDOR: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "GL_RENDERER: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "GL_VERSION: " << glGetString(GL_VERSION) << std::endl;
    glEnable(GL_DEPTH_TEST);
    GLuint texture = makeTexture(texture);
    GLuint shaderProgram = InitializeProgram();
    GLuint NormalshaderProgram = InitializeNormalProgram();
    GLuint WhiteShaderProgram = InitializeWhiteProgram();
    GLuint SingleColorShaderProgram = InitializeSingleColorProgram();
    GLuint RedShaderProgram = InitializeRedProgram();
    GLuint GreenShaderProgram = InitializeGreenProgram();
    GLuint BlueShaderProgram = InitializeBlueProgram();
    GLuint DepthBufferShaderProgram = InitializeDepthBufferProgram();
    GLuint BasicColorMapShaderProgram = InitializeBasicColorMapProgram();

    //Make a normal map for the height maps
    GLuint mNormalMap = makeNormalMapTexture("./Textures/normal_map_flippedY.png");

    /*******************************************************************************************************************************************/
    //create matrices
    glm::mat4 CenterTranslationMatrix;
    glm::mat4 LeftMobileTranslationMatrix;
    glm::mat4 RightMobileTranslationMatrix;
    glm::mat4 LeftStaticTranslationMatrix;
    glm::mat4 RightStaticTranslationMatrix;

    glm::mat4 rotationMatrix;

    glm::mat4 CenterScaleMatrix;
    glm::mat4 MobileScaleMatrix;
    glm::mat4 StaticScaleMatrix;
    
    //model matrix
    //glm::mat4 MainModelMatrix = translationMatrix * rotationMatrix * MainScaleMatrix;//world space
    
    glm::mat4 CenterModelMatrix        = CenterTranslationMatrix * rotationMatrix * CenterScaleMatrix;
    glm::mat4 LeftMobileModelMatrix    = LeftMobileTranslationMatrix * rotationMatrix * MobileScaleMatrix;
    glm::mat4 RightMobileModelMatrix   = RightMobileTranslationMatrix * rotationMatrix * MobileScaleMatrix;
    glm::mat4 LeftStaticModelMatrix    = LeftStaticTranslationMatrix * rotationMatrix * StaticScaleMatrix;
    glm::mat4 RightStaticModelMatrix   = RightStaticTranslationMatrix * rotationMatrix * StaticScaleMatrix;

    /*******************************************************************************************************************************************/
    int current_slices = 20;
    //create objects to swap when pressing buttons
    
    //RenderableMeshObject::RenderableMeshObject(MeshType t, int slices, glm::mat4 ModelMatrix)

    RenderableMeshObject CenterPlaneObject(MeshType::PLANE, current_slices, CenterModelMatrix);
    RenderableMeshObject CenterCubeObject(MeshType::CUBE, current_slices, CenterModelMatrix);
    RenderableMeshObject CenterConeObject(MeshType::CONE, current_slices, CenterModelMatrix);
    RenderableMeshObject CenterCylinderObject(MeshType::CYLINDER, current_slices, CenterModelMatrix);
    RenderableMeshObject CenterSphereObject(MeshType::SPHERE, current_slices, CenterModelMatrix);

    RenderableMeshObject LeftMobilePlaneObject(MeshType::PLANE, current_slices, LeftMobileModelMatrix);
    RenderableMeshObject LeftMobileCubeObject(MeshType::CUBE, current_slices, LeftMobileModelMatrix);
    RenderableMeshObject LeftMobileConeObject(MeshType::CONE, current_slices, LeftMobileModelMatrix);
    RenderableMeshObject LeftMobileCylinderObject(MeshType::CYLINDER, current_slices, LeftMobileModelMatrix);
    RenderableMeshObject LeftMobileSphereObject(MeshType::SPHERE, current_slices, LeftMobileModelMatrix);

    RenderableMeshObject RightMobilePlaneObject(MeshType::PLANE, current_slices, RightMobileModelMatrix);
    RenderableMeshObject RightMobileCubeObject(MeshType::CUBE, current_slices, RightMobileModelMatrix);
    RenderableMeshObject RightMobileConeObject(MeshType::CONE, current_slices, RightMobileModelMatrix);
    RenderableMeshObject RightMobileCylinderObject(MeshType::CYLINDER, current_slices, RightMobileModelMatrix);
    RenderableMeshObject RightMobileSphereObject(MeshType::SPHERE, current_slices, RightMobileModelMatrix);

    RenderableMeshObject LeftStaticPlaneObject(MeshType::PLANE, current_slices, LeftStaticModelMatrix);
    RenderableMeshObject LeftStaticCubeObject(MeshType::CUBE, current_slices, LeftStaticModelMatrix);
    RenderableMeshObject LeftStaticConeObject(MeshType::CONE, current_slices, LeftStaticModelMatrix);
    RenderableMeshObject LeftStaticCylinderObject(MeshType::CYLINDER, current_slices, LeftStaticModelMatrix);
    RenderableMeshObject LeftStaticSphereObject(MeshType::SPHERE, current_slices, LeftStaticModelMatrix);

    RenderableMeshObject RightStaticPlaneObject(MeshType::PLANE, current_slices, RightStaticModelMatrix);
    RenderableMeshObject RightStaticCubeObject(MeshType::CUBE, current_slices, RightStaticModelMatrix);
    RenderableMeshObject RightStaticConeObject(MeshType::CONE, current_slices, RightStaticModelMatrix);
    RenderableMeshObject RightStaticCylinderObject(MeshType::CYLINDER, current_slices, RightStaticModelMatrix);
    RenderableMeshObject RightStaticSphereObject(MeshType::SPHERE, current_slices, RightStaticModelMatrix);
    /*******************************************************************************************************************************************/
  
    Light mLight = Light();
    Material mMaterial = Material();
    
    //render from lights point of view and store it in a depth buffer texture
    glm::vec3 light_pos3(mLight.light_position.x, mLight.light_position.y, mLight.light_position.z);
    glm::mat4 light_ViewMatrix = glm::lookAt(light_pos3, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 light_ProjectionMatrix = glm::perspective(2 * glm::radians(mLight.outer), 1.0f, 0.1f, 150.0f);
    
    /*******************************************************************************************************************************************/
    //view matrix
    glm::mat4 ViewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 50.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 ViewDirection = glm::vec3(0.0f, 0.0f, 0.0f) - glm::vec3(0.0f, 0.0f, 50.0f);
    
    float CameraRadius = 50.f;  //radius of the camera from origin
    float CameraRadius_increment = 0.5;  //radius of the camera from origin
    float alpha_rad = 0.0f;     //angle of the camera on the xz plane, y axis
    float gamma_rad = 0.0f;   //angle of the camera from the top
    float alpha_increment = 0.05f;
    float gamma_increment = 0.05f;
    
    float cam_x = CameraRadius * glm::cos(gamma_rad) * glm::sin(alpha_rad);
    float cam_y = CameraRadius * glm::sin(gamma_rad);
    float cam_z = CameraRadius * glm::cos(gamma_rad) * glm::cos(alpha_rad);
    glm::vec3 cam_pos(cam_x, cam_y, cam_z);
    //projection matrix
    float aspect = (float)WIDTH / HEIGHT;
    glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 150.0f);
    /*******************************************************************************************************************************************/
    float SideObjectAngle = 0.0f;
    float SideObjectAngleIncrement = 0.001f;

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    /*******************************************************************************************************************************************/
    //DEPTH BUFFER AND SHADOWMAP TEXTURE
    //create a 2D texture to use as the framebuffer's depth buffer
    GLuint depthTex;
    GLuint depthMapFBO;
    glGenTextures(1, &depthTex);
    glBindTexture(GL_TEXTURE_2D, depthTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    // Create and set up the FBO
    glGenFramebuffers(1, &depthMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D, depthTex, 0);

    GLenum drawBuffers[] = { GL_NONE };
    glDrawBuffers(1, drawBuffers);

    GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result == GL_FRAMEBUFFER_COMPLETE) {
        printf("Framebuffer is complete.\n");
    }
    else {
        printf("Framebuffer is not complete.\n");
    }
    /*******************************************************************************************************************************************/

    //1 = plane, 1: Plane, 2: Cube, 3 : Cone,  4 : Cylinder, 5 : Sphere       
    int current_mesh_to_display = 1;
    int Display_Normals = 0;
    bool Display_Wireframe = false;
    int RenderMode = 0;
    int UsingFaceNormals = 1;
    int MovingSideObjects = 1;
    int neighbor = 2;

    SDL_Event event;
    bool      quit = false;
    while (!quit)
    {
        glEnable(GL_DEPTH_TEST);
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                {
                    quit = true;
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_DOWN)
                {
                    MainPlaneObject.Renderable_RotateModel(glm::radians(-5.0f), glm::vec3(1.0, 0.0, 0.0));
                    LeftPlaneObject.Renderable_RotateModel(glm::radians(-5.0f), glm::vec3(1.0, 0.0, 0.0));
                    RightPlaneObject.Renderable_RotateModel(glm::radians(-5.0f), glm::vec3(1.0, 0.0, 0.0));

                    MainCubeObject.Renderable_RotateModel(glm::radians(-5.0f), glm::vec3(1.0, 0.0, 0.0));
                    LeftCubeObject.Renderable_RotateModel(glm::radians(-5.0f), glm::vec3(1.0, 0.0, 0.0));
                    RightCubeObject.Renderable_RotateModel(glm::radians(-5.0f), glm::vec3(1.0, 0.0, 0.0));

                    MainCylinderObject.Renderable_RotateModel(glm::radians(-5.0f), glm::vec3(1.0, 0.0, 0.0));
                    LeftCylinderObject.Renderable_RotateModel(glm::radians(-5.0f), glm::vec3(1.0, 0.0, 0.0));
                    RightCylinderObject.Renderable_RotateModel(glm::radians(-5.0f), glm::vec3(1.0, 0.0, 0.0));

                    MainConeObject.Renderable_RotateModel(glm::radians(-5.0f), glm::vec3(1.0, 0.0, 0.0));
                    LeftConeObject.Renderable_RotateModel(glm::radians(-5.0f), glm::vec3(1.0, 0.0, 0.0));
                    RightConeObject.Renderable_RotateModel(glm::radians(-5.0f), glm::vec3(1.0, 0.0, 0.0));

                    MainSphereObject.Renderable_RotateModel(glm::radians(-5.0f), glm::vec3(1.0, 0.0, 0.0));
                    LeftSphereObject.Renderable_RotateModel(glm::radians(-5.0f), glm::vec3(1.0, 0.0, 0.0));
                    RightSphereObject.Renderable_RotateModel(glm::radians(-5.0f), glm::vec3(1.0, 0.0, 0.0));
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_UP)
                {
                    MainPlaneObject.Renderable_RotateModel(glm::radians(5.0f), glm::vec3(1.0, 0.0, 0.0));
                    LeftPlaneObject.Renderable_RotateModel(glm::radians(5.0f), glm::vec3(1.0, 0.0, 0.0));
                    RightPlaneObject.Renderable_RotateModel(glm::radians(5.0f), glm::vec3(1.0, 0.0, 0.0));

                    MainCubeObject.Renderable_RotateModel(glm::radians(5.0f), glm::vec3(1.0, 0.0, 0.0));
                    LeftCubeObject.Renderable_RotateModel(glm::radians(5.0f), glm::vec3(1.0, 0.0, 0.0));
                    RightCubeObject.Renderable_RotateModel(glm::radians(5.0f), glm::vec3(1.0, 0.0, 0.0));

                    MainCylinderObject.Renderable_RotateModel(glm::radians(5.0f), glm::vec3(1.0, 0.0, 0.0));
                    LeftCylinderObject.Renderable_RotateModel(glm::radians(5.0f), glm::vec3(1.0, 0.0, 0.0));
                    RightCylinderObject.Renderable_RotateModel(glm::radians(5.0f), glm::vec3(1.0, 0.0, 0.0));

                    MainConeObject.Renderable_RotateModel(glm::radians(5.0f), glm::vec3(1.0, 0.0, 0.0));
                    LeftConeObject.Renderable_RotateModel(glm::radians(5.0f), glm::vec3(1.0, 0.0, 0.0));
                    RightConeObject.Renderable_RotateModel(glm::radians(5.0f), glm::vec3(1.0, 0.0, 0.0));

                    MainSphereObject.Renderable_RotateModel(glm::radians(5.0f), glm::vec3(1.0, 0.0, 0.0));
                    LeftSphereObject.Renderable_RotateModel(glm::radians(5.0f), glm::vec3(1.0, 0.0, 0.0));
                    RightSphereObject.Renderable_RotateModel(glm::radians(5.0f), glm::vec3(1.0, 0.0, 0.0));
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_LEFT)
                {
                    MainPlaneObject.Renderable_RotateModel(glm::radians(-5.0f), glm::vec3(0.0, 1.0, 0.0));
                    LeftPlaneObject.Renderable_RotateModel(glm::radians(-5.0f), glm::vec3(0.0, 1.0, 0.0));
                    RightPlaneObject.Renderable_RotateModel(glm::radians(-5.0f), glm::vec3(0.0,1.0, 0.0));

                    MainCubeObject.Renderable_RotateModel(glm::radians(-5.0f), glm::vec3(0.0, 1.0, 0.0));
                    LeftCubeObject.Renderable_RotateModel(glm::radians(-5.0f), glm::vec3(0.0, 1.0, 0.0));
                    RightCubeObject.Renderable_RotateModel(glm::radians(-5.0f), glm::vec3(0.0,1.0, 0.0));

                    MainCylinderObject.Renderable_RotateModel(glm::radians(-5.0f), glm::vec3(0.0, 1.0, 0.0));
                    LeftCylinderObject.Renderable_RotateModel(glm::radians(-5.0f), glm::vec3(0.0, 1.0, 0.0));
                    RightCylinderObject.Renderable_RotateModel(glm::radians(-5.0f), glm::vec3(0.0,1.0, 0.0));

                    MainConeObject.Renderable_RotateModel(glm::radians(-5.0f), glm::vec3(0.0, 1.0, 0.0));
                    LeftConeObject.Renderable_RotateModel(glm::radians(-5.0f), glm::vec3(0.0, 1.0, 0.0));
                    RightConeObject.Renderable_RotateModel(glm::radians(-5.0f), glm::vec3(0.0,1.0, 0.0));

                    MainSphereObject.Renderable_RotateModel(glm::radians(-5.0f), glm::vec3(0.0, 1.0, 0.0));
                    LeftSphereObject.Renderable_RotateModel(glm::radians(-5.0f), glm::vec3(0.0, 1.0, 0.0));
                    RightSphereObject.Renderable_RotateModel(glm::radians(-5.0f), glm::vec3(0.0,1.0, 0.0));
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT)
                {
                    MainPlaneObject.Renderable_RotateModel(glm::radians(5.0f), glm::vec3(0.0, 1.0, 0.0));
                    LeftPlaneObject.Renderable_RotateModel(glm::radians(5.0f), glm::vec3(0.0, 1.0, 0.0));
                    RightPlaneObject.Renderable_RotateModel(glm::radians(5.0f), glm::vec3(0.0, 1.0, 0.0));

                    MainCubeObject.Renderable_RotateModel(glm::radians(5.0f), glm::vec3(0.0, 1.0, 0.0));
                    LeftCubeObject.Renderable_RotateModel(glm::radians(5.0f), glm::vec3(0.0, 1.0, 0.0));
                    RightCubeObject.Renderable_RotateModel(glm::radians(5.0f), glm::vec3(0.0, 1.0, 0.0));

                    MainCylinderObject.Renderable_RotateModel(glm::radians(5.0f), glm::vec3(0.0, 1.0, 0.0));
                    LeftCylinderObject.Renderable_RotateModel(glm::radians(5.0f), glm::vec3(0.0, 1.0, 0.0));
                    RightCylinderObject.Renderable_RotateModel(glm::radians(5.0f), glm::vec3(0.0, 1.0, 0.0));

                    MainConeObject.Renderable_RotateModel(glm::radians(5.0f), glm::vec3(0.0, 1.0, 0.0));
                    LeftConeObject.Renderable_RotateModel(glm::radians(5.0f), glm::vec3(0.0, 1.0, 0.0));
                    RightConeObject.Renderable_RotateModel(glm::radians(5.0f), glm::vec3(0.0, 1.0, 0.0));

                    MainSphereObject.Renderable_RotateModel(glm::radians(5.0f), glm::vec3(0.0, 1.0, 0.0));
                    LeftSphereObject.Renderable_RotateModel(glm::radians(5.0f), glm::vec3(0.0, 1.0, 0.0));
                    RightSphereObject.Renderable_RotateModel(glm::radians(5.0f), glm::vec3(0.0, 1.0, 0.0));
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_1)
                {
                    current_mesh_to_display = 1;
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_2)
                {
                    current_mesh_to_display = 2;
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_3)
                {
                    current_mesh_to_display = 3;
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_4)
                {
                    current_mesh_to_display = 4;
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_5)
                {
                    current_mesh_to_display = 5;
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_F)
                {
                    if (UsingFaceNormals == 1)
                    {
                        UsingFaceNormals = 0;
                    }
                    else
                    {
                        UsingFaceNormals = 1;
                    }
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_M)
                {
                    Display_Wireframe = !Display_Wireframe;
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_N)
                {
                    if (Display_Normals == 1)
                    {
                        Display_Normals = 0;
                    }
                    else
                    {
                        Display_Normals = 1;
                    }
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_KP_PLUS)
                {
                    current_slices += 1;
                    MeshType cy = MeshType::CYLINDER;
                    MeshType cn = MeshType::CONE;
                    MeshType sp = MeshType::SPHERE;
                    MainCylinderObject.Renderable_ChangeSlices(current_slices, cy);
                    LeftCylinderObject.Renderable_ChangeSlices(current_slices, cy);
                    RightCylinderObject.Renderable_ChangeSlices(current_slices, cy);
                    MainConeObject.Renderable_ChangeSlices(current_slices, cn);
                    LeftConeObject.Renderable_ChangeSlices(current_slices, cn);
                    RightConeObject.Renderable_ChangeSlices(current_slices, cn);
                    MainSphereObject.Renderable_ChangeSlices(current_slices, sp);
                    LeftSphereObject.Renderable_ChangeSlices(current_slices, sp);
                    RightSphereObject.Renderable_ChangeSlices(current_slices, sp);
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_KP_MINUS)
                {
                    if (current_slices == 4)
                    {
                        break;
                    }
                    current_slices -= 1;
                    MeshType cy = MeshType::CYLINDER;
                    MeshType cn = MeshType::CONE;
                    MeshType sp = MeshType::SPHERE;
                    MainCylinderObject.Renderable_ChangeSlices(current_slices, cy);
                    LeftCylinderObject.Renderable_ChangeSlices(current_slices, cy);
                    RightCylinderObject.Renderable_ChangeSlices(current_slices, cy);
                    MainConeObject.Renderable_ChangeSlices(current_slices, cn);
                    LeftConeObject.Renderable_ChangeSlices(current_slices, cn);
                    RightConeObject.Renderable_ChangeSlices(current_slices, cn);
                    MainSphereObject.Renderable_ChangeSlices(current_slices, sp);
                    LeftSphereObject.Renderable_ChangeSlices(current_slices, sp);
                    RightSphereObject.Renderable_ChangeSlices(current_slices, sp);
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_T)
                {
                    //RenderMode += 1;
                    //if (RenderMode > 3)
                    //{
                    //    RenderMode = 0;
                    //}
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_W)
                {
                    gamma_rad += gamma_increment;
                    if (gamma_rad > (PIValue / 2.0f))
                    {
                        gamma_rad = (PIValue / 2.0f);
                    }
                    
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_S)
                {
                    gamma_rad -= gamma_increment;
                    if (gamma_rad < -(PIValue / 2.0f))
                    {
                        gamma_rad = -(PIValue / 2.0f);
                    }
                    glm::clamp(gamma_rad, -(PIValue / 2.0f), (PIValue / 2.0f));
                    
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_A)
                {
                    alpha_rad -= alpha_increment;
                   
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_D)
                {
                    alpha_rad += alpha_increment;
                    
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_Q)
                {
                    //closer
                    CameraRadius -= CameraRadius_increment;
                    if (CameraRadius < 0.1f)
                    {
                        CameraRadius = 0.1f;
                    }
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_E)
                {
                    //further
                    CameraRadius += CameraRadius_increment;
                    if (CameraRadius > 100.0f)
                    {
                        CameraRadius = 100.0f;
                    }
                }
                else if (event.key.keysym.scancode == SDL_SCANCODE_O)
                {
                    if (MovingSideObjects == 1)
                    {
                        MovingSideObjects = 0;
                    }
                    else
                    {
                        MovingSideObjects = 1;
                    }
                }
                
                break;
            }
        }
        ////////////////////////////////////////////////////////////////////////////////
        //update camera
        cam_x = CameraRadius * glm::cos(gamma_rad) * glm::sin(alpha_rad);
        cam_y = CameraRadius * glm::sin(gamma_rad);
        cam_z = CameraRadius * glm::cos(gamma_rad) * glm::cos(alpha_rad);
        glm::vec3 cam_pos(cam_x, cam_y, cam_z);
        glm::mat4 ViewMatrix2 = glm::lookAt(cam_pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ViewMatrix = ViewMatrix2;
        ////////////////////////////////////////////////////////////////////////////////
        // update light
        if (MovingSideObjects)
        {
            SideObjectAngle += SideObjectAngleIncrement;
            float left_y = 10.0f * glm::sin(SideObjectAngle);
            
            LeftTranslationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-20.0f, left_y, 0.0f));
            RightTranslationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(20.0f, left_y, 0.0f));

            LeftModelMatrix = LeftTranslationMatrix * rotationMatrix * SecondaryScaleMatrix;//world space
            RightModelMatrix = RightTranslationMatrix * rotationMatrix * SecondaryScaleMatrix;//world space
           
            LeftPlaneObject.SetModel(LeftModelMatrix);
            LeftCubeObject.SetModel(LeftModelMatrix);
            LeftCylinderObject.SetModel(LeftModelMatrix);
            LeftConeObject.SetModel(LeftModelMatrix);
            LeftSphereObject.SetModel(LeftModelMatrix);

            RightPlaneObject.SetModel(RightModelMatrix);
            RightCubeObject.SetModel(RightModelMatrix);
            RightCylinderObject.SetModel(RightModelMatrix);
            RightConeObject.SetModel(RightModelMatrix);
            RightSphereObject.SetModel(RightModelMatrix);
        }
        ////////////////////////////////////////////////////////////////////////////////
        //Bind the depth buffer
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        //clear it before we draw

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear current buffer
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);      //set viewport of first pass
        //render all the shapes for the shadow map using the light view and light projection matrices
        switch (current_mesh_to_display)
        {
            case 1:
            {
                
            }
            break;
            case 2:
            {
               
            }
            break;
            case 3:
            {
                
            }
            break;
            case 4:
            {
                
            }
            break;
            case 5:
            {
                
            }
            break;
        }

        //unbind depth buffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        ////////////////////////////////////////////////////////////////////////////////
        //change shader program to receive matrices as inputs
        // original
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, WIDTH, HEIGHT);
        ////////////////////////////////////////////////////////////////////////////////
        switch (current_mesh_to_display)
        {
            case 1:
            {
                
            }
            break;
            case 2:
            {
                
            }
            break;
            case 3:
            {
                
            }
            break;
            case 4:
            {
                
            }
            break;
            case 5:
            {
                
            }
            break;
        }
        
        //glDisable(GL_DEPTH_TEST);
        
        SDL_GL_SwapWindow(window);        
    }
    glDeleteProgram(shaderProgram);
    SDL_GL_DeleteContext(context_);
    SDL_DestroyWindow(window);
    SDL_Quit();
	return 0;
}