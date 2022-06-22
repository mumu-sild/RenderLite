#include "QShader.h"

QShader::QShader(const char* vertexPath, const char* fragmentPath,const char* geometryPath,QOpenGLFunctions* f)
    :f(f)
{
    //read Shader from file
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;
    const char* vShaderCode;
    const char* fShaderCode;
    const char* gShaderCode;

    vShaderFile.exceptions(std::ifstream::failbit|std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit|std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit|std::ifstream::badbit);


    try
    {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();

        if (geometryPath != nullptr){
            gShaderFile.open(geometryPath);
            std::stringstream gShaderStream;
            gShaderStream << gShaderFile.rdbuf();
            gShaderFile.close();
            geometryCode = gShaderStream.str();
         }
    }
    catch(const std::exception e)
    {
        std::cout<<"ERROR: shader file read failed"<<e.what()<<std::endl;
    }
     vShaderCode = vertexCode.c_str();
     fShaderCode = fragmentCode.c_str();

     //----------------------------------------------
     //compile Shader

    GLunit vertexShader = CreateVertexShaders((GLchar*)vShaderCode);
    GLuint fragmentShader = CreateFragmentShaders((GLchar*)fShaderCode);
    GLuint geometryShader;
        if (geometryPath != nullptr){
            gShaderCode = geometryCode.c_str();
            geometryShader = CreateGeometryShaders((GLchar*)gShaderCode);
        }

    //-------------------------------------------------
    //bind Shader
    ID = f->glCreateProgram();
    f->glAttachShader(ID,vertexShader);
    f->glAttachShader(ID,fragmentShader);
    if (geometryPath != nullptr)  f->glAttachShader(ID, geometryShader);
    f->glLinkProgram(ID);
    checkCompileErrors(ID);

    f->glDeleteShader(vertexShader);
    f->glDeleteShader(fragmentShader);
        if (geometryPath != nullptr){
            f->glDeleteShader(geometryShader);
        }
}

void QShader::use(){
    f->glUseProgram(ID);
}

void QShader::release(){
    f->glUseProgram(0);
}

void QShader::setBool(const std::string& name, bool value)const{
    f->glUniform1i(f->glGetUniformLocation(ID,name.c_str()),(int)value);
}
void QShader::setInt(const std::string& name, int value)const{
    f->glUniform1i(f->glGetUniformLocation(ID,name.c_str()),value);
}
void QShader::setFloat(const std::string& name, float value)const{
    f->glUniform1f(f->glGetAttribLocation(ID,name.c_str()),value);
}
void QShader::setVec3(const std::string& name, float x,float y,float z)const{
    f->glUniform3f(f->glGetUniformLocation(ID,name.c_str()),x,y,z);
}
void QShader::setVec3(const std::string& name, QVector3D value) const{
    f->glUniform3f(f->glGetUniformLocation(ID,name.c_str()),value.x(),value.y(),value.z());
}
void QShader::setMat4(const std::string& name, QMatrix4x4 value) const{
    f->glUniformMatrix4fv(f->glGetUniformLocation(ID,name.c_str()),1,GL_FALSE,value.data());
}
void QShader::setMat3(const std::string& name, QMatrix3x3 value) const{
    f->glUniformMatrix3fv(f->glGetUniformLocation(ID,name.c_str()),1,GL_FALSE,value.data());
}
GLuint QShader::CreateVertexShaders(const GLchar* ShaderSource){
    GLunit vertexShader;
    vertexShader =f->glCreateShader(GL_VERTEX_SHADER);
    f->glShaderSource(vertexShader,1,&ShaderSource,NULL);
    f->glCompileShader(vertexShader);
    //test if success
    GLint success;
    GLchar infoLog[512];
    f->glGetShaderiv(vertexShader,GL_COMPILE_STATUS,&success);
    if(!success){
        f->glGetShaderInfoLog(vertexShader,512,NULL,infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return vertexShader;
}

GLuint QShader::CreateGeometryShaders(const GLchar* ShaderSource) {
    GLuint geometryShader;
    geometryShader = f->glCreateShader(GL_GEOMETRY_SHADER);
    f->glShaderSource(geometryShader, 1, &ShaderSource, NULL);
    f->glCompileShader(geometryShader);

    GLint success;
    GLchar infoLog[512];
    f->glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        f->glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return geometryShader;
}

GLuint QShader::CreateFragmentShaders(const GLchar* ShaderSource) {
    GLuint fragmentShader;
    GLint success;
    GLchar infoLog[512];
    fragmentShader = f->glCreateShader(GL_FRAGMENT_SHADER);
    f->glShaderSource(fragmentShader, 1, &ShaderSource, NULL);
    f->glCompileShader(fragmentShader);

    f->glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        f->glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return fragmentShader;
}

void QShader::checkCompileErrors(GLuint shader){
    GLint success;
    GLchar infoLog[1024];

    f->glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success){
        f->glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cout << "ERROR::SHADER_COMPILATION_ERROR" << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
    }
};


