#include <iostream>
#include <string>
#include <unistd.h>

using namespace std;

const int MAX_USERS = 100;
const int MAX_MESSAGES = 100;

struct User {
    string username;
    string password;
};

struct Message {
    string content;
    User *sender;
    User *receiver;
};

User users[MAX_USERS];
Message u_messages[MAX_MESSAGES];

User empty_user;
User *current_user = &empty_user;

bool login(string username, string password);
bool logout();

bool create_user(string username, string password);
bool delete_user(string username);

bool create_message(string content, string receiver);

int find_empty_user();
int find_user(string username);

int find_empty_message();

bool ui_login();

int main() {
    return 0;
}

bool login(string username, string password) {
    int index = find_user(username);

    if (index == -1) {
        return false;
    }

    if (users[index].password == password) {
        current_user = &users[index];
        return true;
    }

    return false;
}

bool logout() {
    if (current_user->username != "") {
        current_user = &empty_user;
        return true;
    }

    return false;
}

bool create_user(string username, string password) {
    for (int i = 0; i < MAX_USERS; i++) {
        if (users[i].username == username) {
            return false;
        }
    }

    int index = find_empty_user();

    if (index != -1) {
        users[index].username = username;
        users[index].password = password;
        return true;
    }

    return false;
}

bool delete_user(string username) {
    int index = find_user(username);

    if (index == -1) {
        return false;
    }

    users[index].username = "";
    users[index].password = "";
    return true;
}

bool create_message(string content, string receiver) {
    int m_index = find_empty_message();
    if (m_index == -1) {
        return false;
    }

    int u_index = find_user(receiver);
    if (u_index == -1) {
        return false;
    }

    u_messages[m_index].sender = current_user;
    u_messages[m_index].receiver = &users[u_index];
    u_messages[m_index].content = content;
    return true;
}

// HELPER FUNCTIONS
int find_user(string username) {
    for (int i = 0; i < MAX_USERS; i++) {
        if (users[i].username == username) {
            return i;
        }
    }
    return -1;
}

int find_empty_user() {
    for (int i = 0; i < MAX_USERS; i++) {
        if (users[i].username == "") {
            return i;
        }
    }
    return -1;
}

int find_empty_message() {
    for (int i = 0; i < MAX_MESSAGES; i++) {
        if (u_messages[i].content == "") {
            return i;
        }
    }
    return -1;
}

// UI FUNCTIONS
bool ui_login() {
    system("clear");

    string username;
    string password;
    int status;
    int option;

    cout
        << "1. login\n"
        << "2. signup\n\n"
        << "option: ";
    cin >> option;
    system("clear");


    switch (option) {
        case 1:
            cout << "username: ";
            cin >> username;
            cout << "password: ";
            cin >> password;
            system("clear");

            status = login(username, password);
            if (status == true) {
                 cout << "logged in\n";
                 sleep(3);
                 system("clear");
                 return true;
            } else {
                cout << "wrong username or password\n";
                sleep(3);
                system("clear");
            }
            break;
        case 2:
            cout << "username: ";
            cin >> username;
            cout << "password: ";
            cin >> password;
            system("clear");

            status = create_user(username, password);
            if (status == true) {
                cout << "user was created\n";
                sleep(3);
                system("clear");
            } else {
                cout << "user was not created\n";
                sleep(3);
                system("clear");
            }
            break;
        default:
            cout << "unavailable option";
    }
    return false;
}
