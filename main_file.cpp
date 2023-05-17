/*
Niniejszy program jest wolnym oprogramowaniem; możesz go
rozprowadzać dalej i / lub modyfikować na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundację Wolnego
Oprogramowania - według wersji 2 tej Licencji lub(według twojego
wyboru) którejś z późniejszych wersji.

Niniejszy program rozpowszechniany jest z nadzieją, iż będzie on
użyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyślnej
gwarancji PRZYDATNOŚCI HANDLOWEJ albo PRZYDATNOŚCI DO OKREŚLONYCH
ZASTOSOWAŃ.W celu uzyskania bliższych informacji sięgnij do
Powszechnej Licencji Publicznej GNU.

Z pewnością wraz z niniejszym programem otrzymałeś też egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
jeśli nie - napisz do Free Software Foundation, Inc., 59 Temple
Place, Fifth Floor, Boston, MA  02110 - 1301  USA
*/

#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "myCube.h"
#include "myTeapot.h"
#include "threeDModel.h"
#include <iostream>
#include <cmath>

float speed_x=0;
float speed_y=0;
float speed =2*PI;
float aspectRatio=1;

ShaderProgram *sp;

threeDModel connectingRod1("objects/connecting_rod1.obj", 1);
threeDModel connectingRod2("objects/connecting_rod2.obj", -1);
threeDModel connectingRod3("objects/connecting_rod3.obj", -1);
threeDModel connectingRod4("objects/connecting_rod4.obj", 1);
std::vector<threeDModel> connectingRods = {
	connectingRod1,
	connectingRod2,
	connectingRod3,
	connectingRod4,
};
threeDModel crankshaft("objects/crankshaft.obj", 1);
threeDModel piston1("objects/piston1.obj", 1);
threeDModel piston2("objects/piston2.obj", -1);
threeDModel piston3("objects/piston3.obj", -1);
threeDModel piston4("objects/piston4.obj", 1);
std::vector<threeDModel> pistons = {
	piston1,
	piston2,
	piston3,
	piston4,
};


//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}


void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action==GLFW_PRESS) {
        if (key==GLFW_KEY_LEFT) speed_x=-PI/2;
        if (key==GLFW_KEY_RIGHT) speed_x=PI/2;
        if (key==GLFW_KEY_UP) speed_y=PI/2;
        if (key==GLFW_KEY_DOWN) speed_y=-PI/2;
    }
    if (action==GLFW_RELEASE) {
        if (key==GLFW_KEY_LEFT) speed_x=0;
        if (key==GLFW_KEY_RIGHT) speed_x=0;
        if (key==GLFW_KEY_UP) speed_y=0;
        if (key==GLFW_KEY_DOWN) speed_y=0;
    }
}

void windowResizeCallback(GLFWwindow* window,int width,int height) {
    if (height==0) return;
    aspectRatio=(float)width/(float)height;
    glViewport(0,0,width,height);
}

//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który należy wykonać raz, na początku programu************
	glClearColor(0,0,0,1);
	glEnable(GL_DEPTH_TEST);
	glfwSetWindowSizeCallback(window,windowResizeCallback);
	
	
	glfwSetKeyCallback(window,keyCallback);

	sp=new ShaderProgram("v_simplest.glsl",NULL,"f_simplest.glsl");
}


//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
    //************Tutaj umieszczaj kod, który należy wykonać po zakończeniu pętli głównej************

    delete sp;
}




//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window,float angle_x,float angle_y, float angle, float rodAngle) {
	bool ifCrankshaft = true;
	bool ifConnectingRod = true;
	bool ifPiston = true;

	//************Tutaj umieszczaj kod rysujący obraz******************l
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 V=glm::lookAt(
         glm::vec3(0, 0, -4),
         glm::vec3(0,0,0),
         glm::vec3(0.0f,1.0f,0.0f)); //Wylicz macierz widoku
	
	glm::mat4 P = glm::perspective(glm::radians(50.0f), 1.0f, 1.0f, 50.0f); //Wyliczenie macierzy rzutowania

    glm::mat4 M=glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(0, -0.5, 0));
	M=glm::rotate(M,angle_x,glm::vec3(0.0f,1.0f,0.0f)); //Wylicz macierz modelu
	M=glm::rotate(M,angle_y,glm::vec3(0.0f,0.0f,1.0f)); //Wylicz macierz modelu
	
	M = glm::scale(M, glm::vec3(0.2, 0.2, 0.2));
	sp->use();//Aktywacja programu cieniującego
	//Przeslij parametry programu cieniującego do karty graficznej
	glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));
	
	glm::mat4 wal, korbowod, tlok;
	if (ifCrankshaft) {
		wal = glm::rotate(M, angle, glm::vec3(1, 0, 0));
		glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(wal));
		glEnableVertexAttribArray(sp->a("vertex"));  //Włącz przesyłanie danych do atrybutu vertex
		glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, &crankshaft.verts[0]); //Wskaż tablicę z danymi dla atrybutu vertex
		glEnableVertexAttribArray(sp->a("color"));
		glVertexAttribPointer(sp->a("color"), 4, GL_FLOAT, false, 0, &crankshaft.colors[0]);
		glEnableVertexAttribArray(sp->a("normal"));  //Włącz przesyłanie danych do atrybutu normal
		glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, &crankshaft.norms[0]);
		glDrawArrays(GL_TRIANGLES, 0, crankshaft.vertexCount); //Narysuj obiekt
		glDisableVertexAttribArray(sp->a("vertex"));  //Wyłącz przesyłanie danych do atrybutu vertex
		glDisableVertexAttribArray(sp->a("color"));
		glDisableVertexAttribArray(sp->a("normal"));
	}
	for (int i = 0; i < 4; i++) {
		if (ifPiston) {
			tlok = glm::translate(M, glm::vec3(0, pistons[i].movementDirection*(0.8 * cos(angle) - 0.78), 0));
			glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(tlok));
			glEnableVertexAttribArray(sp->a("vertex"));
			glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, &pistons[i].verts[0]);
			glEnableVertexAttribArray(sp->a("color"));
			glVertexAttribPointer(sp->a("color"), 4, GL_FLOAT, false, 0, &pistons[i].colors[0]);
			glEnableVertexAttribArray(sp->a("normal"));
			glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, &pistons[i].norms[0]);
			glDrawArrays(GL_TRIANGLES, 0, pistons[i].vertexCount); //Narysuj obiekt
			glDisableVertexAttribArray(sp->a("vertex"));
			glDisableVertexAttribArray(sp->a("color"));
			glDisableVertexAttribArray(sp->a("normal"));
		}
		if (ifConnectingRod) {
			korbowod = glm::translate(tlok, glm::vec3(0, 0, connectingRods[i].movementDirection * 0.8 * sin(angle)));
			korbowod = glm::rotate(korbowod, connectingRods[i].movementDirection * rodAngle, glm::vec3(1, 0, 0));
			glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(korbowod));
			glEnableVertexAttribArray(sp->a("vertex"));  //Włącz przesyłanie danych do atrybutu vertex
			glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, &connectingRods[i].verts[0]); //Wskaż tablicę z danymi dla atrybutu vertex
			glEnableVertexAttribArray(sp->a("color"));
			glVertexAttribPointer(sp->a("color"), 4, GL_FLOAT, false, 0, &connectingRods[i].colors[0]);
			glEnableVertexAttribArray(sp->a("normal"));  //Włącz przesyłanie danych do atrybutu normal
			glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, &connectingRods[i].norms[0]);
			glDrawArrays(GL_TRIANGLES, 0, connectingRods[i].vertexCount); //Narysuj obiekt
			glDisableVertexAttribArray(sp->a("vertex"));  //Wyłącz przesyłanie danych do atrybutu vertex
			glDisableVertexAttribArray(sp->a("color"));
			glDisableVertexAttribArray(sp->a("normal"));
		}
	}
    glfwSwapBuffers(window); //Przerzuć tylny bufor na przedni
}


int main(void)
{
	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów

	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(800, 800, "OpenGL", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

	if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekę GLEW
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Operacje inicjujące

	//Główna pętla
	float angle_x=0; //Aktualny kąt obrotu obiektu
	float angle_y=0; //Aktualny kąt obrotu obiektu
	float angle = 0;
	float rodAngle = 0, rodAngle1 = 0;
	glfwSetTime(0); //Zeruj timer
	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{
		if (angle >= 20 * PI) angle = 0;
		angle += speed * glfwGetTime();
        angle_x+=speed_x*glfwGetTime(); //Zwiększ/zmniejsz kąt obrotu na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki
        angle_y+=speed_y*glfwGetTime(); //Zwiększ/zmniejsz kąt obrotu na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki
		std::cout << angle << std::endl;
		rodAngle += -cos(angle)*PI*10/180*speed * glfwGetTime();
        glfwSetTime(0); //Zeruj timer
		drawScene(window,angle_x,angle_y, angle, rodAngle); //Wykonaj procedurę rysującą
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}
