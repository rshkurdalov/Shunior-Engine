#include "timer.h"
#include "os_api.h"

set<timer *> timer_storage;

timer::timer()
{
    state = timer_state::inactive;
}

timer::~timer()
{
    reset();
}

void timer::run()
{
    if(state == timer_state::active) return;
    else if(state == timer_state::inactive)
        trigger_time = now().value + period.value;
    else trigger_time = now().value + pause_hold.value;
    state = timer_state::active;
    timers()->insert(this);
    os_update_internal_timer();
}

void timer::pause()
{
    if(state != timer_state::active) return;
    pause_hold = trigger_time.value - now().value;
    state = timer_state::paused;
    timers()->remove(key<timer *>(this));
}

void timer::reset()
{
    if(state == timer_state::active)
        timers()->remove(key<timer *>(this));
    state = timer_state::inactive;
}

nanoseconds timer::remaining_time()
{
    if(state == timer_state::active)
        return trigger_time.value - now().value;
    else if(state == timer_state::paused)
        return pause_hold;
    else return 0;
}

set<timer *> *timers()
{
    return &timer_storage;
}
