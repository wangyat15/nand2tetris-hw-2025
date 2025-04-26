#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <bitset>
#include <algorithm>
#include <cctype>
using namespace std;

// this is a hel-per function that trims whitespace from both ends of a string
string trim(const string &s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos)
        return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

// Remove comments BOM and then trim by using helper function the line
string cleanLine(string line) {
    // Check for UTF-8 BOM at the start
    const string BOM = "\xEF\xBB\xBF";
    if (line.rfind(BOM, 0) == 0) {
        line = line.substr(BOM.size());
    }
    // Remove comments 
    size_t pos = line.find("//");
    if (pos != string::npos) {
        line = line.substr(0, pos);
    }
    return trim(line);
}

// Convertiung destination mnemonic to its 3-bit binary code
string destToBinary(const string &dest) {
    if(dest == "M")   return "001";
    if(dest == "D")   return "010";
    if(dest == "MD")  return "011";
    if(dest == "A")   return "100";
    if(dest == "AM")  return "101";
    if(dest == "AD")  return "110";
    if(dest == "AMD") return "111";
    return "000"; 
}

// Converting computation mnemonic to its 7-bit binary code
string compToBinary(const string &comp) {
    if(comp == "0")    return "0101010";
    if(comp == "1")    return "0111111";
    if(comp == "-1")   return "0111010";
    if(comp == "D")    return "0001100";
    if(comp == "A")    return "0110000";
    if(comp == "!D")   return "0001101";
    if(comp == "!A")   return "0110001";
    if(comp == "-D")   return "0001111";
    if(comp == "-A")   return "0110011";
    if(comp == "D+1")  return "0011111";
    if(comp == "A+1")  return "0110111";
    if(comp == "D-1")  return "0001110";
    if(comp == "A-1")  return "0110010";
    if(comp == "D+A")  return "0000010";
    if(comp == "D-A")  return "0010011";
    if(comp == "A-D")  return "0000111";
    if(comp == "D&A")  return "0000000";
    if(comp == "D|A")  return "0010101";
    if(comp == "M")    return "1110000";
    if(comp == "!M")   return "1110001";
    if(comp == "-M")   return "1110011";
    if(comp == "M+1")  return "1110111";
    if(comp == "M-1")  return "1110010";
    if(comp == "D+M")  return "1000010";
    if(comp == "D-M")  return "1010011";
    if(comp == "M-D")  return "1000111";
    if(comp == "D&M")  return "1000000";
    if(comp == "D|M")  return "1010101";
    return "0000000"; 
}

// Convert jump mnemonic to its 3-bit binary code
string jumpToBinary(const string &jump) {
    if(jump == "JGT") return "001";
    if(jump == "JEQ") return "010";
    if(jump == "JGE") return "011";
    if(jump == "JLT") return "100";
    if(jump == "JNE") return "101";
    if(jump == "JLE") return "110";
    if(jump == "JMP") return "111";
    return "000"; 
    // No jump
}

// the output file name by replacing the extension .asm with .hack
string getOutputFileName(const string &inputName) {
    size_t dotPos = inputName.rfind('.');
    if (dotPos != string::npos) {
        return inputName.substr(0, dotPos) + ".hack";
    }
    return inputName + ".hack";
}

int main() {
   
    string inputFileName;
    cout << "Enter the input ASM file name (e.g., Add.asm): ";
    getline(cin, inputFileName);
    
    ifstream inputFile(inputFileName);
    if (!inputFile.is_open()) {
        cerr << "Error: Cannot open " << inputFileName << endl;
        return 1;
    }
    
    // Initializing the ssymbol table with predefined symbols
    unordered_map<string, int> symbolTable = {
        {"SP", 0}, {"LCL", 1}, {"ARG", 2},
        {"THIS", 3}, {"THAT", 4}, {"SCREEN", 16384},
        {"KBD", 24576}
    };
    for (int i = 0; i < 16; i++) {
        symbolTable["R" + to_string(i)] = i;
    }
    
    // First Pass----- Record label declarations and store instructions
    vector<string> instructions;
    string line;
    int romAddress = 0;
    while (getline(inputFile, line)) {
        string clean = cleanLine(line);
        if (clean.empty()) continue;
        // If the line is a label, e.g., (LOOP)
        if (clean.front() == '(' && clean.back() == ')') {
            string label = clean.substr(1, clean.size() - 2);
            symbolTable[label] = romAddress;
        } else {
            instructions.push_back(clean);
            romAddress++;
        }
    }
    inputFile.close();
    
    // Generate the output file name
    string outputFileName = getOutputFileName(inputFileName);
    // Open the output file in binary mode to force LF line endings.
    ofstream outputFile(outputFileName, ios::out | ios::binary);
    if (!outputFile.is_open()) {
        cerr << "Error: Cannot open " << outputFileName << " for writing." << endl;
        return 1;
    }
    
    int nextVariableAddress = 16; 
    // Variables start at address 16
    // Second Pass-------------Convert instructions to binary code
    for (size_t i = 0; i < instructions.size(); i++) {
        string instr = instructions[i];
        string binaryLine;
        // A-instruction
        if (instr[0] == '@') {//all a instru start with @
            string symbol = instr.substr(1);
            int address = 0;
            if (isdigit(symbol[0])) {
                address = stoi(symbol);
            } else {
                if (symbolTable.find(symbol) == symbolTable.end()) {
                    symbolTable[symbol] = nextVariableAddress;
                    nextVariableAddress++;
                }
                address = symbolTable[symbol];
            }
            binaryLine = "0" + bitset<15>(address).to_string();
        } else { 
            // C-instruction
            string dest, comp, jump;
            size_t eqPos = instr.find('=');
            size_t semicolonPos = instr.find(';');
            if (eqPos != string::npos) {
                dest = instr.substr(0, eqPos);
                if (semicolonPos != string::npos) {
                    comp = instr.substr(eqPos + 1, semicolonPos - eqPos - 1);
                    jump = instr.substr(semicolonPos + 1);
                } else {
                    comp = instr.substr(eqPos + 1);
                }
            } else {
                dest = "";
                if (semicolonPos != string::npos) {
                    comp = instr.substr(0, semicolonPos);
                    jump = instr.substr(semicolonPos + 1);
                }
            }
            string compBin = compToBinary(comp);
            string destBin = destToBinary(dest);
            string jumpBin = jumpToBinary(jump);
            binaryLine = "111" + compBin + destBin + jumpBin;
        }
        // Write each line 
        outputFile << binaryLine;
        if (i < instructions.size() - 1)
            outputFile << '\n';
    }
    outputFile.close();
    
    cout << "Assembly complete. Output saved to " << outputFileName << endl;
    return 0;
}
