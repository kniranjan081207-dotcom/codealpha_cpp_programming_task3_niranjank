#include <cctype>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <string>
#include <vector>

using namespace std;

const double MAX_POINTS = 10.0;

const map<string, double> gradeScale = {
    {"O", 10}, {"A+", 9}, {"A", 8}, {"B+", 7}, {"B", 6}, {"C", 5}, {"P", 4}, {"F", 0}
    };

struct Course {
    string name;
    string grade;
    double points;
    double credits;
    double weighted() const {
         return points * credits;
    }
};

struct Totals {
    double credits = 0, points = 0;
    double gpa() const { return credits > 0 ? points / credits : 0.0; }
};

static void clearLine() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

static void checkEof() {
    if (cin.eof()) {
        cerr << "\nInput ended unexpectedly.\n";
        exit(1);
    }
}

int readInt(const string& prompt, int minimum = 1) {
    int v;
    while (true) {
        cout << prompt;
        if (cin >> v && v >= minimum) { clearLine(); return v; }
        checkEof();
        cout << "  Please enter a whole number >= " << minimum << ".\n";
        clearLine();
    }
}

double readCredits(const string& prompt) {
    double v;
    while (true) {
        cout << prompt;
        if (cin >> v && v > 0) { clearLine(); return v; }
        checkEof();
        cout << "  Credit hours must be a number greater than 0.\n";
        clearLine();
    }
}

void readGrade(const string& prompt, string& label, double& points) {
    while (true) {
        cout << prompt;
        string raw;
        cin >> raw;
        checkEof();
        clearLine();

        string upper = raw;
        for (char& c : upper) c = toupper(static_cast<unsigned char>(c));
        auto it = gradeScale.find(upper);
        if (it != gradeScale.end()) { label = upper; points = it->second; return; }

        try {
            size_t used = 0;
            double p = stod(raw, &used);
            if (used == raw.size() && p >= 0 && p <= MAX_POINTS) {
                label = raw; points = p; return;
            }
        } catch (...) {}
        cout << "  Enter a letter (O, A+, A, B+, B, C, P, F) or points (0-10).\n";
    }
}

vector<Course> enterSemester(int number) {
    cout << "\n=== Semester " << number << " ===\n";
    int n = readInt("Number of courses taken: ");
    vector<Course> courses;
    for (int i = 1; i <= n; ++i) {
        cout << "Course " << i << ":\n";
        Course c;
        cout << "  Course name (optional): ";
        getline(cin, c.name);
        checkEof();
        if (c.name.empty()) c.name = "Course " + to_string(i);
        readGrade("  Grade (letter or points): ", c.grade, c.points);
        c.credits = readCredits("  Credit hours: ");
        courses.push_back(c);
    }
    return courses;
}

Totals printSemester(int number, const vector<Course>& courses) {
    Totals t;
    cout << "\n--- Semester " << number << " Report ---\n";
    cout << left << setw(22) << "Course" << right << setw(7) << "Grade" << setw(8) << "Points"
         << setw(9) << "Credits" << setw(10) << "Pts x Cr" << "\n";
    cout << fixed << setprecision(2);
    for (const auto& c : courses) {
        cout << left << setw(22) << c.name.substr(0, 21) << right << setw(7) << c.grade
             << setw(8) << c.points << setw(9) << setprecision(1) << c.credits
             << setw(10) << setprecision(2) << c.weighted() << "\n";
        t.credits += c.credits;
        t.points += c.weighted();
    }
    cout << "Total credits: " << setprecision(1) << t.credits
         << " | Total grade points: " << setprecision(2) << t.points
         << " | GPA: " << t.gpa() << "\n";
    return t;
}

int main() {
    cout << "CGPA CALCULATOR\n";
    int semesters = readInt("Number of semesters to enter: ");

    Totals cumulative;
    struct Row { int sem; double gpa, cgpa; };
    vector<Row> history;

    for (int s = 1; s <= semesters; ++s) {
        vector<Course> courses = enterSemester(s);
        Totals t = printSemester(s, courses);
        cumulative.credits += t.credits;
        cumulative.points += t.points;
        history.push_back({s, t.gpa(), cumulative.gpa()});
        cout << "CGPA after semester " << s << ": " << fixed << setprecision(2)
             << cumulative.gpa() << "\n";
    }

    cout << "\n=== FINAL SUMMARY ===\n";
    cout << left << setw(10) << "Semester" << right << setw(8) << "GPA" << setw(8) << "CGPA" << "\n";
    cout << fixed << setprecision(2);
    for (const auto& r : history)
        cout << left << setw(10) << r.sem << right << setw(8) << r.gpa << setw(8) << r.cgpa << "\n";

    cout << "\nTotal credits: " << setprecision(1) << cumulative.credits << "\n";
    cout << "Total grade points: " << setprecision(2) << cumulative.points << "\n";
    cout << "FINAL CGPA: " << cumulative.gpa() << "\n";
    return 0;
}
