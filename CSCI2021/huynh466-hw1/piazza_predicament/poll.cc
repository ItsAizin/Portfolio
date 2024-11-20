#include "poll.h"

Poll::Poll(std::string title_, std::string text_, std::string author_, long created_, std::vector<std::string> options_): Post(title_, text_, author_, created_) {
    this->options = options_;
    for (auto _ : this->options) {
        this->votes.push_back(0);
    }
}

void Poll::view() {
    std::cout << "================================================================================\n";
    std::cout << "Poll: " << this->title << std::endl;
    std::cout << "> By: " << this->author << std::endl;
    std::cout << "> " << this->text << std::endl;
    this->views++;
    printf("> Views: %d\n",this->views);
    for (int c = 0; c < this->options.size(); c++) {
        printf("  Option %d: [Votes:%d] %s\n",c,this->votes[c],this->options[c].c_str());
    }
    std::cout << "================================================================================\n";
    return;
}


void Poll::castVote(int selection) {
    if(selection < 0 || selection >= this->votes.size()) {
        std::cout << "Invalid selection: " << selection << std::endl;
        return;
    }
    this->votes[selection]++;
}
