#include "process_image.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

unsigned char compute_COG(VmbFrame_t *frame, COG_rx *Cog_rx, COG_tx *Cog_tx)
{
    if (NULL != frame && NULL != frame->buffer)
    {
			
	int i;
	int j;
	
	float cog_x;
	float cog_y;
   	float cog_x_ref;
	float cog_y_ref;
	float delta_cog_x;
	float delta_cog_y;
	
	double intensity;
   	double intensity_sum;
   	
	cog_x 		= 0;
	cog_y 		= 0;
	delta_cog_x	= 0;
	delta_cog_y	= 0;
	intensity 	= 0;
	intensity_sum 	= 0;
	
	cog_x_ref = Cog_rx->cog_x_ref.value; 
	if (Cog_rx->cog_x_ref.assigned == false)
	{
	    cog_x_ref	= frame->height/2;
	}
	
	cog_y_ref = Cog_rx->cog_y_ref.value; 
	if (Cog_rx->cog_y_ref.assigned == false)
	{
	    cog_y_ref	= frame->width/2;
	}
	
	//TO DO: CHECK FOR THE OFFSET ERROR
	i = 0;
	while (i < frame->height)
	{
	    j = 0;
	    
	    while (j < frame->width)
	    {
		
		intensity = get_pixel(frame,i,j);
		intensity_sum += intensity;
		cog_x += i * intensity;
		cog_y += j * intensity;

		j++;
	    }
	    i++;
	}
	
        cog_x /= intensity_sum;
        cog_y /= intensity_sum;
	
	Cog_tx->cog_x.value = cog_x;
	Cog_tx->cog_y.value = cog_y;

	Cog_tx->cog_x.assigned = true;
	Cog_tx->cog_y.assigned = true;
	
	
	//TO DO: To convert delta_cog values to voltages
	/*
	delta_cog_x = cog_x_ref - cog_x;
	delta_cog_y = cog_y_ref - cog_y;
	printf("Delta COG (x,y) = (%.3lf, %.3lf)\n",delta_cog_x,delta_cog_y);
	delta_cog_x *= cog_rx->gain.value; 
	delta_cog_y *= cog_rx->gain.value;
	printf("Delta COG (x,y)*gain = (%.3lf, %.3lf)\n",delta_cog_x,delta_cog_y);
	*/

	return 1;
    }
    return 0;
}

/*TO DO: FIX get_pixel offset
 *TO DO: Modify output type
 * */

int get_pixel(VmbFrame_t *frame, int x, int y)
{
    unsigned char *buffer = (unsigned char *) frame->buffer; 
    return (int) buffer[x*frame->width + y];
}
