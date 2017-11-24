#include <stdlib.h>
#include <ruby-2.3.0/ruby.h>
#include <lirc/lirc_client.h>

#define STRLEN_MAX 4096

static VALUE io;
static VALUE lirc_config_class;

static void log_with_puts(const char *message) {
	VALUE r_message = rb_str_new(message, strnlen(message, STRLEN_MAX));
	rb_funcall(io, rb_intern("puts"), 1, r_message);
}

static int lirc_config_check(char *s) {
	return 0;
}

static void lirc_config_mark(void *config) { }

static void lirc_config_free(void *config) {
	lirc_freeconfig(config);
}

static size_t lirc_code_size(const struct lirc_code *code) {
	size_t size = sizeof(struct lirc_code);

	if (code->remote != NULL)
		size += strnlen(code->remote, STRLEN_MAX);
	if (code->button != NULL)
		size += strnlen(code->button, STRLEN_MAX);

	return size;
}

static size_t lirc_list_size(const struct lirc_list *list) {
	size_t size = sizeof(struct lirc_list);

	if (list->string != NULL)
		size += strnlen(list->string, STRLEN_MAX);

	return size;
}

static size_t lirc_config_entry_size(const struct lirc_config_entry *entry) {
	struct lirc_code *code = entry->code;
	struct lirc_list *list = entry->config;
	size_t size = sizeof(struct lirc_config_entry);

	if (entry->prog != NULL)
		size += strnlen(entry->prog, STRLEN_MAX);
	if (entry->change_mode != NULL)
		size += strnlen(entry->change_mode, STRLEN_MAX);
	if (entry->mode != NULL)
		size += strnlen(entry->mode, STRLEN_MAX);

	while (code != NULL) {
		size += lirc_code_size(code);
		code = code->next;
	}

	while (list != NULL) {
		size += lirc_list_size(list);
		list = list->next;
	}

	return size;
}

static size_t lirc_config_size(const void *obj) {
	struct lirc_config *config = (struct lirc_config *)obj;
	struct lirc_config_entry *entry = config->first;
	size_t size = sizeof(struct lirc_config);

	if (config->lircrc_class != NULL)
		size += strnlen(config->lircrc_class, STRLEN_MAX);
	if (config->current_mode != NULL)
		size += strnlen(config->current_mode, STRLEN_MAX);

	while (entry != NULL) {
		size += lirc_config_entry_size(entry);
		entry = entry->next;
	}

	return size;
}

static struct rb_data_type_struct lirc_config_data_type = {
	"lirc_config",
	{ lirc_config_mark, lirc_config_free, lirc_config_size, {0,0} },
	0,
	NULL,
	RUBY_TYPED_FREE_IMMEDIATELY
};

static VALUE ruby_lirc_init(VALUE client, VALUE prog, VALUE verbose) {
	const char *c_prog = StringValuePtr(prog);
	int c_verbose = FIX2INT(verbose);
	log_with_puts("calling init");
	int fd = lirc_init(c_prog, c_verbose);

	if (fd < 0) {
		rb_raise(rb_eException, "%s", c_prog);
	}

	return INT2NUM(fd);
}

static VALUE ruby_lirc_readconfig(VALUE client, VALUE path) {
	char *c_path = StringValuePtr(path);
	struct lirc_config *config = NULL;

	if (lirc_readconfig(c_path, &config, lirc_config_check) < 0) {
		rb_raise(rb_eException, "%s", "Unable to read config object");
	}

	return TypedData_Wrap_Struct(lirc_config_class, &lirc_config_data_type, config);
}

static VALUE ruby_lirc_deinit(VALUE client) {
	lirc_deinit();
	return Qnil;
}

static VALUE ruby_lirc_code2char(VALUE client, VALUE config, VALUE code) {
	struct lirc_config *c_config;
	char *c_code = StringValuePtr(code);
	char *app_string;

	TypedData_Get_Struct(config, struct lirc_config, &lirc_config_data_type, c_config); 

	if (lirc_code2char(c_config, c_code, &app_string) != 0)
		rb_raise(rb_eException, "%s: %s", "Unable to translate code to string", c_code);
	else if (app_string == NULL)
		return Qnil;
	else
		return rb_str_new(app_string, strnlen(app_string, STRLEN_MAX));
}

static VALUE ruby_lirc_nextcode(VALUE client) {
	char *code;

	if (lirc_nextcode(&code) != 0)
		rb_raise(rb_eException, "%s", "Unable to read next code");

	return rb_str_new(code, strnlen(code, STRLEN_MAX));
}

void Init_ruby_lirc_client(void) {
	VALUE top_module = rb_define_module("RubyLirc");
	VALUE client_class = rb_define_class_under(top_module, "Client", rb_cObject);
	lirc_config_class = rb_define_class_under(client_class, "Config", rb_cObject);
	io = rb_define_class("IO", rb_cObject);

	rb_define_method(client_class, "init", ruby_lirc_init, 2);
	rb_define_method(client_class, "close", ruby_lirc_deinit, 0);
	rb_define_method(client_class, "read_config", ruby_lirc_readconfig, 1);
	rb_define_method(client_class, "translate", ruby_lirc_code2char, 2);
	rb_define_method(client_class, "next_code", ruby_lirc_nextcode, 0);
}
