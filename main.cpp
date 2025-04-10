#include "WFC_Input.h"

// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or
// click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
int main()
{
    WFC_Input wfc_input;
    wfc_input.initialize_variables();
    if (!wfc_input.read_file("wfc.in"))
    {
        std::cerr << "Error reading file." << std::endl;
    }

    return 0;
}