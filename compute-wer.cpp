#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <fstream>
#include <string>
#include <sstream>
#include <codecvt>
#include <cmath>
#include ".//wer//wer.h"

using namespace std;
const double ZERO_TOLERANCE = 1e-6;

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

        map<string,
            unordered_map<string, vector<string>>> data;

        for (string line(""); getline(ref_file, line);) {
            // Split ID and text
            vector<string> all_words = split(line, ' ');
            if(all_words.size() == 0) {
                cout << "Empty line in [REF] !!! Require atleast utterance ID in each line" << endl;
                return 1;
            }
            string id = all_words[0];
            vector<string> text(all_words.begin()+1, all_words.end());

            data.insert( {id, {
                {"REF", text},
                {"HYP", vector<string>()}
            }});
        }
        ref_file.close();

        // Input: Hyp
        string input_2_file(argv[2]);
        ifstream hyp_file(input_2_file);
        if (!hyp_file) {
            cout << "Hypothesis file does not exist !!!" << endl;
            return 1;
        }

        for (string line(""); getline(hyp_file, line);) {
            // Split ID and text
            vector<string> all_words = split(line, ' ');
            if(all_words.size() == 0) {
                cout << "Empty line in [HYP] !!! Require atleast utterance ID in each line" << endl;
                return 1;
            }
            string id = all_words[0];
            vector<string> text(all_words.begin()+1, all_words.end());


            if(data.count(id) == NULL) {
                cout << id << " not appeare in [REF]" << endl;
                return 1;
            }

            data.at(id).at("HYP") = text;
        }
        hyp_file.close();

        // Run program
        unsigned n_word_ref = 0, inserts = 0, deletes = 0, substitutes = 0;
        unsigned total_error_sentences = 0;
        unsigned total_not_in_hyp_sentences = 0;
        for(const auto &it:data) {
            string id = it.first;
            vector<string> ref = it.second.at("REF");
            vector<string> hyp = it.second.at("HYP");

            // utterance stats
            WER wer(ref, hyp);
            cout << "ID : " << id << endl;
            unordered_map<string, double> result = wer.get_wer();
            double utt_wer = result["WER"];
            cout << "<For_parsing>," << id << ',' << roundf(utt_wer * 10000) / 10000 << endl << endl;

            // Update global stats
            n_word_ref += ref.size();
            inserts += static_cast<unsigned>(result["INS"]);
            deletes += static_cast<unsigned>(result["DEL"]);
            substitutes += static_cast<unsigned>(result["SUB"]);
            if( abs(utt_wer) > ZERO_TOLERANCE ) total_error_sentences += 1;
            if(hyp.size() == 0 && ref.size() > 0) total_not_in_hyp_sentences += 1;
        }

        // Summarize global stats
        unsigned total_errors = inserts + deletes + substitutes;
        unsigned total_sentences =  data.size();
        double global_wer = static_cast<double>(total_errors) / n_word_ref * 100.0;
        global_wer = roundf(global_wer * 10000) / 10000;
        double ser = static_cast<double>(total_error_sentences) / total_sentences * 100.0;

        cout << endl << "#### Global Statistics ####" << endl \
            << "%WER " << global_wer << "% " \
            << "[ "  << total_errors << " / " << n_word_ref << ", " \
            << inserts << " ins, " << deletes << " del, " << substitutes << " sub ]"
            << endl;

        cout << "%SER " << ser << "% " \
            << "[ "  << total_error_sentences << " / " << total_sentences << " ]" \
            << endl;

        cout << "Scored " << total_sentences << " sentences, " \
            << total_not_in_hyp_sentences << " not present in hyp." \
            << endl;

        return 0;
    }
    else {
        string input_program(argv[0]);
        cout << "Please rerun with format " << input_program << " [Reference] [Hypothesis]" << endl;
        return 1;
    }

    return 1;
}
