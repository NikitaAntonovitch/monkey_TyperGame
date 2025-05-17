#include <iostream>
#include <cstdlib>
#include <sstream>

#include "Utils.h"

auto main() -> int{
    sf::RenderWindow mainWindow(sf::VideoMode(1400, 900), "MonkeyTyper",
                                sf::Style::Titlebar | sf::Style::Close);

    Configuration config;//keep configuration settings
    config.font.loadFromFile("FONTfiles/arial.ttf");//load default system font
    config.fontSize = 40;//font size by default

    const std::string resultsFile = "FONTfiles/results.txt";
    Start confWindow(mainWindow, config);//ConfigurationWindow

    while (mainWindow.isOpen()) {

        int choice = confWindow.run(); // LoadSettings in Start window
        confWindow.LoadFont();//Load Font
        confWindow.LoadFontSize();//Load Font Size
        confWindow.LoadSpeedWords();//Load the speed of words
        confWindow.LoadMaxWordsLength();//Load the speed of words
        confWindow.LoadTopicWords();//Load words according to the selected topic

        if (choice == 1) { // game
            if (!config.topicWords.empty()) {
                Game game(config, resultsFile);
                game.run();
            }
        } else if (choice == 0) { // result
            displayResultsWindow(resultsFile, config.font);
        } else {
            break; // Exit
        }
    }
}