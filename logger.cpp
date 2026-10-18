#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <sstream>
#include <iomanip>
using namespace std;

int main(int argcount, char *argv[])
{
    if (argcount != 2)
    {
        cerr << "Usage: " << argv[0] << " <logfile>" << endl;
        return 1; // error case
    }
    //////
    // append, create new file if needed
    ofstream logfile(argv[1], ios::app);
    if (!logfile.is_open())
    {
        cerr << "Can't open log file: " << argv[1] << endl;
        return 1;
    }

    // main logger loop
    // keep reading until QUIT
    // string parsing: ACTION-MESSAGE
    string line;
    while (getline(cin, line))
    {
        if (line == "QUIT")
            break;

        if (line.empty())
            continue;

        istringstream iss(line);

        string action, message; // string store variables
        iss >> action;
        getline(iss, message); // read rest of line

        if (!message.empty() && message[0] == ' ')
        {
            message.erase(0, 1); // emove leading space if any
        }

        // figure out timestamp in next session
        time_t current = time(nullptr);
        // time local = localtime(&now);

        tm *local = localtime(&current);

        // format timestamp
        char buffer[32];
        strftime(buffer, sizeof(buffer), "%Y-%m-%m %H:%M:%S", local);

        // must write formatted var to logfile
        //**finish formatting later
        // logfile << buffer <<
        logfile << buffer << " [" << action << "] " << message << endl;
        logfile.flush();
    }

    logfile.close();
    return 0;
}