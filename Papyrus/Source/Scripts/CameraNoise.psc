Scriptname CameraNoise

bool Function LoadINI(String asFilepath) global native
{Loads frequency & amplitude values from the given ini.

All file paths are evaluated relative to "Data/SKSE/Plugins/_CameraNoise/"}

bool Function UnloadINI(String asFilepath) global native
{Reverts frequency & amplitude value changes that were previously made from the given ini.}

bool Function ResetINI() global native
{Resets frequency & amplitude values to the default CameraNoise ini.}