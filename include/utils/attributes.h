#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

#define __noreturn__                [[noreturn]]
#define __nodiscard__               [[nodiscard]]
#define __fallthrough__             [[fallthrough]]
#define __packed__                  [[packed]]
#define __align__(x)                [[aligned(x)]]
#define __inline__                  [[always_inline]]
#define __nonnull__(x)              [[gnu::nonnull(x)]]
#define __unsequenced__             [[unsequenced]]
#define __reproduclible__           [[reproducible]]
#define __unused__                  [[gnu::unused]]

#endif // ATTRIBUTES_H
