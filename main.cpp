#include <iostream>
#include <string>
#include <limits>
#include <ctime>
#include <cstdlib>

// check if os is windows, if not assume linux based
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace std;

// set the maximum amount of users and messages in an instance
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
    string time_sent[6];
};

// holds all users
User users[MAX_USERS];

// holds all messages
Msg msgs[MAX_MSGS];

// holds addresses of all messages directed to the logged in user
Msg *inbox[MAX_MSGS];

// holds the amount of messages in the inbox array
int inbox_count = 0;

// holds the address of the current logged in user in the users array
User *current_user = NULL;

// logs in the user.
// returns true if the user was logged in.
// returns false if not.
bool login(string username, string password);

// logs out the user.
// returns true if the user was logged out.
// returns false if not.
bool logout();

// creates a new user.
// returns true if the user was created.
// returns false if not.
bool signup(string username, string password);

// sends a message from current user to a specified user.
// returns true if message was sent.
// returns false if not.
bool create_msg(string content, string receiver);

// edits the current user's username.
// returns true if the username was edited.
// returns false if not.
bool edit_username(string new_username);

// edits the current user's password.
// returns true if the password was edited.
// returns false if not.
bool edit_password(string new_password);

// finds the first empty spot in the users array.
// returns the index of the empty spot.
// returns -1 if no empty spot was found.
int find_empty_user();

// finds the user in the users array based on a specified username.
// returns the user's index in the array.
// returns -1 if the user was not found.
int find_user(string username);

// finds the first empty spot in the msgs array.
// returns the index of the empty spot.
// returns -1 if no empty spot was found.
int find_empty_msg();

// stores messages from the msgs array, sent to current_user, to inbox array.
void load_inbox();

// removes all messages from the inbox array.
void empty_inbox();

// clears input buffer.
// returns true if last input failed (eg. a letter passed to an integer variable).
// returns false if not.
bool clean_buffer();

// shows the signin menu used to signup and login.
// returns true if the user logs in.
// returns false if not.
bool ui_signin();

// shows the login interface used to login.
// returns true if the users logs in.
// returns false if not.
bool ui_login();

// shows the sign up interface used to create an account.
// returns true if the user was created and logged in.
// returns false if not.
bool ui_signup();

// shows the main interface of the program.
// returns false if user requested to exit.
// returns true if not.
bool ui_home();

// shows the interface used to send a message.
void ui_send_msg();

// shows the interface used to view messages.
void ui_view_msgs();

// shows the interface used to edit the username.
void ui_edit_username();

// shows the interface used to edit the password.
void ui_edit_password();

// clears the terminal window from previous output.
void ui_clear_console();

// freezes the terminal window on the last output for 2 seconds.
void ui_freeze_console();

// freezes the terminal window and then clears it.
void ui_freeze_clear();

// shows only the specified message in the terminal window without any
// other output surrounding it for as long as ui_freeze_console works.
void ui_alert(string message);

// prints a single message.
void ui_show_msg(Msg *msg);

int main() {
    // loop call ui_signin until it returns true.
    do {} while(!ui_signin());
    // loop call ui_home until it returns false.
    do {} while(ui_home());
    return 0;
}

// ##############
// MAIN FUNCTIONS
// ##############

bool login(string username, string password) {
    int index = find_user(username);

    // if user does not exist.
    if (index == -1) {
        return false;
    }

    // if user exists and password is correct.
    if (users[index].password == password) {
        // log user in.
        current_user = &users[index];
        // load the users messages.
        load_inbox();
        return true;
    }

    // if password is wrong.
    return false;
}

bool logout() {
    // if a user is logged in.
    if (current_user != NULL) {
        // log the user out.
        current_user = NULL;
        // remove the user messages.
        empty_inbox();
        return true;
    }

    // if a user is not logged in.
    return false;
}

bool signup(string username, string password) {
    // ensure no user with the username already exists.
    for (int i = 0; i < MAX_USERS; i++) {
        // if user with username exists.
        if (users[i].username == username) {
            return false;
        }
    }

    int index = find_empty_user();

    // if users is not maxed out.
    if (index != -1) {
        users[index].username = username;
        users[index].password = password;
        login(username, password);
        return true;
    }

    // if maximum amount of users was reached.
    return false;
}

bool create_msg(string content, string receiver) {
    int m_index = find_empty_msg();
    // if maximum amount of messages was reached.
    if (m_index == -1) {
        return false;
    }

    int u_index = find_user(receiver);
    // if user doesn't exist.
    if (u_index == -1) {
        return false;
    }
    // if user is sending the message to themself
    if (&users[u_index] == current_user) {
        return false;
    }

    // create the message
    msgs[m_index].sender = current_user;
    msgs[m_index].receiver = &users[u_index];
    msgs[m_index].content = content;

    time_t t = time(0);
    tm* current = localtime(&t);
    msgs[m_index].time_sent[0] = to_string(current->tm_year + 1900);
    msgs[m_index].time_sent[1] = to_string(current->tm_mon + 1);
    msgs[m_index].time_sent[2] = to_string(current->tm_mday);
    msgs[m_index].time_sent[3] = to_string(current->tm_hour);

    // if the minute is a single digit prepend 0.
    if (current->tm_min < 10) {
        msgs[m_index].time_sent[4] = "0" + to_string(current->tm_min);
    } else {
        msgs[m_index].time_sent[4] = to_string(current->tm_min);
    }

    msgs[m_index].time_sent[5] = to_string(current->tm_sec);

    return true;
}

bool edit_username(string new_username) {
    // ensure username is not already used.
    if (find_user(new_username) == -1) {
        current_user->username = new_username;
        return true;
    }
    // if username is already used.
    return false;
}

bool edit_password(string new_password) {
    // ensure password is not empty.
    if (new_password != "") {
        current_user->password = new_password;
        return true;
    }
    // if password is empty.
    return false;
}

// ################
// HELPER FUNCTIONS
// ################

int find_user(string username) {
    // loop through all users.
    for (int i = 0; i < MAX_USERS; i++) {
        // if user is found.
        if (users[i].username == username) {
            return i;
        }
    }
    // if user is not found.
    return -1;
}

int find_empty_user() {
    // loop through all users.
    for (int i = 0; i < MAX_USERS; i++) {
        // if empty spot found.
        if (users[i].username == "") {
            return i;
        }
    }
    // if no empty spot found.
    return -1;
}

int find_empty_msg() {
    // loop through all messages.
    for (int i = 0; i < MAX_MSGS; i++) {
        // if empty spot found.
        if (msgs[i].content == "") {
            return i;
        }
    }
    // if no empty spot found.
    return -1;
}

void load_inbox() {
    // initialize counter c to count messages sent to current user.
    int c = 0;
    // loop through all messages
    for (int i = 0; i < MAX_MSGS; i++) {
        // if message sent to current user.
        if (msgs[i].receiver == current_user) {
            // place the message address in the inbox array at position c.
            inbox[c] = &msgs[i];
            // increment c.
            c++;
        }
    }
    // set inbox_count to c.
    inbox_count = c;
}

void empty_inbox() {
    // loop through all pointers in inbox array.
    for (int i = 0; i < MAX_MSGS; i++) {
        // set the pointer to null.
        inbox[i] = NULL;
    }
    // reset inbox_count to 0.
    inbox_count = 0;
}

bool clean_buffer() {
    // declare variables.
    bool return_value = false;

    // if last input failed.
    if (cin.fail()) {
        // clear the error.
        cin.clear();
        // set the return value to true.
        return_value = true;
    }
    // remove the input buffer leftovers until a new line character is met.
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    return return_value;
}

// ############
// UI FUNCTIONS
// ############

bool ui_signin() {
    // declare and initialize variables
    int option;

    ui_clear_console();
    // print to terminal the options.
    cout <<
        "==================================================\n"
        "                   Sign In\n"
        "==================================================\n";
    cout
        << "1. login\n"
        << "2. signup\n"
        << "option: ";
    // get the user option choice and store it in option.
    cin >> option;

    // clean the input buffer.
    // if the user did not enter an integer.
    if (clean_buffer()) {
        // set option to an invalid integer option.
        option = -1;
    }

    ui_clear_console();

    switch (option) {
        // if the user chose login.
        case 1:
            // call the login interface.
            // return the return value from the call.
            return ui_login();
        // if the user chose signup.
        case 2:
            // call the signup interface.
            // return the return value from the call.
            return ui_signup();
        // if the user did not choose a valid option.
        default:
            ui_alert("unavailable option");
            return false;
    }
}

bool ui_login() {
    // declare variables.
    string username;
    string password;

    // get the username from the user.
    cout << "username: ";
    cin >> username;
    // clear the input buffer leftovers (specifically the new line character).
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    // get the password from the user.
    cout << "password: ";
    getline(cin, password);

    ui_clear_console();

    // login the user.
    // if user logged in.
    if (login(username, password)) {
        ui_alert("logged in");
        return true;
    } else {
    // if user not logged in.
        ui_alert("wrong username or password");
        return false;
    }
}

bool ui_signup() {
    // declare variables.
    string username;
    string password;

    // get username.
    cout << "username: ";
    cin >> username;
    // clear input buffer.
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    // get password.
    cout << "password: ";
    getline(cin, password);

    ui_clear_console();

    // if user created and logged in.
    if (signup(username, password)) {
        ui_alert("user was created");
        return true;
    } else {
    // if user was not created.
        ui_alert("user was not created (try different username)");
        return false;
    }
}

bool ui_home() {
    // declare variables.
    int option;

    ui_clear_console();
    cout <<
        "==================================================\n"
        "         Welcome " << current_user->username << " Let's Get You Messaging\n"
        "==================================================\n";
    cout
        << "0. logout\n"
        << "1. send message\n"
        << "2. view messages\n"
        << "3. edit username\n"
        << "4. edit password\n"
        << "9. exit\n"
        << "option: ";
    // get the user option choice and store it in option.
    cin >> option;

    // clean the input buffer.
    // if the user did not enter an integer.
    if (clean_buffer()) {
        // set option to an invalid integer option.
        option = -1;
    }

    ui_clear_console();
    switch (option) {
        // if the user chose to logout.
        case 0:
            // log the user out.
            logout();
            // ensure the user is logged in.
            do {} while(!ui_signin());
            break;
        // if the user chose to send a message.
        case 1:
            ui_send_msg();
            break;
        // if the user chose to view their messages.
        case 2:
            ui_view_msgs();
            break;
        // if the user chose to edit their username.
        case 3:
            ui_edit_username();
            break;
        // if the user chose to edit their password.
        case 4:
            ui_edit_password();
            break;
        // if the user chose to exit.
        case 9:
            return false;
        // if the user did not choose a valid option.
        default:
            ui_alert("unavailable option");
            break;
    }
    return true;
}

void ui_send_msg() {
    // declare variables.
    string user;
    string msg;

    ui_clear_console();
    cout <<
        "==================================================\n"
        "                   Send a Message\n"
        "==================================================\n";

    // get the username.
    cout << "username: ";
    cin >> user;
    // clear the input buffer.
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    // get the message.
    cout << "message: ";
    getline(cin, msg);

    ui_clear_console();

    // if message is sent.
    if (create_msg(msg, user)) {
        ui_alert("message sent successfully");
    } else {
    // if message is not sent.
        ui_alert("message was not sent (ensure user exists and is not you)");
    }
}

void ui_view_msgs() {
    // declare variables.
    int option;

    ui_clear_console();
    // if inbox is empty.
    if (inbox_count == 0) {
        ui_alert("no messages to show");
        return;
    }

    // set i to the index of the last element in the inbox array.
    int i = inbox_count - 1;
    // loop infinitely.
    while (true) {
        ui_clear_console();
        // show message at i.
        ui_show_msg(inbox[i]);
        // if the message is not the most recent message.
        if (i != inbox_count - 1) {
            cout << "9. next\n";
        }
        // if the message is not the oldest message.
        if (i != 0) {
            cout << "0. previous\n";
        }
        cout << "1. return home\n";

        cout << "option: ";

        // get the user option choice and store it in option.
        cin >> option;

        // clean the input buffer.
        // if the user did not enter an integer.
        if (clean_buffer()) {
            // set option to an invalid integer option.
            option = -1;
        }

        switch (option) {
            // if the user chose to view the previous message.
            case 0:
                // if the message is the oldest message.
                if (i == 0) {
                    ui_alert("unavailable option");
                } else {
                // if the message is not the oldest message.
                    // decrement i.
                    i--;
                }
                break;
            // if the user chose to return to main menu.
            case 1:
                return;
            // if the user chose to view the next message.
            case 9:
                // if the message is the latest message.
                if (i == inbox_count - 1) {
                    ui_alert("unavailable option");
                } else {
                // if the message is not the latest message.
                    // increment i
                    i++;
                }
                break;
            // if the user did not choose a valid option.
            default:
                ui_alert("unavailable option");
        }
    }
    ui_clear_console();
}

void ui_edit_username() {
    // declare variables.
    string new_username;

    ui_clear_console();

    cout <<
        "==================================================\n"
        "                     Edit Username\n"
        "==================================================\n";

    // get username.
    cout << "new username: ";
    cin >> new_username;
    // clear the input buffer.
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    // if username was successfully edited.
    if (edit_username(new_username)) {
        ui_alert("username was updated");
    } else {
    // if username was not edited.
        ui_alert("username was not changed (try different username)");
    }
}

void ui_edit_password() {
    // declare variables.
    string old_password;
    string new_password;
    string confirm_password;

    ui_clear_console();

    cout <<
        "==================================================\n"
        "                     Edit Password\n"
        "==================================================\n";


    // get current password.
    cout << "current password: ";
    getline(cin, old_password);

    // if current password is wrong.
    if (current_user->password != old_password) {
        ui_alert("wrong password");
        // exit.
        return;
    }

    // get new password.
    cout << "new password: ";
    getline(cin, new_password);

    // get new password confirmation.
    cout << "confirm password: ";
    getline(cin,confirm_password);

    // check if the new password and its confirmation are the same.
    if (new_password != confirm_password) {
        ui_alert("passwords do not match");
    }

    // if the password was edited.
    if (edit_password(new_password)) {
        ui_alert("password was updated");
    } else {
    // if the password was not edited.
        ui_alert("password was not changed (ensure it is not empty)");
    }
}

// ###################
// UI HELPER FUNCTIONS
// ###################

void ui_clear_console() {
    // if using windows.
#ifdef _WIN32
    // use cls command to clean the terminal.
    system("cls");
#else
    // if not windows (assuming unix based).
    // use clear command to clean the terminal.
    system("clear");
#endif
}

void ui_freeze_console() {
    // if using windows.
#ifdef _WIN32
    // call Sleep() from windows.h library and freeze for 2000 milliseconds.
    Sleep(2000);
#else
    // if not windows (assuming unix based).
    // call sleep() from unistd.h library and freeze for 2 seconds.
    sleep(2);
#endif
}

void ui_freeze_clear() {
    ui_freeze_console();
    ui_clear_console();
}

void ui_alert(string message) {
    ui_clear_console();
    cout << message << "\n";
    ui_freeze_clear();
}

void ui_show_msg(Msg *msg) {
    string time_sent;
    time_sent = msg->time_sent[3] + ":" + msg->time_sent[4] + " "
        + msg->time_sent[2] + "/" + msg->time_sent[1];
    cout <<
        "==================================================\n"
        "                   Sent By: " << msg->sender->username << "\n"
        "                   At: " << time_sent << "\n"
        "==================================================\n";
    cout << msg->content << "\n"
        "==================================================\n";
}
