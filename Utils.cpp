//
// Created by nikit on 5/7/2025.
//

#include "Utils.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <cstdio>

using namespace std;

static const std::string SAVE_FILE = "savegame.txt";

void loadWordsFromFile(const std::string& fileName, Configuration& config) { // read words for game from file
    std::ifstream inputFile(fileName);
    if (inputFile.is_open()) {
        std::string word;
        while (inputFile >> word) {
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
void displayResultsWindow(const std::string& resultsFile, const sf::Font& font) { // display window with scores
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

Result::Result(const std::string& playerName, int correct, int incorrect, int time, int gameScore)
        : name(playerName), correctWords(correct),
            incorrectWords(incorrect), timePlayed(time), score(gameScore) {}

void Result::saveToFile(const std::string& filename) const { // write result to file
    std::ofstream file(filename, std::ios::app);
    if (!file) {
        std::cerr << "Unable to open file for saving results.\n";
        return;
    }
    file << toString() << '\n';
}

std::string Result::toString() const {
    return "name: " + name +
           ", correctWords: " + std::to_string(correctWords) +
           ", incorrectWords: " + std::to_string(incorrectWords) +
           ", timePlayed: " + std::to_string(timePlayed) +
           ", score: " + std::to_string(score);
}

std::vector<Result> Result::loadFromFile(const std::string& filename) {  // read results from file, sort them, show only top 10, if you want see more open file
    std::vector<Result> results;
    std::ifstream file(filename);

    if (file.is_open()) {
        std::string line;

        int i = 0;
        while (std::getline(file, line)) {
            if (i >= 10) {
                break;
            }
            std::string playerName;
            int correct = 0, incorrect = 0, time = 0, score = 0;
            float speed = 0.0f;

            std::size_t namePos = line.find("name: ");
            std::size_t correctPos = line.find("correctWords: ");
            std::size_t incorrectPos = line.find("incorrectWords: ");
            std::size_t timePos = line.find("timePlayed: ");
            std::size_t scorePos = line.find("score: ");

            if (namePos != std::string::npos &&
                correctPos != std::string::npos &&
                incorrectPos != std::string::npos &&
                timePos != std::string::npos &&
                scorePos != std::string::npos) {

                playerName = line.substr(namePos + 6, correctPos - namePos - 8);
                correct = std::stoi(line.substr(correctPos + 14, incorrectPos - correctPos - 16));
                incorrect = std::stoi(line.substr(incorrectPos + 16, timePos - incorrectPos - 18));
                time = std::stoi(line.substr(timePos + 12, scorePos - timePos - 14));
                score = std::stof(line.substr(scorePos + 7));

                results.emplace_back(playerName, correct, incorrect, time, score);
            }
            i++;
        }

        file.close();
    } else {
        std::cerr << "Unable to open file for loading results.\n";
    }

    // Sort by score descending
    std::sort(results.begin(), results.end(), [](const Result& a, const Result& b) {
        return a.score > b.score;
    });

    return results;
}

Game::Game(Configuration& configSets, const std::string& file)
        : config(configSets), resultFile(file),
          correctWordsCount(0), incorrectWordsCount(0),
          timePlayed(0), speed(0.02f), score(0), inputBuffer(""),
          currentWordIndex(0) {
    if (!config.fontFile.empty()) {
        config.font.loadFromFile(config.fontFile);
    }
}

// method response by moving words
void Game::run(bool resume) {
    speedWordsLabel.setFont(config.font);
    speedWordsLabel.setString("speed: " + std::to_string(speed)); // config.moveSpeed
    speedWordsLabel.setCharacterSize(30);
    speedWordsLabel.setFillColor(sf::Color::White);
    speedWordsLabel.setPosition(0, 0); // x, y

    sf::RenderWindow textWindow(sf::VideoMode(1500, 900),
                                "MonkeyTyper - Game", sf::Style::Titlebar | sf::Style::Close);
    sf::Clock gameClock;

    std::vector<sf::Text> wordTexts;
    sf::RectangleShape strikeLine;//strikethrough the typed letters (strike out the word)
    std::size_t numWordsToDisplay = 5 + std::rand() % 6; // count of words on display

    if (resume && loadState(wordTexts)) {
        numWordsToDisplay = wordTexts.size();
    } else {
        // create unvisible lines to avoid unclear words
        for (std::size_t i = 0; i < numWordsToDisplay; ++i) {
            std::size_t idx = currentWordIndex++ % config.topicWords.size();

            sf::Text text(config.topicWords[idx], config.font, config.fontSize);
            text.setFillColor(sf::Color::White);
            float x = (float)(std::rand() % (textWindow.getSize().x / 3));
            int maxAmountOfLines = ((textWindow.getSize().y-100)/config.fontSize) - 1;
            int lineIndex = rand() % maxAmountOfLines;
            int y = 50 + lineIndex * config.fontSize;
            text.setPosition(x, y);
            wordTexts.push_back(text);
        }
    }

    setSpeed();//set speed according to configSets

    while (textWindow.isOpen()) {
        sf::Event event;
        while (textWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                timePlayed += (int)(gameClock.getElapsedTime().asSeconds());
                std::string name = askPlayerName();
                Result(name, correctWordsCount, incorrectWordsCount, timePlayed,
                       score).saveToFile(resultFile);
                std::remove(SAVE_FILE.c_str());
                textWindow.close();
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P) {
                saveState(wordTexts, (int)gameClock.getElapsedTime().asSeconds());
                textWindow.close();
            }

            if (event.type == sf::Event::TextEntered) {
                char ch = (char) (event.text.unicode);
                if (ch == '\b' && !inputBuffer.empty()) {
                    inputBuffer.pop_back();
                } else if (ch >= 32 && ch <= 126) {
                    inputBuffer += ch;
                }
                if (wordTexts[0].getString().find(inputBuffer, 0) == 0) {
                    // Get position of first and (first + lettersToStrike) character
                    sf::Vector2f start = wordTexts[0].findCharacterPos(0);
                    int iLen = inputBuffer.length();
                    sf::Vector2f end = wordTexts[0].findCharacterPos(iLen);//inputBuffer.length()
                    float strikeWidth = end.x - start.x;
                    strikeLine.setSize(sf::Vector2f(strikeWidth, 5)); // 5 pixels thick
                    strikeLine.setPosition(wordTexts[0].getPosition().x,
                                           wordTexts[0].getPosition().y +
                                           wordTexts[0].getCharacterSize() / 1.5); // mid-height
                }
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                config.moveSpeed--;
                setSpeed();
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                config.moveSpeed++;
                setSpeed();
            }
        }
        //animation to move words
        for (auto& text : wordTexts) {
            text.move(speed, 0.0f);
        }
        strikeLine.move(speed, 0.0f);//move strikethrough line as well

        std::sort(wordTexts.begin(), wordTexts.end(), [](const sf::Text& a, const sf::Text& b) {
            return a.getPosition().x > b.getPosition().x;
        });
        // distinguish first second and third word
        if (!wordTexts.empty()) wordTexts[0].setFillColor(sf::Color::Red);
        if (wordTexts.size() > 1) wordTexts[1].setFillColor(sf::Color::Yellow);
        if (wordTexts.size() > 2) wordTexts[2].setFillColor(sf::Color::Blue);

        //check if typed word (correctwords ++)
        if (!wordTexts.empty() && inputBuffer == wordTexts[0].getString()) {
            wordTexts.erase(wordTexts.begin());
            strikeLine.setSize(sf::Vector2f(0, 5));//erase strikethrough like
            score += inputBuffer.length() * points_for_correct_letter * config.moveSpeed; //add points for the correct word
            inputBuffer.clear();
            correctWordsCount++;

            if (correctWordsCount >= config.topicWords.size()) {
                showWinScreen();
                std::remove(SAVE_FILE.c_str());
                textWindow.close();
            }
        }
        // then erase word from the screen
        auto it = std::remove_if(wordTexts.begin(), wordTexts.end(), [&](sf::Text& text) {
            if (text.getPosition().x > textWindow.getSize().x) {
                score -= text.getString().getSize() * points_for_incorrect_letter * config.moveSpeed; //penalty points for the correct word
                incorrectWordsCount++;
                if (incorrectWordsCount == 5) {
                    showLoseScreen(); //
                    std::remove(SAVE_FILE.c_str());
                    textWindow.close();
                }
                return true;
            }
            return false;
        });
        wordTexts.erase(it, wordTexts.end());

        // display words only correct length
        while (wordTexts.size() < numWordsToDisplay) {
            std::size_t idx = currentWordIndex++ % config.topicWords.size();
            sf::Text text(config.topicWords[idx], config.font, config.fontSize);
            text.setFillColor(sf::Color::White);
            float x = (float)(std::rand() % (textWindow.getSize().x / 2));
            float y = (float)(std::rand() % (textWindow.getSize().y - 30));
            text.setPosition(x, y);
            wordTexts.push_back(text);
        }

        int secondsElapsed = timePlayed + (int)(gameClock.getElapsedTime().asSeconds());

        //clear window
        textWindow.clear();

        for (const auto& text : wordTexts) {
            textWindow.draw(text);
        }
        strikeLine.setFillColor(sf::Color::White);
        textWindow.draw(strikeLine);

        sf::Text inputText(inputBuffer, config.font, 30);
        inputText.setFillColor(sf::Color::Yellow);
        inputText.setPosition(800, 850);
        textWindow.draw(inputText);

        std::stringstream footer;
        footer << "Correct: " << correctWordsCount
               << "  Incorrect: " << incorrectWordsCount
               << "  Time: " << secondsElapsed << "s"
                << "  Score: " << score;

        sf::Text footerText(footer.str(), config.font, 30);
        footerText.setFillColor(sf::Color::White);
        footerText.setPosition(20, 850);
        textWindow.draw(footerText);

        textWindow.draw(speedWordsLabel);
        textWindow.display();
    }
}

void Game::setSpeed() {
    if (config.moveSpeed < 1)
        config.moveSpeed = 1;
    if (config.moveSpeed > speedLabels.size())
        config.moveSpeed = speedLabels.size();
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
    speedWordsLabel.setString("Speed [press keys <- or ->]: " + speedLabels[config.moveSpeed-1]);
}

void Game::showWinScreen() { // display dialog win information in the end of game
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

void Game::showLoseScreen() { // display dialog lose information
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

std::string Game::askPlayerName() { // ask user name to save the result
    sf::RenderWindow nameWindow(sf::VideoMode(450, 200), "Enter Your Name");
    std::string name;
    sf::Text prompt("Enter your name:", config.font, 40);
    prompt.setFillColor(sf::Color::White);
    prompt.setPosition(50, 30);

    sf::Text nameText("", config.font, 40);
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

void Game::saveState(const std::vector<sf::Text>& wordTexts, int elapsedSeconds) {
    std::ofstream out(SAVE_FILE);
    if (!out) return;
    out << config.fontFile << '\n';
    out << config.fontSize << ' ' << config.maxWordsLength << ' ' << config.moveSpeed << '\n';
    out << config.topicWords.size() << '\n';
    for (const auto& w : config.topicWords) out << w << '\n';
    out << correctWordsCount << ' ' << incorrectWordsCount << ' ' << currentWordIndex << ' ' << score << ' ' << (timePlayed + elapsedSeconds) << ' ' << speed << '\n';
    out << inputBuffer << '\n';
    out << wordTexts.size() << '\n';
    for (const auto& t : wordTexts) {
        out << t.getString().toAnsiString() << ' ' << t.getPosition().x << ' ' << t.getPosition().y << '\n';
    }
}

bool Game::loadState(std::vector<sf::Text>& wordTexts) {
    std::ifstream in(SAVE_FILE);
    if (!in) return false;
    std::getline(in, config.fontFile);
    config.font.loadFromFile(config.fontFile);
    in >> config.fontSize >> config.maxWordsLength >> config.moveSpeed;
    size_t topicCount; in >> topicCount;
    std::string dummy; std::getline(in, dummy); // consume endline
    config.topicWords.clear();
    for (size_t i=0;i<topicCount;i++) {
        std::string w; std::getline(in, w); config.topicWords.push_back(w);
    }
    in >> correctWordsCount >> incorrectWordsCount >> currentWordIndex >> score >> timePlayed >> speed;
    std::getline(in, dummy); // consume endline
    std::getline(in, inputBuffer);
    size_t wordsNum; in >> wordsNum;
    wordTexts.clear();
    for (size_t i=0;i<wordsNum;i++) {
        std::string word; float x,y; in >> word >> x >> y;
        sf::Text t(word, config.font, config.fontSize);
        t.setFillColor(sf::Color::White);
        t.setPosition(x,y);
        wordTexts.push_back(t);
    }
    return true;
}

bool Game::hasSavedGame(const std::string& filename) {
    std::ifstream f(filename);
    return f.good();
}

Start::Start(sf::RenderWindow& win, Configuration& configSets)
        : window(win), configSets(configSets){
    // initialise variables
    std::vector<std::string> fontNames = {"Random Wednesday", "Times", "Verdana"};
    std::vector<std::string> topics = {"Cpp", "JAVA", "SQL"};
    std::vector<std::string> sizeLabels = {"Small", "Medium", "Large"};
    std::vector<std::string> speedLabels = {"Slow", "Normal", "Fast"};
    std::vector<std::string> sizeOfWords = {"<= 5 letters", "<= 10 letters", "<= 20 letters"};

    fontNameDropdownLabel.setFont(configSets.font);
    fontNameDropdownLabel.setString("choose font");
    fontNameDropdownLabel.setCharacterSize(20);
    fontNameDropdownLabel.setFillColor(sf::Color::White);
    fontNameDropdownLabel.setPosition(100, 177);

    fontSizeDropdownLabel.setFont(configSets.font);
    fontSizeDropdownLabel.setString("choose text size");
    fontSizeDropdownLabel.setCharacterSize(20);
    fontSizeDropdownLabel.setFillColor(sf::Color::White);
    fontSizeDropdownLabel.setPosition(320, 177);

    speedDropdownLabel.setFont(configSets.font);
    speedDropdownLabel.setString("choose speed moving words");
    speedDropdownLabel.setCharacterSize(20);
    speedDropdownLabel.setFillColor(sf::Color::White);
    speedDropdownLabel.setPosition(100, 377);

    topicDropdownLabel.setFont(configSets.font);
    topicDropdownLabel.setString("choose topic of the text");
    topicDropdownLabel.setCharacterSize(20);
    topicDropdownLabel.setFillColor(sf::Color::White);
    topicDropdownLabel.setPosition(100, 577);

    wordSizeLabel.setFont(configSets.font);
    wordSizeLabel.setString("choose words size");
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

    // Initialize buttons
    levelOneBtn.setSize(sf::Vector2f(200, 50));
    levelOneBtn.setPosition(950, 300);
    levelOneBtn.setFillColor(sf::Color::Green);

    bool saved = Game::hasSavedGame(SAVE_FILE);
    if (saved) {
        continueBtn.setSize(sf::Vector2f(200, 50));
        continueBtn.setPosition(950, 400);
        continueBtn.setFillColor(sf::Color::Yellow);
    }

    scoreBtn.setSize(sf::Vector2f(200, 50));
    scoreBtn.setPosition(950, 600);
    scoreBtn.setFillColor(sf::Color::Blue);

    configLabel.setFont(configSets.font);
    configLabel.setString("Configuration");
    configLabel.setCharacterSize(20);
    configLabel.setFillColor(sf::Color::White);
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

    scoreText.setFont(configSets.font);
    scoreText.setCharacterSize(30);
    scoreText.setString("Score");
    scoreText.setPosition(950, 610);
    scoreText.setFillColor(sf::Color::White);

    if (saved) {
        continueText.setFont(configSets.font);
        continueText.setCharacterSize(30);
        continueText.setString("Continue");
        continueText.setPosition(950, 410);
        continueText.setFillColor(sf::Color::White);
    }
}

// execute pressing buttons
int Start::run() {
    while (window.isOpen()) {
        int action = handleEvents(); // process buttons
        if (action != -1) return action;
        draw();
    }
    return -1; // window closed
}

const std::string fontFiles[] = {
        "FONTfiles/Random Wednesday.ttf",
        "FONTfiles/times.ttf",
        "FONTfiles/verdana.ttf"
};

void Start::LoadFont() {
    int selectedIndex = fontNameDropdown.getSelectedIndex();
    switch (selectedIndex) {
        case RANDOM_WEDNESDAY:
        case TIMES:
        case VERDANA:
            configSets.fontFile = fontFiles[selectedIndex];
            if (!configSets.font.loadFromFile(configSets.fontFile)) {
                std::cerr << "Error loading font: " << configSets.fontFile << "\n";
            }
            break;
        default:
            std::cerr << "Invalid font index!\n";
            break;
    }
}

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
// choose words speed

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

std::string Start::LoadTopicWords() {
    std::string path;
    int selectedIndex = topicDropdown.getSelectedIndex();

    switch (selectedIndex) {
        case CPP:
            path = "TXTfiles/Cpp.txt";
            break;
        case JAVA:
            path = "TXTfiles/JAVA.txt";
            break;
        case SQL:
            path = "TXTfiles/SQL.txt";
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
                if (Game::hasSavedGame(SAVE_FILE) && continueBtn.getGlobalBounds().contains(mouseF))
                    return 2;
                if (scoreBtn.getGlobalBounds().contains(mouseF))
                    return 0;
            }
    }
    return -1;
}

void Start::draw() { // draw everything
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
    if (Game::hasSavedGame(SAVE_FILE))
        window.draw(continueBtn);
    window.draw(scoreBtn);

    window.draw(levelOneText);
    window.draw(levelTwoText);
    if (Game::hasSavedGame(SAVE_FILE))
        window.draw(continueText);
    window.draw(scoreText);

    window.display();
}