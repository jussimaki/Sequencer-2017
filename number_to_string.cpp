#include "number_to_string.h"

bool stringToDouble(std::string string, double *number)
{
	double sign = 1;


	bool pointRead = false;
	double factor = 1;

	*number = 0;

	for (unsigned int index = 0; index < string.size(); index++)
	{
		if (string[index] == '-' && index == 0)
			sign = -1;
		else if (string[index] <= '9' && string[index] >= '0')
		{
			if (!pointRead)
			{
				*number *= 10;
				*number += string[index] - '0';

			}
			else
			{
				factor *= 0.1;
				*number += (string[index] - '0')*factor;
			}
		}
		else if (string[index] == '.')
		{
			if (!pointRead) pointRead = true;
			else return false;
		}
		else return false;
	}

	*number *= sign;

	return true;
}


bool stringToInt(std::string string, int *number)
{
	double sign = 1;
		
	*number = 0;

	for (unsigned int index = 0; index < string.size(); index++)
	{
		if (string[index] == '-' && index == 0)
			sign = -1;
		else if (string[index] <= '9' && string[index] >= '0')
		{
			*number *= 10;
			*number += string[index] - '0';
		}
		else return false;
	}

	*number *= sign;

	return true;
}