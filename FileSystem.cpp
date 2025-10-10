#include<bits/stdc++.h>
using namespace std;

class INode{
    public:
    string name;
    bool isFile;
    INode(string &s, bool f) : name(s), isFile(f) {}
    virtual ~INode() {}
};

class File : public INode{
    public :
    string content;
    File(string &s) : INode(s,true), content("") {}
};

class Directory : public INode{
    public:
    unordered_map<string,INode*> Children;
    Directory(string &s) : INode(s,false) {}
};

vector<string> splitPath(const string& path){
    vector<string> res;
    string curr;
    for(char c : path){
        if(c == '/'){
            if(!curr.empty()){
                res.push_back(curr);
                curr.clear();
            }
        } else {
            curr.push_back(c);
        }
    }
    if(!curr.empty()) res.push_back(curr);
    return res;
}
class FileSystem{
    Directory *root;

    INode* findNode(const string &path){
        if(path.empty() || path == "/")return root;
        vector<string> parts = splitPath(path);
        Directory* curr = root;

        for(auto &str : parts){
            auto it = curr->Children.find(str);
            if(it == curr->Children.end()) return nullptr;
            INode* next = it->second;
            if(next->isFile){
                return nullptr;
            }else{
                curr = dynamic_cast<Directory*>(next);
                if(!curr) return nullptr;
            }
        }
        return curr;
    }

    Directory* traverse(const string & path, bool createIfMissing){
        if(path.empty() || path == "/") return root;
        vector<string> parts = splitPath(path);
        Directory* curr = root;
        for(auto &p : parts){
            auto it = curr->Children.find(p);
            if(it == curr->Children.end()){
                if(createIfMissing){
                    Directory *newDir = new Directory(p);
                    curr->Children[p] = newDir;
                    curr = newDir;
                }else{
                    return nullptr;
                }
            }else{
                INode * next = it->second;
                if(next->isFile) return nullptr;
                curr = dynamic_cast<Directory*>(next);
            }
        } 
        return curr;       
    }

    public:
    FileSystem(){
        string rootName = "/";
        root = new Directory(rootName);
    }
    bool mkdir(const string &path){
        if(path.empty()) return false;
        if(path == "/") return true;
        Directory* dir = traverse(path,true);
        return dir != nullptr;
    }

    vector<string> ls(const string &path){
        INode* node = findNode(path);
        vector<string> res;
        if(!node) return res;
        if(node->isFile){
            res.push_back(node->name);
            return res;
        } else {
            Directory *dir = dynamic_cast<Directory*>(node);
            for(auto &p : dir->Children){
                res.push_back(p.first);
            }
            sort(res.begin(), res.end());
            return res;           
        }
 }

    bool addFile(const string &path, const string &content){
        if(path.empty() || path == "/") return false;
        vector<string> parts = splitPath(path);
        string fileName = parts.back();
        parts.pop_back();
        string parentPath = "/";
        for(auto &p : parts){
            parentPath += p + "/";
        }
        Directory* parent = traverse(parentPath, true);
        if(!parent) return false;
        auto it = parent->Children.find(fileName);
        if(it != parent->Children.end()){
            INode* existing = it->second;
            if(existing->isFile){
                File* f = dynamic_cast<File*>(existing);
                f->content = content;
                return true;
            }else{
                return false;
            }
        }else{
            File *newFile = new File(fileName);
            newFile->content = content;
            parent->Children[fileName] = newFile;
            return true;
        }  
    }

    bool appendFile(const string &path, const string &content){
        if(path.empty() || path == "/") return false;
        vector<string> parts = splitPath(path);
        string fileName = parts.back();
        parts.pop_back();
        string parentPath = "/";
        for(auto &p : parts){
            parentPath += p + "/";
        }
        Directory* parent = traverse(parentPath, true);
        if(!parent) return false;
        auto it = parent->Children.find(fileName);
        if(it != parent->Children.end()){
            INode* existing = it->second;
            if(existing->isFile){
                File* f = dynamic_cast<File*>(existing);
                f->content += content;
                return true;
            }else{
                return false;
            }
        }else{
            File *newFile = new File(fileName);
            newFile->content = content;
            parent->Children[fileName] = newFile;
            return true;
        }  
    }

    bool rm(const string &path){
        if(path.empty() || path == "/" )return false;
        vector<string> parts = splitPath(path);
        string name = parts.back();
        parts.pop_back();
        string parentPath = "/";
        for(auto &p : parts){
            parentPath += p + "/";
        }
        Directory* parent = traverse(parentPath, false);
        if(!parent) return false;
        auto it = parent->Children.find(name);
        if(it == parent->Children.end()) return false;
        INode * node = it->second;
        if(!node->isFile){
            Directory* dir = dynamic_cast<Directory*>(node);
            if(!dir->Children.empty()) return false;
        }
        parent->Children.erase(it);
        return true;        
    }
};


int main (){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    FileSystem fs;

    fs.mkdir("/a/b/c");
    cout << "Made /a/b/c\n";

    // add file
    fs.addFile("/a/b/c/file1.txt", "Hello, world!\n");
    cout << "Added /a/b/c/file1.txt\n";

    // append file
    fs.appendFile("/a/b/c/file1.txt", "Second line.\n");
    cout << "Appended to /a/b/c/file1.txt\n";

    // ls root
    auto rootList = fs.ls("/");
    cout << "ls /:";
    for (auto &s : rootList) cout << " " << s;
    cout << "\n";

    // ls /a/b/c
    auto list = fs.ls("/a/b/c");
    cout << "ls /a/b/c:";
    for (auto &s : list) cout << " " << s;
    cout << "\n";

    // create another file
    fs.addFile("/a/b/c/file2.txt", "Another file\n");
    cout << "Added /a/b/c/file2.txt\n";

    // try removing non-empty directory
    bool removed = fs.rm("/a/b");
    cout << "rm /a/b (should fail because non-empty): " << (removed ? "OK" : "Failed") << "\n";

    // remove files then directory
    fs.rm("/a/b/c/file1.txt");
    fs.rm("/a/b/c/file2.txt");
    cout << "Removed files in /a/b/c\n";
    bool removedC = fs.rm("/a/b/c");
    cout << "rm /a/b/c: " << (removedC ? "OK" : "Failed") << "\n";

    // final ls /a
    auto la = fs.ls("/a");
    cout << "ls /a:";
    for (auto &s : la) cout << " " << s;
    cout << "\n";

    return 0;

}