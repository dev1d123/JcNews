#include "trie.h"

TrieNode::TrieNode() : isEndOfWord(false) {}

TrieNode::~TrieNode() {
    qDeleteAll(children); // libera recursivamente
}

Trie::Trie() {
    root = new TrieNode();
}

Trie::~Trie() {
    delete root; // esto llama recursivamente a ~TrieNode()
}

void Trie::insert(const QString& word) {
    TrieNode* node = root;
    for (const QChar& c : word) {
        if (!node->children.contains(c)) {
            node->children[c] = new TrieNode();
        }
        node = node->children[c];
    }
    node->isEndOfWord = true;
    node->palabra = word;
}

void Trie::searchPrefix(const QString& prefix, QStringList& results, int maxResults) {
    TrieNode* node = root;
    for (const QChar& c : prefix) {
        if (!node->children.contains(c))
            return;
        node = node->children[c];
    }
    dfs(node, results, maxResults);
}

void Trie::dfs(TrieNode* node, QStringList& results, int maxResults) {
    if (!node || results.size() >= maxResults)
        return;

    if (node->isEndOfWord)
        results.append(node->palabra);

    for (TrieNode* child : node->children)
        dfs(child, results, maxResults);
}
