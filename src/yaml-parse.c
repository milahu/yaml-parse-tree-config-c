#include <glib.h>
#include <glib/gprintf.h>
#include <stdio.h>
#include <stdlib.h>

#include <yaml.h>
#include "parse-yaml-to-glib-nary-tree.h"

typedef struct {
  int debug;
  char **whitelist;
  char **blacklist;
} config_t;

int load_config(char *file_path) {

    GNode *cfg = g_node_new(file_path); // file_path is the root node
    yaml_parser_t parser;

    FILE *source = fopen(file_path, "rb");
    yaml_parser_initialize(&parser);
    yaml_parser_set_input_file(&parser, source);

    parse_yaml_to_g_nary_tree(&parser, cfg);

    yaml_parser_delete(&parser);
    fclose(source);

    //printf("Results iteration:\n");
    //g_node_traverse(cfg, G_PRE_ORDER, G_TRAVERSE_ALL, -1, dump_g_nary_tree, NULL);

    char **whitelist_tail;
    char **blacklist_tail;

    int LIST_INIT_SIZE = 100;
    config_t *config;
    config = malloc(sizeof(config_t));
    config->debug = FALSE;
    config->whitelist = malloc(LIST_INIT_SIZE * sizeof(char **)); // TODO realloc to grow array https://linux.die.net/man/3/realloc
    whitelist_tail = config->whitelist;
    *whitelist_tail = NULL;
    config->blacklist = malloc(LIST_INIT_SIZE * sizeof(char **)); // TODO realloc to grow array https://linux.die.net/man/3/realloc
    blacklist_tail = config->blacklist;
    *blacklist_tail = NULL;

    void whitelist_foreach (GNode *val, gpointer data) {
      //printf("whitelist_foreach: val->data = %s\n", (char *) val->data);
      *whitelist_tail = (char *) val->data;
      whitelist_tail++; // TODO realloc on demand
      *whitelist_tail = NULL;
    }

    void blacklist_foreach (GNode *val, gpointer data) {
      //printf("blacklist_foreach: val->data = %s\n", (char *) val->data);
      *blacklist_tail = (char *) val->data;
      blacklist_tail++; // TODO realloc on demand
      *blacklist_tail = NULL;
    }

    void cfg_foreach (GNode *key, gpointer data) {
      //printf("cfg_foreach: key->data = %s\n", (char *) key->data);
      if (strcmp(key->data, "debug") == 0) {
        GNode *val;
        val = g_node_first_child(key);
        if (strcmp(val->data, "true") == 0) { // debug is normally off
          //printf("debug -> true\n");
          config->debug = TRUE;
        }
      }
      else if (strcmp(key->data, "whitelist") == 0) {
        g_node_children_foreach (key, G_TRAVERSE_ALL, whitelist_foreach, NULL); // data = NULL
      }
      else if (strcmp(key->data, "blacklist") == 0) {
        g_node_children_foreach (key, G_TRAVERSE_ALL, blacklist_foreach, NULL); // data = NULL
      }
    }

    g_node_children_foreach (cfg, G_TRAVERSE_ALL, cfg_foreach, NULL); // data = NULL

    // print tree
    g_node_traverse(cfg, G_PRE_ORDER, G_TRAVERSE_ALL, -1, dump_g_nary_tree, NULL);

    // print config
    printf("debug: %s\n", (config->debug == TRUE ? "true" : "false"));
    char **list_entry;
    for (list_entry = config->whitelist; *list_entry != NULL; list_entry++) {
      printf("whitelist entry: %s\n", *list_entry);
    }
    for (list_entry = config->blacklist; *list_entry != NULL; list_entry++) {
      printf("blacklist entry: %s\n", *list_entry);
    }

    g_node_destroy(cfg); // TODO move to program cleanup
    return TRUE;
}

int main ()
{
  const gchar *HOME = g_getenv ("HOME");
  printf("HOME = %s\n", (HOME != NULL) ? HOME : "getenv('HOME') returned NULL");

  //const gchar *user_config_dir = g_get_user_config_dir();
  gchar *user_config_dir = g_get_user_config_dir();

  gchar *user_config;
  user_config = g_strconcat (user_config_dir, "/spotify-adblock-linux/config.yaml", NULL);
  g_free(user_config_dir);

  gchar *system_config;
  system_config = g_strconcat (SYSCONFDIR, "/spotify-adblock-linux/config.yaml", NULL);

  char **config_list[4];
  config_list[0] = system_config;
  config_list[1] = user_config;
  config_list[2] = "src/config.yaml"; // debug
  config_list[3] = NULL;

  char **config_file;
  for (config_file = config_list; *config_file != NULL; config_file++) {
    if (g_file_test (*config_file, G_FILE_TEST_EXISTS) == FALSE) {
      printf("config not found %s\n", *config_file);
      continue;
    }
    printf("Loading config %s\n", *config_file);
    load_config(*config_file);
    printf("Success loading config %s\n", *config_file);
    break;
  }

  printf("done\n");

  return 0;
}
