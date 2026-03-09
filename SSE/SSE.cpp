#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

#pragma pack(push, 1)
struct BMPHeader {
    uint16_t type; uint32_t size; uint16_t r1; uint16_t r2; uint32_t offset;
};
struct BMPInfoHeader {
    uint32_t size; int32_t width; int32_t height; uint16_t planes; uint16_t bitCount;
    uint32_t compression; uint32_t sizeImage; int32_t xPels; int32_t yPels;
    uint32_t clrUsed; uint32_t clrImportant;
};
#pragma pack(pop)

void convertirAGrisSSE(unsigned char* datos, int ancho, int alto) {
    int totalPixeles = ancho * alto;

    float coeffB = 0.114f;
    float coeffG = 0.587f;
    float coeffR = 0.299f;

    _asm {
        mov esi, datos
        mov ecx, totalPixeles

        movss xmm5, coeffB
        shufps xmm5, xmm5, 0
        movss xmm6, coeffG
        shufps xmm6, xmm6, 0
        movss xmm7, coeffR
        shufps xmm7, xmm7, 0

        bucle_sse:
        test ecx, ecx
            jz fin_sse

            movzx eax, byte ptr[esi]
            movzx ebx, byte ptr[esi + 1]
            movzx edx, byte ptr[esi + 2]

            cvtsi2ss xmm1, eax
            cvtsi2ss xmm2, ebx
            cvtsi2ss xmm3, edx

            mulss xmm1, xmm5
            mulss xmm2, xmm6
            mulss xmm3, xmm7

            addss xmm1, xmm2
            addss xmm1, xmm3

            cvttss2si eax, xmm1

            mov byte ptr[esi], al
            mov byte ptr[esi + 1], al
            mov byte ptr[esi + 2], al

            add esi, 3
            dec ecx
            jmp bucle_sse

            fin_sse :
    }
}

int main() {
    string nombreEntrada = "entrada.bmp";
    ifstream file(nombreEntrada, ios::binary);
    if (!file) { cerr << "Error al abrir entrada.bmp" << endl; return 1; }

    BMPHeader header;
    BMPInfoHeader info;
    file.read((char*)&header, sizeof(header));
    file.read((char*)&info, sizeof(info));

    int dataSize = info.width * info.height * 3;
    vector<unsigned char> pixeles(dataSize);
    file.seekg(header.offset, ios::beg);
    file.read((char*)pixeles.data(), dataSize);
    file.close();

    cout << "Procesando " << info.width << "x" << info.height << " con SSE..." << endl;

    convertirAGrisSSE(pixeles.data(), info.width, info.height);

    ofstream outFile("resultado_sse.bmp", ios::binary);
    outFile.write((char*)&header, sizeof(header));
    outFile.write((char*)&info, sizeof(info));
    outFile.write((char*)pixeles.data(), dataSize);
    outFile.close();

    cout << "¡Exito! Imagen guardada como resultado_sse.bmp" << endl;
    return 0;
}