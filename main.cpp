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
    int time_sent[6];
};

User users[MAX_USERS];
Msg msgs[MAX_MSGS];
Msg *inbox[MAX_MSGS];
int inbox_count = 0;

User *current_user = NULL;

bool login(string username, string password);
bool logout();
bool signup(string username, string password);
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
bool ui_home();
void ui_send_msg();
void ui_view_msgs();

void ui_clear_console();
void ui_freeze_console();
void ui_freeze_clear();
void ui_iprint(string message);
void ui_show_msg(Msg *msg);

int main() {
    ui_clear_console();

    do {} while(!ui_signin());
    do {} while(ui_home());
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
        current_user = NULL;
        empty_inbox();
        return true;
    }

    return false;
}

bool signup(string username, string password) {
    for (int i = 0; i < MAX_USERS; i++) {
        if (users[i].username == username) {
            return false;
        }
    }

    int index = find_empty_user();

    if (index != -1) {
        users[index].username = username;
        users[index].password = password;
        login(username, password);
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
    if (&users[u_index] == current_user) {
        return false;
    }

    msgs[m_index].sender = current_user;
    msgs[m_index].receiver = &users[u_index];
    msgs[m_index].content = content;

    time_t t = time(0);
    tm* current = localtime(&t);
    msgs[m_index].time_sent[0] = (current->tm_year + 1900);
    msgs[m_index].time_sent[1] = (current->tm_mon + 1);
    msgs[m_index].time_sent[2] = (current->tm_mday);
    msgs[m_index].time_sent[3] = (current->tm_hour);
    msgs[m_index].time_sent[4] = (current->tm_min);
    msgs[m_index].time_sent[5] = (current->tm_sec);

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
    inbox_count = c;
}

void empty_inbox() {
    for (int i = 0; i < MAX_MSGS; i++) {
        inbox[i] = NULL;
    }
    inbox_count = 0;
}

// UI FUNCTIONS
bool ui_signin() {
    int option;

    bool return_bool = false;

    ui_clear_console();
    cout
        << "1. login\n"
        << "2. signup\n"
        << "option: ";
    cin >> option;
    if (cin.fail()) {
        cin.clear();
        cin.ignore();
        option = -1;
    }

    ui_clear_console();

    switch (option) {
        case 1:
            return_bool = ui_login();
            break;
        case 2:
            return_bool = ui_signup();
            break;
        default:
            ui_iprint("unavailable option");
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
        ui_iprint("logged in");
        return_bool = true;
    } else {
        ui_iprint("wrong username or password");
    }
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

    if (signup(username, password)) {
        ui_iprint("user was created");
        return_bool = true;
    } else {
        ui_iprint("user was not created (try different username)");
    }
    return return_bool;
}

bool ui_home() {
    int option;
    int return_bool;

    ui_clear_console();
    cout
        << "0. logout\n"
        << "1. send message\n"
        << "2. view messages\n"
        << "9. exit\n"
        << "option: ";
    cin >> option;
    if (cin.fail()) {
        cin.clear();
        cin.ignore();
        option = -1;
    }

    ui_clear_console();
    switch (option) {
        case 0:
            logout();
            do {} while(!ui_signin());
            break;
        case 1:
            ui_send_msg();
            break;
        case 2:
            ui_view_msgs();
            break;
        case 9:
            return false;
        default:
            ui_iprint("unavailable option");
            break;
    }
    return true;
}

void ui_send_msg() {
    string user;
    string msg;

    ui_clear_console();

    cout << "username: ";
    cin >> user;
    cin.ignore();

    cout << "message: ";
    getline(cin, msg);

    ui_clear_console();

    bool return_bool = create_msg(msg, user);
    if (return_bool) {
        ui_iprint("message sent successfully");
    } else {
        ui_iprint("message was not sent (try a different user)");
    }
}

void ui_view_msgs() {
    int option;

    ui_clear_console();
    if (inbox_count == 0) {
        ui_iprint("no messges to show");
        return;
    }

    int i = inbox_count - 1;
    while (true) {
        ui_clear_console();
        ui_show_msg(inbox[i]);
        cout << "\n";
        if (i != 0) {
            cout << "0. next\n";
        }
        cout << "1. quit\n";
        if (i != inbox_count - 1) {
            cout << "9. previous\n";
        }

        cout << "option: ";
        cin >> option;
        if (cin.fail()) {
            cin.clear();
            cin.ignore();
            option = -1;
        }

        switch (option) {
            case 0:
                if (i == 0) {
                    ui_iprint("unavailable option");
                } else {
                    i--;
                }
                break;
            case 1:
                return;
            case 9:
                if (i == inbox_count - 1) {
                    ui_iprint("unavailable option");
                } else {
                    i++;
                }
                break;
            default:
                ui_iprint("unavailable option");
        }
    }
    ui_clear_console();
}

// UI HELPER FUNCTIONS
void ui_clear_console() {
    system("clear");
}

void ui_freeze_console() {
    sleep(2);
}

void ui_freeze_clear() {
    ui_freeze_console();
    ui_clear_console();
}

void ui_iprint(string message) {
    ui_clear_console();
    cout << message << "\n";
    ui_freeze_clear();
}

void ui_show_msg(Msg *msg) {
    cout
        << "------\n"
        << msg->sender->username
        << " @ "
        << msg->time_sent[3]
        << ":"
        << msg->time_sent[4]
        << " "
        << msg->time_sent[2]
        << "/"
        << msg->time_sent[1]
        << "\n------\n"
        << "######\n"
        << msg->content
        << "\n######\n";
}
