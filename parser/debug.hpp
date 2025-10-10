#ifndef DEBUG_HPP
#define DEBUG_HPP

// Uncomment to enable parser debug output
// #define DEBUG_PARSER 1

#ifdef DEBUG_PARSER
#define DBG(msg) (std::cerr << msg << std::endl)
#else
#define DBG(msg) ((void)0)
#endif

#endif
