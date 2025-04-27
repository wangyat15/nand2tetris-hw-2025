#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <map>
#include <regex>
#include <vector>

using namespace std;

//Command tables
map <string, string> ctbl = {
    {"0","0101010"},{"1","0111111"},{"-1","0111010"},
    {"D","0001100"},{"A","0110000"},{"!D","0001101"},
    {"!A","0110001"},{"-D","0001111"},{"-A","0110011"},
    {"D+1","0011111"},{"A+1","0110111"},{"D-1","0001110"},
    {"A-1","0110010"},{"D+A","0000010"},{"D-A","0010011"},
    {"A-D","0000111"},{"D&A","0000000"},{"D|A","0010101"},
    {"M","1110000"},{"!M","1110001"},{"-M","1110011"},
    {"M+1","1110111"},{"M-1","1110010"},{"D+M","1000010"},
    {"D-M","1010011"},{"M-D","1000111"},{"D&M","1000000"},
    {"D|M","1010101"}
};

map <string, string> dtbl = {
    {"","000"},{"M","001"},{"D","010"},{"MD","011"},
    {"A","100"},{"AM","101"},{"AD","110"},{"AMD","111"}
};

map <string, string> jtbl = {
    {"","000"},{"JGT","001"},{"JEQ","010"},{"JGE","011"},
    {"JLT","100"},{"JNE","101"},{"JLE","110"},{"JMP","111"}
};

//Trim string
string trim(const string& s)
{
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

//Remove comments
string clearComms(string prog)
{
    int n = prog.length();
    string res;

    bool s_cmt = false;
    bool m_cmt = false;

    for (int i=0; i<n; i++)
    {
        if (s_cmt == true && prog[i] == '\n')
            s_cmt = false;
        else if  (m_cmt == true && prog[i] == '*' && prog[i+1] == '/')
            m_cmt = false,  i++;
        else if (s_cmt || m_cmt)
            continue;
        else if (prog[i] == '/' && prog[i+1] == '/')
            s_cmt = true, i++;
        else if (prog[i] == '/' && prog[i+1] == '*')
            m_cmt = true,  i++;
        else  res += prog[i];
    }
    return res;
}

//Parse jump commands into parts
void parseC(const string& instr, string& dest, string& comp, string& jump)
{
    size_t eq = instr.find('=');
    size_t semi = instr.find(';');

    dest = (eq != string::npos) ? instr.substr(0, eq) : "";
    comp = instr.substr(eq != string::npos ? eq + 1 : 0,
        (semi != string::npos ? semi : instr.length())
        - (eq != string::npos ? eq + 1 : 0));
    jump = (semi != string::npos) ? instr.substr(semi + 1) : "";
}

//Convert to binary
string to_bin(int value)
{
    return bitset<16>(value).to_string();
}

//Assemble C-instruction
string concatC(const string& instr)
{
    string dest, comp, jump;
    parseC(instr, dest, comp, jump);
    return "111" + ctbl[comp] + dtbl[dest] + jtbl[jump];
}

//Extract labels
bool isLabel(const string& line)
{
    return line.front() == '(' && line.back() == ')';
}

string extract(const string& line)
{
    return line.substr(1, line.size() - 2);
}

int main()
{
    ifstream fin("Prog.asm");
    ofstream fout("Prog.hack");

    if (!fin){
        cout << "Failed to open input file.\n";
        return 1;
    }

    stringstream in;
    in << fin.rdbuf();
    string raw = clearComms(in.str());
    vector<string> str;
    map<string, int> stbl;

    //Predefined symbols
    for (int i = 0; i <= 15; i++)
        stbl["R" + to_string(i)] = i;
    stbl["SCREEN"] = 16384;
    stbl["KBD"] = 24576;
    stbl["SP"] = 0;
    stbl["LCL"] = 1;
    stbl["ARG"] = 2;
    stbl["THIS"] = 3;
    stbl["THAT"] = 4;

    //Collect labels
    istringstream cdin(raw);
    string line;
    int rom_addr = 0;
    while (getline(cdin, line)){
        while(line[0] == ' ')
            line.erase(0, 1);
        if (line.empty()) continue;

        if (isLabel(line)){
            string label = extract(line);
            stbl[label] = rom_addr;
        }
        else{
            str.push_back(line);
            rom_addr++;
        }
    }

    //Parse instructions
    vector<string> bin_out;
    int next_addr = 16;

    for (const auto& instr : str){
        if (instr[0] == '@'){
            string symbol = instr.substr(1);
            int address;

            if (isdigit(symbol[0]))
                address = stoi(symbol);
            else{
                if (stbl.find(symbol) == stbl.end())
                    stbl[symbol] = next_addr++;
                address = stbl[symbol];
            }
            bin_out.push_back(to_bin(address));
        }
        else
            bin_out.push_back(concatC(instr));
    }

    for (const string& bin : bin_out)
        fout << bin << "\n";

    cout << "Conversion complete" << "\n";
    return 0;
}
