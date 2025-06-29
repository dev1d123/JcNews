#ifndef TRIE_H
#define TRIE_H

#include <QString>
#include <QMap>
#include <QStringList>

class TrieNode {
public:
    QMap<QChar, TrieNode*> children;
    bool isEndOfWord;
    QString palabra;

    TrieNode();
    ~TrieNode();
};

class Trie {
public:
    Trie();
    ~Trie();

    void insert(const QString& word);
    void searchPrefix(const QString& prefix, QStringList& results, int maxResults = 10);

private:
    TrieNode* root;
    void dfs(TrieNode* node, QStringList& results, int maxResults);
    void freeNode(TrieNode* node);
};

#endif // TRIE_H
