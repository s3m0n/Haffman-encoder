#include "main.h"

int main() {
    std::string plainText = "plain_text.txt";
    std::string cipherText = "encoded.txt";
    std::string decodedText = "decoded.txt";

    encode(plainText, cipherText);
    decode(cipherText, decodedText);
}
