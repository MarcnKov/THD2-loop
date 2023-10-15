#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "../../Common/process_image.h"

int main()
{
	float c_array[50][50] = carraytoreplacehere; // array filled by tests/run_tests.sh

	int i,j;
	float intensity;
	float intensity_sum = 0;
	float cog_x, cog_y;
	float cog_x_ref = 24.5; // middle is 24.5 because we have: 0 to 24 pixels |middle| 25 to 49
	float cog_y_ref = 24.5;
	float delta_cog_x, delta_cog_y;
	float gain = 0.5;
	float max_delta = 3.;
	short loop_is_on = 1;
	unsigned long time_before, time_after;
	
	struct timeval tv;
	//gettimeofday(&tv, NULL);
	//time_before = 1000000 * tv.tv_sec + tv.tv_usec; // in micro

	i = 0;
	while (i < 50)
	{
	    j = 0;
	    while (j < 50)
	    {
		
		intensity = c_array[j][i];
		intensity_sum += intensity;
		cog_x += i * intensity;
		cog_y += j * intensity;

		j++;
	    }
	    i++;
	}
	
    cog_x /= intensity_sum;
    cog_y /= intensity_sum;

	delta_cog_x = cog_x_ref - cog_x;
	delta_cog_y = cog_y_ref - cog_y;

	//gettimeofday(&tv, NULL);
	//time_after = 1000000 * tv.tv_sec + tv.tv_usec; // in micro

	//printf("Time: %ld microsecs\n\n", time_after - time_before);

	//printf("COG reference (x,y) = (%.3lf, %.3lf)\n", cog_x_ref, cog_y_ref);
	//printf("COG detected (x,y) = (%.3lf, %.3lf)\n", cog_x, cog_y);
	//printf("Delta COG (x,y) = (%.3lf, %.3lf)\n", delta_cog_x, delta_cog_y);
	printf("%.3lf,%.3lf\n", delta_cog_x, delta_cog_y);
	delta_cog_x *= gain;
	delta_cog_y *= gain;
	//printf("Delta COG (x,y) with gain = (%.3lf, %.3lf)\n", delta_cog_x, delta_cog_y);
	//printf("(Note: here the first pixel is (0, 0) and the last one (49, 49) for a 50x50 image)\n");

	if (delta_cog_x > max_delta || delta_cog_y > max_delta) {
		loop_is_on = 0;
		//printf("\nSAFETY: BREAK THE LOOP BECAUSE DELTA (WITH GAIN) IS GREATER THAN %.3lf\n", max_delta);
	}

	return 0;
}
