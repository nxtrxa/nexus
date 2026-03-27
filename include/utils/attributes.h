#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

#define __noreturn__                [[noreturn]]
#define __nodiscard__               [[nodiscard]]
#define __fallthrough__             [[fallthrough]]
#define __align__(x)                [[aligned(x)]]
#define __unsequenced__             [[unsequenced]]
#define __reproduclible__           [[reproducible]]

#define __inline__                  [[gnu::always_inline]] inline
#define __packed__                  [[gnu::packed]]
#define __unused__                  [[gnu::unused]]
#define __nonnull__(x)              [[gnu::nonnull(x)]]

#endif // ATTRIBUTES_H
