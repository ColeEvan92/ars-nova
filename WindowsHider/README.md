A highly questionable work with the sole purpose of keeping winapi identifier out of my global namespace. Entirely because of the one time `<numeric-limits>` broke when I included `<Windows.h>` and it macroed "min" and "max". Not something to ever be used for real.

Basically I just `#include "WindowsHider.h"` and then my main function is `int BleakMain(bleak::StartupData)`. I have to add any new identifiers I want to use to `WindowsHiderItems.h`. Like I said, not great, but I didn't make it to be great. Just as a pet peeve.
