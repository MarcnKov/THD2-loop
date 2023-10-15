/*=============================================================================
  Copyright (C) 2012 - 2013 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        SynchronousGrab.cpp

  Description: The SynchronousGrab example will grab a single image
               synchronously and save it to a file using VimbaC.

-------------------------------------------------------------------------------

  THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF TITLE,
  NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR  PURPOSE ARE
  DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
  TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#ifdef WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

#include <VimbaC/Include/VimbaC.h>
#include "../../Common/PrintVimbaVersion.h"
#include "../../Common/DiscoverGigECameras.h"
#include <SynchronousGrab.h>

#include "../../../../Common/process_image.h"
#include "../../../../Common/msg_queue.h"
#include "../../../../Common/config.h"
#include "../../../../Common/dac.h"

//
// Starts Vimba API
// Opens the given camera, if no camera ID was provided, the first camera found will be used
// Sets the maximum possible Ethernet packet size
// Adjusts the image format
// Acquires one image synchronously
// Writes the image as bitmap to file
// Closes the camera in case of failure
//
// Parameters:
//  [in]    pCameraID           The ID of the camera to work on. Can be NULL.
//  [in]    pFileName           The path of the bitmap where the image is saved to
//
// Returns:
//  An API status code
//

VmbError_t SynchronousGrab( const char* pCameraID)
{
    /*Define Vimba variables*/
    VmbError_t          err                 = VmbStartup();     // Initialize the Vimba API
    VmbCameraInfo_t     *pCameras           = NULL;             // A list of camera details
    VmbUint32_t         nCount              = 0;                // Number of found cameras
    VmbUint32_t         nFoundCount         = 0;                // Change of found cameras
    const VmbUint32_t   nTimeout            = 10000000;            // Timeout for Grab 2000 <-- default
    VmbAccessMode_t     cameraAccessMode    = VmbAccessModeFull;// We open the camera with full access
    VmbHandle_t         cameraHandle        = NULL;             // A handle to our camera
    VmbBool_t           bIsCommandDone      = VmbBoolFalse;     // Has a command finished execution
    VmbFrame_t          frame;                                  // The frame we capture
    const char*         pPixelFormat        = NULL;             // The pixel format we use for acquisition
    VmbInt64_t          nPayloadSize        = 0;                // The size of one frame
    VmbFeaturePersistSettings_t settings;


    /*Define THD2 loop acquisition variables*/

    const char *file_name_save = "../../../../../../Camera_settings/mako_settings_save.xml";
    const char *file_name_load = "../../../../../../Camera_settings/mako_settings_load.xml";
    const char *file_name_acq  = "../../../../../../../Visualization/RX_TX_data/get_param.c";
    const char *file_name_gui  = "../../../../../../../Visualization/RX_TX_data/set_param.c";
    
    /* 
    const char *file_name_save = "VimbaC/Camera_settings/mako_settings_save.xml";
    const char *file_name_load = "VimbaC/Camera_settings/mako_settings_load.xml";
    const char *file_name_acq  = "Visualization/RX_TX_data/get_param.c";
    const char *file_name_gui  = "Visualization/RX_TX_data/set_param.c";
    */


    int rx_msgq_id;
    int tx_msgq_id;
    int tx_msgq_size;
    int frame_counter;
    int rx_msg_status;
    int tx_msg_status;
    int msgq_ctl_status;

    unsigned int buffer_size;
        
    double exposure_time;

    struct msqid_ds Msgq_stats;

    GUI_msg 	Gui_msg;
    ACQ_msg 	Acq_msg;
    
    COG_rx 	Cog_rx;
    COG_tx 	Cog_tx;
    DAC_tx 	Dac_tx;
    CAM_set	Cam_set;	//Camera	settings
    ACQ_set	Acq_set;	//Acquisition	settings

    VmbError_t set_param_err = VmbErrorSuccess;  


    exposure_time 					= 0;
    frame_counter					= 0;
    msgq_ctl_status					= 0;
    tx_msg_status					= 0;
    rx_msg_status 					= 0;
    rx_msgq_id						= 0;
    tx_msgq_id						= 0;
    tx_msgq_size					= 0;
    
    Cog_tx.cog_x.value					= 0;
    Cog_tx.cog_y.value 					= 0;
    Cog_rx.cog_x_ref.value				= 0;
    Cog_rx.cog_y_ref.value				= 0;
    Cog_rx.gain.value					= 0.5;
    Acq_set.data_tx_rate.value				= 1;
    Acq_set.loop_state					= ON;
    Cam_set.frame_w.value				= 40; 
    Cam_set.frame_h.value				= 40;
    Cam_set.offset_x.value				= MATRIX_W/2;
    Cam_set.offset_y.value				= MATRIX_H/2;
    Cam_set.expos_time.value				= 19;
    
    buffer_size						= Cam_set.frame_h.value*Cam_set.frame_w.value;

    Cog_tx.cog_x.assigned 				= false;
    Cog_tx.cog_y.assigned 				= false;
    Cam_set.frame_w.value				= true;
    Cam_set.frame_h.value				= true;

    //TO FINISH ASSIGNING THE VALUES
    
    Acq_msg.Acq_data.Acq_set.loop_state 		= OFF;
    Acq_msg.Acq_data.Acq_set.data_tx_rate.value 	= 0;
    Acq_msg.Acq_data.Cam_set.frame_w.value		= 0;
    Acq_msg.Acq_data.Cam_set.frame_h.value		= 0;
    Acq_msg.Acq_data.Cam_set.offset_x.value		= 0;
    Acq_msg.Acq_data.Cam_set.offset_y.value		= 0;
    Acq_msg.Acq_data.Cam_set.expos_time.value		= 0;
    Acq_msg.Acq_data.Cog_tx.cog_x.value			= 0;
    Acq_msg.Acq_data.Cog_tx.cog_y.value			= 0;    
    Acq_msg.Acq_data.Dac_tx.vltg_avg_x.value		= 0;
    Acq_msg.Acq_data.Dac_tx.vltg_avg_y.value		= 0;
    
    Acq_msg.Acq_data.Cam_set.frame_w.assigned		= false;
    Acq_msg.Acq_data.Cam_set.frame_h.assigned		= false;
    Acq_msg.Acq_data.Cam_set.offset_x.assigned		= false;
    Acq_msg.Acq_data.Cam_set.offset_y.assigned		= false;
    Acq_msg.Acq_data.Cam_set.expos_time.assigned 	= false;
    Acq_msg.Acq_data.Acq_set.data_tx_rate.assigned	= false;
    Acq_msg.Acq_data.Cog_tx.cog_x.assigned		= false;
    Acq_msg.Acq_data.Cog_tx.cog_y.assigned 		= false;
    Acq_msg.Acq_data.Dac_tx.vltg_avg_x.assigned 	= false;
    Acq_msg.Acq_data.Dac_tx.vltg_avg_y.assigned 	= false;
    
    rx_msgq_id = init_msg_queue(file_name_gui);
    if (rx_msgq_id == MSGQ_ERROR)
    {
	fprintf(stderr,"Could not initialize RX message queue.\n");
	return MSGQ_ERROR;
    }

    tx_msgq_id = init_msg_queue(file_name_acq);
    if (tx_msgq_id == MSGQ_ERROR) 
    {
	fprintf(stderr,"Could not initialize TX message queue.\n");
	return MSGQ_ERROR;
    }

    msgq_ctl_status = msgctl(tx_msgq_id, IPC_STAT, &Msgq_stats);
    if (msgq_ctl_status == MSGQ_SUCCESS)
    {
	tx_msgq_size = Msgq_stats.msg_qnum;
	if (tx_msgq_size != 0)
	{
	    fprintf(stderr,"Message queue buffer of id %d is not empty.\n", tx_msgq_id);
	    fprintf(stderr,"Please launch the visualisation program.\n");
	}
    }
    
    
    //PrintVimbaVersion();
    if ( VmbErrorSuccess == err )
    {
        // Is Vimba connected to a GigE transport layer?
        DiscoverGigECameras();
        
        // If no camera ID was provided use the first camera found
        if ( NULL == pCameraID )
        {
            // Get the amount of known cameras
            err = VmbCamerasList( NULL, 0, &nCount, sizeof *pCameras );
            if (    VmbErrorSuccess == err && 0 < nCount )
            {
                pCameras = (VmbCameraInfo_t*)malloc( nCount * sizeof( *pCameras ));
                if ( NULL != pCameras )
                {
                    // Actually query all static details of all known cameras without having to open the cameras
                    // If a new camera was connected since we queried the amount of cameras (nFoundCount > nCount) we can ignore that one
                    err = VmbCamerasList( pCameras, nCount, &nFoundCount, sizeof *pCameras );
                    if (    VmbErrorSuccess != err && VmbErrorMoreData != err )
                    {
                        printf( "Could not list cameras. Error code: %d\n", err );
                    }
                    else
                    {
                        // Use the first camera
                        if( nFoundCount != 0)
                        {
                            pCameraID = pCameras[0].cameraIdString;
                        }
                        else
                        {
                            pCameraID = NULL;
                            err = VmbErrorNotFound;
                            printf( "Camera lost.\n" );
                        }
                    }

                    free( pCameras );
                    pCameras = NULL;
                }
                else
                {
                    printf( "Could not allocate camera list.\n" );
                }
            }
            else
            {
                printf( "Could not list cameras or no cameras present. Error code: %d\n", err );
            }
        }
	if ( NULL != pCameraID )
	{
	    // Open camera
	    err = VmbCameraOpen( pCameraID, cameraAccessMode, &cameraHandle ); //Rename err1 <-- open camera
	    if ( VmbErrorSuccess == err )
            {	
		err = VmbCameraSettingsSave( cameraHandle, file_name_save, NULL, 0 );
		if( VmbErrorSuccess != err )
		{
		    printf( "Could not save feature values to given XML file '%s' [error code: %i]\n", file_name_save, err );
		    err = VmbCameraClose( cameraHandle );
		    if( VmbErrorSuccess != err )
		    {
			printf( "Could not close camera [error code: %i]\n", err );
		    }
		    VmbShutdown();
		    return err;
		}
		err = VmbFeatureCommandRun( cameraHandle, "UserSetLoad" );
		if( VmbErrorSuccess != err )
		{
		    printf( "Could not run 'UserSetLoad' command [error code: %i]\n", err );
		    err = VmbCameraClose( cameraHandle );
		    if( VmbErrorSuccess != err )
		    {
			printf( "Could not close camera [error code: %i]\n", err );
		    }
		    VmbShutdown();
		    return err;
		}
	     	err = VmbCameraSettingsLoad( cameraHandle, file_name_load, &settings, sizeof(settings) );
		if( VmbErrorSuccess != err )
		{
		    printf( "Could not load feature values from given XML file '%s' [error code: %i]\n", file_name_load, err );
		    err = VmbCameraClose( cameraHandle );
		    if( VmbErrorSuccess != err )
		    {
			printf( "Could not close camera [error code: %i]\n", err );
		    }
		    VmbShutdown();
		    return err;
		}
		// Set the GeV packet size to the highest possible value
                // (In this example we do not test whether this cam actually is a GigE cam)
		if ( VmbErrorSuccess == VmbFeatureCommandRun( cameraHandle, "GVSPAdjustPacketSize" ))
                {
                    do
                    {
			if ( VmbErrorSuccess != VmbFeatureCommandIsDone(    cameraHandle,
                                                                            "GVSPAdjustPacketSize",
                                                                            &bIsCommandDone ))
                        {
                            break;
                        }
                    } while ( VmbBoolFalse == bIsCommandDone );
                }
		
		err = VmbFeatureEnumSet( cameraHandle, "PixelFormat", "Mono8" );
		VmbFeatureEnumGet( cameraHandle, "PixelFormat", &pPixelFormat );
				
		while(1)
		{
		    
		    rx_msg_status = rx_gui_msg(&Gui_msg, rx_msgq_id);
		    if (rx_msg_status == MSGQ_SUCCESS)
		    {

			Acq_set		= Gui_msg.Gui_data.Acq_set;
			Cam_set		= Gui_msg.Gui_data.Cam_set;			
			Cog_rx		= Gui_msg.Gui_data.Cog_rx;

			exposure_time 	= Gui_msg.Gui_data.Cam_set.expos_time.value;

			set_param_err 	= VmbFeatureFloatSet(cameraHandle, "ExposureTimeAbs", exposure_time);
			if (set_param_err != VmbErrorSuccess)
			{
			    fprintf(stdout,"Exposure time is not set. Retry sending settings.\n");
			}
			
			set_param_err 	= VmbFeatureIntSet(cameraHandle, "OffsetX", Cam_set.offset_x.value);
			if (set_param_err != VmbErrorSuccess)
			{
			    fprintf(stdout,"OffsetX is not set. Retry sending settings.\n");
			}
			
			set_param_err 	= VmbFeatureIntSet(cameraHandle, "OffsetY", Cam_set.offset_y.value);
			if (set_param_err != VmbErrorSuccess)
			{
			    fprintf(stdout,"OffsetY is not set. Retry sending settings.\n");
			}
			/*	
			set_param_err 	= VmbFeatureIntSet(cameraHandle, "Width", Cam_set.frame_w.value);
			if (set_param_err != VmbErrorSuccess)
			{
			    fprintf(stdout,"Width is not set. Retry sending settings.\n");
			    Cam_set.frame_w.assigned = false;
			}
			
			set_param_err 	= VmbFeatureIntSet(cameraHandle, "Height", Cam_set.frame_h.value);
			if (set_param_err != VmbErrorSuccess)
			{
			    fprintf(stdout,"Height is not set. Retry sending settings.\n");
			    Cam_set.frame_h.assigned = false;
			}
			*/

			
		    }
		    /* 
		    if (Cam_set.frame_h.assigned == true && Cam_set.frame_w.assigned == true)
		    {
			//allocate memory
			buffer_size		= Cam_set.frame_h.value*Cam_set.frame_w.value;
			Acq_msg.Acq_data.buffer = (unsigned char *)malloc(buffer_size*sizeof(unsigned char));
			//Don't forget to free the memory, see lines 507
		    }
		    */

		    if ( VmbErrorSuccess == err ) //Rename err1 <-- Open camera
		    {
                        if ( VmbErrorSuccess == err )
                        {
			    // Evaluate frame size
                            err = VmbFeatureIntGet( cameraHandle, "PayloadSize", &nPayloadSize );
                            if ( VmbErrorSuccess == err )
                            {
                                frame.buffer        = (unsigned char*)malloc( (VmbUint32_t)nPayloadSize );
                                frame.bufferSize    = (VmbUint32_t)nPayloadSize;

                                // Announce Frame
                                err = VmbFrameAnnounce( cameraHandle, &frame, (VmbUint32_t)sizeof( VmbFrame_t ));
                                if ( VmbErrorSuccess == err )
                                {
                                    // Start Capture Engine
                                    err = VmbCaptureStart( cameraHandle );
                                    if ( VmbErrorSuccess == err )
                                    {
                                        // Queue Frame
                                        err = VmbCaptureFrameQueue( cameraHandle, &frame, NULL );
                                        if ( VmbErrorSuccess == err )
                                        {
                                            // Start Acquisition
                                            err = VmbFeatureCommandRun( cameraHandle,"AcquisitionStart" );
                                            if ( VmbErrorSuccess == err )
                                            {
                                                // Capture one frame synchronously
                                                err = VmbCaptureFrameWait( cameraHandle, &frame, nTimeout );
						if ( VmbErrorSuccess == err )
						{
						    if ( VmbFrameStatusComplete == frame.receiveStatus )
                                                    {
						 			
		    				    	if ( 0 == compute_COG( &frame, &Cog_rx, &Cog_tx))
		    				    	{
							    fprintf(stderr,"Could not compute COG of a frame.\n");
							}

							msgq_ctl_status = msgctl(tx_msgq_id, IPC_STAT, &Msgq_stats);
							if (msgq_ctl_status == MSGQ_SUCCESS)
						    	{
							    tx_msgq_size = Msgq_stats.msg_qnum;
							} 
							if (frame_counter == Acq_set.data_tx_rate.value && tx_msgq_size == 0)
							{
							    memcpy(Acq_msg.Acq_data.buffer, frame.buffer, BUFFER_SIZE);
							    //memcpy(Acq_msg.Acq_data.buffer, frame.buffer, buffer_size);
							    
							    //TO REPLACE IT BY THE CAM_SET
							    Acq_msg.Acq_data.Cam_set.frame_w.value 	= frame.width;
							    Acq_msg.Acq_data.Cam_set.frame_h.value 	= frame.height;
							    Acq_msg.Acq_data.Cam_set.offset_x.value	= frame.offsetX;
							    Acq_msg.Acq_data.Cam_set.offset_y.value	= frame.offsetY;
							    Acq_msg.Acq_data.Cog_tx 			= Cog_tx;
							    Acq_msg.Acq_data.Dac_tx			= Dac_tx;
							    Acq_msg.Acq_data.Acq_set			= Acq_set;

							    err = VmbFeatureFloatGet(cameraHandle, "ExposureTimeAbs", &exposure_time);
							    if (VmbErrorSuccess == err)
							    {
							        Acq_msg.Acq_data.Cam_set.expos_time.value = exposure_time;
							    }
							    
							    tx_msg_status = tx_acq_msg(&Acq_msg, tx_msgq_id);
							    if (tx_msg_status == MSGQ_ERROR)
							    {
							       printf("Message send failure.\n");
							    }
							    
							    frame_counter = 0;
							}
							else if (frame_counter >= Acq_set.data_tx_rate.value && tx_msgq_size == 1)
							{
							     frame_counter = 0;
							}	    
							
						    }
						    else
                                                    {
                                                        printf( "Frame not successfully received. Error code: %d\n", frame.receiveStatus );
                                                    }
                                                }
                                                else
                                                {
                                                    printf( "Could not capture frame. Error code: %d\n", err );
						}
						// Stop Acquisition
                                                err = VmbFeatureCommandRun( cameraHandle,"AcquisitionStop" );
                                                if ( VmbErrorSuccess != err )
                                                {
                                                    printf( "Could not stop acquisition. Error code: %d\n", err );
                                                }
                                            }
                                            else
                                            {
                                                printf( "Could not start acquisition. Error code: %d\n", err );
                                            }
                                        }
                                        else
                                        {
                                            printf( "Could not queue frame. Error code: %d\n", err );
                                        }

                                        // Stop Capture Engine
                                        err = VmbCaptureEnd( cameraHandle );
                                        if ( VmbErrorSuccess != err )
                                        {
                                            printf( "Could not end capture. Error code: %d\n", err );
                                        }
                                    }
                                    else
                                    {
                                        printf( "Could not start capture. Error code: %d\n", err );
                                    }

                                    // Revoke frame
                                    err = VmbFrameRevoke( cameraHandle, &frame );
                                    if ( VmbErrorSuccess != err )
                                    {
                                        printf( "Could not revoke frame. Error code: %d\n", err );
                                    }
                                }
                                else
                                {
                                    printf( "Could not announce frame. Error code: %d\n", err );
                                }
                                free( frame.buffer );
                                frame.buffer = NULL;
				//if developed dynamic frame change
				//Acq_msg.Acq_data.buffer should be freed here
				//free(Acq_msg.Acq_data.buffer); 
                            }
                        }
                        else
                        {
                            printf( "Could not set pixel format to either RGB or Mono. Error code: %d\n", err );
                        }
                    }
                    else
                    {
                        printf( "Could not adjust packet size. Error code: %d\n", err );
                    }
		    frame_counter++;
		}
				
		err = VmbCameraClose ( cameraHandle );
                if ( VmbErrorSuccess != err )
                {
		    printf( "Could not close camera. Error code: %d\n", err );
		}
	    
            }
            else
            {
                printf( "Could not open camera. Error code: %d\n", err );
            }
        }
	
        VmbShutdown();
    }
    else
    {
        printf( "Could not start system. Error code: %d\n", err );
    }
    return err;
}
