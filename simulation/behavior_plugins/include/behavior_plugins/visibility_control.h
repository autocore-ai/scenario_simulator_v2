#ifndef BEHAVIOR_PLUGINS__VISIBILITY_CONTROL_H_
#define BEHAVIOR_PLUGINS__VISIBILITY_CONTROL_H_

// This logic was borrowed (then namespaced) from the examples on the gcc wiki:
//     https://gcc.gnu.org/wiki/Visibility

#if defined _WIN32 || defined __CYGWIN__
  #ifdef __GNUC__
    #define BEHAVIOR_PLUGINS_EXPORT __attribute__ ((dllexport))
    #define BEHAVIOR_PLUGINS_IMPORT __attribute__ ((dllimport))
  #else
    #define BEHAVIOR_PLUGINS_EXPORT __declspec(dllexport)
    #define BEHAVIOR_PLUGINS_IMPORT __declspec(dllimport)
  #endif
  #ifdef BEHAVIOR_PLUGINS_BUILDING_LIBRARY
    #define BEHAVIOR_PLUGINS_PUBLIC BEHAVIOR_PLUGINS_EXPORT
  #else
    #define BEHAVIOR_PLUGINS_PUBLIC BEHAVIOR_PLUGINS_IMPORT
  #endif
  #define BEHAVIOR_PLUGINS_PUBLIC_TYPE BEHAVIOR_PLUGINS_PUBLIC
  #define BEHAVIOR_PLUGINS_LOCAL
#else
  #define BEHAVIOR_PLUGINS_EXPORT __attribute__ ((visibility("default")))
  #define BEHAVIOR_PLUGINS_IMPORT
  #if __GNUC__ >= 4
    #define BEHAVIOR_PLUGINS_PUBLIC __attribute__ ((visibility("default")))
    #define BEHAVIOR_PLUGINS_LOCAL  __attribute__ ((visibility("hidden")))
  #else
    #define BEHAVIOR_PLUGINS_PUBLIC
    #define BEHAVIOR_PLUGINS_LOCAL
  #endif
  #define BEHAVIOR_PLUGINS_PUBLIC_TYPE
#endif

#endif  // BEHAVIOR_PLUGINS__VISIBILITY_CONTROL_H_
