#include <iostream>
#include "bot.h"


int main() {
    try{
      Bot<> b;

      for (;;) {
        b.runBot();
      }
    } catch(const std::system_error& e) {
        std::cout << "Caught system_error with code " << e.code() 
                  << " meaning " << e.what() << '\n';
    }
}
