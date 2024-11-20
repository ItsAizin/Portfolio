#include "note.h"

void Note::view() {
    std::cout << "================================================================================\n";
    std::cout << "Note: " << this->title << std::endl;
    std::cout << "> By: " << this->author << std::endl;
    std::cout << "> " << this->text << std::endl;
    this->views++;
    printf("> Views: %d\n",this->views);
    std::cout << "================================================================================\n";
    return;
}
