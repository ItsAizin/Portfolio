#include "question.h"
#include <iostream>

Question::view() {
    std::cout << "Question: " << question.getTitle << std::endl;
    std::cout << "By: " << Question.getAuthor<< std::endl;
    std::cout << Question.getText << std::endl;
    Question.addView();
    std::cout << Question.getViews << std::endl;
};

Question::Question() {
      title = "NoTitle";
      text = "NoText";
      author = "NoAuthor";
      views = 0;
};

Question::~Question() {
    responses.clear();
};

Question::addResponse(std::string response) {
    reponses.push_back(response);
};
