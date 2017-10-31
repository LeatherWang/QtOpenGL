#include "openglwindow.h"
#include <iostream>

/*******************************************************************************
 * 着色器、着色器程序、VAO（顶点数组对象）、VBO（顶点缓冲对象）、EBO(索引缓冲对象)
 ******************************************************************************/

const GLuint NumVertexShader = 1;       //顶点着色器的数目
const GLuint NumFragmentShader = 1;     //片段着色器的数目
const GLuint NumShaderProgram = 1;      //着色器程序的数目
const GLuint NumVAO = 1;                //VAO的数目
const GLuint NumVBO = 1;                //VBO的数目
const GLuint NumEBO = 1;                //EBO的数目

GLuint IDVertexShader[NumVertexShader];
GLuint IDFragmentShader[NumFragmentShader];
GLuint IDShaderProgram[NumShaderProgram];
GLuint IDVAO[NumVAO];
GLuint IDVBO[NumVBO];
GLuint IDEBO[NumEBO];

/*******************************************************************************
 * 着色器源码
 ******************************************************************************/

const GLchar *vertexShaderSource =
        "#version 330 core\n"
        "layout(location = 0) in vec3 vPosition;\n"
        "layout(location = 1) in vec3 vColor;\n"
        "out vec3 Color;\n"
        "void main()\n"
        "{\n"
            "gl_Position = vec4(vPosition, 1.0);\n"
            "Color = vColor;\n"
        "}\n";

const GLchar *fragmentShaderSource =
        "#version 330 core\n"
        "in vec3 Color;\n"
        "out vec4 fColor;\n"
        "void main()\n"
        "{\n"
            "fColor = vec4(Color, 1.0f);\n"
        "}\n";

openglwindow::openglwindow(QWidget *parent)
    :QOpenGLWidget(parent)
{
    //设置OpenGL的版本信息
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(3,3);
    setFormat(format);
}

openglwindow::~openglwindow()
{

}

void openglwindow::initializeGL()
{
    //初始化OpenGL函数
    initializeOpenGLFunctions();

    //设置全局变量
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    /******************************* 顶点着色器创建 *******************************/
    /* 第一个顶点着色器 */
    IDVertexShader[0] = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(IDVertexShader[0], 1, &vertexShaderSource, NULL);
    glCompileShader(IDVertexShader[0]);
    //检查编译是否出错
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(IDVertexShader[0], GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(IDVertexShader[0], 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    /******************************* 片段着色器创建 *******************************/
    /* 第一个片元着色器 */
    IDFragmentShader[0] = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(IDFragmentShader[0], 1, &fragmentShaderSource, NULL);
    glCompileShader(IDFragmentShader[0]);
    //检查编译是否出错
    glGetShaderiv(IDFragmentShader[0], GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(IDFragmentShader[0], 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    /********************************* 链接着色器 *********************************/
    /* 第一个着色器程序 */
    IDShaderProgram[0] = glCreateProgram();
    glAttachShader(IDShaderProgram[0], IDVertexShader[0]);
    glAttachShader(IDShaderProgram[0], IDFragmentShader[0]);
    glLinkProgram(IDShaderProgram[0]);
    //检查链接错误
    glGetProgramiv(IDShaderProgram[0], GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(IDShaderProgram[0], 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    //删除着色器对象（生成着色器程序之后不再需要）
    glDeleteShader(IDVertexShader[0]);
    glDeleteShader(IDFragmentShader[0]);

    /******************************** 设置顶点数据 ********************************/
    //彩色正方体
    GLfloat vertices[] =
    {
        -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
         0.5f,  0.5f, -0.5f, 0.8f, 0.5f, 0.2f,
        -0.5f,  0.5f, -0.5f, 0.2f, 0.8f, 0.5f
    };
    GLuint indices[] =
    {
        0, 1, 2, 2, 3, 0,   //前
        4, 5, 6, 6, 7, 4,   //后
        0, 4, 7, 7, 3, 0,   //左
        1, 5, 6, 6, 2, 1,   //右
        0, 4, 5, 5, 1, 0,   //上
        3, 7, 6, 6, 2, 3    //下
    };

    /****************************************************************************/
    /**************************** VAO\VBO\顶点属性指针 ****************************/
    /****************************************************************************/

    /* 创建相关对象 */
    glGenVertexArrays(NumVAO, IDVAO);
    glGenBuffers(NumVBO, IDVBO);
    glGenBuffers(NumEBO, IDEBO);

    /* 显示立方体 */
    glBindVertexArray(IDVAO[0]);    //开始记录状态信息

    glBindBuffer(GL_ARRAY_BUFFER, IDVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IDEBO[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);           //结束记录状态信息
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);   //在VAO后解绑，是为了不让VAO把解绑EBO的信息包含进入
    /* 显示立方体 */

    /* 固定属性区域 */
    glEnable(GL_DEPTH_TEST);        //开启深度测试
}

void openglwindow::paintGL()
{
    //清理屏幕
    glClear(GL_COLOR_BUFFER_BIT);

    //渲染彩色正方体
    glUseProgram(IDShaderProgram[0]);
    glBindVertexArray(IDVAO[0]);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    //强制刷新缓冲区，保证命令被执行
    glFlush();
}

void openglwindow::resizeGL(int width, int height)
{
    //未使用
    Q_UNUSED(width);
    Q_UNUSED(height);
}
