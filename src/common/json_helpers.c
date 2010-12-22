#include <jansson.h>

json_t *array, *integer;

array = json_array();
integer = json_integer(42);

json_array_append(array, integer);
json_decref(integer);
Note how the caller has to release the reference to the integer value by calling json_decref(). By using a reference stealing function json_array_append_new() instead of json_array_append(), the code becomes much simpler:

json_t *array = json_array();
json_array_append_new(array, json_integer(42));

int json_object_set(json_t *object, const char *key, json_t *value)¶
    Set the value of key to value in object. key must be a valid null terminated UTF-8 encoded Unicode string. If there already is a value for key, it is replaced by the new value. Returns 0 on success and -1 on error.
