#include <iostream>
#include <fstream>
#include <vector>


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
