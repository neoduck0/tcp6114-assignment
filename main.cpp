#include <iostream>
#include <string>
#include <unistd.h>

using namespace std;

const int MAX_USERS = 100;
const int MAX_MSGS = 100;

struct User {
    string username;
    string password;
};

struct Msg {
    string content;
    User *sender;
    User *receiver;
};

User users[MAX_USERS];
Msg msgs[MAX_MSGS];
Msg *inbox[MAX_MSGS];

User empty_user;
User *current_user = &empty_user;

bool login(string username, string password);
bool logout();
bool create_user(string username, string password);
bool delete_user(string username);
bool create_msg(string content, string receiver);

int find_empty_user();
int find_user(string username);
int find_empty_msg();
void load_inbox();
void empty_inbox();

bool ui_signin();
bool ui_login();
bool ui_signup();
void ui_send_msg();

void ui_clear_console();
void ui_freeze_console();
void ui_freeze_clear();

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
        load_inbox();
        return true;
    }

    return false;
}

bool logout() {
    if (current_user->username != "") {
        current_user = &empty_user;
        empty_inbox();
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

bool create_msg(string content, string receiver) {
    int m_index = find_empty_msg();
    if (m_index == -1) {
        return false;
    }

    int u_index = find_user(receiver);
    if (u_index == -1) {
        return false;
    }

    msgs[m_index].sender = current_user;
    msgs[m_index].receiver = &users[u_index];
    msgs[m_index].content = content;
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

int find_empty_msg() {
    for (int i = 0; i < MAX_MSGS; i++) {
        if (msgs[i].content == "") {
            return i;
        }
    }
    return -1;
}

void load_inbox() {
    int c = 0;
    for (int i = 0; i < MAX_MSGS; i++) {
        if (msgs[i].receiver == current_user) {
            inbox[c] = &msgs[i];
            c++;
        }
    }
}

void empty_inbox() {
    for (int i = 0; i < MAX_MSGS; i++) {
        inbox[i] = NULL;
    }
}

// UI FUNCTIONS
bool ui_signin() {
    int option;

    bool return_bool = false;

    cout
        << "1. login\n"
        << "2. signup\n\n"
        << "option: ";
    cin >> option;
    ui_clear_console();

    switch (option) {
        case 1:
            return_bool = ui_login();
            break;
        case 2:
            ui_signup();
            break;
        default:
            cout << "unavailable option\n";
            ui_freeze_clear();
    }
    return return_bool;
}

bool ui_login() {
    string username;
    string password;

    bool return_bool = false;

    cout << "username: ";
    cin >> username;
    cout << "password: ";
    cin >> password;

    ui_clear_console();

    if (login(username, password)) {
        cout << "logged in\n";
        return_bool = true;
    } else {
        cout << "wrong username or password\n";
    }
    ui_freeze_clear();
    return return_bool;
}

bool ui_signup() {
    string username;
    string password;

    bool return_bool = false;

    cout << "username: ";
    cin >> username;
    cout << "password: ";
    cin >> password;

    ui_clear_console();

    if (create_user(username, password)) {
        cout << "user was created\n";
        return_bool = true;
    } else {
        cout << "user was not created\n(try different username)\n";
    }
    ui_freeze_clear();
    return return_bool;
}

void ui_send_msg() {
    string user;
    string msg;

    ui_clear_console();

    cout << "username: ";
    cin >> user;

    cout << "message: ";
    cin >> msg;

    ui_clear_console();

    bool return_bool = create_msg(msg, user);
    if (return_bool) {
        cout << "message sent successfully\n";
    } else {
        cout << "message was not sent (user may not exist)\n";
    }

    ui_freeze_clear();
}

// UI HELPER FUNCTIONS
void ui_clear_console() {
    system("clear");
}

void ui_freeze_console() {
    sleep(3);
}

void ui_freeze_clear() {
    ui_freeze_console();
    ui_clear_console();
}
