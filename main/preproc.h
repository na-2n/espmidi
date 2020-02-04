#define _GET_ARG2(_1, _2, ...) _2
#define _GET_ARG3(_1, _2, _3, ...) _3
#define _GET_ARG4(_1, _2, _3, _4, ...) _4
#define _GET_ARG5(_1, _2, _3, _4, _5, ...) _5

#define CONST_RETURN(x) const { return x; }
