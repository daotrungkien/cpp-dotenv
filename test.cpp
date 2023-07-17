// Please compile and run the program with the following command to see the result:
//   ./test --VAR2=12345

#include "dotenv.h"
#include <iostream>

using namespace std;

int main(int argc, const char** argv) {
    // includes command-line arguments, environment variables,
    // files (`.env` in current folder, `.env` in program folder,
    // and `test-custom.env` file in program folder)
    dotenv env{argc, argv, true, {
        dotenv::in_current_folder(),
        dotenv::in_program_folder(),
        dotenv::in_program_folder("test-custom.env")
    }};

    // looks for variables from any source
    cout << "PATH = " << env["PATH"] << endl;
    cout << "VAR1 = " << env["VAR1"] << endl;
    cout << "VAR2 = " << env["VAR2"] << endl;
    cout << "VAR3 = " << env["VAR3"] << endl;

    // gets the value of VAR3 if existing, or a default value if not
    cout << "VAR3 (with default value) = " << env.get("VAR3", "default-var3") << endl;

    // checks if a variable exists in one specific source and prints its value if it does
    if (env.exists("VAR2", dotenv::source::environment))
        cout << "VAR2 = " << env.get("VAR2", dotenv::source::environment) << endl;
    else cout << "VAR2 is not set as an env variable" << endl;

    return 0;
}
