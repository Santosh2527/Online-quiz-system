#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <conio.h>
#include <fstream>

using namespace std;
using namespace std::chrono;

struct Question {
    string text;
    vector<string> options;
    int correctIndex;
};

class QuizSystem {
private:
    vector<pair<string, int>> leaderboard;
    const string filename = "leaderboard.txt";

    void loadLeaderboard() {
        ifstream file(filename);
        if (!file) return;

        string name;
        int score;

        while (file >> name >> score) {
            leaderboard.push_back({name, score});
        }
        file.close();
    }

    void saveLeaderboard() {
        ofstream file(filename);
        for (auto &entry : leaderboard) {
            file << entry.first << " " << entry.second << endl;
        }
        file.close();
    }

    vector<Question> getQuestions(int category) {
        if (category == 1) {
            return {
                {"Time complexity of Binary Search?",
                 {"O(n)", "O(log n)", "O(n log n)", "O(1)"}, 1},
                {"Which data structure uses LIFO?",
                 {"Queue", "Stack", "Tree", "Graph"}, 1},
                {"Which traversal is level order?",
                 {"DFS", "BFS", "Inorder", "Postorder"}, 1},
                {"Which data structure uses FIFO?",
                 {"Stack", "Queue", "Tree", "Graph"}, 1}
            };
        }
        else {
            return {
                {"15% of 200?",
                 {"20", "30", "25", "35"}, 1},
                {"25% of 400?",
                 {"50", "75", "100", "125"}, 2},
                {"Simple interest formula?",
                 {"P*T*R/100", "P+R+T", "P-R-T", "R*T"}, 0},
                {"5 workers take 10 days, 10 workers take?",
                 {"5", "10", "20", "2"}, 0}
            };
        }
    }

    void shuffleQuestions(vector<Question>& q) {
        random_device rd;
        mt19937 g(rd());
        shuffle(q.begin(), q.end(), g);
    }

    int conductQuiz(vector<Question>& questions) {

        const int TOTAL_TIME = 90;
        const int QUESTION_TIME = 15;

        int score = 0;
        int skipped = 0;

        auto quizStart = steady_clock::now();

        for (int i = 0; i < questions.size(); i++) {

            auto now = steady_clock::now();
            int totalElapsed =
                duration_cast<seconds>(now - quizStart).count();

            if (totalElapsed >= TOTAL_TIME) {
                cout << "\nTotal Time Over!\n";
                break;
            }

            cout << "\n=================================\n";
            cout << "Q" << i + 1 << ": " << questions[i].text << "\n\n";

            for (int j = 0; j < 4; j++)
                cout << j + 1 << ". " << questions[i].options[j] << endl;

            cout << "\nPress 1-4 to answer.\n";

            auto questionStart = steady_clock::now();
            int userAnswer = -1;

            while (true) {

                auto current = steady_clock::now();

                int totalElapsed =
                    duration_cast<seconds>(current - quizStart).count();
                int questionElapsed =
                    duration_cast<seconds>(current - questionStart).count();

                int totalLeft = TOTAL_TIME - totalElapsed;
                int questionLeft = QUESTION_TIME - questionElapsed;

                if (totalLeft <= 0) {
                    cout << "\nTotal Time Over!\n";
                    return score;
                }

                if (questionLeft <= 0) {
                    cout << "\nQuestion Time Over!\n";
                    skipped++;
                    break;
                }

                cout << "\rQuestion Left: "
                     << questionLeft
                     << "s | Total Left: "
                     << totalLeft
                     << "s   "
                     << flush;

                if (_kbhit()) {
                    char ch = _getch();
                    if (ch >= '1' && ch <= '4') {
                        userAnswer = ch - '0';
                        cout << "\nYou selected: " << userAnswer << endl;
                        break;
                    }
                }
            }

            if (userAnswer == -1)
                continue;

            if (userAnswer - 1 == questions[i].correctIndex) {
                cout << "Correct!\n";
                score++;
            }
            else {
                cout << "Wrong!\n";
            }
        }

        cout << "\n===== FINAL SUMMARY =====\n";
        cout << "Score: " << score << endl;
        cout << "Skipped: " << skipped << endl;

        return score;
    }

    void showLeaderboard() {

        if (leaderboard.empty()) {
            cout << "\nNo attempts yet.\n";
            return;
        }

        sort(leaderboard.begin(), leaderboard.end(),
             [](auto &a, auto &b) {
                 return a.second > b.second;
             });

        cout << "\n===== LEADERBOARD =====\n";
        for (int i = 0; i < leaderboard.size() && i < 5; i++) {
            cout << i + 1 << ". "
                 << leaderboard[i].first
                 << " - "
                 << leaderboard[i].second << endl;
        }
    }

public:
    QuizSystem() {
        loadLeaderboard();
    }

    void start() {

        while (true) {

            cout << "\n===== ONLINE QUIZ SYSTEM =====\n";
            cout << "1. Start Quiz\n";
            cout << "2. Show Leaderboard\n";
            cout << "3. Exit\n";
            cout << "Choose: ";

            int choice;
            cin >> choice;

            if (choice == 3)
                break;

            if (choice == 2) {
                showLeaderboard();
                continue;
            }

            string name;
            cout << "Enter your name: ";
            cin >> name;

            cout << "\nSelect Category:\n";
            cout << "1. DSA\n2. Aptitude\nChoice: ";
            int cat;
            cin >> cat;

            vector<Question> questions = getQuestions(cat);
            shuffleQuestions(questions);

            int score = conductQuiz(questions);

            leaderboard.push_back({name, score});
            saveLeaderboard();   // SAVE AFTER EACH QUIZ
        }
    }
};

int main() {
    QuizSystem system;
    system.start();
    return 0;
}
