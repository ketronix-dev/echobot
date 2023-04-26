#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <unistd.h>


#include <tgbot/tgbot.h>

using namespace std;
using namespace TgBot;

vector<string> parseWordsFromFile(string filename) {
    
    vector<string> words;
    ifstream file(filename);
    string word;
    while (getline(file, word)) {
        words.push_back(word);
    }
    return words;
}

void writeStat(string user, int words_count, int count_words_with_errors,
                 string words_with_errors, float percentage, string grade) {
    ofstream myfile;
    myfile.open("stats.txt", ios::out);

    myfile << "Статистика" << endl << "----------" << endl;
    myfile << "Користувач: " << user << endl;
    myfile << "Кількість слів загалом: " << words_count << endl;
    myfile << "Кількість слів з помилками: " << count_words_with_errors << endl;
    myfile << "Слова з помилками: " << words_with_errors << endl;
    myfile << "Відсоток помилок: " << percentage << "%" << endl;
    myfile << "Оцінка: " << grade << endl;

    myfile.close();
}

string Spell(string text, string user, vector<string> dictionary) {
    stringstream result;
    stringstream result_errors;
    stringstream words(text);

    float words_count = 0;
    float errors_words_count = 0;

    string word;
    while (words >> word) {
        words_count++;
        
        if (find(dictionary.begin(), dictionary.end(), word) == dictionary.end()) {
            errors_words_count++;
            
            result_errors << word << " ";
            result << "<s>" << word << "</s>" << " ";
        } else {
            result << word << " ";
        }
    }

    float percentage = errors_words_count / words_count * 100;
    string grade;

    if(percentage <= 10)
    {
        grade = "відмінно";
    }
    else if(percentage <= 20 && percentage >= 10)
    {
        grade = "добре";
    }
    else if(percentage <= 30 && percentage >= 20)
    {
        grade = "задовільно";
    }
    else
    {
        grade = "незадовільно";
    }
    
    writeStat(user, words_count, errors_words_count, result_errors.str(), percentage, grade);

    return result.str();
}



string GetToken()
{
    ifstream file("/home/artem/Documents/spell-bot/cmake-build-debug/token");
    string token;
    getline(file, token);

    return token;
}

void HandleCommands(Bot& botClient)
{
    botClient.getEvents().onCommand("start", [&botClient](Message::Ptr message) {
        botClient.getApi().sendMessage(message->chat->id, "Привіт " + message->chat->firstName + "! \n Шоб перевірити текст на правильність - просто відправте повідомлення з цим текстом.");
    });
}
void HandleEvents(Bot& botClient)
{
    botClient.getEvents().onAnyMessage([&botClient](Message::Ptr message) {
        printf("User wrote %s\n", message->text.c_str());
        if (StringTools::startsWith(message->text, "/start")) {
            return;
        }
        if(message->text == "")
        {
            botClient.getApi().sendMessage(message->chat->id, "Можливо ви не написали текст у вашому повідомленні, або відправили непдтримуваний файл.");
        }
        else
        {
            botClient.getApi().sendMessage(message->chat->id,
                                     "Ваш перевірений текст: " + Spell(message->text, message->chat->firstName, parseWordsFromFile("/home/artem/Documents/spell-bot/cmake-build-debug/Dictionary")),
                                      false, 0, nullptr,
                                       "HTML");
            botClient.getApi().sendDocument(message->chat->id, 
                                            InputFile::fromFile("/home/artem/Documents/spell-bot/build/stats.txt",""),
                                            "",
                                            "Статистика користувача");
        }
    });
}

int main() {
    printf("Token: %s\n", GetToken().c_str());

    Bot bot(GetToken());

    HandleCommands(bot);
    HandleEvents(bot);

    signal(SIGINT, [](int s) {
        printf("SIGINT got\n");
        exit(0);
    });

    try {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        bot.getApi().deleteWebhook();

        TgLongPoll longPoll(bot);
        while (true) {
            printf("Long poll started\n");
            longPoll.start();
        }
    } catch (exception& e) {
        printf("error: %s\n", e.what());
    }

    return 0;
}