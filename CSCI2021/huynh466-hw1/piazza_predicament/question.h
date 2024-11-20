#include <string>
#include <iostream>

#ifndef QUESTION_H
#define QUESTION_H

#include "post.h"

class Question : public Post {
    private:
        vector<std::string> responses;
    public:
        Question();
        ~Question();
        void addResponse(std::string response) {};
};


#endif // QUESTION_H