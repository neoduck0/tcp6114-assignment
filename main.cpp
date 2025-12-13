#include <iostream>

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

User empty_user;
User *current_user = &empty_user;

bool login(string username, string password);
bool logout();

bool create_user(string username, string password);
bool delete_user(string username);

bool create_message(string content, string receiver);

int find_empty_user();
int find_user(string username);

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
