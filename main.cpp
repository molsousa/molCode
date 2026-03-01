#include "include/molCode.hpp"
#include <memory>

int main(int argc, char **argv)
{
    if(argc > 1){
        auto code = std::make_shared<molCode>(argv[1]);
        code->inicializar();
    }
    else{
        auto code = std::make_shared<molCode>("");
        code->inicializar();
    }


    return 0;
}
