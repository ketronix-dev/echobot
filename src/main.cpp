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

string user;

vector<string> parseWordsFromFile(string filename) {
    // Создаем вектор (вектор это типо массива, но можно менять размер) для тех слов которые в словаре
    vector<string> words;
    // Открываем файл который мы передали методу
    ifstream file(filename);
    // Создаем переменную в которую будем писать слово из файла
    string word;
    // Цикл в котором читаем каждую строку файла
    while (getline(file, word)) {
        // Добавляем эту самую строку в вектор
        words.push_back(word);
    }
    // Возвращаем словарь
    return words;
}

string writeStat(string user, int words_count, int count_words_with_errors,
                 string words_with_errors, float percentage, string grade) {
    ofstream myfile;
    myfile.open("stats.txt", ios::out); // открываем файл в режиме добавления записей

    // Записываем статистику в файл
    myfile << "Statistic" << endl << "----------" << endl;
    myfile << "User: " << user << endl;
    myfile << "Words count total: " << words_count << endl;
    myfile << "Count of words with errors: " << count_words_with_errors << endl;
    myfile << "Words with errors: " << words_with_errors << endl;
    myfile << "Percentage words with errors: " << percentage << "%" << endl;
    myfile << "Grade: " << grade << endl;

    myfile.close(); // закрываем файл

    return "stats.txt";
}

string checkSpell(string text, vector<string> dictionary) {
    // Создаем поток строки для результата проверки, который будем возвращать
    stringstream result;
    // Поток для слов с ошибками
    stringstream result_errors;
    // А еще создаем поток для текста который мы передали
    stringstream words(text);

    // Счетчики
    float words_count = 0;
    float errors_words_count = 0;

    // Создаем переменную для слова с которым будем работать
    string word;
    // Обьявляем цикл в котором по очереди берем слова из потока
    while (words >> word) {
        words_count++;
        /*
         * Проверяем есть ли слово в словаре
         * В функцию финд передаем начало и конец словаря, а точнее их итераторы (можно сказать указатели)
         * и в ту же функцию передаем слово которое нужно найти, и если результат поиска равен концу, то слово не найдено
         * Если что конец не равен последнему элементу
         * */
        if (find(dictionary.begin(), dictionary.end(), word) == dictionary.end()) {
            errors_words_count++;
            // Записиваем в поток с словами в которых ошибка
            result_errors << word << " ";
            // Если слово не найдено - окрашиваем его в красный для консоли.
            result << "<s>" << word << "</s>" << " ";
        } else {
            // Если слово найдено - просто вставляем его в результат
            result << word << " ";
        }
    }

    // Считаем процент слов с ошибкой
    float percentage = errors_words_count / words_count * 100;
    // Переменная для оценки
    string grade;
    // Высчитывание и определение оценки
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
    // Записываем статистику в файл
    string file = writeStat(user, words_count, errors_words_count, result_errors.str(), percentage, grade);
    // Выводим ее на экран
    cout << endl;
    cout << "Statistic" << endl << "----------" << endl;
    cout << "User: " << user << endl;
    cout << "Words count total: " << words_count << endl;
    cout << "Count of words with errors: " << errors_words_count << endl;
    cout << "Words with errors: " << result_errors.str() << endl;
    cout << "Percentage words with errors: " << percentage << "%" << endl;
    cout << "Grade: " << grade << endl;
    cout << "File with stats: " << file << endl;

    // После всех обработок - возвращаем результат в виде строки
    return result.str();
}



string GetToken()
{
    ifstream file("token");
    string token;
    getline(file, token);

    return token;
}

void HandleCommands(Bot& botClient)
{
    botClient.getEvents().onCommand("start", [&botClient](Message::Ptr message) {
        botClient.getApi().sendMessage(message->chat->id, "Hi!");
    });
}
void HandleEvents(Bot& botClient)
{
    botClient.getEvents().onAnyMessage([&botClient](Message::Ptr message) {
        printf("User wrote %s\n", message->text.c_str());
        if (StringTools::startsWith(message->text, "/start")) {
            return;
        }
        botClient.getApi().sendMessage(message->chat->id, "Your message is: " + message->text);
        botClient.getApi().sendDocument(message->chat->id, 
                                        InputFile::fromFile("token",""),
                                        "",
                                        "File caption");
    });
}

int main() {

    // Ну тут я думаю все понятно
    cout << "Введите ФИО пользователя: ";
    // Читаем ввод из cin, и записываем в ту переменную которую для этого создали
    getline(cin, user);
    // Переменная в которой будем хранить ввод пользователя
    string input;
    // Создаем вектор в который выгружаем словарь из файла
    vector<string> dictionary = parseWordsFromFile("/home/artem/Documents/Code/C++/Dictionary");
    // Вечный цикл
    while (true) {
        // Ну тут я думаю все понятно
        cout << "Введите текст или 'exit' для выхода: ";
        // Читаем ввод из cin, и записываем в ту переменную которую для этого создали
        getline(cin, input);
        // Если написали "выход"
        if (input == "exit") {
            // Выход из цикла
            break;
        }
            // Если написали что-то кроме "выход"
        else
        {
            // Проверяем наличие всех слов в словаре и записываем итоговую строчку в переменную
            string result = checkSpell(input, dictionary);
            // Выводим эту самую строчку
            cout << "Input text: " << result << endl << endl;
        }
    }

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
