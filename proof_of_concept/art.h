#ifndef ART_GUARD
#define ART_GUARD

#include <iostream>
#include <string>
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */

namespace art 
{

const std::string ROCK_SIGN =
R"(
    _______
---'   ____)
      (_____)
      (_____)
      (____)
---.__(___)
)";

const std::string PAPRT_SIGN =
R"(
    _______
---'   ____)____
          ______)
          _______)
         _______)
---.__________)
)";


const std::string SCISSOR_SIGN = 
R"(
    _______
---'   ____)____
          ______)
       __________)
      (____)
---.__(___)
)";

}

#endif
