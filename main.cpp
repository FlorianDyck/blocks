#include <iostream>
#include <vector>
#include <string>
#include "types.h"

void test_print(u64 val) {
   std::cout << Board(val) << std::endl;
   std::cout << Brick(val) << std::endl;
}

int main()
{
   test_print(0b11111 | 0b11111 << BOARD_WIDTH);
   test_print(0b11 | 0b110 << BOARD_WIDTH);
   test_print(0x0101010101);

   std::vector<std::string> msg {"Hello", "C++", "World", "from", "VS Code", "and the C++ extension!"};
   for (const std::string& word : msg)
   {
      std::cout << word << " ";
   }
   std::cout << std::endl;
}