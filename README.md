# cracking-up-credentials

Cracking-Up Credentials is an offline-based password manager built using **C++** and **SQLite** for secure database storage. This project is designed to provide a local, efficient, and secure solution for managing user credentials and ensuring user authentication without requiring an internet connection.

## Features

### 1. **User Administration**
   The system allows an authorized administrator to manage user accounts. The administrator can grant or revoke access, ensuring that only authorized individuals can use the password manager while maintaining complete control over its functionality.

### 2. **Password Strength Checking**
   The password manager incorporates a built-in password strength checker to evaluate the strength of user-generated passwords. This feature helps users create strong passwords by providing immediate feedback on password complexity.

### 3. **Authentication Mechanism**
   A robust authentication system verifies the identity of users before granting access to the password manager. This ensures that only authorized individuals can view and manage the stored credentials.

### 4. **Credential Management**
   Users can easily add, retrieve, and remove credentials in a user-friendly manner. The password manager simplifies the process of managing login information for various accounts, keeping it all organized in one place.

### 5. **Comprehensive Search**
   A search functionality is integrated into the password manager, enabling users to quickly locate specific credentials. This improves the efficiency and convenience of managing large sets of login information.

### 6. **Credential Removal**
   Users can securely and intuitively remove credentials from the system. This ensures that outdated or unused credentials can be safely deleted, preventing unauthorized access to accounts.

### 7. **Credential Overview**
   The password manager provides an organized overview of all stored credentials, making it easy for users to view and manage their login information in a clear and accessible manner.

### 8. **Offline Capability**
   The entire password manager operates offline, eliminating the reliance on internet connectivity. This ensures that sensitive data is stored locally, minimizing potential vulnerabilities associated with online access.

## Files

- **`main.cpp`**: The main program that runs the password manager.
- **`sqlite.h`**: The header file for SQLite database management.

## Prerequisites

To build and run the project, you will need:
- **C++ compiler** (e.g., GCC, Clang)
- **SQLite3** library

Ensure you have the SQLite development libraries installed to successfully compile and link the project.

## How to Build

1. Clone this repository to your local machine:
   ```bash
   git clone https://github.com/shnchm/cracking-up-credentials.git

