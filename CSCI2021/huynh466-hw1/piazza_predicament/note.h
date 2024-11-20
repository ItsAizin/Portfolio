
#include "post.h"

#ifndef NOTE_H
#define NOTE_H

class Note : public Post {
 public:
    ~Note() {}
    Note(std::string title_, std::string text_, std::string author_, long created_): Post(title_, text_, author_, created_) {}
    void view();
};

#endif  // NOTE_H
