#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <json-c/json.h>

#define MAX_COMMAND_LEN 2048
#define MAX_PATH_LEN 512
#define MAX_APPS 10
#define CACHE_DIR "/data/data/com.termux/files/home/.notification_cache"
#define POLL_INTERVAL 5

typedef struct {
    char package_name[128];
    char cache_name[64];
    char command_template[512];
} AppConfig;

// configurations
AppConfig app_configs[] = {
    {"com.discord", "discord", "./notification '{TITLE}' '{CONTENT}' images/discord.png"},
    {"com.spotify.music", "spotify", "./notification 'Artist: {CONTENT}' 'Song: {TITLE}' images/spotify.png"},
    // add more apps here if needed

};
int num_apps = 2;

// if it doesn't exist
int create_cache_dir() {
    struct stat st = {0};
    if (stat(CACHE_DIR, &st) == -1) {
        if (mkdir(CACHE_DIR, 0700) != 0) {
            perror("Failed to create cache directory");
            return -1;
        }
    }
    return 0;
}

// escape single quotes in a string
char* escape_string(const char* input) {
    if (!input) return strdup("");
    
    size_t len = strlen(input);
    char* escaped = malloc(len * 4 + 1); // Worst case: every char needs escaping
    if (!escaped) return NULL;
    
    size_t j = 0;
    for (size_t i = 0; i < len; i++) {
        if (input[i] == '\'') {
            escaped[j++] = '\'';
            escaped[j++] = '\\';
            escaped[j++] = '\'';
            escaped[j++] = '\'';
        } else {
            escaped[j++] = input[i];
        }
    }
    escaped[j] = '\0';
    return escaped;
}

char* replace_placeholder(const char* template, const char* placeholder, const char* value) {
    if (!template || !placeholder || !value) return strdup(template ? template : "");
    
    char* result = malloc(MAX_COMMAND_LEN);
    if (!result) return NULL;
    
    const char* pos = template;
    char* dest = result;
    size_t placeholder_len = strlen(placeholder);
    size_t value_len = strlen(value);
    
    while (*pos) {
        if (strncmp(pos, placeholder, placeholder_len) == 0) {
            strcpy(dest, value);
            dest += value_len;
            pos += placeholder_len;
        } else {
            *dest++ = *pos++;
        }
    }
    *dest = '\0';
    return result;
}

// read cache file
char* read_cache_file(const char* cache_file) {
    FILE* fp = fopen(cache_file, "r");
    if (!fp) return NULL;
    
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    if (size <= 0) {
        fclose(fp);
        return strdup("");
    }
    
    char* content = malloc(size + 1);
    if (!content) {
        fclose(fp);
        return NULL;
    }
    
    fread(content, 1, size, fp);
    content[size] = '\0';
    fclose(fp);
    return content;
}

// write cache file
int write_cache_file(const char* cache_file, const char* content) {
    FILE* fp = fopen(cache_file, "w");
    if (!fp) {
        perror("Failed to write cache file");
        return -1;
    }
    fprintf(fp, "%s", content);
    fclose(fp);
    return 0;
}

// execute command and get output
char* execute_command(const char* cmd) {
    FILE* pipe = popen(cmd, "r");
    if (!pipe) {
        perror("Failed to execute command");
        return NULL;
    }
    
    char* output = malloc(65536); // 64KB buffer
    if (!output) {
        pclose(pipe);
        return NULL;
    }
    
    size_t total = 0;
    size_t bytes_read;
    char buffer[4096];
    
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), pipe)) > 0) {
        if (total + bytes_read < 65536) {
            memcpy(output + total, buffer, bytes_read);
            total += bytes_read;
        }
    }
    output[total] = '\0';
    pclose(pipe);
    return output;
}

// process notifications for a specific app
void process_app_notification(AppConfig* config) {
    char cache_file[MAX_PATH_LEN];
    snprintf(cache_file, sizeof(cache_file), "%s/.previous_%s_notification", 
             CACHE_DIR, config->cache_name);
    
    // Get notifications
    char* notifications_json = execute_command("termux-notification-list");
    if (!notifications_json || strlen(notifications_json) == 0) {
        free(notifications_json);
        return;
    }
    
    // Parse JSON
    struct json_object* root = json_tokener_parse(notifications_json);
    if (!root) {
        fprintf(stderr, "Failed to parse JSON\n");
        free(notifications_json);
        return;
    }
    
  
    struct json_object* notification = NULL;
    size_t n_notifications = json_object_array_length(root);
    
    for (size_t i = 0; i < n_notifications; i++) {
        struct json_object* notif = json_object_array_get_idx(root, i);
        struct json_object* package_obj;
        
        if (json_object_object_get_ex(notif, "packageName", &package_obj)) {
            const char* package_name = json_object_get_string(package_obj);
            if (strcmp(package_name, config->package_name) == 0) {
                notification = notif;
                break;
            }
        }
    }
    
    if (notification) {
      
        const char* notif_str = json_object_to_json_string(notification);
        
        // read previous cache
        char* cached = read_cache_file(cache_file);
        
        // check if notification is new
        if (!cached || strcmp(notif_str, cached) != 0) {
            // extract title and content
            struct json_object* title_obj, *content_obj;
            const char* title = "";
            const char* content = "";
            
            if (json_object_object_get_ex(notification, "title", &title_obj)) {
                title = json_object_get_string(title_obj);
            }
            if (json_object_object_get_ex(notification, "content", &content_obj)) {
                content = json_object_get_string(content_obj);
            }
            
            // escape strings
            char* escaped_title = escape_string(title);
            char* escaped_content = escape_string(content);
            
            // build command
            char* cmd = strdup(config->command_template);
            char* temp;
            
            temp = replace_placeholder(cmd, "{TITLE}", escaped_title);
            free(cmd);
            cmd = temp;
            
            temp = replace_placeholder(cmd, "{CONTENT}", escaped_content);
            free(cmd);
            cmd = temp;
            
            temp = replace_placeholder(cmd, "{PACKAGE}", config->package_name);
            free(cmd);
            cmd = temp;
            
            // execute notification command
            printf("Executing: %s\n", cmd);
            system(cmd);
            
            // update cache
            write_cache_file(cache_file, notif_str);
            
            // cleanup
            free(cmd);
            free(escaped_title);
            free(escaped_content);
        }
        
        free(cached);
    }
    
    json_object_put(root);
    free(notifications_json);
}

int main() {
    printf("Starting notification daemon...\n");
    
    // create cache directory
    if (create_cache_dir() != 0) {
        fprintf(stderr, "Failed to create cache directory\n");
        return 1;
    }
    
    // main loop
    while (1) {
        for (int i = 0; i < num_apps; i++) {
            process_app_notification(&app_configs[i]);
        }
        
        sleep(POLL_INTERVAL);
    }
    
    return 0;
}
