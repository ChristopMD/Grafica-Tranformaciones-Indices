#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;

void framebuffer_tamanho_callback(GLFWwindow* ventana, int ancho, int alto) {
	glViewport(0, 0, ancho, alto);
}
void procesarEntrada(GLFWwindow* ventana) {
	if (glfwGetKey(ventana, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(ventana, true);
}

const unsigned int ANCHO = 800;
const unsigned int ALTO = 600;

//Definir el código de Vertex Shader
//La matriz de transformacion geometrica se indica en el vertex shader
//ya que, en el vertex shader se colocan las posiciones y
//una transformacion geometrica cambia esas posiciones


//Definir el código de Fragment Shader

class CProgrmaShaders {
	GLuint idPrograma;
public:
	CProgrmaShaders(string rutaShaderVertice, string rutaShaderFragmento) {
		//Variables para leer los archivos de código
		string strCodigoShaderVertice;
		string strCodigoShaderFragmento;
		ifstream pArchivoShaderVertice;
		ifstream pArchivoShaderFragmento;
		//Mostramos excepciones en caso haya
		pArchivoShaderVertice.exceptions(ifstream::failbit | ifstream::badbit);
		pArchivoShaderFragmento.exceptions(ifstream::failbit | ifstream::badbit);
		try
		{
			//Abriendo los archivos de código de los shader
			pArchivoShaderVertice.open(rutaShaderVertice);
			pArchivoShaderFragmento.open(rutaShaderFragmento);
			//Leyendo la información de los archivos
			stringstream lectorShaderVertice, lectorShaderFragmento;
			lectorShaderVertice << pArchivoShaderVertice.rdbuf();
			lectorShaderFragmento << pArchivoShaderFragmento.rdbuf();
			//Cerar los archivos
			pArchivoShaderVertice.close();
			pArchivoShaderFragmento.close();
			//Pasando la informacion leída a string
			strCodigoShaderVertice = lectorShaderVertice.str();
			strCodigoShaderFragmento = lectorShaderFragmento.str();
		}
		catch (ifstream::failure)
		{
			cout << "ERROR: Los archivos no pudieron ser leidos correctamente.\n";
		}
		const char* codigoShaderVertice = strCodigoShaderVertice.c_str();
		const char* codigoShaderFragmento = strCodigoShaderFragmento.c_str();
		//Asociando y compilando los códigos de los shader
		GLuint idShaderVertice, idShderFragmento;

		//Shader de Vertice
		idShaderVertice = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(idShaderVertice, 1, &codigoShaderVertice, NULL);
		glCompileShader(idShaderVertice);
		verificarErrores(idShaderVertice, "Vertice");

		//Shader de Fragmento
		idShderFragmento = glCreateShader(GL_FRAGMENT_SHADER);
		//Asociando al programa, de la tarjeta gráfica, con identificador "idShderFragmento" el código en la dirección apuntada por
		//"codigoShaderFragmento"
		glShaderSource(idShderFragmento, 1, &codigoShaderFragmento, NULL);
		//Pedimos a la tarjeta gráfica que compile el programa con identificador "idShderFragmento"
		glCompileShader(idShderFragmento);
		verificarErrores(idShderFragmento, "Fragmento");

		//Creando un programa en la tarjeta gráfica para mi shader, y obteniendo el identificador asociado

		this->idPrograma = glCreateProgram();
		//Enlazando vertex y fragment shaders
		glAttachShader(this->idPrograma, idShaderVertice);
		glAttachShader(this->idPrograma, idShderFragmento);
		glLinkProgram(this->idPrograma);
		//verificarErrores(this->idPrograma, "Programa");

		//Ahora ya podemos eliminar los programas de los shaders
		glDeleteShader(idShaderVertice);
		glDeleteShader(idShderFragmento);

	}
	~CProgrmaShaders() {
		glDeleteProgram(this->idPrograma);
	}
	void usar() const{
		glUseProgram(this->idPrograma);
	}
	void setVec3(const string &nombre, float x, float y, float z) const {
		glUniform3f(glGetUniformLocation(this->idPrograma, nombre.c_str()), x, y, z);
	}
	void setVec3(const string &nombre, const glm::vec3 &valor) const {
		glUniform3fv(glGetUniformLocation(this->idPrograma, nombre.c_str()), 1, &valor[0]);
	}
	void setMat4(const string &nombre, const glm::mat4 &mat) const {
		glUniformMatrix4fv(glGetUniformLocation(this->idPrograma, nombre.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

private:
	void verificarErrores(GLuint identificador, string tipo) {
		GLint ok;
		GLchar log[1024];

		if (tipo == "Programa") {
			glGetProgramiv(this->idPrograma, GL_LINK_STATUS, &ok);
			if (!ok) {
				glGetProgramInfoLog(this->idPrograma, 1024, NULL, log);
				cout << "Error de enlace con el programa: " << log << "\n";
			}
		}
		else {
			glGetShaderiv(identificador, GL_COMPILE_STATUS, &ok);
			if (!ok) {
				glGetShaderInfoLog(identificador, 1024, nullptr, log);
				cout << "Error de compilación con el Shader de : " << tipo <<": "<< log << "\n";
			}
		}
		
	}
};

//Global dx dy
float dx = 0.005;
float dy = 0.005;
void moverHex(float x, float y) {
	if (x >= 1) dx *= -1.0;
	else if (x <= -1)dx *= -1.0;

	if (y >= 1) dy *= -1.0;
	else if (y <= -1)dy *= -1.0;
}

int main() {
	//Inicializar glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Creando la ventana
	GLFWwindow* ventana = glfwCreateWindow(ANCHO, ALTO, "Compu Grafica", NULL, NULL);
	if (ventana == NULL) {
		cout << "Problemas al crear la ventana\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(ventana);
	glfwSetFramebufferSizeCallback(ventana, framebuffer_tamanho_callback);

	//Cargar Glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Problemas al cargar GLAD\n";
		return -1;
	}
	
	CProgrmaShaders programa_shaders = CProgrmaShaders("GLSL/codigo.vs", "GLSL/codigo.fs");

	//Definiendo la geometría de la figura en función de vértices
	float vertices[] = {
		-0.4, 0.0, 0.0, 0.0, 1.0, 0.0,
		-0.2, 0.4, 0.0, 1.0, 0.0, 0.0,
		 0.0,  0.0, 0.0, 0.0, 1.0, 1.0,

		 0.2, 0.4, 0.0, 0.0, 0.0, 1.0,

		 0.4, 0.0, 0.0, 1.0, 1.0, 0.0,

		 0.2, -0.4, 0.0, 1.0, 0.0, 1.0,

		-0.2, -0.4, 0.0, 0.0, 1.0, 0.0

	};

	unsigned int indices[] = {
		0,1,2,
		2,1,3,
		2,3,4,
		2,4,5,
		2,5,6,
		2,6,0
	};

	//Enviando la geometría al GPU: Definiendo los buffers (Vertex Array Objects y Vertex Buffer Objects)
	unsigned int id_array_vertices, id_array_buffers, id_element_buffer;
	glGenVertexArrays(1, &id_array_vertices);
	glGenBuffers(1, &id_array_buffers);
	glGenBuffers(1, &id_element_buffer);
	//Anexando los buffers para su uso en la tarjeta gráfica
	glBindVertexArray(id_array_vertices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_element_buffer);

	//Anexando buffers cargandolos con los datos
	glBindBuffer(GL_ARRAY_BUFFER, id_array_buffers);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_element_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//Indicando las especificaciones de los atributos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	float x = 0.0;
	float y = 0.9;
	while (!glfwWindowShouldClose(ventana)) {
		procesarEntrada(ventana);
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		programa_shaders.usar();
		//creando una matriz homográfica
		glm::mat4 transformacion = glm::mat4(1.0);
		
		//translacion
		transformacion = glm::translate(transformacion, glm::vec3(glm::sin((float)glfwGetTime()), glm::cos((float)glfwGetTime()), 0.0));

		//rotacion
		transformacion = glm::rotate(transformacion, (float)glfwGetTime(), glm::vec3(0, 0, 1));

		//scalamiento
		transformacion = glm::scale(transformacion, glm::vec3(glm::sin((float)glfwGetTime()), glm::sin((float)glfwGetTime()), 1.0));

		//Sepuede cambiar el orden de traslacion , rotacion y scalamiento para obtener diferentes resultados

		//programa_shaders.setMat4("transformacion", glm::vec3(glm::sin((float)glfwGetTime()), glm::cos((float)glfwGetTime()), 0.0)); //poner x e y para hacer movimiento de rombo y coseno y seno para circular
		programa_shaders.setMat4("transformacion", transformacion);
		glBindVertexArray(id_array_vertices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_element_buffer);
		//glDrawArrays(GL_TRIANGLES, 0, 18);
		glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(ventana);
		glfwPollEvents();

		
		//Movimiento de Hexagono
		//moverHex(x, y);
		//x += dx;
		//y += dy;
	}
	//Liberando memoria
	glDeleteVertexArrays(1, &id_array_vertices);
	glDeleteBuffers(1, &id_array_buffers);
	
	glfwTerminate();
	return 0;
}