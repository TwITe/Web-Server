//
// Created by twite on 10.06.18.
//

#include "client.h"

namespace webserver {
    void client::set_id(int id) {
        this->id = id;
    }

    int client::get_id() {
        return id;
    }
}
