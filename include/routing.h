#ifndef ROUTING_H
#define ROUTING_H

int add_route(const char* dst_ip, const char* via_ip, const char* ifname, int metric);
int del_route(const char* dst_ip);

#endif