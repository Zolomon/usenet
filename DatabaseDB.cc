#include "DatabaseDB.h"
#include "MessageHandler.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <cstring>

using namespace std;

namespace usenet
{
typedef map<int, NewsGroup> MapNewsGroup;
typedef map<int, Article> MapArticle;

DatabaseDB::DatabaseDB()
{
    int rc;
    const char *dbname = "usenet.sqlite";
    rc = sqlite3_open(dbname, &db);
    if (SQLITE_OK != rc)
    {
        cerr << "\tCan not open database " << dbname << " (" << rc << "): " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        exit(1);
    }
    else
    {
        cout << "\tDatabase connection initialised ... [Keep 'em queries coming]" << endl;
    }

    FillDatabase();
}

void DatabaseDB::FillDatabase()
{
    CreateNewsGroupTable();
    CreateArticleTable();
}

bool DatabaseDB::TableExists(string tablename)
{
    // Build out query. Will return 1 if the table exists, otherwise 0.
    const char *select_sql = "Select count(type) from sqlite_master where type='table' and name = ?;";

    // Statement to prepare
    sqlite3_stmt *select_stmt = NULL;

    // The NULL is "Don't attempt to free() the valye when it's bound", since it's on the stack here
    int rc = sqlite3_bind_text(select_stmt, 1, tablename.c_str(), strlen(tablename.c_str()), NULL);
    if (SQLITE_OK != rc)
    {
        cerr << "\tError binding value in select statement (" << rc << "): " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        exit(1);
    }
    // Prepare our statement
    //  The last parameter, dbend, is used if you have multiple sql statements in one string
    rc = sqlite3_prepare_v2(db, select_sql, -1, &select_stmt, NULL);
    if (SQLITE_OK != rc)
    {
        cerr << "\tCan not prepare select statement " << select_sql << " (" << rc << "): " << sqlite3_errmsg(db) << endl;
        return NULL;
    }

    bool found = false;

    // Execute statement and set 'found' if table exists.
    while (SQLITE_ROW == (rc = sqlite3_step(select_stmt)))
    {

        found = sqlite3_column_int(select_stmt, 0);
    }

    if (SQLITE_DONE != rc)
    {
        cerr << "\tSelect statement did not finish with DONE (" << rc << "): " << sqlite3_errmsg(db) << endl;
        return NULL;
    }

    if (found) cout << "\tTable[" << tablename << "] exists ..." << endl;
    else cerr << "\tTable[" << tablename << "] does not exist ..." << endl;

    sqlite3_finalize(select_stmt);

    return found;
}

void DatabaseDB::CreateNewsGroupTable()
{
    const char create_sql[] = "CREATE TABLE IF NOT EXISTS newsgroups ("
                              "id INTEGER PRIMARY KEY,"
                              "name TEXT,"
                              "deleted INTEGER, "
                              "UNIQUE(name)"
                              ")";
    // SQLite exec return errors with this
    char *exec_errmsg;
    int rc = sqlite3_exec(db, create_sql, NULL, NULL, &exec_errmsg);
    if (SQLITE_OK != rc)
    {
        cerr << "\tError creating table (" << rc << "): " << exec_errmsg << endl;
        sqlite3_free(exec_errmsg);
        sqlite3_close(db);
        exit(1);
    }

    cout << "\tTable[newsgroups] created successfully ..." << endl;
}

void DatabaseDB::CreateArticleTable()
{
    const char create_sql[] = "CREATE TABLE IF NOT EXISTS articles ("
                              "id INTEGER PRIMARY KEY,"
                              "ngID INTEGER,"
                              "title TEXT,"
                              "author TEXT,"
                              "text TEXT,"
                              "deleted INTEGER"
                              ")";
    // SQLite exec return errors with this
    char *exec_errmsg;
    int rc = sqlite3_exec(db, create_sql, NULL, NULL, &exec_errmsg);
    if (SQLITE_OK != rc)
    {
        cerr << "\tError creating table (" << rc << "): " << exec_errmsg << endl;
        sqlite3_free(exec_errmsg);
        sqlite3_close(db);
        exit(1);
    }

    cout << "\tTable[articles] created successfully ..." << endl;
}

DatabaseDB::~DatabaseDB()
{
    sqlite3_close(db);
    //free(db);
}

MapNewsGroup *DatabaseDB::ListNewsGroups()
{
    // Our SQL query
    const char *select_sql = "SELECT id, name, deleted FROM newsgroups";

    // Statement to prepare
    sqlite3_stmt *select_stmt = NULL;

    // Prepare our statement
    //  The last parameter, dbend, is used if you have multiple sql statements in one string
    int rc = sqlite3_prepare_v2(db, select_sql, -1, &select_stmt, NULL);
    if (SQLITE_OK != rc)
    {
        cerr << "\tCan not prepare select statement " << select_sql << " (" << rc << "): " << sqlite3_errmsg(db) << endl;
        return NULL;
    }

    MapNewsGroup *newsgroups = new MapNewsGroup();

    // Execute statement and build article
    while (SQLITE_ROW == (rc = sqlite3_step(select_stmt)))
    {

        int id = sqlite3_column_int(select_stmt, 0);
        string name(reinterpret_cast<const char *>(
                        sqlite3_column_text(select_stmt, 1)
                    ));
        bool deleted = sqlite3_column_int(select_stmt, 2);

        NewsGroup ng(name, deleted);

        newsgroups->insert(make_pair(id, ng));
    }

    if (SQLITE_DONE != rc)
    {
        cerr << "\tSelect statement did not finish with DONE (" << rc << "): " << sqlite3_errmsg(db) << endl;
        return NULL;
    }

    sqlite3_finalize(select_stmt);

    cout << "\tCreated list of NewsGroups successfully ..." << endl;

    return newsgroups;
}

bool DatabaseDB::CreateNewsGroup(string title)
{
    // Our query
    const char insert_sql[] = "INSERT INTO newsgroups (name, deleted) VALUES (?, 0)";

    // SQlite statement to prepare
    sqlite3_stmt *insert_stmt = NULL;

    // Prepare the two statements we need to prepare
    //  The last parameter, dbend, is used if you have multiple sql statements in one string
    int rc = sqlite3_prepare_v2(db, insert_sql, -1, &insert_stmt, NULL);
    if (SQLITE_OK != rc)
    {
        cerr << "\tCan not prepare insert statement " << insert_sql << " (" << rc << "): " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        exit(1);
    }

    // The NULL is "Don't attempt to free() the valye when it's bound", since it's on the stack here
    rc = sqlite3_bind_text(insert_stmt, 1, title.c_str(), strlen(title.c_str()), NULL);

    if (SQLITE_OK != rc)
    {
        cerr << "\tError binding value in insert (" << rc << "): " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        exit(1);
    }

    // Perform the statement
    rc = sqlite3_step(insert_stmt);
    if (SQLITE_DONE != rc)
    {
        cerr << "\tInsert statement did not return DONE (" << rc << "): " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(insert_stmt);
        return false;
    }
    else
    {
        // Newsgroup created successfully
        cout << "\tCreated new NewsGroup successfully ..." << endl;
        sqlite3_finalize(insert_stmt);
        return true;
    }
}

bool DatabaseDB::DeleteNewsGroup(int ngID)
{
    // Our query
    const char update_sql[] = "UPDATE newsgroups SET deleted = 1 WHERE id = ?";

    // SQlite statement to prepare
    sqlite3_stmt *update_stmt = NULL;

    // Prepare the two statements we need to prepare
    //  The last parameter, dbend, is used if you have multiple sql statements in one string
    int rc = sqlite3_prepare_v2(db, update_sql, -1, &update_stmt, NULL);
    if (SQLITE_OK != rc)
    {
        cerr << "\tCan not prepare update statement " << update_sql << " (" << rc << "): " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        exit(1);
    }

    // The NULL is "Don't attempt to free() the valye when it's bound", since it's on the stack here
    rc = sqlite3_bind_int(update_stmt, 1, ngID);
    if (SQLITE_OK != rc)
    {
        cerr << "\tError binding value in update statement (" << rc << "): " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        exit(1);
    }

    // Perform the statement
    rc = sqlite3_step(update_stmt);
    if (SQLITE_DONE != rc)
    {
        cerr << "\tUpdate statement did not return DONE (" << rc << "): " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(update_stmt);
        return false;
    }
    else
    {
        // Newsgroup created successfully
        cout << "\tDeleted NewsGroup successfully ..." << endl;
        sqlite3_finalize(update_stmt);
        return true;
    }
}

bool DatabaseDB::NewsGroupExists(int ngID)
{
    // Our SQL query
    const char *select_sql = "SELECT id FROM newsgroups WHERE id = ?";

    // Statement to prepare
    sqlite3_stmt *select_stmt = NULL;

    // Prepare our statement
    //  The last parameter, dbend, is used if you have multiple sql statements in one string
    int rc = sqlite3_prepare_v2(db, select_sql, -1, &select_stmt, NULL);
    if (SQLITE_OK != rc)
    {
        cerr << "\tCan not prepare select statement " << select_sql << " (" << rc << "): " << sqlite3_errmsg(db) << endl;
        return NULL;
    }

    // Bind to query
    sqlite3_bind_int(select_stmt, 1, ngID);
    if (SQLITE_OK != rc)
    {
        cerr << "\tError binding value in select statement (" << rc << "): " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        exit(1);
    }

    bool found = false;

    // Execute statement and build article
    while (SQLITE_ROW == (rc = sqlite3_step(select_stmt)))
    {
        found = sqlite3_column_int(select_stmt, 0);
    }

    if (SQLITE_DONE != rc)
    {
        cerr << "\tSelect statement did not finish with DONE (" << rc << "): " << sqlite3_errmsg(db) << endl;
        return NULL;
    }

    if (found) cout << "\tNewsGroup exists ..." << endl;
    else cerr << "\tNewsGroup could not be found ..." << endl;

    sqlite3_finalize(select_stmt);
    return found;
}

bool DatabaseDB::NewsGroupExists(string title)
{
    // Our SQL query
    const char *select_sql = "SELECT COUNT(id) FROM newsgroups WHERE name = ?";

    // Statement to prepare
    sqlite3_stmt *select_stmt = NULL;

    // Prepare our statement
    //  The last parameter, dbend, is used if you have multiple sql statements in one string
    int rc = sqlite3_prepare_v2(db, select_sql, -1, &select_stmt, NULL);
    if (SQLITE_OK != rc)
    {
        cerr << "\tCan not prepare select statement " << select_sql << " (" << rc << "): " << sqlite3_errmsg(db) << endl;
        return NULL;
    }

    // Bind to query
    rc = sqlite3_bind_text(select_stmt, 1, title.c_str(), strlen(title.c_str()), NULL);
    if (SQLITE_OK != rc)
    {
        cerr << "\tError binding value in select statement (" << rc << "): " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        exit(1);
    }

    bool found = false;

    // Execute statement and build article
    while (SQLITE_ROW == (rc = sqlite3_step(select_stmt)))
    {
        found = sqlite3_column_int(select_stmt, 0);
    }

    if (SQLITE_DONE != rc)
    {
        cerr << "\tSelect statement did not finish with DONE (" << rc << "): " << sqlite3_errmsg(db) << endl;
        return NULL;
    }

    if (found) cout << "\tNewsGroup exists ..." << endl;
    else cerr << "\tNewsGroup could not be found ..." << endl;

    sqlite3_finalize(select_stmt);
    return found;
}

size_t DatabaseDB::NonDeletedNewsGroupCount()
{
    // Our SQL query
    const char *select_sql = "SELECT COUNT(deleted) FROM newsgroups WHERE deleted = 0";

    // Statement to prepare
    sqlite3_stmt *select_stmt = NULL;

    // Prepare our statement
    //  The last parameter, dbend, is used if you have multiple sql statements in one string
    int rc = sqlite3_prepare_v2(db, select_sql, -1, &select_stmt, NULL);
    if (SQLITE_OK != rc)
    {
        cerr << "\tCan not prepare select statement " << select_sql << " (" << rc << "): " << sqlite3_errmsg(db) << endl;
        return 0;
    }

    size_t count = 0;

    // Execute statement and build article
    while (SQLITE_ROW == (rc = sqlite3_step(select_stmt)))
    {
        count = static_cast<size_t>(sqlite3_column_int(select_stmt, 0));
    }
    if (SQLITE_DONE != rc)
    {
        cerr << "\tSelect statement did not finish with DONE (" << rc << "): " << sqlite3_errmsg(db) << endl;
        return 0;
    }

    cout << "\tNondeleted NewsGroup Count: " << count << " ..." << endl;

    sqlite3_finalize(select_stmt);
    return count;
}


MapArticle *DatabaseDB::ListArticles(int ngID)
{
    // Our SQL query
    const char *select_sql = "SELECT id, title, author, text, deleted FROM articles WHERE ngID = ? ORDER BY id ASC";

    // Statement to prepare
    sqlite3_stmt *select_stmt = NULL;

    // Prepare our statement
    //  The last parameter, dbend, is used if you have multiple sql statements in one string
    int rc = sqlite3_prepare_v2(db, select_sql, -1, &select_stmt, NULL);
    if (SQLITE_OK != rc)
    {
        cerr << "\tCan not prepare select statement " << select_sql << " (" << rc << "): " << sqlite3_errmsg(db) << endl;
        return NULL;
    }

    // Bind parameters
    rc = sqlite3_bind_int(select_stmt, 1, ngID);
    if (SQLITE_OK != rc)
    {
        cerr << "\tError binding value in select statement (" << rc << "): " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        exit(1);
    }
    MapArticle *articles = new MapArticle();

    // Execute statement and build article
    while (SQLITE_ROW == (rc = sqlite3_step(select_stmt)))
    {
        int id = sqlite3_column_int(select_stmt, 0);

        string title = string(reinterpret_cast<const char *>(
                                  sqlite3_column_text(select_stmt, 1)
                              ));
        string author = string(reinterpret_cast<const char *>(
                                   sqlite3_column_text(select_stmt, 2)
                               ));
        string text = string(reinterpret_cast<const char *>(
                                 sqlite3_column_text(select_stmt, 3)
                             ));
        bool deleted = sqlite3_column_int(select_stmt, 4);
        Article article(title, author, text, deleted);

        articles->insert(make_pair(id, article));
    }
    if (SQLITE_DONE != rc)
    {
        cerr << "\tSelect statement did not finish with DONE (" << rc << "): " << sqlite3_errmsg(db) << endl;
        return NULL;
    }

    cout << "\tCreated list of Articles successfully ..." << endl;

    sqlite3_finalize(select_stmt);
    return articles;
}

bool DatabaseDB::CreateArticle(int ngID, string title, string author, string text)
{
    if (!NewsGroupExists(ngID)) return false;

    // Our query
    const char insert_sql[] = "INSERT INTO articles (ngID, title, author, text, deleted) VALUES (?,?,?,?,0)";

    // SQlite statement to prepare
    sqlite3_stmt *insert_stmt = NULL;

    // Prepare the two statements we need to prepare
    //  The last parameter, dbend, is used if you have multiple sql statements in one string
    int rc = sqlite3_prepare_v2(db, insert_sql, -1, &insert_stmt, NULL);
    if (SQLITE_OK != rc)
    {
        cerr << "\tCan not prepare insert statement " << insert_sql << " (" << rc << "): " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        exit(1);
    }


    // The NULL is "Don't attempt to free() the valye when it's bound", since it's on the stack here
    rc = sqlite3_bind_int(insert_stmt, 1, ngID);
    if (SQLITE_OK != rc)
    {
        cerr << "\tError binding value in insert statement (" << rc << "): " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        exit(1);
    }
    rc = sqlite3_bind_text(insert_stmt, 2, title.c_str(), strlen(title.c_str()), NULL);
    if (SQLITE_OK != rc)
    {
        cerr << "\tError binding value in insert statement (" << rc << "): " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        exit(1);
    }
    rc = sqlite3_bind_text(insert_stmt, 3, author.c_str(), strlen(author.c_str()), NULL);
    if (SQLITE_OK != rc)
    {
        cerr << "\tError binding value in insert statement (" << rc << "): " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        exit(1);
    }
    rc = sqlite3_bind_text(insert_stmt, 4, text.c_str(), strlen(text.c_str()), NULL);
    if (SQLITE_OK != rc)
    {
        cerr << "\tError binding value in insert statement (" << rc << "): " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        exit(1);
    }

    if (SQLITE_OK != rc)
    {
        cerr << "\tError binding value in insert (" << rc << "): " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        exit(1);
    }

    // Perform the statement
    rc = sqlite3_step(insert_stmt);
    if (SQLITE_DONE != rc)
    {
        cerr << "\tInsert statement did not return DONE (" << rc << "): " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(insert_stmt);
        return false;
    }
    else
    {
        cout << "\tArticle created successfully ..." << endl;
        sqlite3_finalize(insert_stmt);
        return true;
    }
}

bool DatabaseDB::DeleteArticle(int ngID, int aID)
{
    cout << "\tTrying to delete Article[" << ngID << "." << aID << "]" << endl;
    // Our query
    const char update_sql[] = "UPDATE articles SET deleted = 1 WHERE id = ? AND ngID = ? ";

    // SQlite statement to prepare
    sqlite3_stmt *update_stmt = NULL;

    // Prepare the two statements we need to prepare
    //  The last parameter, dbend, is used if you have multiple sql statements in one string
    int rc = sqlite3_prepare_v2(db, update_sql, -1, &update_stmt, NULL);
    if (SQLITE_OK != rc)
    {
        cerr << "\tCan not prepare update statement " << update_sql << " (" << rc << "): " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        exit(1);
    }

    // The NULL is "Don't attempt to free() the valye when it's bound", since it's on the stack here
    rc = sqlite3_bind_int(update_stmt, 1, aID);
    if (SQLITE_OK != rc)
    {
        cerr << "\tError binding value in update statement (" << rc << "): " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        exit(1);
    }
    rc = sqlite3_bind_int(update_stmt, 2, ngID);
    if (SQLITE_OK != rc)
    {
        cerr << "\tError binding value in update statement (" << rc << "): " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        exit(1);
    }

    // Perform the statement
    rc = sqlite3_step(update_stmt);
    if (SQLITE_DONE != rc)
    {
        cerr << "\tUpdate statement did not return DONE (" << rc << "): " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(update_stmt);
        return false;
    }
    else
    {
        // Article deleted successfully
        cout << "\tArticle deleted successfully ..." << endl;
        sqlite3_finalize(update_stmt);
        return true;
    }
}

Article const *DatabaseDB::GetArticle(int ngID, int aID)
{
    cout << "\tTrying to get Article[" << aID << "]" << endl;
    // Our SQL query
    const char *select_sql = "SELECT title, author, text FROM articles WHERE ngID = ? AND id = ? AND deleted = 0 ORDER BY id ASC";

    // Statement to prepare
    sqlite3_stmt *select_stmt = NULL;

    // Prepare our statement
    //  The last parameter, dbend, is used if you have multiple sql statements in one string
    int rc = sqlite3_prepare_v2(db, select_sql, -1, &select_stmt, NULL);
    if (SQLITE_OK != rc)
    {
        cerr << "\tCan not prepare select statement " << select_sql << " (" << rc << "): " << sqlite3_errmsg(db) << endl;
        return NULL;
    }
    // Bind to query
    rc = sqlite3_bind_int(select_stmt, 1, ngID);
    if (SQLITE_OK != rc)
    {
        cerr << "\tError binding value in select statement (" << rc << "): " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        exit(1);
    }
    rc = sqlite3_bind_int(select_stmt, 2,  aID);
    if (SQLITE_OK != rc)
    {
        cerr << "\tError binding value in select statement (" << rc << "): " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        exit(1);
    }

    Article *article = NULL;

    // Execute statement and build article
    while (SQLITE_ROW == (rc = sqlite3_step(select_stmt)))
    {

        string title = string(reinterpret_cast<const char *>(
                                  sqlite3_column_text(select_stmt, 0)
                              ));
        string author = string(reinterpret_cast<const char *>(
                                   sqlite3_column_text(select_stmt, 1)
                               ));
        string text = string(reinterpret_cast<const char *>(
                                 sqlite3_column_text(select_stmt, 2)
                             ));

        article = new Article(title, author, text);
    }

    if (SQLITE_DONE != rc)
    {
        cerr << "\tSelect statement did not finish with DONE (" << rc << "): " << sqlite3_errmsg(db) << endl;
        return NULL;
    }

    if (article != NULL) cout << "\tArticle retrieved successfully ..." << endl;
    else cerr << "\tCould not retrieve Article ..." << endl;
    sqlite3_finalize(select_stmt);
    return article;
}

bool DatabaseDB::ArticleExists(int ngID, int aID)
{
    // Our SQL query
    const char *select_sql = "SELECT COUNT(id) FROM articles WHERE id = ? AND ngID = ?";

    // SQlite statement to prepare
    sqlite3_stmt *select_stmt = NULL;

    // Prepare the two statements we need to prepare
    //  The last parameter, dbend, is used if you have multiple sql statements in one string
    int rc = sqlite3_prepare_v2(db, select_sql, -1, &select_stmt, NULL);
    if (SQLITE_OK != rc)
    {
        cerr << "\tCan not prepare select statement " << select_sql << " (" << rc << "): " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        exit(1);
    }

    // Bind to query
    rc = sqlite3_bind_int(select_stmt, 1, aID);
    if (SQLITE_OK != rc)
    {
        cerr << "\tError binding value in select statement (" << rc << "): " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        exit(1);
    }
    rc = sqlite3_bind_int(select_stmt, 2, ngID);
    if (SQLITE_OK != rc)
    {
        cerr << "\tError binding value in select statement (" << rc << "): " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        exit(1);
    }

    bool found = false;

    // Execute statement and build article
    while (SQLITE_ROW == (rc = sqlite3_step(select_stmt)))
    {
        found = sqlite3_column_int(select_stmt, 0);
    }

    if (SQLITE_DONE != rc)
    {
        cerr << "\tSelect statement did not finish with DONE (" << rc << "): " << sqlite3_errmsg(db) << endl;
        return NULL;
    }

    if (found) cout << "\tArticle exists ..." << endl;
    else cerr << "\tArticle could not be found ..." << endl;

    sqlite3_finalize(select_stmt);
    return found;
}

size_t DatabaseDB::NonDeletedArticleCount(int ngID)
{
    // Our SQL query
    const char *select_sql = "SELECT COUNT(deleted) FROM articles WHERE deleted = 0 AND ngID = ?";

    // Statement to prepare
    sqlite3_stmt *select_stmt = NULL;

    // Prepare our statement
    //  The last parameter, dbend, is used if you have multiple sql statements in one string
    int rc = sqlite3_prepare_v2(db, select_sql, -1, &select_stmt, NULL);
    if (SQLITE_OK != rc)
    {
        cerr << "\tCan not prepare select statement " << select_sql << " (" << rc << "): " << sqlite3_errmsg(db) << endl;
        return 0;
    }

    // Bind parameters
    rc = sqlite3_bind_int(select_stmt, 1, ngID);
    if (SQLITE_OK != rc)
    {
        cerr << "\tError binding value in select statement (" << rc << "): " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        exit(1);
    }
    size_t count = 0;

    // Execute statement and build article
    while (SQLITE_ROW == (rc = sqlite3_step(select_stmt)))
    {
        count = static_cast<size_t>(sqlite3_column_int(select_stmt, 0));
    }
    if (SQLITE_DONE != rc)
    {
        cerr << "\tSelect statement did not finish with DONE (" << rc << "): " << sqlite3_errmsg(db) << endl;
        return 0;
    }

    cout << "\tNondeleted Article Count: " << count << " ..." << endl;

    sqlite3_finalize(select_stmt);
    return count;
}


string DatabaseDB::ToString()
{
    return "NotImplemented ...";
}

}