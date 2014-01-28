
#include <iostream>
#include <string>
#include <sqlite3.h> 

#define SQL_HELPERS 1


namespace sql_helpers	{


	static void checkErrors(int rc, char * zErrMsg, std::string title, bool verbose = false)
		{
		if( rc != SQLITE_OK )
			{
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			}
		else if (verbose)
			{
			fprintf(stdout, ("SQL " + title + " successful\n").c_str());
			}
		}


	// prints the output from SQL queries
	// argc: number of columns in the result
	// argv: array of pointers to strings, one for each column; col contents
	// azColName: array of pointers to strings, one for each column; col name
	// sqlite3_exec() invokes this once for each row returned 
	static int callback(void *NotUsed, int argc, char **argv, char **azColName){
		int i;
		for(i=0; i<argc; i++){
			printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
			}
		printf("\n");
		return 0;
		}



	void insertRow(sqlite3* db, std::string tableName, std::string * args, size_t nargs, bool verbose = false)
		{
		char* zErrMsg = 0;
		std::string sql = "INSERT INTO " + tableName + " VALUES( " + args[0];

		for (unsigned int i = 1; i < nargs; i++)
			{
			sql += ", " + args[i];
			}

		sql += ");";
		int rc = sqlite3_exec(db, sql.c_str(), sql_helpers::callback, 0, &zErrMsg);
		sql_helpers::checkErrors(rc, zErrMsg, "CREATE TABLE", verbose);
		}

	int getOneValue_int(sqlite3* db, std::string sqlQuery)
		{
		sqlite3_stmt * st;
		char* zErrMsg = 0;
		int rc = sqlite3_prepare_v2(db,sqlQuery.c_str(), -1, &st, NULL);    
		sql_helpers::checkErrors(rc, zErrMsg, "DB prepare", false);
		rc = sqlite3_step(st);
		if (rc != SQLITE_ROW)
			{
			std::cout << "Error getting row" << std::endl;
			return 0;
			}
		int result = sqlite3_column_int(st, 0);
		rc = sqlite3_finalize(st);
		if (rc != SQLITE_OK)
			std::cout << "error in finalize" << std::endl;
		return result;
		}

	double getOneValue_double(sqlite3* db, std::string sqlQuery)
		{
		sqlite3_stmt * st;
		char* zErrMsg = 0;
		int rc = sqlite3_prepare_v2(db,sqlQuery.c_str(), -1, &st, NULL);    
		sql_helpers::checkErrors(rc, zErrMsg, "DB prepare", false);
		rc = sqlite3_step(st);
		if (rc != SQLITE_ROW)
			{
			std::cout << "Error getting row" << std::endl;
			return 0;
			}
		double result = sqlite3_column_double(st, 0);
		rc = sqlite3_finalize(st);
		if (rc != SQLITE_OK)
			std::cout << "error in finalize" << std::endl;
		return result;
		}
	};

