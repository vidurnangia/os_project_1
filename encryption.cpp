#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <sstream>
#include <iomanip>
using namespace std;

// heading vignere cipher
// store password --> encrypt --> decrypt --> status (success, error) --> exit

// need a command to set pass
// command to encrypt

// command to decrypt: ENCRYPT <text>
// Decrypt command, quit command

// decrypt --> negative shift from VC; encrypt --> positive shift from VC

string pw;
bool isSet = false;

string vc(const string &text, bool encrypt)
{
    string final;
    // length and index:
    int len = pw.length();
    int ind = 0;

    for (char c : text)
    {
        if (isalpha(c))
        {
            char upper = toupper(c);
            int shift = toupper(pw[ind % len]) - 'A'; // encrypt
            // decrypt w negative shift
            if (!encrypt)
                shift = 26 - shift;
            char encrypt = (upper - 'A' + shift) % 26 + 'A';
            final += encrypt;
            ind++;
        }
        else
        {
            final += c; // only alphabet changes
        }
    }
    return final;
}

// input validation function:
bool isVal(const string &str)
{
    for (char c : str)
    {
        if (!isalpha(c))
        {
            return false;
        }
    }
    return true;
}

int main()
{
    string line;
    while (getline(cin, line))
    {
        if (line.empty())
            continue;

        istringstream iss(line);
        string command;
        iss >> command;

        if (command == "QUIT")
        {
            break;
        }

        string str;
        getline(iss, str);
        // remove leading space
        if (!str.empty() && str[0] == ' ')
        {
            str.erase(0, 1);
        }

        if (command == "PASS")
        {
            if (!isVal(str))
            {
                cout << "ERROR: letters only." << endl;
                continue;
            }
            else
            {
                pw = str;
                isSet = true;
                cout << "result" << endl;
            }
        }
        else if (command == "ENCRYPT")
        {
            if (!isSet)
            {
                cout << "ERROR: password not set." << endl;
            }
            else if (!isVal(str))
            {
                cout << "ERROR: letters only." << endl;
            }
            else
            {
                string result = vc(str, true);
                cout << "RESULT" << result << endl;
            }
        }
        else if (command == "DECRYPT")
        {
            if (!isSet)
            {
                cout << "ERROR: password not set." << endl;
            }
            else if (!isVal(str))
            {
                cout << "ERROR: letters only." << endl;
            }
            else
            {
                string result = vc(str, false);
                cout << "RESULT" << result << endl;
            }
        }
        else
        {
            cout << "ERROR: unknown command." << endl;
        }
        cout.flush(); // best practice
    }
    return 0;
}