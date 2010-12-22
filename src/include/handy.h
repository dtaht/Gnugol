extern char *jsonrequest(const char *url);
extern int strip_html(int len, char *htmlstr);

// Might want a different call here for utf-8

#define jsv(a) json_string_value(a)

// Mapping one language to another requires 
// lots of repetitive code and error checking
// Macros may be ugly, but they do (in this case)
// ensure that all errors are captured.

// These macros assume that the first object is named root

#define GETOBJ(obj,newobj) do { newobj = json_object_get(obj, #newobj); \
    if(!newobj) { \
        fprintf(stderr, "error: " #newobj " is not an object\n"); \
	json_decref(root); \
        return 1;    } } while(1==0)

#define GETARRAY(obj,newobj) do { newobj = json_object_get(obj, #newobj); \
    if(!json_is_array(newobj)) {				       \
        fprintf(stderr, "error: " #newobj " is not an array\n"); \
	json_decref(root); \
        return 1;    } } while(1==0)

#define GETARRAYIDX(obj,newobj,idx) do { newobj = json_array_get(obj, idx); \
    if(!json_is_object(newobj)) {				       \
      fprintf(stderr, "error: " #newobj " is not an object\n", idx + 1);       \
	json_decref(root); \
        return 1;    } } while(1==0)

#define GETSTRING(obj,newobj) do { newobj = json_object_get(obj, #newobj); \
    if(!json_is_string(newobj)) {				       \
        fprintf(stderr, "error: " #newobj " is not a string\n"); \
	json_decref(root); \
        return 1;    } } while(1==0)
