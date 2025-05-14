//
// Created by nikit on 5/7/2025.
//

#include "Utils.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

/*std::string Destructor::promptUserName(sf::Font& font) {
    sf::RenderWindow window(sf::VideoMode(400, 200), "Enter Your Name");
    std::string name;

    sf::Text prompt("Enter your name:", font, 30);
    prompt.setFillColor(sf::Color::White);
    prompt.setPosition(50, 30);

    sf::Text nameText("", font, 30);
    nameText.setFillColor(sf::Color::Yellow);
    nameText.setPosition(50, 80);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return "Player";  // default name if user closes
            }

            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode == '\b' && !name.empty()) {
                    name.pop_back();  // backspace
                } else if (event.text.unicode >= 32 && event.text.unicode <= 126) {
                    name += static_cast<char>(event.text.unicode);
                }
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter && !name.empty()) {
                window.close();
                return name;
            }
        }

        nameText.setString(name);

        window.clear();
        window.draw(prompt);
        window.draw(nameText);
        window.display();

    }
    return "Player";  // fallback
}

void Destructor::showWinScreen(sf::Font& font) {
    sf::RenderWindow window(sf::VideoMode(400, 200), "Game Over");

    sf::Text winText("You Won!", font, 40);
    winText.setFillColor(sf::Color::Green);
    winText.setPosition(100, 80);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed ||
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter)) {
                window.close();
            }
        }

        window.clear(sf::Color::Black);
        window.draw(winText);
        window.display();
    }
}

void Destructor::showLoseScreen(sf::Font& font) {
    sf::RenderWindow window(sf::VideoMode(400, 200), "Game Over");

    sf::Text loseText("You Lose!", font, 40);
    loseText.setFillColor(sf::Color::Red);
    loseText.setPosition(100, 80);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed ||
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter)) {
                window.close();
            }
        }

        window.clear(sf::Color::Black);
        window.draw(loseText);
        window.display();
    }
}*/

void loadWordsFromFile(const std::string& fileName, Configuration& config) {
    std::ifstream inputFile(fileName);
    if (inputFile.is_open()) {
        std::string word;
        while (inputFile >> word) { // read words from file
            if (word.length() <= config.maxWordsLength)
                config.topicWords.push_back(word);
        }
        inputFile.close();
        std::cout << "Loaded words from " << fileName << "\n";
    } else {
        std::cerr << "Unable to open file: " << fileName << "\n";
    }
}

// Optional: display results window, on dialog window
void displayResultsWindow(const std::string& resultsFile, const sf::Font& font) {
    std::vector<Result> results = Result::loadFromFile(resultsFile);
    sf::RenderWindow scoreWindow(sf::VideoMode(800, 400), "Game Scores", sf::Style::Close);

    std::vector<sf::Text> resultTexts;
    for (std::size_t i = 0; i < results.size(); ++i) {
        sf::Text resultText;
        resultText.setFont(font);
        resultText.setCharacterSize(20);
        resultText.setFillColor(sf::Color::White);
        resultText.setString(results[i].toString());
        resultText.setPosition(20, 30 * i + 20);
        resultTexts.push_back(resultText);
    }

    while (scoreWindow.isOpen()) {
        sf::Event event;
        while (scoreWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                scoreWindow.close();
        }

        scoreWindow.clear(sf::Color::Black);
        for (const auto& text : resultTexts) {
            scoreWindow.draw(text);
        }
        scoreWindow.display();
    }
}

Result::Result(const std::string& playerName, int correct, int incorrect, int time)
        : name(playerName), correctWords(correct), incorrectWords(incorrect), timePlayed(time) {}
// write result to file
void Result::saveToFile(const std::string& filename) const {
    std::ofstream file(filename, std::ios::app);
    if (file.is_open()) {
        file << "name: " << name << ", correctWords: " << correctWords << ", incorrectWords: " << incorrectWords << ", timePlayed: " << timePlayed << std::endl;
        file.close();
    } else {
        std::cerr << "Unable to open file for saving results.\n";
    }
}

std::vector<Result> Result::loadFromFile(const std::string& filename) {
    std::vector<Result> results;
    std::ifstream file(filename);
    if (file.is_open()) {
        std::string line;

        while (std::getline(file, line)) {
            std::string playerName;
            int correct = 0, incorrect = 0, time = 0;

            // Expected format:
            // name: Nikita, correctWords: 5, incorrectWords: 3, timePlayed: 20

            std::size_t namePos = line.find("name: ");
            std::size_t correctPos = line.find("correctWords: ");
            std::size_t incorrectPos = line.find("incorrectWords: ");
            std::size_t timePos = line.find("timePlayed: ");

            if (namePos != std::string::npos &&
                correctPos != std::string::npos &&
                incorrectPos != std::string::npos &&
                timePos != std::string::npos) {

                playerName = line.substr(namePos + 6, correctPos - namePos - 8);
                correct = std::stoi(line.substr(correctPos + 14, incorrectPos - correctPos - 16));
                incorrect = std::stoi(line.substr(incorrectPos + 16, timePos - incorrectPos - 18));
                time = std::stoi(line.substr(timePos + 12));

                results.emplace_back(playerName, correct, incorrect, time);
            }
        }

        file.close();
    } else {
        std::cerr << "Unable to open file for loading results.\n";
    }
    return results;
}

std::string Result::toString() const {
    return "name: " + name + ", " + "correctWords: " + std::to_string(correctWords) + ", incorrectWords: " + to_string(incorrectWords) + ", timePlayed: " +
            to_string(timePlayed);
}


Game::Game(Configuration& configSets, const std::string& file)
        : config(configSets), resultFile(file),
          correctWordsCount(0), incorrectWordsCount(0),
          timePlayed(0), speed(0.02f), inputBuffer(""),
          currentWordIndex(0) {}
// method response by moving words
void Game::run() {
    speedWordsLabel.setFont(config.font);
    speedWordsLabel.setString("speed: " + std::to_string(speed)); // config.moveSpeed
    speedWordsLabel.setCharacterSize(config.fontSize);
    speedWordsLabel.setFillColor(sf::Color::White);
    speedWordsLabel.setPosition(0, 0); // x, y

    setSpeed();//set speed according to configSets

    sf::RenderWindow textWindow(sf::VideoMode(1200, 900), "MonkeyTyper - Game");
    sf::Clock gameClock;

    std::vector<sf::Text> wordTexts;

    std::size_t numWordsToDisplay = 5 + std::rand() % 6;

    for (std::size_t i = 0; i < numWordsToDisplay; ++i) {
        std::size_t idx = currentWordIndex++ % config.topicWords.size();
        //////////
        //word inside config.topicWords[idx]. You need a new for through letters and create text control for each letter.
        //positions x should include space between letters and increased letter by letter. y is the same.

        sf::Text text(config.topicWords[idx], config.font, config.fontSize);
        text.setFillColor(sf::Color::White);
        float x = (float)(std::rand() % (textWindow.getSize().x / 3));
        float y = (float)(std::rand() % (textWindow.getSize().y - 30));
        text.setPosition(x, y);

        wordTexts.push_back(text);
    }

    while (textWindow.isOpen()) {
        sf::Event event;
        while (textWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                timePlayed = (int)(gameClock.getElapsedTime().asSeconds());
                std::string name = askPlayerName();
                Result(name, correctWordsCount, incorrectWordsCount, timePlayed).saveToFile(resultFile);
                textWindow.close();
            }

            if (event.type == sf::Event::TextEntered) {
                char ch = (char)(event.text.unicode);
                if (ch == '\b' && !inputBuffer.empty()) {
                    inputBuffer.pop_back();
                } else if (ch >= 32 && ch <= 126) {
                    inputBuffer += ch;
                }
            }
        }
        //animation to move words
        for (auto& text : wordTexts) {
            text.move(speed, 0.0f);

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                config.moveSpeed--;
                setSpeed();
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                config.moveSpeed++;
                setSpeed();
            }
        }

        std::sort(wordTexts.begin(), wordTexts.end(), [](const sf::Text& a, const sf::Text& b) {
            return a.getPosition().x > b.getPosition().x;
        });
        // distinguish first second and third word
        if (!wordTexts.empty()) wordTexts[0].setFillColor(sf::Color::Red);
        if (wordTexts.size() > 1) wordTexts[1].setFillColor(sf::Color::Yellow);
        if (wordTexts.size() > 2) wordTexts[2].setFillColor(sf::Color::Blue);

        if (!wordTexts.empty() && inputBuffer == wordTexts[0].getString()) {
            wordTexts.erase(wordTexts.begin());
            inputBuffer.clear();
            correctWordsCount++;

            if (correctWordsCount >= config.topicWords.size()) {
                showWinScreen();
                textWindow.close();
            }
        }

        auto it = std::remove_if(wordTexts.begin(), wordTexts.end(), [&](sf::Text& text) {
            if (text.getPosition().x > textWindow.getSize().x) {
                incorrectWordsCount++;
                if (incorrectWordsCount == 5) {
                    showLoseScreen(); //
                    textWindow.close();
                }
                return true;
            }
            return false;
        });
        wordTexts.erase(it, wordTexts.end());

        while (wordTexts.size() < numWordsToDisplay) {
            std::size_t idx = currentWordIndex++ % config.topicWords.size();
            sf::Text text(config.topicWords[idx], config.font, config.fontSize);
            text.setFillColor(sf::Color::White);
            float x = (float)(std::rand() % (textWindow.getSize().x / 2));
            float y = (float)(std::rand() % (textWindow.getSize().y - 30));
            text.setPosition(x, y);
            wordTexts.push_back(text);
        }

        int secondsElapsed = (int)(gameClock.getElapsedTime().asSeconds());

        textWindow.clear();

        for (const auto& text : wordTexts) {
            textWindow.draw(text);
        }

        sf::Text inputText(inputBuffer, config.font, config.fontSize);
        inputText.setFillColor(sf::Color::Yellow);
        inputText.setPosition(20, 850);
        textWindow.draw(inputText);

        std::stringstream footer;
        footer << "Correct: " << correctWordsCount
               << "  Incorrect: " << incorrectWordsCount
               << "  Time: " << secondsElapsed << "s";

        sf::Text footerText(footer.str(), config.font, config.fontSize);
        footerText.setFillColor(sf::Color::White);
        footerText.setPosition(20, 870);
        textWindow.draw(footerText);

        textWindow.draw(speedWordsLabel);
        textWindow.display();
    }
}

void Game::setSpeed()
{
    switch (config.moveSpeed) {
        case 1:
            speed = 0.02f;
            break;
        case 2:
            speed = 0.04f;
            break;
        case 3:
            speed = 0.08f;
            break;
    }
}

void Game::showWinScreen() {
    sf::RenderWindow winWindow(sf::VideoMode(400, 200), "Game Over");
    sf::Text winText("You Won!", config.font, config.fontSize);
    winText.setFillColor(sf::Color::Green);
    winText.setPosition(100, 80);

    while (winWindow.isOpen()) {
        sf::Event e;
        while (winWindow.pollEvent(e)) {
            if (e.type == sf::Event::Closed || (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Enter))
                winWindow.close();
        }
        winWindow.clear(sf::Color::Black);
        winWindow.draw(winText);
        winWindow.display();
    }
}

void Game::showLoseScreen() {
    sf::RenderWindow loseWindow(sf::VideoMode(400, 200), "Game Over");
    sf::Text loseText("You Lose!", config.font, config.fontSize);
    loseText.setFillColor(sf::Color::Red);
    loseText.setPosition(100, 80);

    while (loseWindow.isOpen()) {
        sf::Event e;
        while (loseWindow.pollEvent(e)) {
            if (e.type == sf::Event::Closed || (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Enter))
                loseWindow.close();
        }
        loseWindow.clear(sf::Color::Black);
        loseWindow.draw(loseText);
        loseWindow.display();
    }
}

std::string Game::askPlayerName() {
    sf::RenderWindow nameWindow(sf::VideoMode(400, 200), "Enter Your Name");
    std::string name;
    sf::Text prompt("Enter your name:", config.font, config.fontSize);
    prompt.setFillColor(sf::Color::White);
    prompt.setPosition(50, 30);

    sf::Text nameText("", config.font, config.fontSize);
    nameText.setFillColor(sf::Color::Yellow);
    nameText.setPosition(50, 80);

    while (nameWindow.isOpen()) {
        sf::Event e;
        while (nameWindow.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                nameWindow.close();
                return "Player";
            }

            if (e.type == sf::Event::TextEntered) {
                if (e.text.unicode == '\b' && !name.empty()) {
                    name.pop_back();
                } else if (e.text.unicode >= 32 && e.text.unicode <= 126) {
                    name += (char)(e.text.unicode);
                }
            }

            if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Enter && !name.empty()) {
                nameWindow.close();
                return name;
            }
        }

        nameText.setString(name);
        nameWindow.clear();
        nameWindow.draw(prompt);
        nameWindow.draw(nameText);
        nameWindow.display();
    }

    return "Player";
}

Start::Start(sf::RenderWindow& win, Configuration& configSets)
        : window(win), configSets(configSets){
    std::vector<std::string> fontNames = {"Random Wednesday", "Times", "Verdana"};
    std::vector<std::string> topics = {"Cpp", "JAVA", "SQL"};
    std::vector<std::string> sizeLabels = {"Small", "Medium", "Large"};
    std::vector<std::string> speedLabels = {"Slow", "Normal", "Fast"};
    std::vector<std::string> sizeOfWords = {"not more then 5 word", "not more than 10 word", "not more than 20 word"};

    fontNameDropdownLabel.setFont(configSets.font);
    fontNameDropdownLabel.setString("Chose font");
    fontNameDropdownLabel.setCharacterSize(20);
    fontNameDropdownLabel.setFillColor(sf::Color::White);
    fontNameDropdownLabel.setPosition(100, 177);

    fontSizeDropdownLabel.setFont(configSets.font);
    fontSizeDropdownLabel.setString("Chose text size");
    fontSizeDropdownLabel.setCharacterSize(20);
    fontSizeDropdownLabel.setFillColor(sf::Color::White);
    fontSizeDropdownLabel.setPosition(320, 177);

    speedDropdownLabel.setFont(configSets.font);
    speedDropdownLabel.setString("Chose speed moving words");
    speedDropdownLabel.setCharacterSize(20);
    speedDropdownLabel.setFillColor(sf::Color::White);
    speedDropdownLabel.setPosition(100, 377);

    topicDropdownLabel.setFont(configSets.font);
    topicDropdownLabel.setString("Chose topic of the text");
    topicDropdownLabel.setCharacterSize(20);
    topicDropdownLabel.setFillColor(sf::Color::White);
    topicDropdownLabel.setPosition(100, 577);

    wordSizeLabel.setFont(configSets.font);
    wordSizeLabel.setString("Chose words size");
    wordSizeLabel.setCharacterSize(20);
    wordSizeLabel.setFillColor(sf::Color::White);
    wordSizeLabel.setPosition(350, 577);

    fontNameDropdown.Initialize(100, 200, 200, fontNames, configSets.font);
    fontSizeDropdown.Initialize(320, 200, 100, sizeLabels, configSets.font);
    speedDropdown.Initialize(100, 400, 100, speedLabels, configSets.font);
    topicDropdown.Initialize(100, 600, 200, topics, configSets.font);
    wordSizeDropdown.Initialize(350, 600, 200, sizeOfWords, configSets.font);

    // add two rectangles like layout
    panelConfig.setSize(sf::Vector2f(700, 900));
    panelConfig.setPosition(0, 0);
    panelConfig.setFillColor(sf::Color::Cyan); // problem with color

    panelButtons.setSize(sf::Vector2f(700, 900));
    panelButtons.setPosition(700, 0);
    panelButtons.setFillColor(sf::Color::Magenta);

/*
    panelButtons.setSize(sf::Vector2f(10, 900));
    panelButtons.setPosition(700, 0);
    panelButtons.setFillColor(sf::Color::Magenta);
*/

    // Initialize buttons
    levelOneBtn.setSize(sf::Vector2f(200, 50));
    levelOneBtn.setPosition(950, 300);
    levelOneBtn.setFillColor(sf::Color::Green);

//    levelTwoBtn.setSize(sf::Vector2f(200, 50));
//    levelTwoBtn.setPosition(950, 450);
//    levelTwoBtn.setFillColor(sf::Color::Green);

    scoreBtn.setSize(sf::Vector2f(200, 50));
    scoreBtn.setPosition(950, 600);
    scoreBtn.setFillColor(sf::Color::Blue);

    configLabel.setFont(configSets.font);
    configLabel.setString("Configuration");
    configLabel.setCharacterSize(20);
    configLabel.setFillColor(sf::Color::Cyan);
    configLabel.setPosition(350, 50);

    buttonLabel.setFont(configSets.font);
    buttonLabel.setString("Buttons");
    buttonLabel.setCharacterSize(20);
    buttonLabel.setFillColor(sf::Color::White);
    buttonLabel.setPosition(1000, 50);

    // Initialize text
    levelOneText.setFont(configSets.font);
    levelOneText.setCharacterSize(30);
    levelOneText.setString("Play");
    levelOneText.setPosition(950, 310);
    levelOneText.setFillColor(sf::Color::White);

    /*levelTwoText.setFont(configSets.font);
    levelTwoText.setCharacterSize(30);
    levelTwoText.setString("Level 2");
    levelTwoText.setPosition(950, 460);
    levelTwoText.setFillColor(sf::Color::White);
*/
    scoreText.setFont(configSets.font);
    scoreText.setCharacterSize(30);
    scoreText.setString("Score");
    scoreText.setPosition(950, 610);
    scoreText.setFillColor(sf::Color::White);
}

int Start::run() {
    while (window.isOpen()) {
        int action = handleEvents(); // process buttons

        if (action != -1) return action;

        draw();
    }
    return -1; // window closed
}
// chose font
enum Fonts {
    RANDOM_WEDNESDAY,
    TIMES,
    VERDANA
};

const std::string fontFiles[] = {
        "../Random Wednesday.ttf",
        "../times.ttf",
        "../verdana.ttf"
};

void Start::LoadFont() {
    int selectedIndex = fontNameDropdown.getSelectedIndex();
    switch (selectedIndex) {
        case RANDOM_WEDNESDAY:
        case TIMES:
        case VERDANA:
            if (!configSets.font.loadFromFile(fontFiles[selectedIndex])) {
                std::cerr << "Error loading font: " << fontFiles[selectedIndex] << "\n";
            }
            break;
        default:
            std::cerr << "Invalid font index!\n";
            break;
    }
}
// chose font size
enum FontSizes {
    SMALL,
    MEDIUM,
    LARGE
};

void Start::LoadFontSize() {
    int selectedIndex = fontSizeDropdown.getSelectedIndex();
    switch (selectedIndex) {
        case SMALL:
            configSets.fontSize = 40;
            break;
        case MEDIUM:
            configSets.fontSize = 60;
            break;
        case LARGE:
            configSets.fontSize = 80;
            break;
        default:
            configSets.fontSize = 20;
            break;
    }
}
// chose words speed
enum WordSpeeds {
    SLOW_1,
    NORMAL_2,
    FAST_3
};

void Start::LoadSpeedWords() {
    int selectedIndex = speedDropdown.getSelectedIndex();

    switch (selectedIndex) {
        case SLOW_1:
            configSets.moveSpeed = 1;
            break;
        case NORMAL_2:
            configSets.moveSpeed = 2;
            break;
        case FAST_3:
            configSets.moveSpeed = 3;
            break;
        default:
            configSets.moveSpeed = 1;  // fallback speed
            break;
    }
}

// chose max words length
enum WordSizes {
    SHORT_5,
    MEDIUM_10,
    LONG_20
};

void Start::LoadMaxWordsLength() {
    int selectedIndex = wordSizeDropdown.getSelectedIndex();

    switch (selectedIndex) {
        case SHORT_5:
            configSets.maxWordsLength = 5;
            break;
        case MEDIUM_10:
            configSets.maxWordsLength = 10;
            break;
        case LONG_20:
            configSets.maxWordsLength = 20;
            break;
        default:
            configSets.maxWordsLength = 5;  // fallback value
            break;
    }
}

// chose topic
enum Topics {
    CPP,
    JAVA,
    SQL
};

std::string Start::LoadTopicWords() {
    std::string path;
    int selectedIndex = topicDropdown.getSelectedIndex();

    switch (selectedIndex) {
        case CPP:
            path = "../Cpp.txt";
            break;
        case JAVA:
            path = "../JAVA.txt";
            break;
        case SQL:
            path = "../SQL.txt";
            break;
        default:
            std::cerr << "Selected topic index is invalid.\n";
            return "";
    }

    // Load words using the selected path
    loadWordsFromFile(path, configSets);

    return path;
}

int Start::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
            return -1;
        }
        //handlers
        fontNameDropdown.handleEvent(event, window);
        fontSizeDropdown.handleEvent(event, window);
        speedDropdown.handleEvent(event, window);
        topicDropdown.handleEvent(event, window);
        wordSizeDropdown.handleEvent(event, window);

        if (event.type == sf::Event::MouseButtonPressed) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            sf::Vector2f mouseF((float )(mousePos.x), (float )(mousePos.y));
            //Save settings into configSets object
            configSets.moveSpeed = speedDropdown.getSelectedIndex();
            configSets.maxWordsLength = wordSizeDropdown.getSelectedIndex();
            //check buttons click
            if (levelOneBtn.getGlobalBounds().contains(mouseF))
                return 1;
//            if (levelTwoBtn.getGlobalBounds().contains(mouseF))
//                return 2;
            if (scoreBtn.getGlobalBounds().contains(mouseF))
                return 0;
        }
    }
    return -1;
}

void Start::draw() {
    window.clear();

    window.draw(panelConfigText);
    window.draw(panelButtonsText);

    window.draw(panelConfig);
    window.draw(panelButtons);

    // draw labels
    window.draw(fontNameDropdownLabel);
    window.draw(fontSizeDropdownLabel);
    window.draw(speedDropdownLabel);
    window.draw(topicDropdownLabel);
    window.draw(wordSizeLabel);

    window.draw(buttonLabel);
    window.draw(configLabel);

    fontNameDropdown.draw(window);//redraw font combobox
    fontSizeDropdown.draw(window);// redraw size combobox
    speedDropdown.draw(window);
    topicDropdown.draw(window);
    wordSizeDropdown.draw(window);

    window.draw(levelOneBtn);
    //window.draw(levelTwoBtn);
    window.draw(scoreBtn);

    window.draw(levelOneText);
    window.draw(levelTwoText);
    window.draw(scoreText);

    window.display();
}