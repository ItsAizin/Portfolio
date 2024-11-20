#include <string>
#include <vector>
#include <iostream>
#include <chrono>

#include "post.h"
#include "note.h"
#include "question.h"
#include "poll.h"

/**
 * TODO: Why is our argument a const? What is the importance of this?
 * Response: it is because const varibles cannot be changed at all. This is to prevent
 * an event that posts is reassigned to something different that would break this function.
 * TODO: What does the "&" symbol do?
 * Response: & is the information stored at that variable (posts). So the arugment is to
 * get the all the information at where it's stored
*/
void previewPosts(const std::vector<Post*>& posts) {
    int counter = 0;
    std::cout << "==============================Posts Preview=====================================\n";
    for(Post* p : posts) {
        std::cout << counter++ << ": " << p->getTitle() << std::endl;
    }
    std::cout << "================================================================================\n\n";
    return;
}
/**
 * TODO: Sometimes this function throws an error. Find out why and fix it
*/
void displayPost(const std::vector<Post*>& posts,int i) {
  if (i > posts.size && i < 0) {
    std::cout << "The index given is incorrect" << std::endl;
  } else {
    posts.at(i)->view();
    return;
  }
}

/**
 * TODO: Give a description of what is happening in this method
 * Response: This function allows users to interact with specific post that they selected by voting or liking it.
 * TODO: Why are we dynamically casting?
 * Response: It is so that it will correctly categorize what is a note, question, and poll
 * TODO: What does dynamic casting do?
 * Response:  It is so that it will correctly categorize what is a note, question, and poll
 * TODO: How is dynamic casting different from static casting?:
 * Response: Static casting has to be hand coded to what cast should the variable be. As dynamic casting will
 * automatically cast the variable for you.
*/
void interactWithPost(const std::vector<Post*>& posts, int i) {
    if(i < 0 || i >= posts.size()) {
      std::cout << "Invalid selection: " << i << std::endl;
      return;
    }

    Question* q = dynamic_cast<Question*>(posts.at(i));
    if (q) {
      q->view();
      std::string response;
      std::cout << "Enter a response to the question above.\n >";
      std::getline(std::cin,response); // Need this to grab the endline character from the previous selection in the cin buffer. This input is "thrown away"
      std::getline(std::cin,response); // Actual user input
      q->addResponse(response);
      return;
    }

    Poll* p = dynamic_cast<Poll*>(posts.at(i));
    if (p) {
      p->view();
      int selection;
      std::cout << "Enter the option you would like to vote for:";
      std::cin >> selection;
      p->castVote(selection);
      return;
    }

    std::cout << "This post can not be interacted with\n";
    
}
/**
 * TODO: Make 'getUsernameFromConsole` method here
*/ 

void getUsernameFromConsole(string username) {
  std::cout << "Welcome to CSCI3081 Question Base." << std::endl;
  
  while (true) {
    if (username != "") {
      return;
    } else {
      std::cout << "Please sign in to continue." << std::endl;
      std::cin >> "Username: " >> username >> std::endl;
    }
  }
}


int main() {

  std::string username = "Notch";
  /** TODO: Get username using the `getUsernameFromConsole` function*/
  getUsernameFromConsole(username);
  

  std::vector<Post*> database;
  database.push_back(new Note("Y2K Computers","Are the computers really going to break tonight?","Tim Hatman",946684800));
  database.push_back(new Note("First Day","First day of class is this Tuesday 1/16","Alec Lorimer",1705400645));
  database.push_back(new Note("Leap Year!","Happy 2024 everyone! It's a leap year this year. How exciting","Calendar man",1704067200));
  database.push_back(new Question("Homework 3 Crunch","Is it possible to start and complete homework 3 on the day its due?","Concerned Student",1714517376));
  database.push_back(new Poll("Who is where?","What lecture section is everybody in?","Potential Teammate",1714517376, std::vector<std::string>({"01","10","20","30"})));

  int selection;
  bool run = true;
  std::string menu = "\n=====Menu=====\n0. Quit\n1. Preview\n2. Read post\n3. Interact with Post\nEnter an action:";

  while(run) {
    std::cout << menu;
    std::cin >> selection;
    switch (selection)
    {
      case 0:
        run = false;
        break;
      case 1:
        previewPosts(database);
        break;
      case 2:
        std::cout << "Enter the index of the post you would like to view.\nPost Index:";
        std::cin >> selection;
        displayPost(database,selection);
        break;
        
      case 3:
        std::cout << "What post would you like to interact with?.\nPost Index:";
        std::cin >> selection;
        interactWithPost(database,selection);
        break;
          
      default:
        break;
    }
  }

  std::cout << "Goodbye.\n";

  return 0;
}
