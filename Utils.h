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

// choose topic
enum Topics {
    CPP,
    JAVA,
    SQL
};

// choose max words length
enum WordSizes {
    SHORT_5,
    MEDIUM_10,
    LONG_20
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

    sf::RectangleShape levelOneBtn, levelTwoBtn, scoreBtn, continueBtn, panelConfig, panelButtons;
    sf::Text levelOneText, levelTwoText, scoreText, continueText, panelConfigText, panelButtonsText;
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
    void run(bool resume = false);
    void setSpeed();
    void saveState(const std::vector<sf::Text>& wordTexts, int elapsedSeconds);
    bool loadState(std::vector<sf::Text>& wordTexts);
    static bool hasSavedGame(const std::string& filename);

private:
    const int points_for_correct_letter = 10;//amount of points for one letter in correct word
    const int points_for_incorrect_letter = 5;//amount of penalty points for one letter when incorrect word
    sf::Text speedWordsLabel;
    std::vector<std::string> speedLabels = {"Slow", "Normal", "Fast"};
    Configuration& config;
    std::string resultFile;

    int correctWordsCount;
    int incorrectWordsCount;
    int timePlayed;
    int score;
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
    int score;

public:
    Result(const std::string &playerName, int correct, int incorrect, int time, int gameScore);
    static std::vector<Result> loadFromFile(const std::string& filename);
    std::string toString() const;
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
