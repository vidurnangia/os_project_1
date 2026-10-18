This project in C++ engaged concepts and processes like fork, dup2, Vigenère cipher (encryption algorithm), and execl.

Programs:
1) driver.cpp: runs the main front-end, talking with the user, collecting input, and streaming that information throughout the various other code in the project to then produce the necessary output. Launches other programs and also maintains the history

2) logger.cpp: manages the log, timestamps the messages, logs messages to mylog.txt

3) encryption.cpp: executes the vigenere cipher, thus executing the main encryption/decryption functionality.gets commands from stdin and outputs to stdout