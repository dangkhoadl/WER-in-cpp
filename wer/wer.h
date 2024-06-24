#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <codecvt>
#include <locale>

using namespace std;

class WER {
private:
    vector<string> r;
    vector<string> h;

    vector<vector<unsigned>> d;
    vector<char> step_list;

    unsigned inserts, substitutes, deletes;

    unsigned __get_min_value(unsigned value_1, unsigned value_2, unsigned value_3) {
        unsigned min_value = min(value_1, value_2);
        return min(min_value, value_3);
    }
    void __calculate_edit_distance() {
        for(unsigned i=0; i<r.size()+1; ++i) {
            d[i][0] = i;
        }
        for(unsigned j=0; j<h.size()+1; ++j) {
            d[0][j] = j;
        }

        unsigned sub_ = 0, ins_ = 0, del_ = 0;
        for(unsigned i=1; i<r.size()+1; ++i) {
            for(unsigned j=1; j<h.size()+1; ++j) {
                if (r[i-1] == h[j-1]) {
                    d[i][j] = d[i-1][j-1];
                }
                else {
                    sub_ = d[i-1][j-1] + 1;
                    ins_ = d[i][j-1] + 1;
                    del_ = d[i-1][j] + 1;
                    d[i][j] = __get_min_value(sub_, ins_, del_);
                }
            }
        }
    }
    void __get_step_list() {
        unsigned ref_ptr = r.size();
        unsigned hyp_ptr = h.size();

        while(true) {
            if (ref_ptr == 0 && hyp_ptr == 0) {
                break;
            } else if (ref_ptr >= 1 && hyp_ptr >= 1 && d[ref_ptr][hyp_ptr] == d[ref_ptr-1][hyp_ptr-1] && r[ref_ptr-1] == h[hyp_ptr-1]) {
                step_list.push_back('e');
                ref_ptr -= 1;
                hyp_ptr -= 1;
            } else if (hyp_ptr >= 1 && d[ref_ptr][hyp_ptr] == d[ref_ptr][hyp_ptr-1]+1) {
                step_list.push_back('i');
                inserts += 1;
                ref_ptr = ref_ptr;
                hyp_ptr -= 1;
            } else if (ref_ptr >= 1 && hyp_ptr >= 1 && d[ref_ptr][hyp_ptr] == d[ref_ptr-1][hyp_ptr-1]+1) {
                step_list.push_back('s');
                substitutes += 1;
                ref_ptr -= 1;
                hyp_ptr -= 1;
            } else {
                step_list.push_back('d');
                deletes += 1;
                ref_ptr -= 1;
                hyp_ptr = hyp_ptr;
            }
        }
        reverse(step_list.begin(), step_list.end());
    }

    unordered_map<string, double> __solve_edgecase_1() {
        /* Case: ref is empty and hyp is empty
        */
        // print WER
        cout << "REF: " << endl;
        cout << "HYP: " << endl;
        cout << "EVA: " << endl;
        cout << endl << "WER: 0% [ 0 / 0, 0 ins, 0 del, 0 sub ]" << endl;

        // Return wer, inserts, deletes, substitutes
        return unordered_map<string, double>({
            {"WER", static_cast<double>(0)},
            {"INS", static_cast<double>(0)}, {"DEL", static_cast<double>(0)}, {"SUB", static_cast<double>(0)}
        });
    }

    unordered_map<string, double> __solve_edgecase_2() {
        /* Case: ref is not empty but hyp is empty
        */
        wstring_convert<codecvt_utf8<char32_t>, char32_t> conv;

        // print WER
        cout << "REF: ";
        for(const string &word:r) cout << word << ' ';
        cout << endl;
        cout << "HYP: " << endl;
        cout << "EVA: ";
        for(const string &word:r) {
            string tmp(conv.from_bytes(word).size()-1, ' ');
            cout << 'D' << tmp << ' ';
        }
        cout << endl << "WER: 100% " \
            << "[ "  << r.size() << " / " << r.size() << ", 0 ins, " << r.size() << " del, 0 sub ]" \
            << endl;

        // Return wer, inserts, deletes, substitutes
        return unordered_map<string, double>({
            {"WER", static_cast<double>(100)},
            {"INS", static_cast<double>(0)}, {"DEL", static_cast<double>(r.size())}, {"SUB", static_cast<double>(0)}
        });
    }

    unordered_map<string, double> __solve_edgecase_3() {
        /* Case: ref is empty but hyp is not empty
        */
        wstring_convert<codecvt_utf8<char32_t>, char32_t> conv;

        // print WER
        cout << "REF: "  << endl;
        cout << "HYP: ";
        for(const string &word:h) cout << word << ' ';
        cout << endl;
        cout << "EVA: ";
        for(const string &word:h) {
            string tmp(conv.from_bytes(word).size()-1, ' ');
            cout << 'I' << tmp << ' ';
        }
        cout << endl << "WER: Infinite% " \
            << "[ "  << h.size() << " / 0, " << h.size() << " ins, 0 del, 0 sub ]" \
            << endl;

        // Return wer, inserts, deletes, substitutes
        return unordered_map<string, double>({
            {"WER", static_cast<double>(100)},
            {"INS", static_cast<double>(static_cast<double>(h.size()))}, {"DEL", static_cast<double>(0)}, {"SUB", static_cast<double>(0)}
        });
    }

public:
    WER(const vector<string> &ref_words, const vector<string> &hyp_words) : r(ref_words), h(hyp_words) {
        d.assign(r.size()+1, vector<unsigned>(h.size()+1, 0));
        step_list.clear();

        inserts = 0;
        substitutes = 0;
        deletes = 0;
    }
    unordered_map<string, double> get_wer() {
        if(r.size() == 0 && h.size() == 0) return __solve_edgecase_1();
        if(r.size() > 0 && h.size() == 0) return __solve_edgecase_2();
        if(r.size() == 0 && h.size() > 0) return __solve_edgecase_3();

        // build the matrix
        __calculate_edit_distance();

        // find out the manipulation steps
        __get_step_list();

        // get wer
        double wer = static_cast<double>(d[r.size()][h.size()]) / static_cast<double>(r.size()) * 100.0;

        //// print result
        unsigned count=0, count1=0, count2=0;
        unsigned index=0, index1=0, index2=0;
        wstring_convert<codecvt_utf8<char32_t>, char32_t> conv;
        string tmp("");

        // print REF
        cout << "REF: ";
        for(unsigned i=0; i<step_list.size(); ++i) {
            if(step_list[i] == 'i') {
                count = 0;
                for(unsigned j=0; j<i; ++j) {
                    if (step_list[j] == 'd') {
                        count += 1;
                    }
                }
                index = i - count;
                tmp.assign(conv.from_bytes(h[index]).size(), '*');
                cout << tmp << ' ';
            } else if(step_list[i] == 's') {
                count1 = 0;
                for(unsigned j=0; j<i; ++j) {
                    if (step_list[j] == 'i') {
                        count1 += 1;
                    }
                }
                index1 = i - count1;

                count2 = 0;
                for(unsigned j=0; j<i; ++j) {
                    if (step_list[j] == 'd') {
                        count2 += 1;
                    }
                }
                index2 = i - count2;

                if (conv.from_bytes(r[index1]).size() < conv.from_bytes(h[index2]).size()) {
                    tmp.assign(conv.from_bytes(h[index2]).size() - conv.from_bytes(r[index1]).size(), ' ');
                    cout << r[index1] << tmp << ' ';
                }
                else {
                    cout << r[index1] << ' ';
                }
            } else {
                count = 0;
                for(unsigned j=0; j<i; ++j) {
                    if (step_list[j] == 'i') {
                        count += 1;
                    }
                }
                index = i - count;
                cout << r[index] << ' ';
            }
        }

        // print HYP
        cout << endl << "HYP: ";
        for(unsigned i=0; i<step_list.size(); ++i) {
            if(step_list[i] == 'd') {
                count = 0;
                for(unsigned j=0; j<i; ++j) {
                    if (step_list[j] == 'i') {
                        count += 1;
                    }
                }
                index = i - count;
                tmp.assign(conv.from_bytes(r[index]).size(), ' ');
                cout << tmp << ' ';
            } else if(step_list[i] == 's') {
                count1 = 0;
                for(unsigned j=0; j<i; ++j) {
                    if (step_list[j] == 'i') {
                        count1 += 1;
                    }
                }
                index1 = i - count1;

                count2 = 0;
                for(unsigned j=0; j<i; ++j) {
                    if (step_list[j] == 'd') {
                        count2 += 1;
                    }
                }
                index2 = i - count2;

                if (conv.from_bytes(r[index1]).size() > conv.from_bytes(h[index2]).size()) {
                    tmp.assign(conv.from_bytes(r[index1]).size() - conv.from_bytes(h[index2]).size(), ' ');
                    cout << h[index2] << tmp << ' ';
                }
                else {
                    cout << h[index2] << ' ';
                }
            } else {
                count = 0;
                for(unsigned j=0; j<i; ++j) {
                    if (step_list[j] == 'd') {
                        count += 1;
                    }
                }
                index = i - count;
                cout << h[index] << ' ';
            }
        }

        // print EVAL
        cout << endl << "EVA: ";
        for(unsigned i=0; i<step_list.size(); ++i) {
            if(step_list[i] == 'd') {
                count = 0;
                for(unsigned j=0; j<i; ++j) {
                    if (step_list[j] == 'i') {
                        count += 1;
                    }
                }
                index = i - count;
                tmp.assign(conv.from_bytes(r[index]).size()-1, ' ');
                cout << 'D' << tmp << ' ';
            } else if(step_list[i] == 'i') {
                count = 0;
                for(unsigned j=0; j<i; ++j) {
                    if (step_list[j] == 'd') {
                        count += 1;
                    }
                }
                index = i - count;
                tmp.assign(conv.from_bytes(h[index]).size()-1, ' ');
                cout << 'I' << tmp << ' ';
            } else if(step_list[i] == 's') {
                count1 = 0;
                for(unsigned j=0; j<i; ++j) {
                    if (step_list[j] == 'i') {
                        count1 += 1;
                    }
                }
                index1 = i - count1;

                count2 = 0;
                for(unsigned j=0; j<i; ++j) {
                    if (step_list[j] == 'd') {
                        count2 += 1;
                    }
                }
                index2 = i - count2;

                if (conv.from_bytes(r[index1]).size() > conv.from_bytes(h[index2]).size()) {
                    tmp.assign(conv.from_bytes(r[index1]).size() - 1, ' ');
                    cout << 'S' << tmp << ' ';
                }
                else {
                    tmp.assign(conv.from_bytes(h[index2]).size() - 1, ' ');
                    cout << 'S' << tmp << ' ';
                }
            } else {
                count = 0;
                for(unsigned j=0; j<i; ++j) {
                    if (step_list[j] == 'i') {
                        count += 1;
                    }
                }
                index = i - count;
                tmp.assign(conv.from_bytes(r[index]).size(), ' ');
                cout << tmp << ' ';
            }
        }

        // print WER
        cout << endl << "WER: " << wer << "% " \
            << "[ "  << d[r.size()][h.size()] << " / " << r.size() << ", "\
            << inserts << " ins, " << deletes << " del, " << substitutes << " sub ]"
            << endl;

        // Return wer, inserts, deletes, substitutes
        return unordered_map<string, double>({
            {"WER", wer},
            {"INS", static_cast<double>(inserts)}, {"DEL", static_cast<double>(deletes)}, {"SUB", static_cast<double>(substitutes)}
        });
    }
};
