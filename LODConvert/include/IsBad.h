#pragma once

#ifndef __F4ISBAD__
#define __F4ISBAD__

bool F4IsBadReadPtr(const void* lp, unsigned int ucb);
bool F4IsBadCodePtr(void* lpfn);
bool F4IsBadWritePtr(void* lp, unsigned int ucb);

#endif