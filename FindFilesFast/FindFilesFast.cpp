#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include <thread>
#include <mutex>

using namespace std;

vector<string> pathes; 
mutex path_mutex; 
void find_files(const string& filestofind, const string& path) {
    string search_path = path + "\\*";
    WIN32_FIND_DATAA fd;
    HANDLE hFind = FindFirstFileA(search_path.c_str(), &fd);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (strcmp(fd.cFileName, ".") != 0 && strcmp(fd.cFileName, "..") != 0) {
                    string subdir_path = path + "\\" + string(fd.cFileName);
                    find_files(filestofind, subdir_path);
                }
            }
            else {
                string file_path = path + "\\" + string(fd.cFileName);
                if (file_path.find(filestofind) != string::npos) {
                    path_mutex.lock();
                    pathes.push_back(file_path);
                    path_mutex.unlock();
                }
            }
        } while (FindNextFileA(hFind, &fd));
        FindClose(hFind);
    }
}

string filetofind(const string& extention, const string& path, int num_threads) {
    string filestofind = "." + extention;

    vector<thread> threads;
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(find_files, filestofind, path);
    }

    for (auto& t : threads) {
        t.join();
    }

    string p = "";
    for (const auto& path : pathes) {
        p += path + "\n";
    }

    return p;
}

int main(int argc, char* argv[]) {
    std::string ascii_art = R"(
        
  ______ _           _ ______ _ _           
 |  ____(_)         | |  ____(_) |          
 | |__   _ _ __   __| | |__   _| | ___  ___ 
 |  __| | | '_ \ / _` |  __| | | |/ _ \/ __|
 | |    | | | | | (_| | |    | | |  __/\__ \
 |_|    |_|_| |_|\__,_|_|    |_|_|\___||___/
 Made By the attacker  
github=https://github.com/TheNewAttacker64/File-Finder    
    )";
    std::cout << ascii_art << std::endl;
    string ext = "";
    string dir = "";
    int threads = 1;

    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg.find("ext=") == 0) {
            ext = arg.substr(4);
        }
        else if (arg.find("path=") == 0) {
            dir = arg.substr(5);
        }
        else if (arg.find("threads=") == 0) {
            threads = stoi(arg.substr(8));
        }
    }

    if (ext.empty() || dir.empty()) {
        cerr << "Error: invalid arguments. Usage: findfiles.exe ext=<extension> path=<directory> [threads=<num_threads>]" << endl;
        return 1;
    }

    string result = filetofind(ext, dir, threads);
    cout << result;

    return 0;
}