#include "server/server.h"

int main(int argc, char **args)
{
    Cardsity::Server server;
    server.run(args, argc);

    return 0;
}