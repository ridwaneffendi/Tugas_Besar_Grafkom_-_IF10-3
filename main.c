#include <math.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>


 
//rotasi angle
float xpos = 0, ypos = 0, zpos = 0, xrot = 0, yrot = 0;
float _angle = -70.0f;
GLfloat lightPos1[] = {0.0f, 0.0f, 0.0f, 0.0f};

GLuint texture[4];
GLint slices = 16; 
GLint stacks = 16;

struct Image {
	unsigned long sizeX;
	unsigned long sizeY;
	char *data;
};
typedef struct Image Image; //struktur data untuk


//ukuran image  sesuai kebutuhan
#define checkImageWidth 164
#define checkImageHeight 164

GLubyte checkImage[checkImageWidth][checkImageHeight][2];

//membuat citra sendiri 
void makeCheckImage(void) {
	int i, j, c;
	for (i = 0; i < checkImageWidth; i++) {
		for (j = 0; j < checkImageHeight; j++) {
			c = ((((i & 0x8) == 0) ^ ((j & 0x1) == 0))) * 255;
			checkImage[i][j][0] = (GLubyte) c;
			checkImage[i][j][1] = (GLubyte) c;
			checkImage[i][j][2] = (GLubyte) c;
		}
	}
}

//mengambil gambar BMP
int ImageLoad(char *filename, Image *image) {
	FILE *file;
	unsigned long size; // ukuran image dalam bytes
	unsigned long i; // standard counter.
	unsigned short int plane; // number of planes in image

	unsigned short int bpp; // jumlah bits per pixel
	char temp; // temporary color storage for var warna sementara untuk memastikan filenya ada


	if ((file = fopen(filename, "rb")) == NULL) {
		printf("File Not Found : %s\n", filename);
		return 0;
	}
	// mencari file header bmp
	fseek(file, 18, SEEK_CUR);
	// read the width
	if ((i = fread(&image->sizeX, 4, 1, file)) != 1) {
		printf("Error reading width from %s.\n", filename);
		return 0;
	}
	//printf("Width of %s: %lu\n", filename, image->sizeX);
	// membaca nilai height
	if ((i = fread(&image->sizeY, 4, 1, file)) != 1) {
		printf("Error reading height from %s.\n", filename);
		return 0;
	}
	//printf("Height of %s: %lu\n", filename, image->sizeY);
	//menghitung ukuran image(asumsi 24 bits or 3 bytes per pixel).

	size = image->sizeX * image->sizeY * 3;
	// read the planes
	if ((fread(&plane, 2, 1, file)) != 1) {
		printf("Error reading planes from %s.\n", filename);
		return 0;
	}
	if (plane != 1) {
		printf("Planes from %s is not 1: %u\n", filename, plane);
		return 0;
	}
	// read the bitsperpixel
	if ((i = fread(&bpp, 2, 1, file)) != 1) {
		printf("Error reading bpp from %s.\n", filename);

		return 0;
	}
	if (bpp != 24) {
		printf("Bpp from %s is not 24: %u\n", filename, bpp);
		return 0;
	}
	// seek past the rest of the bitmap header.
	fseek(file, 24, SEEK_CUR);
	// read the data.
	image->data = (char *) malloc(size);
	if (image->data == NULL) {
		printf("Error allocating memory for color-corrected image data");
		return 0;
	}
	if ((i = fread(image->data, size, 1, file)) != 1) {
		printf("Error reading image data from %s.\n", filename);
		return 0;
	}
	for (i = 0; i < size; i += 3) { // membalikan semuan nilai warna (gbr - > rgb)
		temp = image->data[i];
		image->data[i] = image->data[i + 2];
		image->data[i + 2] = temp;
	}
	// we're done.
	return 1;
}

//mengambil tekstur
Image * loadTexture() {
	Image *image1;
	// alokasi memmory untuk tekstur
	image1 = (Image *) malloc(sizeof(Image));
	if (image1 == NULL) {
		printf("Error allocating space for image");
		exit(0);
	}
	//pic.bmp is a 64x64 picture
	if (!ImageLoad("woods.bmp", image1)) {
		exit(1);
	}
	return image1;
}

Image * loadTextureDua() {
	Image *image2;
	// alokasi memmory untuk tekstur
	image2 = (Image *) malloc(sizeof(Image));
	if (image2 == NULL) {
		printf("Error allocating space for image");
		exit(0);
	}
	//pic.bmp is a 64x64 picture
	if (!ImageLoad("keyboard.bmp", image2)) {
		exit(1);
	}
	return image2;
}
Image * loadTextureTiga() {
	Image *image3;
	// alokasi memmory untuk tekstur
	image3 = (Image *) malloc(sizeof(Image));
	if (image3 == NULL) {
		printf("Error allocating space for image");
		exit(0);
	}
	//pic.bmp is a 64x64 picture
	if (!ImageLoad("layar.bmp", image3)) {
		exit(1);
	}
	return image3;
}



void myinit(void) {
	glClearColor(1.0, 1.0, 1.0, 0.0); //warna untuk background
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	Image *image1 = loadTexture();
	Image *image2 = loadTextureDua();
        Image *image3 = loadTextureTiga();
       

	if (image1 == NULL) {
		printf("Image was not returned from loadTexture\n");
		exit(0);
	}

	makeCheckImage(); //memanggil prodesur makeCheckImage

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);   

	// Generate texture/ membuat texture
	glGenTextures(4, texture);

	//binding atau memasang texture untuk membuat texture 2D
	glBindTexture(GL_TEXTURE_2D, texture[0]);

	//menyesuaikan ukuran textur ketika image lebih besar dari texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //
	//menyesuaikan ukuran textur ketika image lebih kecil dari texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //

	glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->sizeX, image1->sizeY, 0, GL_RGB,
			GL_UNSIGNED_BYTE, image1->data);



	//binding texture untuk membuat texture 2D
	glBindTexture(GL_TEXTURE_2D, texture[1]);

	//menyesuaikan ukuran textur ketika image lebih besar dari texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //
	//menyesuaikan ukuran textur ketika image lebih kecil dari texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //

	glTexImage2D(GL_TEXTURE_2D, 0, 3, image2->sizeX, image2->sizeY, 0, GL_RGB,
			GL_UNSIGNED_BYTE, image2->data);
        

	//baris tekstur buatan 
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);



	//binding texture baru 
	glBindTexture(GL_TEXTURE_2D, texture[3]);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	glTexImage2D(GL_TEXTURE_2D, 0, 3, checkImageWidth, checkImageHeight, 0,
			GL_RGB, GL_UNSIGNED_BYTE, &checkImage[0][0][0]);

	glEnable(GL_TEXTURE_2D);

	glShadeModel(GL_FLAT);
}
	 
	
	 
	//Called when a key is pressed
	void handleKeypress(unsigned char key, int x, int y) {
	     
	    if (key=='w')
	     {
	     xrot += 1;
	     if (xrot >360) xrot -= 360;
	     }
	  
	    if (key=='s')
	     {
	     xrot -= 1;
	     if (xrot < -360) xrot += 360;
	     }
	  
	    if (key=='a')
	     {
	     yrot += 1;
	     if (yrot >360) yrot -= 360;
	     }
	  
	    if (key=='d')
	     {
	     yrot -= 1;
	     if (yrot < -360)yrot += 360;
	     }
	  
	 
	    switch (key) {
	        case 27: //Escape key
	            exit(0);
	    }
	}
	 
	//set camera
	void camera (void) {
	     glRotatef(xrot,1.0,0.0,0.0);
	     glRotatef(yrot,0.0,1.0,0.0);
	     
	 }
	 
	//Initializes 3D rendering
	void initRendering() {
	    glEnable(GL_DEPTH_TEST);
	    glEnable(GL_COLOR_MATERIAL);
	    glEnable(GL_LIGHTING); //Enable lighting
	    glEnable(GL_LIGHT0);
	}
	 
	//Called when the window is resized
	void handleResize(int w, int h) {
	    glViewport(0, 0, w, h); //menghandle ketika objek di resize
	    glMatrixMode(GL_PROJECTION);
	    glLoadIdentity();
	    gluPerspective(30.0, (double)w / (double)h, 1.0, 200.0);
	}
	 
	//Draws the 3D scene
	void drawScene() {
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	     
	 
	    glMatrixMode(GL_MODELVIEW);
	    glLoadIdentity();
	     
	    glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
	 
	    //set new camera position
	    camera();
	 
	    /** menggambar **/
	 
	    glTranslatef(0.0f, 0.0f, -20.0f);
	    glColor3f(0.1f, 0.1f, 0.0f);
	 
	    glBindTexture(GL_TEXTURE_2D, texture[0]);
          
            glBegin(GL_QUADS);
            
            glTexCoord2f(1.0, 0.0);
	    glVertex3f(-5.0f, -5.0f, 0.0f); //kiri-bawah
	 
            glTexCoord2f(1.0, 1.0);
	     glVertex3f(-5.0f, -5.0f, -20.0f);//kiri-atas
	   
             glTexCoord2f(0.0, 1.0);
	     glVertex3f(5.0f, -5.0f, -20.0f);//kanan-atas
             
	      glTexCoord2f(0.0, 0.0);
	     glVertex3f(5.0f, -5.0f, 0.0f);//kanan-bawah
                glEnd(); 
                
            
        
                
            //untuk menggerakkan objek
	    glRotatef(_angle, 0.0f, 1.0f, 0.5f);
	    glColor3f(0.1f, 0.1f, 0.1f);
            
	     
	    //keyboard
	     glPushMatrix();
             glScaled(1.5, 0.05, 1.0);
             glBindTexture(GL_TEXTURE_2D, texture[1]);
             glutSolidCube(3.0);
             glPopMatrix();
	     
             glPushMatrix();
             glBindTexture(GL_TEXTURE_2D, texture[1]);
             glTranslatef(0,0,1.4);
            glBegin(GL_QUADS);
            
            glTexCoord2f(1.0, 0.0);
	    glVertex3f(-2.0f, 0.1f, 0.0f); //kiri-bawah
	 
            glTexCoord2f(1.0, 1.0);
	     glVertex3f(-2.0f, 0.1f, -2.5f);//kiri-atas
	   
             glTexCoord2f(0.0, 1.0);
	     glVertex3f(2.0f, 0.1f, -2.5f);//kanan-atas
             
	      glTexCoord2f(0.0, 0.0);
	     glVertex3f(2.0f, 0.1f, 0.0f);//kanan-bawah
                glEnd(); 
             glPopMatrix();
	    //layar
	     glPushMatrix();
             glScaled(1.5, 1, 0.05);
             glTranslated(0, 1.65, -33);
             glBindTexture(GL_TEXTURE_2D, texture[1]);
             glutSolidCube(3.0);
             glPopMatrix();
             
             glPushMatrix();
             glBindTexture(GL_TEXTURE_2D, texture[2]);
             glTranslatef(0,0,1.5);
             glBegin(GL_QUADS);
            
             glTexCoord2f(1.0, 0.0);
	     glVertex3f(-2.0f, 3.0f, -3.0f); //kiri-bawah
	 
             glTexCoord2f(1.0, 1.0);
	     glVertex3f(-2.0f, 0.5f, -3.0f);//kiri-atas
	   
             glTexCoord2f(0.0, 1.0);
	     glVertex3f(2.0f, 0.5f, -3.0f);//kanan-atas
             
	     glTexCoord2f(0.0, 0.0);
	     glVertex3f(2.0f, 3.0f, -3.0f);//kanan-bawah
             glEnd(); 
             glPopMatrix();
             
             //engsel
             GLUquadric *Cy;
             Cy = gluNewQuadric ();
             glPushMatrix();
             glTranslated(-2.25, 0.0, -1.6);
             glRotated(90, 0.5, 5, 0.5);
             glScaled(1.5, 1.5, 3);
            glBindTexture(GL_TEXTURE_2D, texture[1]);
             gluCylinder(Cy, 0.1, 0.1, 1.5, 50, 10);
             glPopMatrix();
	     
	    glutSwapBuffers();
	}
	 
	void update(int value) {
	    _angle += 1.5f;
	    if (_angle > 360) {
	        _angle -= 360;
	    }
	     
	    glutPostRedisplay();
	    glutTimerFunc(25, update, 0);
	}
	 
	int main(int argc, char** argv) {
	    //Initialize GLUT
	    glutInit(&argc, argv);
	    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	    glutInitWindowSize(800, 600);
	     
	    //Create the window
	    glutCreateWindow("LAPTOP 3D");
	    initRendering();
            myinit();
	    glutDisplayFunc(drawScene);
	    glutKeyboardFunc(handleKeypress);
	    glutReshapeFunc(handleResize);
	     
	    glutTimerFunc(25, update, 0); //Add a timer
	     
	    glutMainLoop();
	    return 0;
	}
