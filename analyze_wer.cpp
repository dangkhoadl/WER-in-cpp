#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <string>
#include <sstream>
#include <codecvt>
#include ".//wer//wer.h"

using namespace std;

vector<string> split(const string &str, char delim) {
    vector<string> result;
    stringstream str_st(str);
    for (string x(""); getline(str_st, x, (delim));) {
        result.emplace_back(x);
    }
    return move(result);
}

int main(int agrc, char *argv[]) {
    if (agrc == 3) {
        // Input: Ref
        string input_1_file(argv[1]);
        ifstream ref_file(input_1_file);
        if (!ref_file) {
            cout << "Reference does not exist !!!" << endl;
            return 1;
        }

        vector<string> references;
        for (string line(""); getline(ref_file, line);) {
            if(line != "") references.push_back(line);
        }
        ref_file.close();

        // Input: Hyp
        string input_2_file(argv[2]);
        ifstream hyp_file(input_2_file);
        if (!hyp_file) {
            cout << "Hypothesis file does not exist !!!" << endl;
            return 1;
        }

        vector<string> hypotheses;
        for (string line(""); getline(hyp_file, line);) {
            if(line != "") hypotheses.push_back(line);
        }
        hyp_file.close();

        // Run program
        if (references.size() != hypotheses.size()) {
            cout << "Hypothesis and Reference sizes mismatch !!!" << endl;
            return 1;
        }

        vector<string> ref, hyp;
        for(int i=0; i<references.size(); ++i) {
            ref = split(references[i], ' ');
            hyp = split(hypotheses[i], ' ');

            WER wer(ref, hyp);
            wer.get_wer();
            cout << endl;
        }
        return 0;
    }
    else {
        string input_program(argv[0]);
        cout << "Please rerun with format " << input_program << " [Reference] [Hypothesis]" << endl;
        return 1;
    }

    return 1;
}
