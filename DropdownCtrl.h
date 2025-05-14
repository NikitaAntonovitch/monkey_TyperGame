//
// Created by nikit on 5/11/2025.
//

#ifndef MONKEY_TYPERGAME_DROPDOWNCTRL_H
#define MONKEY_TYPERGAME_DROPDOWNCTRL_H

#include <SFML/Graphics.hpp>


class DropdownCtrl {
    // font
private:
    sf::RectangleShape Box;
    std::vector<sf::RectangleShape> options;
    std::vector<sf::Text> optionTexts;
    sf::Text label;
    std::vector<std::string> names;
    bool expanded = false;
    int selectedIndex = 0;
public:
    void Initialize(float x, float y, float width, std::vector<std::string> fontNames, sf::Font& labelFont);
    void handleEvent(sf::Event& event, sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);
    int getSelectedIndex();
};

#endif //MONKEY_TYPERGAME_DROPDOWNCTRL_H
