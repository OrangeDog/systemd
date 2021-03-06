/* SPDX-License-Identifier: LGPL-2.1+ */
/***
  This file is part of systemd.

  Copyright 2010 Lennart Poettering

  systemd is free software; you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation; either version 2.1 of the License, or
  (at your option) any later version.

  systemd is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with systemd; If not, see <http://www.gnu.org/licenses/>.
***/

#include <errno.h>
#include <unistd.h>

#include "fileio.h"
#include "fileio-label.h"
#include "fs-util.h"
#include "log.h"
#include "selinux-util.h"
#include "string-util.h"
#include "util.h"

int main(int argc, char*argv[]) {
        int r, k;

        if (argc != 2) {
                log_error("This program requires one argument.");
                return EXIT_FAILURE;
        }

        log_set_target(LOG_TARGET_AUTO);
        log_parse_environment();
        log_open();

        umask(0022);

        mac_selinux_init();

        if (streq(argv[1], "start")) {
                r = unlink_or_warn("/run/nologin");
                k = unlink_or_warn("/etc/nologin");
                if (r < 0 || k < 0)
                        return EXIT_FAILURE;

        } else if (streq(argv[1], "stop")) {
                r = write_string_file_atomic_label("/run/nologin", "System is going down.");
                if (r < 0) {
                        log_error_errno(r, "Failed to create /run/nologin: %m");
                        return EXIT_FAILURE;
                }

        } else {
                log_error("Unknown verb %s.", argv[1]);
                return EXIT_FAILURE;
        }

        mac_selinux_finish();

        return EXIT_SUCCESS;
}
