#include    "../include/sync.h"

extern "C" void init_sync()
{
    g_waitables_p = new Vector<Waitable*>();
}