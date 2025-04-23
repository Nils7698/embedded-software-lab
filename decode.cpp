#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>

// Satellite configuration structure
struct SatelliteConfig {
    int id;
    int tap1;
    int tap2;
    int t;
};

// Configuration of all 24 satellites
std::vector<SatelliteConfig> satelliteConfigs = {
    {1, 2, 6, 5},   {2, 3, 7, 6},   {3, 4, 8, 7},   {4, 5, 9, 8},   {5, 1, 9, 17},
    {6, 2, 10, 18}, {7, 1, 8, 139}, {8, 2, 9, 140}, {9, 3, 10, 141}, {10, 2, 3, 251},
    {11, 3, 4, 252}, {12, 5, 6, 254}, {13, 6, 7, 255}, {14, 7, 8, 256}, {15, 8, 9, 257},
    {16, 9, 10, 258}, {17, 1, 4, 469}, {18, 2, 5, 470}, {19, 3, 6, 471}, {20, 4, 7, 472},
    {21, 5, 8, 473}, {22, 6, 9, 474}, {23, 1, 3, 509}, {24, 4, 6, 512}
};

// Function to read the signal from a file
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

// Function to generate the Gold code for a satellite
std::vector<int> generateGoldCode(const SatelliteConfig& config) {
    std::vector<int> g1(1023, 1); // G1 register
    std::vector<int> g2(1023, 1); // G2 register
    std::vector<int> goldCode(1023);

    for (int i = 0; i < 1023; ++i) {
        // Generate G1
        int g1NewBit = g1[2] ^ g1[9];
        g1.insert(g1.begin(), g1NewBit);
        g1.pop_back();

        // Generate G2
        int g2NewBit = g2[config.tap1 - 1] ^ g2[config.tap2 - 1];
        g2.insert(g2.begin(), g2NewBit);
        g2.pop_back();

        // Combine G1 and G2 to form the Gold code
        goldCode[i] = g1.back() ^ g2[config.t - 1];
    }

    return goldCode;
}

// Function to decode the sum signal
void decodeSignal(const std::vector<int>& signal) {
    for (const auto& config : satelliteConfigs) {
        std::vector<int> goldCode = generateGoldCode(config);

        // Correlate the signal with the Gold code
        int correlation = 0;
        for (size_t i = 0; i < signal.size(); ++i) {
            correlation += signal[i] * goldCode[i];
        }

        // Check if the correlation is strong enough to identify the satellite
        if (correlation > 20) { // Threshold for detection
            std::cout << "Satellite " << config.id << " detected with correlation: " << correlation << std::endl;
        }
    }
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
        if (signal.size() != 1023) {
            std::cerr << "Anzahl Elemente in Summensignal != 1023! \n";
        } else {
            std::cout << "Anzahl Werte: " << signal.size() << "\n";
            decodeSignal(signal);
        }
    }

    return 0;
}
