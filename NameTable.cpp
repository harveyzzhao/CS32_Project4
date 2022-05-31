// NameTable.cpp

// This is a correct but inefficient implementation of
// the NameTable functionality.

#include "NameTable.h"
#include <string>
#include <vector>
#include <list>
#include <functional>   //supports hashFunction
#include <utility>

using namespace std;

const int BUCKET_SIZE = 19997;

//struct Pair
//{
//    Pair(string vn, int ln)
//    {
//        varName = vn;
//        lineNum = ln;
//    }
//    string varName;
//    int lineNum;
//};

class HashTable
{
public:
    HashTable(){}
    ~HashTable(){}
    void insert(string varName, int lineNum);
    int search(string varName) const;
//    HashTable(const HashTable&) = delete;
//    HashTable& operator=(const HashTable&) = delete;
    
private:
    unsigned int hashFunc(string varName) const
    {
        hash<string> str_hasher;    //creates a string hasher
        size_t hashValue = str_hasher(varName);
        return hashValue % BUCKET_SIZE;
    }
    int bucketCap = BUCKET_SIZE;
    vector<pair<string, int>> m_varToLine[BUCKET_SIZE]; //consider using Block ptrs
};

void HashTable::insert(string varName, int lineNum)
{
    int targetBucket = hashFunc(varName);
    m_varToLine[targetBucket].push_back(make_pair(varName, lineNum));
}

int HashTable::search(string varName) const
{
    int targetBucket = hashFunc(varName);
    
    if (m_varToLine[targetBucket].empty())  return -1;
    
    if (m_varToLine[targetBucket].back().first == varName)
        return m_varToLine[targetBucket].back().second;
    
    else
    {
        for (vector<pair<string, int>>::const_reverse_iterator it = m_varToLine[targetBucket].rbegin();
             it != m_varToLine[targetBucket].rend(); ++it)
        {
            if (it->first == varName)
                return it->second;
        }
        return -1;
    }
}

class NameTableImpl
{
  public:
    NameTableImpl(): m_scope(1){m_scopes.push_back(HashTable());}
    ~NameTableImpl(){m_scopes.clear();}
    void enterScope();
    bool exitScope();
    bool declare(const string& id, int lineNum);
    int find(const string& id) const;
    
  private:
    list<HashTable> m_scopes;
    int m_scope;
};

void NameTableImpl::enterScope()
{
    m_scopes.push_back(HashTable());
    m_scope++;
}

bool NameTableImpl::exitScope()
{
    if (m_scopes.empty() || m_scope <= 1)   return false;
    else
    {
        m_scopes.pop_back();
        m_scope--;
        return true;
    }
}

bool NameTableImpl::declare(const string& id, int lineNum)
{
    //if (search(id) == true)   return false;
    if (m_scopes.back().search(id) != -1)   return false;
    else
    {
        m_scopes.back().insert(id, lineNum);
        return true;
    }
}

//FIXME: don't check every single bucket
int NameTableImpl::find(const string& id) const
{
    //search from the last scope to the earliest scope
    for (list<HashTable>::const_reverse_iterator it = m_scopes.rbegin(); it != m_scopes.rend(); ++it)
    {
        int searchResultLineNumber = it->search(id);
        if (searchResultLineNumber != -1)
            return searchResultLineNumber;
    }
    return -1;
}

//*********** NameTable functions **************

// For the most part, these functions simply delegate to NameTableImpl's
// functions.

NameTable::NameTable()
{
    m_impl = new NameTableImpl;
}

NameTable::~NameTable()
{
    delete m_impl;
}

void NameTable::enterScope()
{
    m_impl->enterScope();
}

bool NameTable::exitScope()
{
    return m_impl->exitScope();
}

bool NameTable::declare(const string& id, int lineNum)
{
    return m_impl->declare(id, lineNum);
}

int NameTable::find(const string& id) const
{
    return m_impl->find(id);
}
