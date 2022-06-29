#include "QShader.h"

QShader::QShader(QString& vertexPath, QString& fragmentPath,QString& geometryPath)
{
    initializeOpenGLFunctions();
    //read Shader from file
    QFile vShaderFile(vertexPath);
    QFile fShaderFile(fragmentPath);
    QFile gShaderFile(geometryPath);
    const char* vShaderCode;
    const char* fShaderCode;
    const char* gShaderCode;


        qDebug()<<vertexPath.toStdString().c_str();

        if(!vShaderFile.open(QIODevice::ReadOnly)){
            qDebug()<<"vertexShader read failed";
        }

        if(!fShaderFile.open(QIODevice::ReadOnly)){
            qDebug()<<"fragmentShader read failed";
        }
        QByteArray vShaderByteArray, fShaderByteArray;

        vShaderByteArray = vShaderFile.readAll();
        fShaderByteArray = fShaderFile.readAll();

        vShaderFile.close();
        fShaderFile.close();

        vShaderCode = vShaderByteArray.data();
        fShaderCode = fShaderByteArray.data();

        if (!geometryPath.isEmpty()){
            gShaderFile.open(QIODevice::ReadOnly);
            QByteArray gShaderByteArray;
            gShaderByteArray = gShaderFile.readAll();
            gShaderFile.close();
            gShaderCode = gShaderByteArray.data();
         }


     //----------------------------------------------
     //compile Shader

    GLunit vertexShader = CreateVertexShaders((GLchar*)vShaderCode);
    GLuint fragmentShader = CreateFragmentShaders((GLchar*)fShaderCode);
    GLuint geometryShader;
        if (!geometryPath.isEmpty()){
            geometryShader = CreateGeometryShaders((GLchar*)gShaderCode);
        }

    //-------------------------------------------------
    //bind Shader
    ID = glCreateProgram();
    glAttachShader(ID,vertexShader);
    glAttachShader(ID,fragmentShader);
    if (!geometryPath.isEmpty())  glAttachShader(ID, geometryShader);
    glLinkProgram(ID);
    checkCompileErrors(ID);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
        if (!geometryPath.isEmpty()){
            glDeleteShader(geometryShader);
        }
}

void QShader::use(){
    glUseProgram(ID);
}

void QShader::release(){
    glUseProgram(0);
}

void QShader::setBool(const std::string& name, bool value){
    glUniform1i(glGetUniformLocation(ID,name.c_str()),(int)value);
}
void QShader::setInt(const std::string& name, int value){
    glUniform1i(glGetUniformLocation(ID,name.c_str()),value);
}
void QShader::setFloat(const std::string& name, float value){
    glUniform1f(glGetAttribLocation(ID,name.c_str()),value);
}
void QShader::setVec3(const std::string& name, float x,float y,float z){
    glUniform3f(glGetUniformLocation(ID,name.c_str()),x,y,z);
}
void QShader::setVec3(const std::string& name, vec3 value) {
    glUniform3f(glGetUniformLocation(ID,name.c_str()),value.x(),value.y(),value.z());
}
void QShader::setVec4(const std::string& name,float x,float y,float z,float w){
    glUniform4f(glGetUniformLocation(ID,name.c_str()),x,y,z,w);
}
void QShader::setMat4(const std::string& name, QMatrix4x4 value) {
    glUniformMatrix4fv(glGetUniformLocation(ID,name.c_str()),1,GL_FALSE,value.data());
}
void QShader::setMat3(const std::string& name, QMatrix3x3 value) {
    glUniformMatrix3fv(glGetUniformLocation(ID,name.c_str()),1,GL_FALSE,value.data());
}
GLuint QShader::CreateVertexShaders(const GLchar* ShaderSource){
    GLunit vertexShader;
    vertexShader =glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader,1,&ShaderSource,NULL);
    glCompileShader(vertexShader);
    //test if success
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader,GL_COMPILE_STATUS,&success);
    if(!success){
        glGetShaderInfoLog(vertexShader,512,NULL,infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return vertexShader;
}

GLuint QShader::CreateGeometryShaders(const GLchar* ShaderSource) {
    GLuint geometryShader;
    geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometryShader, 1, &ShaderSource, NULL);
    glCompileShader(geometryShader);

    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return geometryShader;
}

GLuint QShader::CreateFragmentShaders(const GLchar* ShaderSource) {
    GLuint fragmentShader;
    GLint success;
    GLchar infoLog[512];
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &ShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return fragmentShader;
}

void QShader::checkCompileErrors(GLuint shader){
    GLint success;
    GLchar infoLog[1024];

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success){
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cout << "ERROR::SHADER_COMPILATION_ERROR" << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
    }
};


