//
// Created by nikit on 5/11/2025.
//

#include "DropdownCtrl.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

void DropdownCtrl::Initialize(float x, float y, float width, std::vector<std::string> listOfNames, sf::Font& labelFont) {
    Box.setPosition(x, y);
    Box.setSize({width, 40});
    Box.setFillColor(sf::Color(150, 150, 150));
    
    names = listOfNames;

    label.setFont(labelFont);
    label.setCharacterSize(20);
    label.setPosition(x + 10, y + 5);
    label.setString(listOfNames[0]);

    for (size_t i = 0; i < listOfNames.size(); ++i) {
        sf::RectangleShape opt({width, 40});
        opt.setPosition(x, y + (i + 1) * 40);
        opt.setFillColor(sf::Color(180, 180, 180));
        options.push_back(opt);

        sf::Text txt(listOfNames[i], labelFont, 20);
        txt.setPosition(x + 10, y + 5 + (i + 1) * 40);
        optionTexts.push_back(txt);
    }
}

void DropdownCtrl::handleEvent(sf::Event& event, sf::RenderWindow& window) {
    if (event.type == sf::Event::MouseButtonPressed) {
        auto pos = sf::Mouse::getPosition(window);

        if (Box.getGlobalBounds().contains(pos.x, pos.y)) {
            expanded = !expanded;
        } else
            if (expanded) {
                for (size_t i = 0; i < options.size(); ++i) {
                    if (options[i].getGlobalBounds().contains(pos.x, pos.y)) {
                        selectedIndex = i;
                        label.setString(names[i]);
                        expanded = false;
                        break;
                    }
                }
                expanded = false;
            }
    }
}

void DropdownCtrl::draw(sf::RenderWindow& window) {
    window.draw(Box);
    window.draw(label);
    if (expanded) {
        for (size_t i = 0; i < options.size(); ++i) {
            window.draw(options[i]);
            window.draw(optionTexts[i]);
        }
    }
}

int DropdownCtrl::getSelectedIndex(){
    return selectedIndex;
}