#include <glib.h>
#include <glib/gprintf.h>
#include <stdio.h>
#include <stdlib.h>

#include <cyaml/cyaml.h>

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



// config_file will be set to path of the loaded file, or NULL if no file was loaded
// config will be set to a config struct
// workaround for no default values in libcyaml:
// if config_file is NULL, we must manually free all alloc'ed memory
// if config_file != NULL, we can use cyaml_free to free memory

int load_config (char *name, config_t **config, char **config_file) {

  //config_t *config = NULL;
  //char *config_file = NULL;

  //const gchar *HOME = g_getenv ("HOME");
  //printf("getenv HOME: %s\n", (HOME != NULL) ? HOME : "getenv('HOME') returned NULL");

  gchar *workdir_config;
  workdir_config = g_strconcat (name, ".yaml", NULL);

  gchar *user_config;
  const gchar *user_config_dir = g_get_user_config_dir();
  user_config = g_strconcat (user_config_dir, "/", name, "/config.yaml", NULL);
  g_free((void *) user_config_dir); // (void *) to free const ptr

  gchar *system_config;
  // SYSCONFDIR is defined in Makefile (usually "/etc")
  system_config = g_strconcat (SYSCONFDIR, "/", name, "/config.yaml", NULL);

  char *config_list[4];
  config_list[0] = workdir_config;
  config_list[1] = user_config;
  config_list[2] = system_config;
  config_list[3] = NULL;
  // ideally: memcpy() file_cur to config_file and free() all those strings before return

  char **file_cur;
  for (file_cur = config_list; *file_cur != NULL; file_cur++) {
    if (g_file_test (*file_cur, G_FILE_TEST_EXISTS) == FALSE) {
      printf("config not found: %s\n", *file_cur);
      continue;
    }
    printf("config found: %s\n", *file_cur);

    // load config
    cyaml_err_t cyaml_status;
    cyaml_status = cyaml_load_file(
      //*file_cur, &cyaml_config, &top_schema, (cyaml_data_t **) &config, NULL);
      *file_cur, &cyaml_config, &top_schema, (cyaml_data_t **) config, NULL);
    if (cyaml_status != CYAML_OK) {
      fprintf(stderr, "ERROR: %s\n", cyaml_strerror(cyaml_status));
      //return EXIT_FAILURE;
      return 1;
    }

    printf("config loaded\n");
    break;
  }

  printf("tried all config files. *file_cur = %p, *config = %p\n", *file_cur, (void *) *config);

  *config_file = *file_cur; // TODO verify

  if (*file_cur == NULL) {
    // no config found
    // DEVEL for testing, comment line 45 in Makefile (install ... config.yaml)
    printf("no config file was found. fallback to default config\n");

    // set default config
    // workaround. libcyaml not yet supports default values
    // https://github.com/tlsa/libcyaml/issues/96

    *config = malloc(sizeof(config_t));

    (*config)->debug = TRUE;

    int blacklist_size = 2;
    char *blacklist[] = { "bl fallback 1", "bl fallback 2", NULL };
    (*config)->blacklist_count = blacklist_size;
    (*config)->blacklist = malloc((blacklist_size + 1) * sizeof(char *));
    for (int i = 0; i < blacklist_size; i++) {
      (*config)->blacklist[i] = (char *) blacklist[i];
    }

    int whitelist_size = 2;
    char *whitelist[] = { "wl fallback 1", "wl fallback 2", NULL };
    (*config)->whitelist_count = whitelist_size;
    (*config)->whitelist = malloc((whitelist_size + 1) * sizeof(char *));
    for (int i = 0; i < whitelist_size; i++) {
      (*config)->whitelist[i] = (char *) whitelist[i];
    }

    // write default config to ~/.config/${name}/config.yaml (user_config)
    // TODO mkdir -p ~/.config/${name}
    g_mkdir_with_parents (g_strconcat (user_config_dir, "/", name, NULL), 0755);
    FILE *fd_user_config = fopen(user_config, "w");
    if (fd_user_config == NULL) {
      printf("failed to open file: %s\n", user_config);
    }
    else {
      fprintf(fd_user_config, "debug: %s\n", ((*config)->debug == TRUE ? "true" : "false"));
      fprintf(fd_user_config, "whitelist:\n");
      for (unsigned i = 0; i < (*config)->whitelist_count; i++) {
        fprintf(fd_user_config, "  - %s\n", (*config)->whitelist[i]);
      }
      fprintf(fd_user_config, "blacklist:\n");
      for (unsigned i = 0; i < (*config)->blacklist_count; i++) {
        fprintf(fd_user_config, "  - %s\n", (*config)->blacklist[i]);
      }
      fclose(fd_user_config);
      printf("saved default config to: %s\n", user_config);
    }
  }
  return EXIT_SUCCESS;
}

int main (int argc, char **argv) {

  config_t *config = NULL;
  char *config_file = NULL; // null: no config file was loaded

  printf("load_config ...\n");

  load_config("spotify-adblock-linux", &config, &config_file);

  printf("load_config done\n");
  printf("config_file = %s\n", config_file);

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
  if (config_file != NULL) {
    // config was loaded by cyaml
    cyaml_free(&cyaml_config, &top_schema, config, 0);
  }
  else {
    // config was set manually
    free(config->whitelist);
    free(config->blacklist);
    free(config);
  }

  printf("done\n");

  return 0;
}
