#include <kernel/input.h>
#include <kernel/mm.h>
#include <kernel/string.h>
#include <kernel/printf.h>
#include <kernel/vfs.h>
#include <kernel/pci.h>
#include <kernel/elf.h>

#define MAX_PATH_LEN	128
#define MAX_COMMAND_LEN	128
#define MAX_ARGS	8
#define MAX_ARG_LEN	64

struct cli_command {
	char* name;
	void (*handler)(int argc, char *argv[]);
	const char *description;
};

static char current_path[MAX_PATH_LEN] = "/initrd/";

static void cmd_ls(int argc, char* argv[]);
static void cmd_help(int argc, char* argv[]);
static void cmd_cd(int argc, char* argv[]);
static void cmd_cat(int argc, char* argv[]);
static void cmd_pci(int argc, char* argv[]); 

static struct cli_command commands[] = {
	{"LS", cmd_ls, "List directory contents"},
	{"HELP", cmd_help, "Show this help message"},
	{"CD", cmd_cd, "cd"},
	{"CAT", cmd_cat, "cat"},
	{"PCI", cmd_pci, "Show pci devices"},
	{NULL, NULL, NULL}
};

static void normalize_path(char *path) {
	char *components[MAX_PATH_LEN/2];
	int count = 0;
	char buffer[MAX_PATH_LEN];

	strncpy(buffer, path, sizeof(buffer) - 1);
	buffer[sizeof(buffer) - 1] = '\0';
	
	char *token = strtok(buffer, "/");
	while (token != NULL && count < MAX_PATH_LEN/2 - 1) {
		if (strcmp(token, ".") == 0) {
		} else if (strcmp(token, "..") == 0) {
			if (count > 0) count--;
		} else {
			components[count++] = token;
		}
		token = strtok(NULL, "/");
	}
	
	path[0] = '/';
	path[1] = '\0';
	
	for (int i = 0; i < count; i++) {
		if (strlen(path) + strlen(components[i]) + 2 > MAX_PATH_LEN) break;
		
		strcat(path, components[i]);
		strcat(path, "/");
	}
	
	if (count == 0) {
		path[1] = '\0';
	}
}

static void cmd_ls(int argc, char* argv[]) {
	char path[MAX_PATH_LEN];

	if (argc > 1) {
		if (argv[1][0] != '/')
			strncpy(path, current_path, MAX_PATH_LEN - 1);
		strncat(path, argv[1], MAX_PATH_LEN - 1);
	}
	else {
		strncpy(path, current_path, MAX_PATH_LEN - 1);
	}

	normalize_path(path);

	printf("list of %s:\n", path);
	vfs_list(path);
}

static void cmd_help(int argc, char* argv[]) {
	printf("AVAILABLE COMMANDS:\n");
	for (struct cli_command *cmd = commands; cmd->name; cmd++) {
		printf("%s - %s\n", cmd->name, cmd->description);
	}
}

static void cmd_cd(int argc, char* argv[]) {
	if (argc < 2) {
		strcpy(current_path, "/");
		return;
	}

	char *target = argv[1];
	char new_path[MAX_PATH_LEN];

	if (target[0] == '/') {
        	strncpy(new_path, target, MAX_PATH_LEN - 1);
	} else {
		strncpy(new_path, current_path, MAX_PATH_LEN - 1);
		strncat(new_path, target, MAX_PATH_LEN - strlen(new_path) - 1);
	}
	
	normalize_path(new_path);

	if (vfs_exists(new_path) != 0) {
		printf("Directory '%s' not found\n", new_path);
		return;
	}

	strncpy(current_path, new_path, MAX_PATH_LEN - 1);
	current_path[MAX_PATH_LEN - 1] = '\0';
}

static void cmd_cat(int argc, char* argv[]) {
	if (argc < 2) {
		printf("CAT [filename]\n");
		return;
	}

	char *target = argv[1];
	char new_path[MAX_PATH_LEN];

	if (target[0] == '/') {
        	strncpy(new_path, target, MAX_PATH_LEN - 1);
	} else {
		strncpy(new_path, current_path, MAX_PATH_LEN - 1);
		strncat(new_path, target, MAX_PATH_LEN - strlen(new_path) - 1);
	}
	
	normalize_path(new_path);

	if (vfs_exists(new_path) != 0) {
		printf("File '%s' not found\n", new_path);
		return;
	}

	int file_size = vfs_size(new_path);
	char* buffer = kmalloc(file_size);
	
	vfs_read(new_path, buffer, file_size);
	printf("FILE %s:\n%s\n", new_path, buffer);
	kfree(buffer);
}

static void cmd_pci(int argc, char* argv[]) {
	pci_print_list();
}

static void cli_try_elf(int argc, char* argv[]) {
	char *target = argv[0];
	char path[MAX_PATH_LEN];

	if (target[0] == '/') {
        	strncpy(path, target, MAX_PATH_LEN - 1);
	} else {
		strncpy(path, current_path, MAX_PATH_LEN - 1);
		strncat(path, target, MAX_PATH_LEN - strlen(path) - 1);
	}
	
	normalize_path(path);

	if (vfs_exists(path) == 0) {
		elf_execute(path);
	}
	else {
		printf("Unknown command: '%s'\n", argv[0]);
		printf("Type 'help' for available commands\n");
	}
}

static int cli_parse_args(char* input, char* argv[]) {
	int argc = 0;
	char* p = input;

	while (*p && argc < MAX_ARGS) {
		while (*p == ' ') p++;
		
		if (!*p) break;
		
		argv[argc++] = p;
		
		while (*p && *p != ' ') p++;
		
		if (*p) {
			*p++ = '\0';
		}
	}
	
	return argc;
}

void cli_run() {
	char* buffer = kmalloc(MAX_COMMAND_LEN);
	char* argv[MAX_ARGS];

	while (1) {
		memset(buffer, 0, MAX_COMMAND_LEN);
		printf("%s > ", current_path);

		size_t pos = 0;
		while (1) {
			if (!input_has_events()) continue;
			
			key_event e = input_pop_event();
			if (e.ascii == '\n') break;
			
			if (e.ascii >= 32 && e.ascii <= 126 && pos < MAX_COMMAND_LEN-1) {
				printf("%c", e.ascii);
				buffer[pos++] = e.ascii;
			}
		}
		printf("\n");

		if (!buffer[0]) continue;

		int argc = cli_parse_args(buffer, argv);	

		int found = 0;
		for (struct cli_command *cmd = commands; cmd->name; cmd++) {	
			if (stricmp(argv[0], cmd->name) == 0) {
				cmd->handler(argc, argv);
				found = 1;
				break;
			}
		}
		
		if (!found) {
			cli_try_elf(argc, argv);
		}
	}
	kfree(buffer);
}
