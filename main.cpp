#include <iostream>
#include <cstdlib>
#include <sstream>

#include "Utils.h"

auto main() -> int{
    setlocale(LC_ALL, "pl");
    std::srand((int)(std::time(nullptr)));

    sf::RenderWindow mainWindow(sf::VideoMode(1400, 900), "MonkeyTyper", sf::Style::Titlebar | sf::Style::Close);

    Configuration config;//keep configuration settings
    config.font.loadFromFile("../arial.ttf");//load default system font
    config.fontSize = 40;//font size by default

    const std::string resultsFile = "../results.txt";
    Start confWindow(mainWindow, config);//ConfigurationWindow

    while (mainWindow.isOpen()) {

        int choice = confWindow.run(); // LoadSettings
        confWindow.LoadFont();//Load Font
        confWindow.LoadFontSize();//Load Font Size
        confWindow.LoadSpeedWords();//Load the speed of words
        confWindow.LoadMaxWordsLength();//Load the speed of words
        confWindow.LoadTopicWords();//Load words according to the selected topic

        if (choice == 1 || choice == 2) {
            if (!config.topicWords.empty()) {
                Game game(config, resultsFile);
                game.run();
            }
        } else if (choice == 0) {
            displayResultsWindow(resultsFile, config.font);
        } else {
            break; // Exit
        }
    }
}