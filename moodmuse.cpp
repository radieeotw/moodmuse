
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <map>
#include <iomanip>
#include <ctime>

using namespace std;

//helper fn
void bar(char c = '-', int n = 60) { 
    cout << string(n, c) << "\n";
}
void center(const string& s, int w = 60) {
    int p = (w - (int)s.size()) / 2;
    cout << string(max(0,p), ' ') << s << "\n";
}
string trim(const string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    size_t b = s.find_last_not_of(" \t\r\n");
    return a == string::npos ? "" : s.substr(a, b - a + 1);
}
string lower(string s) { 
    transform(s.begin(),s.end(),s.begin(),::tolower); 
    return s; 
}
vector<string> splitCSV(const string& line) {
    vector<string> f; string t; bool q=false;
    for (char c : line) {
        if (c=='"') q=!q;
        else if (c==',' && !q) { 
            f.push_back(trim(t)); 
            t.clear(); 
        }
        else t+=c;
    }
    f.push_back(trim(t)); 
    return f;
}

//emotional system
enum class Emotion { SAD, ANGRY, ANXIOUS, BORED, HAPPY, UNKNOWN };

string emotionName(Emotion e) {
    switch(e) {
        case Emotion::SAD:     return "Sad";
        case Emotion::ANGRY:   return "Angry";
        case Emotion::ANXIOUS: return "Anxious";
        case Emotion::BORED:   return "Bored";
        case Emotion::HAPPY:   return "Happy";
        default:               return "Unknown";
    }
}
string emotionTag(Emotion e) {
    switch(e) {
        case Emotion::SAD:     return "happy";
        case Emotion::ANGRY:   return "anxious";
        case Emotion::ANXIOUS: return "happy";
        case Emotion::BORED:   return "bored";
        case Emotion::HAPPY:   return "happy";
        default:               return "happy";
    }
}
string emotionMsg(Emotion e) {
    switch(e) {
        case Emotion::SAD:     return "Here's something uplifting for you!";
        case Emotion::ANGRY:   return "Take a breath. Let's calm things down.";
        case Emotion::ANXIOUS: return "You're okay. Here's something soothing.";
        case Emotion::BORED:   return "Let's shake things up with these picks!";
        case Emotion::HAPPY:   return "Great vibes! Let's keep them going!";
        default:               return "Here are your recommendations!";
    }
}
Emotion pickEmotion(int n) {
    switch(n) {
        case 1: return Emotion::SAD;
        case 2: return Emotion::ANGRY;
        case 3: return Emotion::ANXIOUS;
        case 4: return Emotion::BORED;
        case 5: return Emotion::HAPPY;
        default: return Emotion::UNKNOWN;
    }
}

// abstract base class
class Item {
protected:
    string title, genre, tag;
    int    year;
public:
    Item() : year(0) {}
    Item(const string& t, const string& g, const string& tg, int y)
        : title(t), genre(g), tag(tg), year(y) {}
    virtual ~Item() = default;
    virtual void show(int i) const = 0;
    virtual string kind() const = 0;
    const string& getTitle() const { return title; }
    bool matches(const string& t) const {
        return lower(tag)==lower(t);
    }
};

// movie
class Movie : public Item {
    double rating;
public:
    Movie() : rating(0) {}
    Movie(const string& t,const string& g,const string& tg,int y,double r)
        : Item(t,g,tg,y), rating(r) {}
    string kind() const override { return "Movie"; }
    void show(int i) const override {
        cout << setw(3) << i << ". " << left << setw(36) << title
             << " [" << year << "] * " << fixed << setprecision(1) << rating
             << "\n     Genre: " << genre << "\n";
    }
};
// song
class Song : public Item {
    string artist;
public:
    Song() {}
    Song(const string& t,const string& a,const string& g,const string& tg,int y)
        : Item(t,g,tg,y), artist(a) {}
    string kind() const override { return "Song"; }
    const string& getArtist() const { return artist; }
    void show(int i) const override {
        cout << setw(3) << i << ". " << left << setw(30) << title
             << " -- " << artist << "  (" << year << ")\n"
             << "     Genre: " << genre << "\n";
    }
};
//book
class Book : public Item {
    string author;
    double rating;
public:
    Book() : rating(0) {}
    Book(const string& t,const string& a,const string& g,const string& tg,int y,double r)
        : Item(t,g,tg,y), author(a), rating(r) {}
    string kind() const override { return "Book"; }
    void show(int i) const override {
        cout << setw(3) << i << ". " << left << setw(36) << title
             << " * " << fixed << setprecision(1) << rating << "\n"
             << "     By: " << author << " | Genre: " << genre << "\n";
    }
};
//temp
template<typename T>
class Loader {
    vector<T> data;
    string    path;
public:
    explicit Loader(const string& p) : path(p) {}
    bool load();
    const vector<T>& all() const { return data; }
    size_t size() const { return data.size(); }
    vector<const T*> byTag(const string& t) const {
        vector<const T*> res;
        for (const auto& x : data) if (x.matches(t)) res.push_back(&x);
        return res;
    }
};

template<> bool Loader<Movie>::load() {
    ifstream f(path); if (!f) { cerr<<"Cannot open "<<path<<"\n"; return false; }
    string line; getline(f, line);
    while (getline(f, line)) {
        if (line.empty()) continue;
        auto v = splitCSV(line); if (v.size()<5) continue;
        try { data.emplace_back(v[0],v[1],v[2],stoi(v[3]),stod(v[4])); } catch(...){}
    }
    return !data.empty();
}
template<> bool Loader<Song>::load() {
    ifstream f(path); if (!f) { cerr<<"Cannot open "<<path<<"\n"; return false; }
    string line; getline(f, line);
    while (getline(f, line)) {
        if (line.empty()) continue;
        auto v = splitCSV(line); if (v.size()<5) continue;
        try { data.emplace_back(v[0],v[1],v[2],v[3],stoi(v[4])); } catch(...){}
    }
    return !data.empty();
}
template<> bool Loader<Book>::load() {
    ifstream f(path); if (!f) { cerr<<"Cannot open "<<path<<"\n"; return false; }
    string line; getline(f, line);
    while (getline(f, line)) {
        if (line.empty()) continue;
        auto v = splitCSV(line); if (v.size()<6) continue;
        try { 
            data.emplace_back(v[0],v[1],v[2],v[3],stoi(v[4]),stod(v[5])); 
        } catch(...){}
    }
    return !data.empty();
}
//engine
class Engine {
    Loader<Movie> ML;
    Loader<Song>  SL;
    Loader<Book>  BL;
    mt19937       rng;
public:
    Engine(const string& m,const string& s,const string& b)
        : ML(m), SL(s), BL(b), rng(random_device{}()) {}
    bool load() { return ML.load() && SL.load() && BL.load(); }
    template<typename T>
    vector<const T*> pick(const Loader<T>& ld, const string& tag, int n) {
        auto v = ld.byTag(tag);
        shuffle(v.begin(), v.end(), rng);
        if (n < (int)v.size()) v.resize(n);
        return v;
    }
    vector<const Movie*> movies(const string& t,int n=5){ 
        return pick(ML,t,n); 
    }
    vector<const Song*>  songs (const string& t,int n=5){
         return pick(SL,t,n); 
    }
    vector<const Book*>  books (const string& t,int n=5){ 
        return pick(BL,t,n); 
    }
    void search(const string& q, vector<const Movie*>& mv,
                vector<const Song*>& sg, vector<const Book*>& bk) const {
        string ql = lower(q);
        for (const auto& x : ML.all())
            if (lower(x.getTitle()).find(ql)!=string::npos) mv.push_back(&x);
        for (const auto& x : SL.all())
            if (lower(x.getTitle()).find(ql)!=string::npos ||
                lower(x.getArtist()).find(ql)!=string::npos) sg.push_back(&x);
        for (const auto& x : BL.all())
            if (lower(x.getTitle()).find(ql)!=string::npos) bk.push_back(&x);
    }
    size_t nm() const {
         return ML.size(); 
    }
    size_t ns() const {
        return SL.size(); 
    }
    size_t nb() const {
        return BL.size(); 
    }
};
//user profile
class Profile {
    string name, histFile;
    vector<pair<string,string>> history;
    vector<string> favs;
public:
    void init(const string& n) {
        name = n; histFile = n + "_hist.txt";
        ifstream f(histFile); string line;
        while (getline(f,line)) {
            size_t p = line.find('|');
            if (p!=string::npos)
                history.push_back({line.substr(0,p), line.substr(p+1)});
        }
    }
    const string& getName() const { return name; }
    void log(Emotion e, const string& t) {
        history.push_back({emotionName(e), t});
        ofstream f(histFile, ios::app); f << emotionName(e) << "|" << t << "\n";
    }
    void addFav(const string& t) { favs.push_back(t); cout << "  Saved!\n"; }
    void showHistory() const {
        if (history.empty()) { cout << "  No history yet.\n"; return; }
        for (int i=(int)history.size()-1,n=1; i>=0; --i,++n)
            cout << "  " << n << ". [" << history[i].first << "] " << history[i].second << "\n";
    }
    void showFavs() const {
        if (favs.empty()) { cout << "  No favourites yet.\n"; return; }
        for (int i=0;i<(int)favs.size();++i) cout << "  " << i+1 << ". " << favs[i] << "\n";
    }
    map<string,int> moodStats() const {
        map<string,int> m;
        for (const auto& h : history) m[h.first]++;
        return m;
    }
};

//ui
class UI {
    Engine&  eng;
    Profile& prof;
    template<typename T>
    void section(const string& hdr, const vector<const T*>& v) {
        bar(); cout << "  " << hdr << "\n"; bar();
        if (v.empty()) { cout << "  Nothing found.\n"; return; }
        for (int i=0;i<(int)v.size();++i) v[i]->show(i+1);
    }
public:
    UI(Engine& e, Profile& p) : eng(e), prof(p) {}
    void recommend() {
        bar('=');
        cout << "  1-Sad  2-Angry  3-Anxious  4-Bored  5-Happy\n";
        bar(); cout << "  Your mood: ";
        int n; cin >> n; cin.ignore();
        Emotion em = pickEmotion(n);
        if (em==Emotion::UNKNOWN) { cout << "  Invalid choice.\n"; return; }
        string tag = emotionTag(em);
        bar('='); center("Recommendations"); bar('=');
        cout << "\n  >> " << emotionMsg(em) << "\n\n";
        auto mv=eng.movies(tag); auto sg=eng.songs(tag); auto bk=eng.books(tag);
        section("MOVIES",mv);
        cout<<endl; 
        section("SONGS",sg); cout<<endl;
        section("BOOKS",bk);
        if (!mv.empty()) prof.log(em, mv[0]->getTitle());
        if (!sg.empty()) prof.log(em, sg[0]->getTitle());
        if (!bk.empty()) prof.log(em, bk[0]->getTitle());
        cout << "\n  Add to favourites? Title (Enter to skip): ";
        string t; getline(cin,t); if (!t.empty()) prof.addFav(t);
    }
    void search() {
        cout << "  Keyword: ";
        string q; 
        getline(cin,q);
        if (q.empty()) return;
        vector<const Movie*> mv; vector<const Song*> sg; vector<const Book*> bk;
        eng.search(q,mv,sg,bk);
        bar('='); center("Results: "+q); bar('=');
        if (mv.empty()&&sg.empty()&&bk.empty()) { cout<<"  No results.\n"; return; }
        if (!mv.empty()) section("MOVIES",mv);
        if (!sg.empty()) { cout<<"\n"; section("SONGS",sg); }
        if (!bk.empty()) { cout<<"\n"; section("BOOKS",bk); }
    }
    void stats() {
        bar('='); center("Mood Stats"); bar('=');
        auto m = prof.moodStats();
        if (m.empty()) { cout<<"  No data yet.\n"; return; }
        int tot=0; for (auto& kv:m) tot+=kv.second;
        for (auto& kv:m) {
            int bl = tot ? kv.second*20/tot : 0;
            cout << "  " << left << setw(10) << kv.first
                 << " | " << setw(3) << kv.second
                 << " | " << string(bl,'#') << "\n";
        }
        cout << "  Total: " << tot << "\n";
    }
    void run() {
        bar('='); center("MoodMuse - Emotion Recommender"); bar('=');
        cout << "  " << eng.nm() << " movies | " << eng.ns()
             << " songs | " << eng.nb() << " books loaded\n";
        int ch;
        do {
            bar('=');
            cout << "  1.Recommend  2.Search  3.History\n"
                 << "  4.Favourites 5.Stats   6.Exit\n";
            bar(); cout << "  Choice: ";
            if (!(cin>>ch)) { cin.clear(); cin.ignore(1000,'\n'); continue; }
            cin.ignore();
            switch(ch) {
                case 1: recommend(); break;
                case 2: search();    break;
                case 3: bar('='); center("History");    bar('='); prof.showHistory(); break;
                case 4: bar('='); center("Favourites"); bar('=');
                        prof.showFavs();
                        cout<<"  Add title (Enter to skip): ";
                        { string t; getline(cin,t); if(!t.empty()) prof.addFav(t); } break;
                case 5: stats(); break;
                case 6: cout<<"  Bye, "<<prof.getName()<<"!\n"; break;
                default: cout<<"  Enter 1-6.\n";
            }
        } while (ch!=6);
    }
};

//main
int main() {
    bar('='); center("Welcome to MoodMuse"); bar('=');
    cout << "  Your name: "; string name; getline(cin,name);
    if (name.empty()) name = "Friend";
    Engine eng("movies.csv","songs.csv","books.csv");
    cout << "  Loading...\n";
    if (!eng.load()) { 
        cerr<<"  Error: CSV files not found.\n"; 
        return 1; 
    }
    Profile prof; 
    prof.init(name);
    UI ui(eng,prof); 
    ui.run();
    return 0;
}
