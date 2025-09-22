#include <arpa/inet.h>
#include <map>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <string>

const std::map<std::string_view, const int> ai_flag_map = {
    {"AI_PASSIVE", AI_PASSIVE},
    {"AI_CANONNAME", AI_CANONNAME},
    {"AI_NUMERICHOST", AI_NUMERICHOST},
    {"AI_NUMERICSERV", AI_NUMERICSERV},
    {"AI_ALL", AI_ALL},
    {"AI_ADDRCONFIG", AI_ADDRCONFIG},
    {"AI_V4MAPPED_CFG", AI_V4MAPPED_CFG},
    {"AI_V4MAPPED", AI_V4MAPPED},
    {"AI_UNUSABLE", AI_UNUSABLE},
    {"AI_MASK", AI_MASK},
    {"AI_DEFAULT", AI_DEFAULT},
};
const std::map<std::string_view, const int> ai_socktype_map = {
    {"SOCK_STREAM", SOCK_STREAM},
    {"SOCK_DGRAM", SOCK_DGRAM},
    {"SOCK_RAW", SOCK_RAW},
    {"SOCK_SEQPACKET", SOCK_SEQPACKET},
    {"SOCK_RDM", SOCK_RDM},
};
const std::map<std::string_view, const int> ai_family_map = {
    {"AF_UNSPEC", AF_UNSPEC},
    {"AF_INET", AF_INET},
    {"AF_INET6", AF_INET6},
    {"AF_UNIX", AF_UNIX},
};
const std::map<std::string_view, const int> ai_protocol_map = {
    {"IPPROTO_TCP", IPPROTO_TCP},
    {"IPPROTO_UDP", IPPROTO_UDP},
    {"IPPROTO_ICMP", IPPROTO_ICMP},
    {"IPPROTO_IP", IPPROTO_IP},
};

int main(const int argc, const char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <ip/hostname> <port/service> [<flags>]\n", argv[0]);
        return 255;
    }

    const std::string_view hostname = argv[1];
    const std::string_view port = argv[2];

    addrinfo hints;
    memset(&hints, 0, sizeof hints);

    for (int i = 3; i < argc; ++i) {
        // fprintf(stderr, "Option: %s\n", argv[i]);
        auto it = ai_flag_map.find(argv[i]);
        if (it != ai_flag_map.end()) {
            fprintf(stderr, "Flag: %s (0x%x)\n", argv[i], it->second);
            hints.ai_flags |= it->second;
        } else if ((it = ai_socktype_map.find(argv[i])) != ai_socktype_map.end()) {
            fprintf(stderr, "Socktype: %s (%d)\n", argv[i], it->second);
            hints.ai_socktype = it->second;
        } else if ((it = ai_family_map.find(argv[i])) != ai_family_map.end()) {
            fprintf(stderr, "Family: %s (%d)\n", argv[i], it->second);
            hints.ai_family = it->second;
        } else if ((it = ai_protocol_map.find(argv[i])) != ai_protocol_map.end()) {
            fprintf(stderr, "Protocol: %s (%d)\n", argv[i], it->second);
            hints.ai_protocol = it->second;
        } else
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
    }
    addrinfo *res;
    {
        int status;
        if ((status = getaddrinfo(hostname.data(), port.data(), &hints, &res)) != 0) {
            fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
            return 1;
        }
    }
    // printf("Results for %s:%s\n\n", hostname, port);
    for (addrinfo *p = res; p != NULL; p = p->ai_next) {
        switch (p->ai_family) {
        case AF_INET: {
            printf("inet ");
            char ipstr[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &((struct sockaddr_in *)p->ai_addr)->sin_addr, ipstr, sizeof ipstr);
            printf("%s", ipstr);
        } break;
        case AF_INET6: {
            printf("inet6 ");
            char ipstr[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, &((struct sockaddr_in6 *)p->ai_addr)->sin6_addr, ipstr, sizeof ipstr);
            printf("%s", ipstr);
        } break;
        default:
            printf("Unrecognized family (%d)", p->ai_family);
        }
        printf("\n");
    }
    freeaddrinfo(res);
    return 0;
}
