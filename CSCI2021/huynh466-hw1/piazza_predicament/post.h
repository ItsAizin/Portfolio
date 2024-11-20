#include <string>
#include <iostream>

#ifndef POST_H
#define POST_H

class Post {
 protected:
    std::string title;
    std::string text;
    std::string author;
    long created;
    int views;
    
 public:
   Post::Post() {
      title = "NoTitle";
      text = "NoText";
      author = "NoAuthor";
      created = 0;
      views = 0;
   };

   virtual Post::~Post() {
      title = "";
      text = "";
      author = "";
      created = 0;
      views = 0;
   };

   Post::Post(std::string title_, std::string text_, std::string author_, long created_) {
      title = title_;
      text = text_;
      author = author_;
      created = created_;
   };

   virtual view(){};

   void addView() {views++};

    /** Getters */
    std::string getTitle();
    std::string getText();
    std::string getAuthor();
    int getViews();
    long getCreated();

    /** Setters */
    void setTitle(std::string title_);
    void setText(std::string text_);
};



#endif  // POST_H
