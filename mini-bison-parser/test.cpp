int main() {
    int score = 85;
    
    if (score >= 90) {
        cout << "Grade: A";
    } else if (score >= 80) {
        cout << "Grade: B";
    } else if (score >= 70) {
        cout << "Grade: C";
    } else {
        cout << "Grade: F";
    }
    
    if (score > 0 && score <= 100) {
        cout << "Valid score";
    }
}