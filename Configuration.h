//
// Created by nikit on 5/11/2025.
//

#ifndef MONKEY_TYPERGAME_CONFIGURATION_H
#define MONKEY_TYPERGAME_CONFIGURATION_H
#include <SFML/Graphics.hpp>

class Configuration {
public:
    sf::Font font;
    std::string fontFile; // path to font for saving/loading
    int fontSize;
    int maxWordsLength;
    int moveSpeed;
    std::vector<std::string> topicWords;//set of words for selected topic
};

#endif //MONKEY_TYPERGAME_CONFIGURATION_H
