/*
In the first step, you will write code to:

    • handle the passing in of a (single) command line argument, or, if nothing is passed in output an error,
    • for loop over the path passed in using <filesystem>'s recursive_directory_iterator, and,
    • within the for loop output (as detailed below) symlink, file, dir, or other information.
*/

// ITEM 1) You will need to make use of four #include files: <string> (for std:string), <filesystem>, <iomanip> (to call std:::quoted), and <iostream> (for std::cout, std::cerr). Write these #include directives at the top of your program.
#include <iostream>
#include <string>
#include <filesystem>
#include <iomanip>
#include <regex>

// ITEM 2) You will be making use of two namespaces: std and std::filesystem. Since std::filesystem is a lot to type each time, know C++ supports "namespace aliases" where the programmer can assign a shorten name to another namespace. Let's make the default namespace searched for unknown symbols be "std" and use a namespace alias called "fs" to access symbols in the std::filesystem namespace by writing:
namespace fs = std::filesystem;
using namespace std;

// ITEM 3) Write the stub for your main() function. Since this program will process command line arguments you must use this version of main():
int main(int argc, char *argv[]) { // ASIDE: A lot of people unfortunately do this so I will make a mention of this here... main() ALWAYS returns an int. ALWAYS. If you do not write a return statement in main(), the standard defines the value of the int returned to be 0 (zero). 

    // ITEM 4) This program requires one command line argument: the path (to a directory) needed to recursively search. This implies that if argc != 2 then the user has not provided such, or, has provided too many arguments. If argc != 2 the output should be "Usage: " followed by the program name (i.e., argv[0]) followed by " <path>\n", and, return 1 from path. This output should be to standard error, which with C++'s <iostream> is called cerr. This would be done like this:
    // Part II Item 2) The command line input to the program will now have an optional third argument: the regular expression to be used. If the argument is omitted, your code should use the regular expression ".*" (detailed in subsequent steps). In this step, simply modify your argc if statement to be:
    if (argc != 2 && argc != 3) { 
        // ASIDE: Recall from COMP-2560 (Systems Programming) that the first value passed to argv in C/C++ programs is the name of the program that was called. Outputting it here is nice to the user
        // and the cerr output message to output "Usage: ", argv[0], " <path> [<regex>]\n".
        cerr << "Usage: " << argv[0] << " <path> [<regex>]\n";
        return 1;
    }

    // Part II ITEM 3) Since the third argument is optional and when it is not specified one needs to hard-code the use of the regular expression ".*". start by writing this after the close of the argc if statement:
    char const* re = ".*";

    /*
    ASIDE: In C++, C-style string literals always have the type char const* (or if you prefer writting const char*) --unlike C. If you only write char* the C++ compiler will report an error that you cannot assign a char const* (i.e., a pointer that points to a read-only character (array)) to a char* (i.e., a pointer that points to a read-write character (array)). As you will see, it is okay to assign a char* (i.e., thing the type involved with argv[index]) to a char const*. (Think about as to why this makes sense. Nicely, C++ allows one to do this.)

    So if argc == 3, then you want to assign the value the third argv array element to the re variable. (Remember that array indexing starts from 0 --not 1.) Write this if statement.
    */
    if (argc == 3) {
        re = argv[2];
    }

    // Part II ITEM 4) With re now set to a regular expression, we need to compile it. This is done using std::regex (which is a type alias to std::basic_regex if you are looking this up in TCPPSL or in cppreference.com). To do this write:
    regex compiled_re(re); // ASIDE: Regular expression libraries will typically require compiling the regular expression first. This is done for speed. In C++, if something goes wrong with compilation, an exception will be thrown. In this assignment, we are not worrying about handling exceptions --so you won't use try..catch in this assignment. (As in other languages, not handling a thrown exception will cause the program to immediately terminate with an ugly error message.)

    // ITEM 5) If program execution passes ITEM 4's if statement, then argv[1] is defined: it is the path the user wants to recursively search. Start by assigning argv[1] to an fs::path variable, e.g.,
    // ASIDE: Since we won't be modifying this variable at all, it has been made const. Also, in general, it is easier to place const to the right of whatever is being made const. In this case, const fs::path and fs::path const is easy either way. However, if you had a more complicated type, e.g., involving a pointer such as fs::path const *, or, fs::path * const, or fs::path const * const what is const:, the pointer, the fs::path, or both? If you write const to the right of what is being made const and (usually) read if from right to left, you will have your answer as to what is const. :-)
    // ASIDE: In C++, it is wise to make anything that is not being written to after construction const. This will help the compiler report issues (e.g., attempts to modify something const), allow the compiler to call const functions in objects (which can be more efficient), and help the compiler better optimize your program code since an object that is read only will never change --which means the compiler might be able to move such outside of loops, etc.
    fs::path const base = argv[1];

    
    /*
    ITEM 6) C++ defines fs::recursive_directory_iterator which allows one to iterate recursively through all directory entries under a certain path. To iterate through all entries, use the C++ range for loop like below thiscomment block

    The C++ range for loop is just like the Java range for loop (i.e., COMP-2120): the variable being iterated across is to the right of the colon, and, the iteration variable is the left of the colon. In C++, if the variable to the left of the colon doesn't exist, one will need to declare such. Note the following about such declarations:

        • make such declarations a reference unless copies of the iteration values are truly needed
        • make such declarations const unless you are modifying the iteration values (and if you want to modify the original, then ensure it is a reference --not a copy!)
        • use auto if you don't want to write the actual type (the compiler can figure such out automatically)

    So "auto const&" is a constant reference to whatever type the compiler determines from what is to the right of the colon in the range for loop.

    ASIDE: Often when writing code one knows the "kind" of thing that is returned but isn't concerned with the exact type of what is returned. In this case, in <filesystem> documentaiton, one can see that directory_recursive_iterator returns directory_entry structures so one could write fs::directory_entry const& instead of auto const& --but that is more typing and this allows you to begin to see how auto can be used in C++. :-)
    */
    for (auto const &entry: fs::recursive_directory_iterator(base, fs::directory_options::skip_permission_denied)) {
        // Part II ITEM 5) In your range for loop, inside its body all of the code you have will now appear inside an if statement. Here you want to declare a regular expression variable to hold the result of a match/non-match, call entry.path().filename().native() to obtain the filename of the entry, and call std::regex_match() on that filename to see if there is a match or not. This would like like:
        // ASIDE: Look up fs::path at cppreference.com and notice that it has a number of handy member functions that allow one to obtain the filename, its extension, etc.
        smatch m;
        if (regex_match(entry.path().filename().native(), m, compiled_re))
        {
            // ITEM 7) Inside the for loop, you will need to obtain the name the entry variable refers to . This can be done by invoking entry's .path() member function, followed by the .native() member function (of a fs::path object). Assign the result to a std::string variable.
            // Part III ITEM 1) Notice in the Sample Part II Runs the directory entries have ../ etc. in them, and, notice in the Sample Part III Runs they do NOT have such in them. In fact, in Smaple Part III Runs the directory names are RELATIVE to the path passed in to the program (i.e., the second command line argument). (Cool, eh?!!)

            // To modify you program to do this, you only need to change one line code! From Part II your program has this line of code (or its equivalent):
            // string const s = entry.path().native();

            // To obtain the relative directory name, you want to use the fs::path type's member function lexically_relative(base) (or the lexically_proximate(base)) member functions before calling native() on the returned object. (The base argument is what you want the path to be relative to.) Thus, this line should look like this:
            string const str = entry.path().lexically_relative(base).native();

            // ITEM 8) C++20 has a function called std::quoted() which will wrap a string in quotes handling escaping the quote character inside of it which will be used to output strings just in case something needs quoting. Call this function like the indicated argument with the string variable you created in the previous step:
            auto const p = quoted(str); // ASIDE: Here using auto is important. If you look at the documentation of std::quoted at cppreference.com you will notice that the return type is unspecified. Since the compiler will know the returned type, using auto works here! :-)

            /*
            ITEM 9) Finally, you will need to write a series of if .. else if ... else if ... else statements to output information about each entry. Do this as follows:

                • The first if statement needs to call fs::is_symlink() and if it is true, output to standard output (i.e., std::cout), "LINK: ", followed by p, followed by '\n' placing << between cout, "LINK: ", p, and '\n'.
                • The second if will call fs::is_regular_file() and if it is true, outputs via cout "FILE: ", fs::file_size on the entry, ' ', p, and '\n'.
                • The third if will call fs::is_directory() and if it is true, outputs via cout "DIR: ", p, and '\n'.
                • Finally the (last) else clause outputs via cout "OTHER: ", p, and '\n'.
            */
            if (fs::is_symlink(entry)) {
                std::cout << "LINK: " << p << "\n";
            } else if (fs::is_regular_file(entry)) {
                std::cout << "FILE: " << fs::file_size(entry) << " " << p << "\n";
            } else if (fs::is_directory(entry)) {
                std::cout << "DIR:" << p << "\n";
            } else {
                std::cout << "OTHER: " << p << "\n";
            }
        }
    }
    return 0;
}