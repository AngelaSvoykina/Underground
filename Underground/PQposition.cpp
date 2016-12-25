#include "PQposition.h"

bool operator>(const PQElement& l, const PQElement& r)
{
	return l.priority > r.priority;
}
