#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <SFML/Graphics.hpp>
#include <fmt/ranges.h>
#include <fstream>
#include <string>

auto genText(sf::RenderWindow& window, auto& txt, const sf::Font& font, auto posY, auto posX, sf::Color color, auto size) -> void;
auto loadFont(auto fontPath) -> sf::Font;
auto genRectangle(auto posY, auto posX, auto sizeX, auto sizeY, sf::Color color) -> sf::RectangleShape;
auto changeFont(auto& selectedFont, auto& fontVec) -> sf::Font;
auto changeSpeed(auto& speed) -> void;
auto saveGame(auto& pkt, auto& hp, auto& time, auto& currentwords, auto& isSaved) -> void;
auto loadGame(auto& words, auto& pkt, auto& hp, auto& time, auto& isLoaded) -> void;
auto changeFontSize(auto& fontSize) -> void;
auto updateHighScore(auto pkt) -> void;
auto getHighScore() -> int;

struct Word {
    std::string text;
    int x;
    int y;
};

class GenWord {
public:
    std::vector<std::string> words;
    std::vector<Word> currentWords;
    int maxWords = 1;

    GenWord(auto& filePath) {
        loadWords(filePath);
        srand((time(nullptr)));
    }

    auto addWord() -> void {
        if(currentWords.size() < maxWords) {
            Word noweSlowo;
            noweSlowo.text = getRandomWord();
            noweSlowo.x = 0;
            noweSlowo.y = genYpos();
            currentWords.push_back(noweSlowo);
        }
    }

    auto setMaxWords(auto points) -> void {
        if(maxWords < 11)
            maxWords = 1 + (points / 10);
        else maxWords = 11;
    }

    auto getCurrentWords() -> std::vector<Word>& {
        return currentWords;
    }

    auto clearCurrentWords() -> void {
        currentWords.clear();
    }

    private:
    auto loadWords(auto& filePath) -> void {
        std::fstream file(filePath);
        std::string word;
        while (file >> word) {
            words.push_back(word);
        }
        file.close();
    }

    auto getRandomWord() -> std::string& {
        return words[rand() % words.size()];
    }

    auto genYpos() -> int{
        int randomPos;
        bool isTaken;
        do {
            isTaken = true;
            randomPos = (rand() % 11 + 1) * 50;
            for (const auto& word : currentWords) {
                if (word.y == randomPos) {
                    isTaken = false;
                    break;
                }
            }
        } while (!isTaken);
        return randomPos;
    }
};

auto main() -> int {
    using namespace std;
    sf::Font poppins = loadFont("../Poppins-Regular.ttf");
    sf::Font lora = loadFont("../Lora.ttf");
    sf::Font mina = loadFont("../Mina-Regular.ttf");
    auto fontVec = vector<sf::Font>{poppins, lora, mina};
    auto wybranyFont = fontVec[0];

    auto window = sf::RenderWindow(
            sf::VideoMode(1280, 720),
            "Monkey Typer",
            sf::Style::Default,
            sf::ContextSettings(0, 0, 8)
    );

    auto startGame = false;
    auto paused = false;
    auto enteredText = string();
    auto pkt = 0;
    auto hp = 3;
    auto selectedFont = 0;
    auto fontSize = 30;
    auto time = 0.0;
    auto speed = 1;
    auto highScore = getHighScore();
    auto isLoaded = false;
    auto isSaved = false;
    auto gamePlayed = false;
    window.setFramerateLimit(60);
    auto slowa = GenWord("../slowa.txt");
    auto backgroundColor = sf::Color {01,13,18};
    auto fontButton = genRectangle(300, 650, 300, 100, {118, 221, 249});
    auto speedButton = genRectangle(450, 650, 300, 100, {118, 221, 249});
    auto loadGameButton = genRectangle(300, 250, 300, 100, {118, 221, 249});
    auto saveGameButton = genRectangle(450, 250, 300, 100, {118, 221, 249});
    auto fontSizeButton = genRectangle(585, 450, 300, 100, {118, 221, 249});

    //Źródło obsługa klawiatury: https://www.sfml-dev.org/tutorials/2.6/window-events.php
    while (window.isOpen()) {
        auto speedStr = " Word Speed: " + to_string(speed);
        auto highScoreStr = "Highest Score: " + to_string(highScore);
        for (auto event = sf::Event(); window.pollEvent(event);) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed && !startGame) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    if (fontButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                        wybranyFont = changeFont(selectedFont, fontVec);
                    }
                    else if (speedButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                        changeSpeed(speed);
                    }
                    else if (loadGameButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                        loadGame(slowa, pkt, hp, time, isLoaded);
                    }
                    else if (saveGameButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                        if(paused) saveGame(pkt, hp, time, slowa.getCurrentWords(), isSaved);
                    }
                    else if (fontSizeButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                        changeFontSize(fontSize);
                    }
                }
            }
            if (event.type == sf::Event::KeyReleased) {
                if (event.key.code == sf::Keyboard::Space) {
                    startGame = true;
                    paused = false;
                } else if (event.key.code == sf::Keyboard::Escape && startGame) {
                    startGame = false;
                    paused = true;
                }
            }
            if (event.type == sf::Event::TextEntered && startGame && !paused) {
                if (event.text.unicode < 128) {
                    if (event.text.unicode == 8 && !enteredText.empty()) {
                        enteredText.pop_back();
                    } else if (event.text.unicode != 8 && event.text.unicode != 32 && enteredText.length() < 15 && event.text.unicode != 27) {
                        enteredText += static_cast<char>(event.text.unicode);
                    }
                }
            }
        }

        window.clear(backgroundColor);

        if(!startGame) {
            if(paused){
                genText(window, "Game Paused", mina, 50, 410, {216, 247, 253}, 60);
                genText(window, "Press SPACE to resume", wybranyFont, 150, 375, {216, 247, 253}, 40);
            } else {
                genText(window, "Monkey Typer", mina, 50, 400, {216, 247, 253}, 60);
                if(!gamePlayed) genText(window, "Press SPACE to start", wybranyFont, 150, 400, {216, 247, 253}, 40);
                else genText(window, "Press SPACE to play again!", wybranyFont, 150, 350, {216, 247, 253}, 40);
            }
            window.draw(loadGameButton);
            window.draw(saveGameButton);
            window.draw(fontButton);
            window.draw(speedButton);
            window.draw(fontSizeButton);
            genText(window, highScoreStr, wybranyFont, 225, 450, {216, 247, 253}, 35);
            genText(window, "Change Font", wybranyFont, 325, 675, {1, 19, 24}, 40);
            genText(window, "Load Game", wybranyFont, 325, 285, {1, 19, 24}, 40);
            genText(window, "Save Game", wybranyFont, 475, 285, {1, 19, 24}, 40);
            if(!paused) genText(window, "Available during game", wybranyFont, 515, 260, {1, 19, 24}, 25);
            if(isSaved) genText(window, "Game saved!", wybranyFont, 520, 305, {1, 19, 24}, 25);
            if(isLoaded) genText(window, "Game Loaded", wybranyFont, 365, 310, {1, 19, 24}, 25);
            genText(window, "Font Size", wybranyFont, 615, 525, {1, 19, 24}, fontSize);
            genText(window, speedStr, wybranyFont, 475, 650, {1, 19, 24}, 40);
            window.display();

        } else if (startGame) {
            isLoaded = false;
            isSaved = false;
            time += 1.0/60.0;
            slowa.setMaxWords(pkt);
            auto pktStr = "Points: " + to_string(pkt);
            auto hpStr = "Health: " + to_string(hp);
            auto timeStr = "Time: " + to_string(time);
            auto statsBox = genRectangle(600, 0, 1280, 120, {118, 221, 249});
            auto textBox = genRectangle(610, 45, 480, 50, {backgroundColor});
            slowa.addWord();
            for (int i = 0; i < slowa.getCurrentWords().size(); i++) {
                auto& slowo = slowa.getCurrentWords()[i];
                genText(window, slowo.text, wybranyFont, slowo.y, slowo.x, {216, 247, 253}, fontSize);
                if(slowo.text.substr(0, enteredText.length()) == enteredText)
                    genText(window, enteredText, wybranyFont, slowo.y, slowo.x, {118, 221, 249}, fontSize);

                slowo.x += speed*2;

                if(enteredText == slowo.text) {
                    enteredText = "";
                    pkt++;
                    slowa.getCurrentWords().erase(slowa.getCurrentWords().begin() + i);
                }
                if(slowo.x>=window.getSize().x) {
                    slowa.getCurrentWords().erase(slowa.getCurrentWords().begin() + i);
                    hp--;
                }
                if(hp <= 0) {
                    startGame = false;
                    gamePlayed = true;
                    updateHighScore(pkt);
                    highScore = getHighScore();
                    slowa.clearCurrentWords();
                    enteredText = "";
                    pkt = 0;
                    hp = 3;
                    time = 0;
                }
            }

            window.draw(statsBox);
            window.draw(textBox);
            genText(window, enteredText, wybranyFont, 620, 50, {216, 247, 253}, 30);
            genText(window, ">", wybranyFont, 610, 20, {216, 247, 253}, 40);
            genText(window, pktStr, wybranyFont, 620, 700, {88, 15, 245}, 30);
            genText(window, hpStr, wybranyFont, 620, 900, sf::Color::Red, 30);
            genText(window, timeStr, wybranyFont, 660, 800, {7, 7, 166}, 30);
            genText(window, "Press ESC to pause", wybranyFont, 675, 50, {backgroundColor}, 25);
            window.display();
        }
    }
    return 0;
}

auto genText(sf::RenderWindow& window, auto& txt, const sf::Font& font, auto posY, auto posX, sf::Color color, auto size) -> void {
    auto text = sf::Text(txt, font);
    text.setFillColor(color);
    text.setPosition(posX, posY);
    text.setCharacterSize(size);
    window.draw(text);
}

auto genRectangle(auto posY, auto posX, auto sizeX, auto sizeY, sf::Color color) -> sf::RectangleShape {
    auto rectangle = sf::RectangleShape(sf::Vector2f(sizeX, sizeY));
    rectangle.setFillColor(color);
    rectangle.setPosition(posX, posY);
    return rectangle;
}

auto loadFont(auto fontPath) -> sf::Font {
    sf::Font font;
    if (!font.loadFromFile(fontPath))
        fmt::println("Can't find font: {}", fontPath);
    return font;
}

auto changeFont(auto& selectedFont, auto& fontVec) -> sf::Font {
    selectedFont += 1;
    if(selectedFont >= fontVec.size())
        selectedFont = 0;
    return fontVec[selectedFont];
}

auto changeFontSize(auto& fontSize) -> void {
    fontSize += 5;
    if(fontSize > 40)
        fontSize = 25;
}

auto changeSpeed(auto& speed) -> void {
    speed += 1;
    if(speed > 5)
        speed = 1;
}

auto saveGame(auto& pkt, auto& hp, auto& time, auto& currentwords, auto& isSaved) -> void {
    auto saveFile = std::fstream("../save.txt");
    saveFile << pkt << " " << hp << " " << time << "\n";
    for (const auto& word : currentwords) {
        saveFile << word.text << " " << word.x << " " << word.y << "\n";
    }
    isSaved = true;
    saveFile.close();
}

auto loadGame(auto& words, auto& pkt, auto& hp, auto& time, auto& isLoaded) -> void {
    std::fstream saveFile("../save.txt");
    if (!saveFile) {
        fmt::println("Error. No save saveFile.");
        return;
    }
    saveFile >> pkt >> hp >> time;
    words.clearCurrentWords();
    std::string text;
    int x, y;
    while (saveFile >> text >> x >> y) {
        Word word;
        word.text = text;
        word.x = x;
        word.y = y;
        words.currentWords.push_back(word);
    }
    isLoaded = true;
    saveFile.close();
}

auto updateHighScore(auto pkt) -> void {
    auto highScore = getHighScore();
    if (pkt > highScore) {
        std::ofstream scoreFile("../highscore.txt");
        scoreFile << pkt;
        scoreFile.close();
    }
}

auto getHighScore() -> int {
    std::ifstream scoreFile("../highscore.txt");
    auto highScore = 0;
    if (scoreFile) {
        scoreFile >> highScore;
        scoreFile.close();
    }
    return highScore;
}