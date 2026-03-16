#include <iostream>
#include <fstream>
#include <vector>
#include <string>

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



void convertirAGrisAssembly(unsigned char* datos, int ancho, int alto) {
    int totalPixeles = ancho * alto;

    float coeffB = 0.114f;
    float coeffG = 0.587f;
    float coeffR = 0.299f;
    // Variables locales auxiliares (accesibles por nombre desde el bloque asm)
    int   fpu_temp;
    short cw_guardado;
    short cw_truncar;

    _asm {
        mov esi, datos
        mov ecx, totalPixeles

        // Configurar FPU en modo TRUNCACIÓN (igual que cvttss2si) 
        // Por defecto la FPU redondea al más cercano
        fstcw cw_guardado          
        mov   ax, cw_guardado
        or ax, 0x0C00           
        mov   cw_truncar, ax
        fldcw cw_truncar          

        bucle :
        test ecx, ecx
            jz   fin

            movzx eax, byte ptr[esi]       // eax = B  (canal azul)
            movzx ebx, byte ptr[esi + 1]   // ebx = G  (canal verde)
            movzx edx, byte ptr[esi + 2]   // edx = R  (canal rojo)

            //  B * coeffB 
            mov  fpu_temp, eax
            fild fpu_temp              
            fmul coeffB                

            // G * coeffG 
            mov  fpu_temp, ebx
            fild fpu_temp              
            fmul coeffG                

            // R * coeffR 
            mov  fpu_temp, edx
            fild fpu_temp              
            fmul coeffR                

            //Sumar los tres productos
            faddp st(1), st(0)         
            faddp st(1), st(0)         

            // Convertir a entero (truncar)
            // (sustituye: cvttss2si eax,xmm1)
            fistp fpu_temp            
            mov   eax, fpu_temp        // eax = gris final

            // Escribir el mismo valor en los 3 canales
            mov byte ptr[esi], al
            mov byte ptr[esi + 1], al
            mov byte ptr[esi + 2], al

            add esi, 3
            dec ecx
            jmp bucle

            fin :
        // Restaurar redondeo FPU
        fldcw cw_guardado
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
    convertirAGrisAssembly(pixeles.data(), infoHeader.width, infoHeader.height);

    ofstream outFile(nombreSalida, ios::binary);
    outFile.write(reinterpret_cast<char*>(&header), sizeof(header));
    outFile.write(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));
    outFile.write(reinterpret_cast<char*>(pixeles.data()), dataSize);
    outFile.close();

    cout << "¡Exito! Imagen guardada como: " << nombreSalida << endl;

    return 0;
}
