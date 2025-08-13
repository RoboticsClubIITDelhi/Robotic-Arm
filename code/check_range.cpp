#include <bits/stdc++.h>
using namespace std;

// Convert degrees ↔ radians
double deg2rad(double deg) { return deg * M_PI / 180.0; }
double rad2deg(double rad) { return rad * 180.0 / M_PI; }

// Normalize angle to [0, 2π)
double normalizeAngle(double ang) {
    ang = fmod(ang, 2 * M_PI);
    if (ang < 0) ang += 2 * M_PI;
    return ang;
}

// Solve inverse kinematics for given x, y
vector<pair<double, double>> solveInverseKinematics(double x, double y) {
    const double l1 = 170.0;
    const double l3 = 200.0;

    vector<pair<double, double>> solutions;

    double R = sqrt(x * x + y * y);
    double alpha = atan2(y, x);
    double K = (x * x + y * y + l1 * l1 - l3 * l3) / (2 * l1);

    // Check reachability
    if (fabs(K) > R) return solutions;

    double delta = acos(K / R);
    vector<double> theta1_candidates = { alpha + delta, alpha - delta };

    for (double th1 : theta1_candidates) {
        double c2 = (x - l1 * cos(th1)) / l3;
        double s2 = (y - l1 * sin(th1)) / l3;
        double th2 = atan2(s2, c2);

        double th1n = normalizeAngle(th1);
        double th2n = normalizeAngle(th2);

        if (th1n > th2n) {
            solutions.emplace_back(rad2deg(th1n), rad2deg(th2n));
        }
    }

    return solutions;
}

int main() {
    double x, y;
    cout << "Enter x and y coordinates: ";
    cin >> x >> y;

    vector<pair<double, double>> solutions = solveInverseKinematics(x, y);

    if (solutions.empty()) {
        cout << "No valid IK solutions found for the given coordinates." << endl;
    } else {
        cout << "Possible IK solutions (theta1, theta2 in degrees):" << endl;
        for (const auto& sol : solutions) {
            cout << "θ1 = " << fixed << setprecision(2) << sol.first
                 << "°, θ2 = " << fixed << setprecision(2) << sol.second << "°" << endl;
        }
    }

    return 0;
}
