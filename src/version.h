/*
 * Software versioning numbers for the main application.
 *
 * The format is X.Y.Z, which correspond to major.minor.patch.
 * A major number increments when significant change has been introduced.
 * The minor number increments when smaller changes or features are added, and
 * the patch version number increments when small changes are made to released
 * software for bugfixes or improvements.
 */

#define VERSION             100004

#define VERSION_MAJOR       VERSION / 100000
#define VERSION_MINOR       VERSION / 100 % 1000
#define VERSION_PATCH       VERSION % 100
