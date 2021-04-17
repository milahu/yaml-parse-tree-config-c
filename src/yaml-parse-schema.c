#include <glib.h>
#include <glib/gprintf.h>
#include <stdio.h>
#include <stdlib.h>

#include <cyaml/cyaml.h>

// work in progress ... build fails

typedef struct {
  int debug;
  char **whitelist; int whitelist_count;
  char **blacklist; int blacklist_count;
} config_t;

// cyaml schema
// TODO why do we need to define the schema bottom-up?

static const cyaml_schema_value_t stringlist_entry = {
  CYAML_VALUE_STRING(CYAML_FLAG_POINTER, config_t, 0, CYAML_UNLIMITED),
};

static const cyaml_schema_field_t top_mapping_schema[] = {
  CYAML_FIELD_BOOL("debug", CYAML_FLAG_DEFAULT, config_t, debug),
  CYAML_FIELD_SEQUENCE("whitelist", CYAML_FLAG_POINTER, config_t, whitelist, &stringlist_entry, 0, CYAML_UNLIMITED),
  CYAML_FIELD_SEQUENCE("blacklist", CYAML_FLAG_POINTER, config_t, blacklist, &stringlist_entry, 0, CYAML_UNLIMITED),
  CYAML_FIELD_END
};

static const cyaml_schema_value_t top_schema = {
	CYAML_VALUE_MAPPING(CYAML_FLAG_POINTER, config_t, top_mapping_schema),
};

// cyaml config

static const cyaml_config_t cyaml_config = {
  .log_level = CYAML_LOG_WARNING, // Logging errors and warnings only.
  .log_fn = cyaml_log,            // Use the default logging function.
  .mem_fn = cyaml_mem,            // Use the default memory allocator.
};



int main ()
{
  //const gchar *HOME = g_getenv ("HOME");
  //printf("getenv HOME: %s\n", (HOME != NULL) ? HOME : "getenv('HOME') returned NULL");

  //const gchar *user_config_dir = g_get_user_config_dir();
  const gchar *user_config_dir = g_get_user_config_dir();

  gchar *user_config;
  user_config = g_strconcat (user_config_dir, "/spotify-adblock-linux/config.yaml", NULL);
  g_free((void *)user_config_dir); // (void *) to free const ptr

  gchar *system_config;
  system_config = g_strconcat (SYSCONFDIR, "/spotify-adblock-linux/config.yaml", NULL);

  char *config_list[4];
  config_list[0] = "spotify-adblock-linux.yaml";
  config_list[1] = user_config;
  config_list[2] = system_config;
  config_list[3] = NULL;

  char **config_file;
  config_t *config = NULL;
  //config = malloc(sizeof(config_t));
  for (config_file = config_list; *config_file != NULL; config_file++) {
    if (g_file_test (*config_file, G_FILE_TEST_EXISTS) == FALSE) {
      printf("config not found: %s\n", *config_file);
      continue;
    }
    printf("config found: %s\n", *config_file);
    //load_config(config, *config_file);

    // load config
    cyaml_err_t cyaml_status;
    cyaml_status = cyaml_load_file(
      *config_file, &cyaml_config, &top_schema, (cyaml_data_t **)&config, NULL);
    if (cyaml_status != CYAML_OK) {
      fprintf(stderr, "ERROR: %s\n", cyaml_strerror(cyaml_status));
      //return EXIT_FAILURE;
      return 1;
    }

    printf("config loaded\n");
    break;
  }

  // use config
  printf("config:\n");
  printf("```yaml\n");
  printf("debug: %s\n", (config->debug == TRUE ? "true" : "false"));
  printf("whitelist:\n");
  for (unsigned i = 0; i < config->whitelist_count; i++) {
    printf("  - %s\n", config->whitelist[i]);
  }
  printf("blacklist:\n");
  for (unsigned i = 0; i < config->blacklist_count; i++) {
    printf("  - %s\n", config->blacklist[i]);
  }
  printf("```\n");

  // free config
  //cyaml_free(&config, &top_schema, config, 0);
  cyaml_free(&cyaml_config, &top_schema, config, 0);

  printf("done\n");

  return 0;
}
