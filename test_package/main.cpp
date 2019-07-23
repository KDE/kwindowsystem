#include <iostream>
#include <string>

#include <QString>

#include <KWindowSystem>

int main()
{

    bool found_X11 = KWindowSystem::isPlatformX11();

    if (found_X11 || !found_X11)
    {
        if (found_X11)
        {
            std::cout << "Found X11" << std::endl;
        }
        std::cout << "Test OK" << std::endl;
        return 0;
    }
    std::cout << "Test FAILED" << std::endl;
    return 1;
}
