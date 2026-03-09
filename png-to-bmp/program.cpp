#include <iostream>
#define STBI_ASSERT(x)
// Definiciones necesarias para que las librerías implementen las funciones
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STBIW_ASSERT(x)
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main() {
	const char* input_path = "imagen.png";
	const char* output_path = "entrada.bmp";
	
	int width, height, channels;
	
	// Forzamos la carga a 3 canales (RGB) para asegurar que el BMP sea de 24 bits
	// (8 bits por canal * 3 canales = 24 bits)
	unsigned char* img = stbi_load(input_path, &width, &height, &channels, 3);
	
	if (img == nullptr) {
		std::cerr << "Error: No se pudo cargar la imagen " << input_path << std::endl;
		return 1;
	}
	
	// El formato BMP por defecto en stb_image_write guarda 3 canales como 24-bit
	if (stbi_write_bmp(output_path, width, height, 3, img)) {
		std::cout << "Conversión exitosa: " << width << "x" << height << " pixeles." << std::endl;
	} else {
		std::cerr << "Error: No se pudo guardar la imagen " << output_path << std::endl;
	}
	
	// Liberar la memoria de la imagen
	stbi_image_free(img);
	
	return 0;
}
