#include "post.h"

std::string Post::getTitle() {
    return this->title;
}
std::string Post::getText() {
    return this->text;
}

int Post::getViews() {
    return this->views;
}

std::string Post::getAuthor() {
    return this->author;
}

long Post::getCreated() {
    return this->created;
}

void Post::setTitle(std::string title_) {
    this->title = title_;
}

void Post::setText(std::string text_) {
    this->text = text_;
}