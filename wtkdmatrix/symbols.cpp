/*
 * symbols.cpp
 *
 *  Created on: May 22, 2013
 *      Author: diegocaro
 */

#include <sys/types.h>
#include "symbols.h"

bool operator==(const usym& lhs, const usym& rhs)
{
    if (lhs.x == rhs.x && lhs.y == rhs.y) return true;
    else return false;
}

bool operator!=(const usym& lhs, const usym& rhs)
{
    if (lhs.x != rhs.x || lhs.y != rhs.y) return true;
    else return false;
}
