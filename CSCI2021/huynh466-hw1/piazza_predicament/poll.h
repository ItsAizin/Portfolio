#include "post.h"

#include <vector>

#ifndef POLL_H
#define POLL_H

class Poll : public Post {
 private:
    std::vector<std::string> options;
    std::vector<int> votes;
 public:
    Poll() = delete;
    Poll(std::string title_, std::string text_, std::string author_, long created_, std::vector<std::string> options_);
    ~Poll() {}
    void view();
    void castVote(int);
    
};

#endif  // POLL_H
