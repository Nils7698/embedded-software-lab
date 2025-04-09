#include <iostream>
#include <fstream>
#include <vector>

struct SatelliteConfig {
    int id;
    int tap1;
    int tap2;
    int t;
};

// Konfiguration aller 24 Satelliten
std::vector<SatelliteConfig> satelliteConfigs = {
    {1, 2, 6, 5},   {2, 3, 7, 6},   {3, 4, 8, 7},   {4, 5, 9, 8},   {5, 1, 9, 17},
    {6, 2, 10, 18}, {7, 1, 8, 139}, {8, 2, 9, 140}, {9, 3, 10, 141}, {10, 2, 3, 251},
    {11, 3, 4, 252}, {12, 5, 6, 254}, {13, 6, 7, 255}, {14, 7, 8, 256}, {15, 8, 9, 257},
    {16, 9, 10, 258}, {17, 1, 4, 469}, {18, 2, 5, 470}, {19, 3, 6, 471}, {20, 4, 7, 472},
    {21, 5, 8, 473}, {22, 6, 9, 474}, {23, 1, 3, 509}, {24, 4, 6, 512}
};

std::vector<int> readFile(const std::string& filename) {
    std::ifstream inputFile(filename);
    if (!inputFile) {
        std::cerr << "Fehler: Datei konnte nicht geöffnet werden!" << std::endl;
        return {};
    }

    std::vector<int> numbers;
    int num;

    while (inputFile >> num) {
        numbers.push_back(num);
    }

    inputFile.close();
    return numbers;
}

int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::cerr << "Fehler: " << argv[0] << " <Dateiname>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    std::cout << "Lese Datei " << filename << std::endl;
    std::vector<int> signal = readFile(filename);

    if (!signal.empty()) {
        std::cout << "Eingelesene Zahlen:\n";
        for (int n : signal) {
            std::cout << n << " | ";
        }
        std::cout << std::endl;
    }

    return 0;
}
