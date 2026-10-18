#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cstring>
#include <cctype>
#include <csignal>
#include <unistd.h>
#include <sys/wait.h>
using namespace std;

/*
code needs to be able to:
start pipe, send encrypted command
prompt the new string, show history of commands
check password, encrypt, decrypt, send encr to logger
print
*/

// must fork logger and encryption processes

int toLogger[2];
int toEncrypt[2];
int fromEncrypt[2];

// process IDs
pid_t logger_pid;
pid_t encrypt_pid;

// 1st we must write the string to file descriptor
void write_to_fd(int fd, const string &s)
{
    size_t total = 0;
    while (total < s.size())
    {
        ssize_t written = write(fd, s.data() + total, s.size() - total);
        if (written <= 0)
        {
            break;
        }
        total += written;
    }
}

// readfrom loaded fd byte per byte til next line '\n'
string readLine(int fd)
{
    string final;
    char c;
    while (true)
    {
        ssize_t bytes_read = read(fd, &c, 1);
        if (bytes_read <= 0)
        {
            break; // EOF or error
        }
        if (c == '\n')
        {
            break; // end of line
        }
        final += c;
    }
    return final;
}

// send log line to logger via pipe
/*
void log  **DO LATER***

//FINISHED
*/

void log(const string &action, const string &message)
{
    write_to_fd(toLogger[1], action + " " + message + "\n");
}

// input validation
bool check_letters(const string &str)
{
    if (str.empty())
        return false;
    for (char c : str)
    {
        if (!isalpha(c))
        {
            return false;
        }
    }
    return true;
}

// fork + execute children
void startChildren(const string &file)
{
    // create pipe
    if (pipe(toLogger) == -1 || pipe(toEncrypt) == -1 || pipe(fromEncrypt) == -1)
    {
        cerr << "Pipe creation failed" << endl;
        exit(1);
    }
    // fork logger process
    // child: close unused pipe ends, dup2, exec ./logger
    logger_pid = fork();
    if (logger_pid == 0)
    {
        // CHILD ONLY READS
        close(toLogger[1]);
        close(toEncrypt[0]);
        close(toEncrypt[1]); // NOT USED
        close(fromEncrypt[0]);
        close(fromEncrypt[1]);

        dup2(toLogger[0], 0);

        /*
        WORK ON REST OF THIS ON NEXT SESSION
        FINISHED
        */

        execl("./logger", "logger", file.c_str(), nullptr);

        cerr << "Failed to exec logger" << endl;
        exit(1);
    }

    // fork encryption process
    encrypt_pid = fork();
    if (encrypt_pid == 0)
    {
        // CHILD ONLY READS
        close(toLogger[0]);
        close(toLogger[1]); // NOT USED

        close(toEncrypt[1]);

        close(fromEncrypt[0]);

        dup2(toEncrypt[0], 0);
        dup2(fromEncrypt[1], 1);

        execl("./encryption", "encryption", nullptr);

        cerr << "Failed to exec encryption" << endl;
        exit(1);
    }

    // close unused pipe ends in parent
    // parents write to logger and encrypt 1, read from encrypt0
    close(toLogger[0]);
    close(toEncrypt[0]);
    close(fromEncrypt[1]);
}

string sendEncryptCommand(const string &cmd)
{
    write_to_fd(toEncrypt[1], cmd + "\n");
    return readLine(fromEncrypt[0]);
}

// get user input
string getUserInput(const string &prompt)
{
    string line;
    cout << "Enter " << prompt << ": ";
    getline(cin, line);
    return line;
}

// we have to show the history as a menu

vector<string> logHistory;

string history()
{
    if (logHistory.empty())
    {
        return "";
    }

    cout << "History:" << endl;
    for (size_t i = 0; i < logHistory.size(); i++)
    {
        cout << "  " << (i + 1) << ") " << logHistory[i] << endl;
    }
    cout << " 0) Enter new command" << endl;
    cout << "Choose an option: ";

    string choice;
    getline(cin, choice);
    // use for string to int conversions
    int x = atoi(choice.c_str());

    if (x >= 1 && x <= logHistory.size())
    {
        return logHistory[x - 1];
    }
    else
    {
        return "";
    }
}

// lets user choose to enter new command or select from history
void doCmd()
{
    string cmd;

    string historyCmd = history();
    if (!historyCmd.empty())
    {
        cmd = historyCmd;
    }
    else
    {
        cmd = getUserInput("password (letters only)");
    }

    // ensure only letters are received
    if (!check_letters(cmd))
    {
        cout << "ERROR: letters only." << endl;
        // log:
        log("ERROR", "letters only");
        return;
    }

    // send to encryption process
    string encrypted = sendEncryptCommand("PASS " + cmd);
    log("PASSWORD", "Password set");

    log("RESULT", encrypted);
    cout << encrypted << endl; // pw not added to history!!!
}

// encrypt function
void doEncrypt()
{
    string txt;
    string historyCmd = history();
    if (!historyCmd.empty())
    {
        txt = historyCmd;
    }
    else
    {
        txt = getUserInput("string to encrypt (letters only)");
        if (!check_letters(txt))
        {
            cout << "ERROR: letters only!!" << endl;
            log("ERROR", "letters only");
            return;
        }
        logHistory.push_back(txt);
    }
    string encrypted = sendEncryptCommand("ENCRYPT " + txt);
    cout << encrypted << endl;
    log("ENCRYPT", txt);
    log("RESULT", encrypted);

    // save resulting cipher text to history too
    if (encrypted.rfind("RESULT ", 0) == 0)
    {
        logHistory.push_back(encrypted.substr(7));
    }
}

// decrypt function
// will be same as encrypt but negative for decrypt

void doDecrypt()
{
    string txt;
    string historyCmd = history();
    if (!historyCmd.empty())
    {
        txt = historyCmd;
    }
    else
    {
        txt = getUserInput("string to decrypt (letters only)");
        if (!check_letters(txt))
        {
            cout << "ERROR: letters only!!" << endl;
            log("ERROR", "letters only");
            return;
        }
        logHistory.push_back(txt);
    }
    string decrypted = sendEncryptCommand("DECRYPT " + txt);
    cout << decrypted << endl;
    log("DECRYPT", txt);
    log("RESULT", decrypted);

    // save resulting plain text to history too
    // if (decrypted.substr(0, 6) == "RESULT")
    // {
    //     logHistory.push_back(decrypted.substr(7));
    // }
    if (decrypted.rfind("RESULT", 0) == 0)
    {
        logHistory.push_back(decrypted.substr(7));
    }
}

// need to print current history list
void printHistory()
{
    cout << "curr History:" << endl;
    if (logHistory.empty())
    {
        cout << "  (empt)" << endl;
    }
    else
    {
        for (size_t i = 0; i < logHistory.size(); ++i)
        {
            cout << "  " << (i + 1) << ") " << logHistory[i] << endl;
        }
    }
    log("HISTORY", "Displayed current history");
}

// main driver
int main(int argc, char **argv)
{
    if (argc != 2)
    {
        cerr << "Usage: " << argv[0] << " <logfile>" << endl;
        return 1;
    }

    signal(SIGPIPE, SIG_IGN);

    string logfile = argv[1];

    // 2 - initiate ./logger and ./encryption, pipes are waiting
    startChildren(logfile);

    // log
    log("START", "Driver started");

    string line;
    bool running = true;
    while (running)
    {
        cout << "\ncmds: password, encrypt, decrypt, history, quit" << endl
             << "> ";
        if (!getline(cin, line))
            break; // EOF on stdin -> exit
        if (line.empty())
            continue;

        istringstream iss(line);
        string cmd;
        iss >> cmd; // first word = command

        if (cmd == "password")
        {
            doCmd();
        }
        else if (cmd == "encrypt")
        {
            doEncrypt();
        }
        else if (cmd == "decrypt")
        {
            doDecrypt();
        }
        else if (cmd == "history")
        {
            printHistory();
        }
        else if (cmd == "quit")
        {
            running = false;
        }
        else
        {
            cout << "Unknown command." << endl;
        }
    }
    // cleanup: close pipes, wait for children
    // close pipes and wait

    log("EXIT", "Driver exiting");
    write_to_fd(toEncrypt[1], "QUIT\n");
    write_to_fd(toLogger[1], "QUIT\n");
    close(toLogger[1]);
    close(toEncrypt[1]);
    close(fromEncrypt[0]);
    waitpid(logger_pid, nullptr, 0);
    waitpid(encrypt_pid, nullptr, 0);
    return 0;
}