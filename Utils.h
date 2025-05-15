//
// Created by nikit on 5/7/2025.
//

#ifndef MONKEY_TYPERGAME_UTILS_H
#define MONKEY_TYPERGAME_UTILS_H

#include <SFML/Graphics.hpp>
#include <string>
#include "DropdownCtrl.h"
#include "Configuration.h"

// chose font
enum Fonts {
    RANDOM_WEDNESDAY,
    TIMES,
    VERDANA
};

enum WordSpeeds {
    SLOW_1,
    NORMAL_2,
    FAST_3
};

// chose font size
enum FontSizes {
    SMALL,
    MEDIUM,
    LARGE
};

class Start {
public:
    Start(sf::RenderWindow& window, Configuration& config);
    int run(); // returns level number (1 or 2), or 0 for Score, or -1 for exit
    void LoadFont();//load font
    void LoadFontSize();//load font size
    void LoadSpeedWords();//load speed for words move
    void LoadMaxWordsLength();//load max words length
    //void LoadTopicWords();//load words from file by the selected topic
    std::string LoadTopicWords();
private:
    Configuration& configSets;
    sf::RenderWindow& window;

    // labels
    sf::Text fontNameDropdownLabel, fontSizeDropdownLabel, speedDropdownLabel, topicDropdownLabel, wordSizeLabel, configLabel, buttonLabel;

    sf::RectangleShape levelOneBtn, levelTwoBtn, scoreBtn, panelConfig, panelButtons;
    sf::Text levelOneText, levelTwoText, scoreText, panelConfigText, panelButtonsText;
    DropdownCtrl fontNameDropdown;
    DropdownCtrl fontSizeDropdown;
    DropdownCtrl speedDropdown;
    DropdownCtrl topicDropdown;
    DropdownCtrl wordSizeDropdown;

    void draw();
    int handleEvents();
};

class Game {
public:
    Game(Configuration& config, const std::string& resultsFile);
    void run();
    void setSpeed();

private:
    sf::Text speedWordsLabel;
    std::vector<std::string> speedLabels = {"Slow", "Normal", "Fast"};
    Configuration& config;
    std::string resultFile;

    int correctWordsCount;
    int incorrectWordsCount;
    int timePlayed;
    float speed;
    std::string inputBuffer;
    std::size_t currentWordIndex;

    void drawStyledText();
    void showWinScreen();
    void showLoseScreen();
    std::string askPlayerName();
    static std::vector<std::string> readWordsByLength(const std::string& filename, Configuration configuration);
};

class Result {
    std::string name;
    int correctWords;
    int incorrectWords;
    int timePlayed;

public:
    static std::vector<Result> loadFromFile(const std::string& filename);

    std::string toString() const;

    Result (const std::string& playerName = "Player", int correct = 0, int incorrect = 0, int time = 0);

    void saveToFile(const std::string& filename) const;
};

/*
class Destructor {
public:
    static void showWinScreen(sf::Font& font);
    static void showLoseScreen(sf::Font& font);
    static std::string promptUserName(sf::Font& font);
};
*/

std::vector<std::string> loadWordsFromFile(const std::string& fileName);
void displayResultsWindow(const std::string& resultsFile, const sf::Font& font);

#endif //MONKEY_TYPERGAME_UTILS_H
