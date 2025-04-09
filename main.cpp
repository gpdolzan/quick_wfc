#include "WFC_Input.h"

// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or
// click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
int main()
{
    if (!WFC_Input::read_file("wfc.in"))
    {
        std::cerr << "Error reading file." << std::endl;
    }

    return 0;
}