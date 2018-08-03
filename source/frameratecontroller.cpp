#include "framerateController.h"

using namespace utils;

// Defining class specific static variables.
long long	FrameRateController::at_start = 0;
long long	FrameRateController::at_end = 0;
long long	FrameRateController::ticks_per_second = 0;
long long	FrameRateController::target_ticks_per_frame = 0;
int			FrameRateController::target_frame_rate = 0;
bool		FrameRateController::locked = false;
bool		FrameRateController::high_resolution_counter_supported = false;
int			FrameRateController::current_frame_rate = 0;
int			FrameRateController::target_time_per_frame;
DWORD		FrameRateController::t_start;
DWORD		FrameRateController::t_end;

