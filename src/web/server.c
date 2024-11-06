#include <mongoose.h>
#include <string.h>
#include "bplus/tree.h"
#include "bplus/web.h"

static struct mg_serve_http_opts s_http_server_opts;
static BPlusTree* tree = NULL;

static void handle_insert(struct mg_connection *nc, struct http_message *hm) {
    char value[32];
    mg_get_http_var(&hm->body, "value", value, sizeof(value));
    
    int val = atoi(value);
    bool success = bplus_tree_insert(tree, val);
    
    char* json = bplus_tree_to_json(tree);
    
    mg_printf(nc, "HTTP/1.1 200 OK\r\n"
              "Content-Type: application/json\r\n"
              "Connection: close\r\n\r\n"
              "%s", json);
    
    free(json);
}
