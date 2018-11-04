#include "cdb.h"
#include "kernel.h"
#include <iostream>

using namespace std;

int CDB::notify_reservations() {
    for (const auto& notification: notifications) {
        string from = notification.first;
        float value = notification.second;
        // iterate all reservations to find if any needs this
        for (const auto& item: kernel -> reservations) {
            Reservation *r = item.second;
            if (r -> get_qj() == from) r -> fill_qj(value);
            if (r -> get_qk() == from) r -> fill_qk(value);
        }
    }
    notifications.clear();
    return 0;
}

int CDB::write_back(string from, float value) {
    notifications.push_back(pair<string, float>(from, value));
    return kernel -> registers -> write_back(from, value);
}