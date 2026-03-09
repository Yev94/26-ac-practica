#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

#pragma pack(push, 1)
struct BMPHeader {
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
};

struct BMPInfoHeader {
    uint32_t size;
    int32_t  width;
    int32_t  height;
    uint16_t planes;
    uint16_t bitCount;
    uint32_t compression;
    uint32_t sizeImage;
    int32_t  xPelsPerMeter;
    int32_t  yPelsPerMeter;
    uint32_t clrUsed;
    uint32_t clrImportant;
};
#pragma pack(pop)

void convertirAGrisCPP(unsigned char* datos, int ancho, int alto) {
    int totalPixeles = ancho * alto;
    for (int i = 0; i < totalPixeles; ++i) {
        unsigned char b = datos[i * 3];
        unsigned char g = datos[i * 3 + 1];
        unsigned char r = datos[i * 3 + 2];

        unsigned char gris = static_cast<unsigned char>(0.299f * r + 0.587f * g + 0.114f * b);

        datos[i * 3] = gris;     
        datos[i * 3 + 1] = gris; 
        datos[i * 3 + 2] = gris; 
    }
}

int main() {
    string nombreEntrada = "entrada.bmp";
    string nombreSalida = "resultado_gris.bmp";

    ifstream file(nombreEntrada, ios::binary);
    if (!file) {
        cerr << "Error: No se pudo abrir la imagen " << nombreEntrada << endl;
        cerr << "Asegurate de que la imagen sea cuadrada y no supere 720px." << endl;
        return 1;
    }

    BMPHeader header;
    BMPInfoHeader infoHeader;

    file.read(reinterpret_cast<char*>(&header), sizeof(header));
    file.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));

    if (infoHeader.width != infoHeader.height) {
        cerr << "Error: La imagen debe ser cuadrada y maximo 720px." << endl;
        return 1;
    }

    int dataSize = infoHeader.width * infoHeader.height * 3;
    vector<unsigned char> pixeles(dataSize);
    file.seekg(header.offset, ios::beg);
    file.read(reinterpret_cast<char*>(pixeles.data()), dataSize);
    file.close();

    cout << "Convirtiendo imagen de " << infoHeader.width << "x" << infoHeader.height << "..." << endl;
    convertirAGrisCPP(pixeles.data(), infoHeader.width, infoHeader.height);

    ofstream outFile(nombreSalida, ios::binary);
    outFile.write(reinterpret_cast<char*>(&header), sizeof(header));
    outFile.write(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));
    outFile.write(reinterpret_cast<char*>(pixeles.data()), dataSize);
    outFile.close();

    cout << "¡Exito! Imagen guardada como: " << nombreSalida << endl;

    return 0;
}