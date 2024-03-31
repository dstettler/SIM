#include "SIM.h"

int main()
{
    SIM s;
    s.init("./", SIM::SIMMode::Compression);
    s.run();
    return 0;
}