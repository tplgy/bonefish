#include "daemon.hpp"

int main(int argc, char** argv)
{
    bonefish::daemon daemon;
    daemon.run();
    return 0;
}
