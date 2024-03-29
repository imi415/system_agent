#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

// For systemd-based system, we can use an uuid generated by systemd during first boot.
int user_system_get_systemd_unique_id(char *uuid) {
    int machid_fd = open("/etc/machine-id", O_RDONLY);
    if(machid_fd < 0) return -1;

    if(read(machid_fd, uuid, 32) < 32) {
        close(machid_fd);
        return -2;
    }

    close(machid_fd);

    return 0;
}

int user_system_get_primary_mac_address(char *mac_addr) {
    return 0;
}