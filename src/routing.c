#include "routing.h"
#include <linux/rtnetlink.h>
#include <net/if.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static int netlink_sendmsg(struct nlmsghdr *nlh, size_t len) {
    int sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (sock < 0) {
        perror("socket");
        return -1;
    }
    struct sockaddr_nl addr = {0};
    addr.nl_family = AF_NETLINK;
    struct iovec iov = { nlh, len };
    struct msghdr msg = {
        .msg_name = &addr,
        .msg_namelen = sizeof(addr),
        .msg_iov = &iov,
        .msg_iovlen = 1,
    };
    ssize_t ret = sendmsg(sock, &msg, 0);
    if (ret < 0) {
        perror("sendmsg");
        close(sock);
        return -1;
    }
    char buf[4096];
    iov.iov_base = buf;
    iov.iov_len = sizeof(buf);
    ret = recvmsg(sock, &msg, 0);
    if (ret < 0) {
        perror("recvmsg");
        close(sock);
        return -1;
    }
    struct nlmsghdr *hdr = (struct nlmsghdr *)buf;
    if (hdr->nlmsg_type == NLMSG_ERROR) {
        struct nlmsgerr *err = (struct nlmsgerr *)NLMSG_DATA(hdr);
        if (err->error != 0) {
            fprintf(stderr, "Netlink error: %s\n", strerror(-err->error));
            close(sock);
            return -1;
        }
    }
    close(sock);
    return 0;
}

int add_route(const char* dst_ip, const char* via_ip, const char* ifname, int metric) {
    struct {
        struct nlmsghdr nh;
        struct rtmsg rt;
        char buf[512];
    } req;

    memset(&req, 0, sizeof(req));
    req.nh.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
    req.nh.nlmsg_flags = NLM_F_REQUEST | NLM_F_CREATE | NLM_F_REPLACE;
    req.nh.nlmsg_type = RTM_NEWROUTE;

    req.rt.rtm_family = AF_INET;
    req.rt.rtm_table = RT_TABLE_MAIN;
    req.rt.rtm_protocol = RTPROT_BOOT;
    req.rt.rtm_scope = RT_SCOPE_UNIVERSE;
    req.rt.rtm_type = RTN_UNICAST;
    req.rt.rtm_dst_len = 32;

    struct in_addr dst, gw;
    if (inet_pton(AF_INET, dst_ip, &dst) != 1) {
        fprintf(stderr, "Invalid dst IP %s\n", dst_ip);
        return -1;
    }
    if (inet_pton(AF_INET, via_ip, &gw) != 1) {
        fprintf(stderr, "Invalid via IP %s\n", via_ip);
        return -1;
    }

    struct rtattr *rta = (struct rtattr *)(((char *)&req) + NLMSG_ALIGN(req.nh.nlmsg_len));
    rta->rta_type = RTA_DST;
    rta->rta_len = RTA_LENGTH(sizeof(dst));
    memcpy(RTA_DATA(rta), &dst, sizeof(dst));
    req.nh.nlmsg_len = NLMSG_ALIGN(req.nh.nlmsg_len) + RTA_LENGTH(sizeof(dst));

    rta = (struct rtattr *)(((char *)&req) + NLMSG_ALIGN(req.nh.nlmsg_len));
    rta->rta_type = RTA_GATEWAY;
    rta->rta_len = RTA_LENGTH(sizeof(gw));
    memcpy(RTA_DATA(rta), &gw, sizeof(gw));
    req.nh.nlmsg_len = NLMSG_ALIGN(req.nh.nlmsg_len) + RTA_LENGTH(sizeof(gw));

    int ifindex = if_nametoindex(ifname);
    if (ifindex == 0) {
        perror("if_nametoindex");
        return -1;
    }
    rta = (struct rtattr *)(((char *)&req) + NLMSG_ALIGN(req.nh.nlmsg_len));
    rta->rta_type = RTA_OIF;
    rta->rta_len = RTA_LENGTH(sizeof(int));
    memcpy(RTA_DATA(rta), &ifindex, sizeof(int));
    req.nh.nlmsg_len = NLMSG_ALIGN(req.nh.nlmsg_len) + RTA_LENGTH(sizeof(int));

    int prio = metric;
    rta = (struct rtattr *)(((char *)&req) + NLMSG_ALIGN(req.nh.nlmsg_len));
    rta->rta_type = RTA_PRIORITY;
    rta->rta_len = RTA_LENGTH(sizeof(int));
    memcpy(RTA_DATA(rta), &prio, sizeof(int));
    req.nh.nlmsg_len = NLMSG_ALIGN(req.nh.nlmsg_len) + RTA_LENGTH(sizeof(int));

    return netlink_sendmsg(&req.nh, req.nh.nlmsg_len);
}

int del_route(const char* dst_ip) {
    struct {
        struct nlmsghdr nh;
        struct rtmsg rt;
        char buf[256];
    } req;

    memset(&req, 0, sizeof(req));
    req.nh.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
    req.nh.nlmsg_flags = NLM_F_REQUEST;
    req.nh.nlmsg_type = RTM_DELROUTE;

    req.rt.rtm_family = AF_INET;
    req.rt.rtm_table = RT_TABLE_MAIN;
    req.rt.rtm_protocol = RTPROT_BOOT;
    req.rt.rtm_scope = RT_SCOPE_UNIVERSE;
    req.rt.rtm_type = RTN_UNICAST;
    req.rt.rtm_dst_len = 32;

    struct in_addr dst;
    if (inet_pton(AF_INET, dst_ip, &dst) != 1) {
        fprintf(stderr, "Invalid dst IP %s\n", dst_ip);
        return -1;
    }

    struct rtattr *rta = (struct rtattr *)(((char *)&req) + NLMSG_ALIGN(req.nh.nlmsg_len));
    rta->rta_type = RTA_DST;
    rta->rta_len = RTA_LENGTH(sizeof(dst));
    memcpy(RTA_DATA(rta), &dst, sizeof(dst));
    req.nh.nlmsg_len = NLMSG_ALIGN(req.nh.nlmsg_len) + RTA_LENGTH(sizeof(dst));

    return netlink_sendmsg(&req.nh, req.nh.nlmsg_len);
}