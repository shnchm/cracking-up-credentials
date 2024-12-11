#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <sqlite3.h>
#include <conio.h>

using namespace std;

struct Credential {
    string email;
    string password;
    string website;
};

struct Admin {
    string username;
    string password;
};

// Check if the password meets the minimum strength requirement
bool passwordStrength(const string& password) {
    return password.length() >= 8;
}

// Authenticate the admin by checking the provided username and password against the stored credentials
bool authenticateAdmin(sqlite3* db, const string& username, const string& password) {
    const char* selectQuery = "SELECT username, password FROM admins WHERE username = ?;";
    sqlite3_stmt* stmt;

    // Prepare the SQL select statement
    int rc = sqlite3_prepare_v2(db, selectQuery, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        cout << "Failed to prepare select statement: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    // Bind the username parameter to the SQL statement
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    // Execute the SQL statement and check if a row was retrieved
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        // Get the stored password from the result set
        const unsigned char* storedPassword = sqlite3_column_text(stmt, 1);
        string decryptedPassword = reinterpret_cast<const char*>(storedPassword);

        // Compare the provided password with the stored password
        if (password == decryptedPassword) {
            sqlite3_finalize(stmt);
            return true;
        }
    }

    sqlite3_finalize(stmt);
    return false;
}

void createAdmin(sqlite3* db, const Admin& admin) {
    const char* insertQuery = "INSERT INTO admins (username, password) VALUES (?, ?);";
    sqlite3_stmt* stmt;

    // Prepare the SQL insert statement
    int rc = sqlite3_prepare_v2(db, insertQuery, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        cout << "Failed to prepare insert statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    // Bind the admin's username and password parameters to the SQL statement
    sqlite3_bind_text(stmt, 1, admin.username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, admin.password.c_str(), -1, SQLITE_STATIC);

    // Execute the SQL statement to insert the admin into the table
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        cout << "Failed to insert admin: " << sqlite3_errmsg(db) << endl;
        return;
    }

    // Finalize the SQL statement
    sqlite3_finalize(stmt);
    cout << "Admin successfully created and stored." << endl;
}

void AddCredential(sqlite3* db) {
    Credential credential;

    cout << "Enter Email (no spaces): ";
    cin >> credential.email;

    cout << "Enter Website name (no spaces and no .com): ";
    cin >> credential.website;

    cout << "Enter Password (no spaces): ";
    cin >> credential.password;

    // Check if the password meets the minimum strength requirement
    if (!passwordStrength(credential.password)) {
        cout << "Password is less than 8 characters. Try again." << endl;
        return;
    }

    const char* insertQuery = "INSERT INTO credentials (email, password, website) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;

    // Prepare the SQL insert statement
    int rc = sqlite3_prepare_v2(db, insertQuery, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        cout << "Failed to prepare insert statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    // Bind the credential's email, password, and website parameters to the SQL statement
    sqlite3_bind_text(stmt, 1, credential.email.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, credential.password.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, credential.website.c_str(), -1, SQLITE_STATIC);

    // Execute the SQL statement to insert the credential into the table
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        cout << "Failed to insert credential: " << sqlite3_errmsg(db) << endl;
        return;
    }

    // Finalize the SQL statement
    sqlite3_finalize(stmt);
    cout << "Credential successfully added and stored." << endl;
}

void FindCredential(sqlite3* db) {
    string email;
    cout << "Enter the email to find: ";
    cin >> email;

    const char* selectQuery = "SELECT email, password, website FROM credentials WHERE email = ?;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, selectQuery, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        cout << "Failed to prepare select statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_STATIC);

    vector<Credential> credentials;

    // Retrieve the matching credentials from the database
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Credential credential;
        const unsigned char* encryptedEmail = sqlite3_column_text(stmt, 0);
        const unsigned char* encryptedPassword = sqlite3_column_text(stmt, 1);
        const unsigned char* website = sqlite3_column_text(stmt, 2);

        // Decrypt and store the credential information
        credential.email = reinterpret_cast<const char*>(encryptedEmail);
        credential.password = reinterpret_cast<const char*>(encryptedPassword);
        credential.website = reinterpret_cast<const char*>(website);

        // Add the credential to the vector
        credentials.push_back(credential);
    }

    sqlite3_finalize(stmt);

    if (!credentials.empty()) {
        cout << "Opening file..." << endl;
        cout << "The credentials for the email '" << email << "' are as follows: " << endl;
        for (const auto& credential : credentials) {
            cout << "Email: " << credential.email << endl;
            cout << "Password: " << credential.password << endl;
            cout << "Website: " << credential.website << endl;
        }
    } else {
        cout << "No credentials found for the email '" << email << "'." << endl;
    }
}

void RemoveCredential(sqlite3* db) {
    string email, website;
    
    cout << "Enter the email: ";
    cin >> email;
    cout << "Enter the website name: ";
    cin >> website;

    const char* deleteQuery = "DELETE FROM credentials WHERE email = ? AND website = ?;";
    sqlite3_stmt* stmt;

    // Prepare the SQL statement for deletion
    int rc = sqlite3_prepare_v2(db, deleteQuery, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        cout << "Failed to prepare delete statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    // Bind the email and website parameters to the SQL statement
    sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, website.c_str(), -1, SQLITE_STATIC);

    // Execute the deletion statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        cout << "Failed to remove credential: " << sqlite3_errmsg(db) << endl;
        return;
    }

    // Finalize the SQL statement
    sqlite3_finalize(stmt);
    cout << "Credential removed successfully." << endl;
}

void ViewAllCredentials(sqlite3* db) {
    string website;
    cout << "Enter the website name: ";
    cin >> website;

    const char* selectQuery = "SELECT * FROM credentials WHERE website = ?;";
    sqlite3_stmt* stmt;

    // Prepare the SQL statement
    int rc = sqlite3_prepare_v2(db, selectQuery, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        cout << "Failed to execute SELECT query: " << sqlite3_errmsg(db) << endl;
        return;
    }

    // Bind the website parameter to the SQL statement
    sqlite3_bind_text(stmt, 1, website.c_str(), -1, SQLITE_STATIC);

    // Execute the SQL statement and fetch the results
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        // Extract the values from the result set
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char* email = sqlite3_column_text(stmt, 1);
        const unsigned char* password = sqlite3_column_text(stmt, 2);
        const unsigned char* website = sqlite3_column_text(stmt, 3);

        // Display the retrieved credentials
        cout << "ID: " << id << endl;
        cout << "Email: " << email << endl;
        cout << "Password: " << password << endl;
        cout << "Website: " << website << endl;
        cout << endl;
    }

    // Finalize the SQL statement
    sqlite3_finalize(stmt);
}


int main() {
    // Create or open the database
    sqlite3* db;
    int rc = sqlite3_open("credentials.db", &db);
    if (rc != SQLITE_OK) {
        cout << "Cannot open database: " << sqlite3_errmsg(db) << endl;
        return 1;
    }

    // Create the admins table if it doesn't exist
    const char* createTableQuery = "CREATE TABLE IF NOT EXISTS admins (username TEXT, password TEXT);";
    rc = sqlite3_exec(db, createTableQuery, 0, 0, 0);
    if (rc != SQLITE_OK) {
        cout << "Failed to create table: " << sqlite3_errmsg(db) << endl;
        return 1;
    }

    // Prompt for admin username and password
    string username;
    string password;

    cout << "Please enter your admin username: ";
    cin >> username;
    cout << "Please enter your admin password: ";
    char ch;
    while ((ch = _getch()) != '\r') {  // Capture each character until Enter is pressed
        if (ch == '\b') {  // Handle backspace
            if (!password.empty()) {
                cout << "\b \b";  // Move cursor back, erase character, move back again
                password.pop_back();
            }
        } else {
            cout << '*';  // Display asterisk instead of the actual character
            password.push_back(ch);
        }
    }
    cout << endl;

    if (authenticateAdmin(db, username, password)) {
    cout << "Authentication successful!" << endl;
} else {
    cout << "Authentication failed! Creating a new admin..." << endl;

    // Create a new admin with the provided username and password
    Admin newAdmin;
    newAdmin.username = username;
    newAdmin.password = password;

    // Call the createAdmin function to store the new admin in the database
    createAdmin(db, newAdmin);
}

// Check if the authenticated user is an admin
bool isAdmin = authenticateAdmin(db, username, password);

if (isAdmin) {
    int choice;

    // Display the menu and prompt for user input
    do {
        cout << "Choose an option:" << endl;
        cout << "1. Add Credential" << endl;
        cout << "2. Find Credential" << endl;
        cout << "3. Remove Credential" << endl;
        cout << "4. View All Credentials" << endl;
        cout << "5. Exit" << endl;
        cout << "Enter your choice (1-5): ";
        cin >> choice;

        // Perform the corresponding action based on the user's choice
        switch (choice) {
            case 1:
                AddCredential(db);
                break;
            case 2:
                FindCredential(db);
                break;
            case 3:
                RemoveCredential(db);
                break;
            case 4:
                ViewAllCredentials(db);
                break;
            case 5:
                cout << "Exiting..." << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
                break;
        }
    } while (choice != 5);
} else {
    cout << "Invalid username or password. Access denied." << endl;
}

// Close the database connection
sqlite3_close(db);

// Return 0 to indicate successful program execution
return 0;

}
