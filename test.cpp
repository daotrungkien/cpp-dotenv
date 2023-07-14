#include "dotenv.h"
#include <iostream>

using namespace std;


int main(int argc, const char** argv) {
   // including command-line arguments, environment variables,
    // files (`.env` in current folder, `.env` in program folder,
    // and `test-custom.env` file in program folder)
    dotenv env1{argc, argv, true, {
        dotenv::in_current_folder(),
        dotenv::in_program_folder(),
        dotenv::in_program_folder("test-custom.env")
    }};

    cout << "PATH = " << env1["PATH"] << endl;
    cout << "VAR1 = " << env1["VAR1"] << endl;
    cout << "VAR2 = " << env1["VAR2"] << endl;
    cout << "VAR3 = " << env1["VAR3"] << endl;

    return 0;
}