/*
 * grid_portback.c
 *
 * Created: 4/12/2019 5:27:13 PM
 * Author : SUKU WC
*/

#include "grid_portback.h"

// PORTS







void grid_port_receive_decode(struct grid_port* por, uint16_t startcommand, uint16_t len){
	

	uint8_t error_flag = 0;
	uint8_t checksum_calculated = 0;
	uint8_t checksum_received = 0;
	
	// Copy data from cyrcular buffer to temporary linear array;
	uint8_t* message;
	
	uint16_t length = len;
	uint8_t buffer[length+1];

	
	// Store message in temporary buffer (MAXMSGLEN = 250 character)
	for (uint16_t i = 0; i<length; i++){
		buffer[i] = por->rx_double_buffer[(por->rx_double_buffer_read_start_index + i)%GRID_DOUBLE_BUFFER_RX_SIZE];
		por->rx_double_buffer[(por->rx_double_buffer_read_start_index + i)%GRID_DOUBLE_BUFFER_RX_SIZE]=0;
	}
	buffer[length] = 0;
	
	message = &buffer[0];
	
	// Clear data from rx double buffer
	for (uint16_t i = 0; i<length; i++){
		por->rx_double_buffer[(por->rx_double_buffer_read_start_index + i)%GRID_DOUBLE_BUFFER_RX_SIZE] = 0;
	}
	
	uint32_t readstartindex = por->rx_double_buffer_read_start_index;
	
	por->rx_double_buffer_read_start_index = (por->rx_double_buffer_read_start_index + length)%GRID_DOUBLE_BUFFER_RX_SIZE;
	por->rx_double_buffer_seek_start_index =  por->rx_double_buffer_read_start_index;
	
	por->rx_double_buffer_status = 0;
	
	// Correct the incorrect frame start location
	for (uint16_t i = 1; i<length; i++){
		
		if (buffer[i] == GRID_CONST_SOH){

			if (i>0){

				for (uint16_t j=0; j<length; j++){
					printf("%d, ", buffer[j]);
				}
				printf("\r\n");
				grid_port_debug_printf("Frame Start Offset %d %d %d", buffer[0], buffer[1], i);
			}
			
			length -= i;
			message = &buffer[i];
		
		}

		if (buffer[i] == '\n' && i<length-1){

			grid_port_debug_printf("Frame End Offset");
			length = i;
			break;
		}
		
	}
	
	

	
	
	
	
	// frame validator
	if (message[0] == GRID_CONST_SOH && message[length-1] == GRID_CONST_LF){
		
				
		
		checksum_received = grid_msg_string_checksum_read(message, length);
		
		checksum_calculated = grid_msg_string_calculate_checksum_of_packet_string(message, length);
		
		// checksum validator
		if (checksum_calculated == checksum_received && error_flag == 0){
					
			if (message[1] == GRID_CONST_BRC){ // Broadcast message
				
				uint8_t error=0;
				
				// Read the received id age values
				uint8_t received_id  = grid_msg_string_get_parameter(message, GRID_BRC_ID_offset, GRID_BRC_ID_length, &error);
				uint8_t received_session = grid_msg_string_get_parameter(message, GRID_BRC_SESSION_offset, GRID_BRC_SESSION_length, &error);
				uint8_t received_msgage = grid_msg_string_get_parameter(message, GRID_BRC_MSGAGE_offset, GRID_BRC_MSGAGE_length, &error);
				
				// Read the received destination X Y values (SIGNED INT)
				int8_t received_dx  = grid_msg_string_get_parameter(message, GRID_BRC_DX_offset, GRID_BRC_DX_length, &error) - GRID_PARAMETER_DEFAULT_POSITION;
				int8_t received_dy  = grid_msg_string_get_parameter(message, GRID_BRC_DY_offset, GRID_BRC_DY_length, &error) - GRID_PARAMETER_DEFAULT_POSITION;
				
				// Read the received source X Y values (SIGNED INT)
				int8_t received_sx  = grid_msg_string_get_parameter(message, GRID_BRC_SX_offset, GRID_BRC_SX_length, &error) - GRID_PARAMETER_DEFAULT_POSITION;
				int8_t received_sy  = grid_msg_string_get_parameter(message, GRID_BRC_SY_offset, GRID_BRC_SY_length, &error) - GRID_PARAMETER_DEFAULT_POSITION;
				
				uint8_t received_rot = grid_msg_string_get_parameter(message, GRID_BRC_ROT_offset, GRID_BRC_ROT_length, &error);
				

				// DO THE DX DY AGE calculations
				
				
				int8_t rotated_dx = 0;
				int8_t rotated_dy = 0;

				int8_t rotated_sx = 0;
				int8_t rotated_sy = 0;
				
				uint8_t updated_rot = (received_rot + por->partner_fi)%4;

				// APPLY THE 2D ROTATION MATRIX
				
				if (por->partner_fi == 0){ // 0 deg
					rotated_dx  += received_dx;
					rotated_dy  += received_dy;

					rotated_sx  += received_sx;
					rotated_sy  += received_sy;
				}
				else if(por->partner_fi == 1){ // 90 deg
					rotated_dx  -= received_dy;
					rotated_dy  += received_dx;

					rotated_sx  -= received_sy;
					rotated_sy  += received_sx;
				}
				else if(por->partner_fi == 2){ // 180 deg
					rotated_dx  -= received_dx;
					rotated_dy  -= received_dy;

					rotated_sx  -= received_sx;
					rotated_sy  -= received_sy;
				}
				else if(por->partner_fi == 3){ // 270 deg
					rotated_dx  += received_dy;
					rotated_dy  -= received_dx;

					rotated_sx  += received_sy;
					rotated_sy  -= received_sx;
				}
				else{
					// TRAP INVALID MESSAGE
				}
				
				uint8_t updated_dx = rotated_dx + GRID_PARAMETER_DEFAULT_POSITION + por->dx;
				uint8_t updated_dy = rotated_dy + GRID_PARAMETER_DEFAULT_POSITION + por->dy;

				uint8_t updated_sx = rotated_sx + GRID_PARAMETER_DEFAULT_POSITION + por->dx;
				uint8_t updated_sy = rotated_sy + GRID_PARAMETER_DEFAULT_POSITION + por->dy;
				
				
				
				uint8_t updated_msgage = received_msgage+1;
				
				if (received_dx + GRID_PARAMETER_DEFAULT_POSITION == 0 && received_dy + GRID_PARAMETER_DEFAULT_POSITION == 0)
				{
					// EDITOR GENERATED GLOBAL MESSAGE
					
				}
				else if (received_dx + GRID_PARAMETER_DEFAULT_POSITION == 255 && received_dy + GRID_PARAMETER_DEFAULT_POSITION == 255){
					
					// GRID GENERATED GLOBAL MESSAGE
					
				}
				else{
					
					// Update message with the new values
					grid_msg_string_set_parameter(message, GRID_BRC_DX_offset, GRID_BRC_DX_length, updated_dx, &error);
					grid_msg_string_set_parameter(message, GRID_BRC_DY_offset, GRID_BRC_DY_length, updated_dy, &error);

				}
				
								
				if (received_sx + GRID_PARAMETER_DEFAULT_POSITION == 0 && received_sy + GRID_PARAMETER_DEFAULT_POSITION == 0)
				{
					// EDITOR GENERATED GLOBAL MESSAGE
					
				}
				else if (received_sx + GRID_PARAMETER_DEFAULT_POSITION == 255 && received_sy + GRID_PARAMETER_DEFAULT_POSITION == 255){
					
					// GRID GENERATED GLOBAL MESSAGE
					
				}
				else{
					
					// Update message with the new values
					grid_msg_string_set_parameter(message, GRID_BRC_SX_offset, GRID_BRC_SX_length, updated_sx, &error);
					grid_msg_string_set_parameter(message, GRID_BRC_SY_offset, GRID_BRC_SY_length, updated_sy, &error);
				}
				
				grid_msg_string_set_parameter(message, GRID_BRC_MSGAGE_offset, GRID_BRC_MSGAGE_length, updated_msgage, &error);
				grid_msg_string_set_parameter(message, GRID_BRC_ROT_offset, GRID_BRC_ROT_length, updated_rot, &error);
				grid_msg_string_set_parameter(message, GRID_BRC_PORTROT_offset, GRID_BRC_PORTROT_length, por->partner_fi, &error);


				uint32_t fingerprint = received_id*256*256*256 + updated_sx*256*256 + updated_sy*256 + received_session;
				
				
				if (0 == grid_msg_recent_fingerprint_find(&grid_msg_state, fingerprint)){
					// WE HAVE NOT HEARD THIS MESSAGE BEFORE
					
					// Recalculate and update the checksum
					
					grid_msg_string_checksum_write(message, length, grid_msg_string_calculate_checksum_of_packet_string(message, length));
					

					// IF WE CAN STORE THE MESSAGE IN THE RX BUFFER
					if (grid_buffer_write_init(&por->rx_buffer, length)){
						
						for (uint16_t i=0; i<length; i++){
							
							grid_buffer_write_character(&por->rx_buffer, message[i]);
							
						}
						
						grid_buffer_write_acknowledge(&por->rx_buffer);
						
						//grid_port_process_inbound(por);
						
						grid_msg_recent_fingerprint_store(&grid_msg_state, fingerprint);
						
					}
					
					
					
				}
				else{
					// WE ALREADY HEARD THIS MESSAGE
					// grid_led_set_alert(&grid_led_state, GRID_LED_COLOR_PURPLE, 20);
					
				}
				
				

				
				// 				uint32_t response_length = strlen(response);
				//
				// 				if(grid_buffer_write_init(&por->tx_buffer, response_length)){
				//
				//
				// 					uint8_t checksum = grid_msg_get_checksum(response, response_length);
				// 					grid_msg_set_checksum(response, response_length, checksum);
				//
				// 					for (uint32_t i=0; i<response_length; i++)
				// 					{
				// 						grid_buffer_write_character(&por->tx_buffer, response[i]);
				// 					}
				//
				// 					grid_buffer_write_acknowledge(&por->tx_buffer);
				//
				// 				}
				
			}
			else if (message[1] == GRID_CONST_DCT){ // Direct Message
				
				//process direct message
				
				if (message[2] == GRID_CONST_ACK){

				}
				else if (message[2] == GRID_CONST_NAK){
					// RESEND PREVIOUS
				}
				else if (message[2] == GRID_CONST_CAN){
					// RESEND PREVIOUS
				}
				else if (message[2] == GRID_CONST_BELL){

					if (por->partner_status == 0){

						// CONNECT
						por->partner_fi = (message[3] - por->direction + 6)%4;
						por->partner_hwcfg = grid_msg_string_read_hex_string_value(&message[length-10], 2, error_flag);
						por->partner_status = 1;
						
						por->rx_double_buffer_timeout = 0;
						
						grid_port_debug_printf("Connect");			
						grid_led_set_alert(&grid_led_state, GRID_LED_COLOR_GREEN, 50);	
						grid_led_set_alert_frequency(&grid_led_state, -2);	
						grid_led_set_alert_phase(&grid_led_state, 100);	


					}
					else{

						por->rx_double_buffer_timeout = 0;
					}
				}
				else if (message[2] == GRID_CONST_BELL + 1){ // OLD IMPLEMENTATION
					
					
					// Handshake logic
					
					uint8_t local_token_received = grid_msg_string_read_hex_string_value(&message[8], 2, error_flag);
					uint8_t partner_token_received = grid_msg_string_read_hex_string_value(&message[6], 2, error_flag);
							
					if (por->partner_status == 0){
												
						if (por->ping_local_token == 255){ // I have no clue
							
							printf("BELL 0\r\n");
							// Generate new local
							
							por->ping_local_token  = grid_sys_rtc_get_time(&grid_sys_state)%128;
							
							//NEW
							grid_msg_string_write_hex_string_value(&por->ping_packet[6], 2, por->ping_local_token);
							grid_msg_string_checksum_write(por->ping_packet, por->ping_packet_length, grid_msg_string_calculate_checksum_of_packet_string(por->ping_packet, por->ping_packet_length));
								
							// No chance to connect now
			
						}
						else if (partner_token_received == 255){
							

							printf("BELL 1\r\n");
							// Remote is clueless
							// No chance to connect now
							
						}
						if (partner_token_received != por->ping_partner_token){
							
							por->ping_partner_token = partner_token_received;							
							
							//NEW
							grid_msg_string_write_hex_string_value(&por->ping_packet[8], 2, partner_token_received);
							grid_msg_string_checksum_write(por->ping_packet, por->ping_packet_length, grid_msg_string_calculate_checksum_of_packet_string(por->ping_packet, por->ping_packet_length));
							
							printf("BELL 2\r\n");	

							
							// Store remote
							// No chance to connect now
						}
						if (por->ping_local_token != local_token_received){
							
							printf("BELL 3\r\n");
							// Remote is clueless
							// No chance to connect now
							
						}
						else{
							
							// CONNECT
							por->partner_fi = (message[3] - por->direction + 6)%4;
							por->partner_hwcfg = grid_msg_string_read_hex_string_value(&message[length-10], 2, error_flag);
							por->partner_status = 1;
							
							
							
							grid_port_debug_printf("Connect");			
							grid_led_set_alert(&grid_led_state, GRID_LED_COLOR_GREEN, 50);	
							grid_led_set_alert_frequency(&grid_led_state, -2);	
							grid_led_set_alert_phase(&grid_led_state, 100);	

							
						}
				
						
						// PUT DIRECT MESSAGE INTO TXBUFFER
						por->ping_flag = 1;
						
											
					
					}
					else{
						
						// VALIDATE CONNECTION
						uint8_t validator = 1;
						
						validator &= local_token_received == por->ping_local_token;
						validator &= partner_token_received == por->ping_partner_token;
						
						validator &= por->partner_fi == (message[3] - por->direction + 6)%4;
						validator &= por->partner_hwcfg == grid_msg_string_read_hex_string_value(&message[length-10], 2, error_flag);
						
						
						if (validator == 1){
							
							// OK nice job!

						}
						else{
							
							//FAILED, DISCONNECT
							por->partner_status = 0;
							
							por->ping_partner_token = 255;
							por->ping_local_token = 255;
							
							grid_msg_string_write_hex_string_value(&por->ping_packet[8], 2, por->ping_partner_token);
							grid_msg_string_write_hex_string_value(&por->ping_packet[6], 2, por->ping_local_token);
							grid_msg_string_checksum_write(por->ping_packet, por->ping_packet_length, grid_msg_string_calculate_checksum_of_packet_string(por->ping_packet, por->ping_packet_length));
							
							
							//printf("LS: %d RS: %d LR: %d RR: %d  (Invalid)\r\n",local_stored,remote_stored,local_received,remote_received);
		
							
						}
					}
					
					
				}
				
			}
			else{ // Unknown Message Type
				
				grid_port_debug_printf("Unknown message type\r\n");
				
			}
			

			
		}
		else{
			// INVALID CHECKSUM
			

			if (error_flag != 0){
				//maybe disable uart
				//maybe enable uart
				grid_port_debug_printf("Invalid Checksum + flag");
			}
			else{
				printf("##  %s", message);
				grid_port_debug_printf("Invalid Checksum %02x %02x", checksum_calculated, checksum_received);
			}
			
			
		}
		

		// if (length>150){
		// 	printf("long frame: ");
		// 	for(uint16_t i=0; i<length; i++){
		// 		printf("%d,", message[i]);
		// 	}
		// 	printf("\r\n");
		// }

	}
	else{
		// frame error

		grid_port_debug_printf("Frame Error %d ", length);

		// printf("\r\bFRAME ");
		// for(uint16_t i=0; i<GRID_DOUBLE_BUFFER_RX_SIZE; i++){
		// 	printf("%02x ", GRID_PORT_H.rx_double_buffer[i]);
		// }
		// printf("\r\n");
		
	}
	
	return;
	
}

void grid_port_receive_task(struct grid_port* por){

	//parity error
	
	if (por->usart_error_flag == 1){
		
		por->usart_error_flag = 0;
		
		grid_port_receiver_hardreset(por);
		grid_port_debug_printf("Parity error");

		grid_led_set_alert(&grid_led_state, GRID_LED_COLOR_RED, 50);	
		grid_led_set_alert_frequency(&grid_led_state, -2);	
		grid_led_set_alert_phase(&grid_led_state, 100);	
		
	}
	

	///////////////////// PART 1 Old receive task

	if	(por->rx_double_buffer_status == 0){
		
		if (por->type == GRID_PORT_TYPE_USART){ // This is GRID usart port

			if (por->rx_double_buffer_timeout > 4000){
			
				if (por->partner_status == 1){
				
					grid_port_debug_printf("Timeout Disconnect 1");
				
						grid_port_receiver_softreset(por);	
						//grid_port_receiver_softreset(por);	
								

						grid_led_set_alert(&grid_led_state, GRID_LED_COLOR_RED, 50);	
						grid_led_set_alert_frequency(&grid_led_state, -2);	
						grid_led_set_alert_phase(&grid_led_state, 100);	
				}
				else{
				
					if (por->rx_double_buffer_read_start_index == 0 && por->rx_double_buffer_seek_start_index == 0){
						// Ready to receive
						
						grid_port_receiver_softreset(por);
					}
					else{
					
						grid_port_debug_printf("Timeout Disconnect 2");
						grid_port_receiver_softreset(por);
					}
				
				}
			
			}
			else{
			
				por->rx_double_buffer_timeout++;
			}			
					
		}
		
		for(uint16_t i = 0; i<490; i++){
				
			if (por->rx_double_buffer[por->rx_double_buffer_seek_start_index] == 10){ // \n
					
				por->rx_double_buffer_status = 1;
				por->rx_double_buffer_timeout = 0;
					
				break;
			}
			else if (por->rx_double_buffer[por->rx_double_buffer_seek_start_index] == 0){
				
				break;
			}
				
				
			uint8_t overrun_condition_1 = (por->rx_double_buffer_seek_start_index == por->rx_double_buffer_read_start_index-1);
			uint8_t overrun_condition_2 = (por->rx_double_buffer_seek_start_index == GRID_DOUBLE_BUFFER_RX_SIZE-1 && por->rx_double_buffer_read_start_index == 0);
			uint8_t overrun_condition_3 = (por->rx_double_buffer[(por->rx_double_buffer_read_start_index + GRID_DOUBLE_BUFFER_RX_SIZE -1)%GRID_DOUBLE_BUFFER_RX_SIZE] !=0);
			
			// Buffer overrun error 1, 2, 3
			if (overrun_condition_1 || overrun_condition_2 || overrun_condition_3){

				grid_port_receiver_hardreset(por);	
				
				//printf("Overrun\r\n"); // never use grid message to indicate overrun directly				

				grid_led_set_alert(&grid_led_state, GRID_LED_COLOR_RED, 50);	
				grid_led_set_alert_frequency(&grid_led_state, -2);	
				grid_led_set_alert_phase(&grid_led_state, 100);	
				return;
			}
				
				
			if (por->rx_double_buffer_seek_start_index < GRID_DOUBLE_BUFFER_RX_SIZE-1){
					
				por->rx_double_buffer_timeout = 0;
				por->rx_double_buffer_seek_start_index++;
			}
			else{
					
				por->rx_double_buffer_timeout = 0;
				por->rx_double_buffer_seek_start_index=0;
			}
				
		}
	}
	
	////////////////// PART 2
	
	// No complete message in buffer
	if (por->rx_double_buffer_status == 0){
		return;
	}


	uint32_t length = 0;
	
	if (por->rx_double_buffer_read_start_index < por->rx_double_buffer_seek_start_index){
		length = por->rx_double_buffer_seek_start_index - por->rx_double_buffer_read_start_index + 1;
	}
	else{
		length = GRID_DOUBLE_BUFFER_RX_SIZE + por->rx_double_buffer_seek_start_index - por->rx_double_buffer_read_start_index + 1;
	}
	

	grid_port_receive_decode(por, por->rx_double_buffer_read_start_index, length);
	

	
	por->rx_double_buffer_status = 0;
	
	
	
}


//=============================== PROCESS INBOUND ==============================//


uint8_t grid_port_process_inbound(struct grid_port* por, uint8_t loopback){
	
	uint16_t packet_size = grid_buffer_read_size(&por->rx_buffer);
	
	if (!packet_size){
		
		// NO PACKET IN RX BUFFER
		return 0;
		 
	}else{
			
		uint8_t port_count = 6;
		struct grid_port* port_array_default[port_count];
		struct grid_port* port_array[port_count];
		
		
		port_array_default[0] = &GRID_PORT_N;
		port_array_default[1] = &GRID_PORT_E;
		port_array_default[2] = &GRID_PORT_S;
		port_array_default[3] = &GRID_PORT_W;
		
		port_array_default[4] = &GRID_PORT_U;
		port_array_default[5] = &GRID_PORT_H;
		
		uint8_t j=0;
		
		for(uint8_t i=0; i<port_count; i++){
			if (port_array_default[i]->partner_status != 0){
				port_array[j] = port_array_default[i];
				j++;
			}	
		}
		port_count = j;
		

			
		// Check all of the tx buffers for sufficient storage space
		
		for (uint8_t i=0; i<port_count; i++)
		{
			if (port_array[i] != por || loopback){
			
				if (packet_size > grid_buffer_write_size(&port_array[i]->tx_buffer)){
					

					grid_led_set_alert(&grid_led_state, GRID_LED_COLOR_BLUE, 128);
					
					// sorry one of the buffers cannot store the packet, we will try later
					return 0;
				}	
			}	
		}
		
		if (packet_size != grid_buffer_read_init(&por->rx_buffer)){
			while(1){			
				// TRAP: WTF
			}
		}
		
		// Let's init all of the buffers for transfer		 
		
		for (uint8_t i=0; i<port_count; i++)
		{
			if (port_array[i] != por || loopback){
				grid_buffer_write_init(&port_array[i]->tx_buffer, packet_size);
			}
		}
		
		// Let's do the transfer
												
		for (uint16_t j=0; j<packet_size; j++)
		{
			
			uint8_t character = grid_buffer_read_character(&por->rx_buffer);
				
			for (uint8_t i=0; i<port_count; i++){
				if (port_array[i] != por || loopback){
					grid_buffer_write_character(&port_array[i]->tx_buffer, character);
					
				}
			}					
		}
			
								
		// Let's acknowledge all of the transfer					
		grid_buffer_read_acknowledge(&por->rx_buffer);
					
		for (uint8_t i=0; i<port_count; i++)
		{
			if (port_array[i] != por || loopback){
				grid_buffer_write_acknowledge(&port_array[i]->tx_buffer);
			}
		}	

		return 1;
	}
		
}



//=============================== PROCESS OUTBOUND ==============================//


uint8_t grid_port_process_outbound_ui(struct grid_port* por){
	
	
	uint16_t length = grid_buffer_read_size(&por->tx_buffer);
	
	if (!length){
				
		// NO PACKET IN RX BUFFER
		return 0;
	}
	else{
		
		uint8_t message[GRID_PARAMETER_PACKET_maxlength] = {0};
		
		// Let's transfer the packet to local memory
		grid_buffer_read_init(&por->tx_buffer);
		
		for (uint16_t i = 0; i<length; i++){
			
			message[i] = grid_buffer_read_character(&por->tx_buffer);
			//usb_tx_double_buffer[i] = character;
					
		}

		grid_buffer_read_acknowledge(&por->tx_buffer);
		
		// GRID-2-UI TRANSLATOR
		
		uint8_t error=0;

		uint8_t id = grid_msg_string_get_parameter(message, GRID_BRC_ID_offset, GRID_BRC_ID_length, &error);
			
		uint8_t dx = grid_msg_string_get_parameter(message, GRID_BRC_DX_offset, GRID_BRC_DX_length, &error);
		uint8_t dy = grid_msg_string_get_parameter(message, GRID_BRC_DY_offset, GRID_BRC_DY_length, &error);

		uint8_t sx = grid_msg_string_get_parameter(message, GRID_BRC_SX_offset, GRID_BRC_SX_length, &error);
		uint8_t sy = grid_msg_string_get_parameter(message, GRID_BRC_SY_offset, GRID_BRC_SY_length, &error);

		uint8_t rot = grid_msg_string_get_parameter(message, GRID_BRC_ROT_offset, GRID_BRC_ROT_length, &error);
		uint8_t portrot = grid_msg_string_get_parameter(message, GRID_BRC_PORTROT_offset, GRID_BRC_PORTROT_length, &error);
			
		uint8_t position_is_me = 0;
		uint8_t position_is_global = 0;
		uint8_t position_is_local = 0;
			
		if (dx == GRID_PARAMETER_DEFAULT_POSITION && dy == GRID_PARAMETER_DEFAULT_POSITION){
			position_is_me = 1;
		}
		else if (dx == GRID_PARAMETER_GLOBAL_POSITION && dy==GRID_PARAMETER_GLOBAL_POSITION){
			position_is_global = 1;
		}
		else if (dx == GRID_PARAMETER_LOCAL_POSITION && dy==GRID_PARAMETER_LOCAL_POSITION){
			position_is_local = 1;
		}
		
			
		uint16_t current_start		= 0;
		uint16_t current_stop		= 0;
		uint16_t current_length		= 0;

		uint16_t start_count		= 0;
		uint16_t stop_count		= 0;
			
			
		uint8_t error_flag = 0;	
		
		
		for (uint16_t i=0; i<length; i++){
	
			if (message[i] == GRID_CONST_STX){

				current_start = i;
				start_count++;
			}
			else if (message[i] == GRID_CONST_ETX && current_start!=0 && (start_count-stop_count) == 1){
				current_stop = i;
				current_length		= current_stop-current_start;
				stop_count++;

				uint8_t msg_class = grid_msg_string_read_hex_string_value(&message[current_start+GRID_PARAMETER_CLASSCODE_offset], GRID_PARAMETER_CLASSCODE_length, &error_flag);
				uint8_t msg_instr = grid_msg_string_read_hex_string_value(&message[current_start+GRID_INSTR_offset], GRID_INSTR_length, &error_flag);
		

		
				if (msg_class == GRID_CLASS_PAGEACTIVE_code){ // dont check address!
						
					uint8_t page = grid_msg_string_read_hex_string_value(&message[current_start+GRID_CLASS_PAGEACTIVE_PAGENUMBER_offset], GRID_CLASS_PAGEACTIVE_PAGENUMBER_length, &error_flag);
								
					
					if (msg_instr == GRID_INSTR_EXECUTE_code){ //SET BANK

						

						if (grid_ui_state.page_change_enabled == 1){

							//grid_port_debug_printf("TRY");
							grid_ui_page_load(&grid_ui_state, page);
							grid_sys_set_bank(&grid_sys_state, page);

						}
						else{

							//grid_port_debug_printf("DISABLE");
						}
													
					}

					if (msg_instr == GRID_INSTR_REPORT_code){ //SET BANK

				
						//printf("RX: %d %d\r\n", sx, sy);

						if (!(sx==GRID_PARAMETER_DEFAULT_POSITION && sy==GRID_PARAMETER_DEFAULT_POSITION)){

							//printf("RX: %s\r\n", &message[current_start]);
							if (grid_ui_state.page_negotiated == 0){

								grid_ui_state.page_negotiated = 1;
								grid_ui_page_load(&grid_ui_state, page);
								grid_sys_set_bank(&grid_sys_state, page);
								
							}
							



						}
													
					}

					
				}
				if (msg_class == GRID_CLASS_MIDI_code && msg_instr == GRID_INSTR_REPORT_code){
						
									
					uint8_t midi_channel = grid_msg_string_read_hex_string_value(&message[current_start+GRID_CLASS_MIDI_CHANNEL_offset], GRID_CLASS_MIDI_CHANNEL_length, &error_flag);	
					uint8_t midi_command = grid_msg_string_read_hex_string_value(&message[current_start+GRID_CLASS_MIDI_COMMAND_offset], GRID_CLASS_MIDI_COMMAND_length, &error_flag);	
					uint8_t midi_param1 = grid_msg_string_read_hex_string_value(&message[current_start+GRID_CLASS_MIDI_PARAM1_offset], GRID_CLASS_MIDI_PARAM1_length, &error_flag);	
					uint8_t midi_param2 = grid_msg_string_read_hex_string_value(&message[current_start+GRID_CLASS_MIDI_PARAM2_offset], GRID_CLASS_MIDI_PARAM2_length, &error_flag);		
					
					//printf("M: %d %d %d %d \r\n", midi_channel, midi_command, midi_param1, midi_param2);

					uint8_t temp[100] = {0};
					sprintf(temp, "midi={} midi.ch,midi.cmd,midi.p1,midi.p2 = %d, %d, %d, %d ", midi_channel, midi_command, midi_param1, midi_param2);
					grid_lua_dostring(&grid_lua_state, temp);

					struct grid_ui_element* ele = &grid_ui_state.element_list[grid_ui_state.element_list_length-1];
					struct grid_ui_event* eve = NULL;

					eve = grid_ui_event_find(ele, GRID_UI_EVENT_MIDIRX);
					if (eve != NULL){

						grid_ui_event_trigger(eve);

					}



														
				}
				if (msg_class == GRID_CLASS_PAGECOUNT_code && (position_is_global || position_is_me)){
				
					if (msg_instr == GRID_INSTR_FETCH_code){ //get page count

						struct grid_msg_packet response;
												
						grid_msg_packet_init(&grid_msg_state, &response, GRID_PARAMETER_GLOBAL_POSITION, GRID_PARAMETER_GLOBAL_POSITION);

						uint8_t response_payload[50] = {0};
						snprintf(response_payload, 49, GRID_CLASS_PAGECOUNT_frame);

						grid_msg_packet_body_append_text(&response, response_payload);
							
						grid_msg_packet_body_set_parameter(&response, 0, GRID_INSTR_offset, GRID_INSTR_length, GRID_INSTR_REPORT_code);					
												
						grid_msg_packet_body_set_parameter(&response, 0, GRID_CLASS_PAGECOUNT_PAGENUMBER_offset, GRID_CLASS_PAGECOUNT_PAGENUMBER_length, grid_ui_state.page_count);

						grid_msg_packet_close(&grid_msg_state, &response);
						grid_port_packet_send_everywhere(&response);
							

													
					}
					
				}
				else if (msg_class == GRID_CLASS_IMEDIATE_code && msg_instr == GRID_INSTR_EXECUTE_code && (position_is_global || position_is_me || position_is_local)){

					uint16_t length = grid_msg_string_read_hex_string_value(&message[current_start+GRID_CLASS_IMEDIATE_ACTIONLENGTH_offset], GRID_CLASS_IMEDIATE_ACTIONLENGTH_length, &error_flag);
					uint8_t lua_script[200] = {0};
					strncpy(lua_script, &message[current_start+GRID_CLASS_IMEDIATE_ACTIONSTRING_offset], length);


					if (0 == strncmp(lua_script, "<?lua ", 6) && lua_script[length-3] == ' ' && lua_script[length-2] == '?' && lua_script[length-1] == '>'){
					
					
						printf("IMEDIATE %d: %s\r\n", length, lua_script);
						
						lua_script[length-3] = '\0';
						grid_lua_dostring(&grid_lua_state, &lua_script[6]);

					
					}
					else{
						printf("IMEDIATE NOT OK %d: %s\r\n", length, lua_script);
					}

					



				}
				else if (msg_class == GRID_CLASS_HEARTBEAT_code){
					
					uint8_t type  = grid_msg_string_read_hex_string_value(&message[current_start+GRID_CLASS_HEARTBEAT_TYPE_offset], GRID_CLASS_HEARTBEAT_TYPE_length, &error_flag);
					
					uint8_t editor_connected_now = 0;


					if (type == 0){
						// from other grid module
					}
					else if (type == 1){

						// from usb connected module
						int8_t received_sx = sx-GRID_PARAMETER_DEFAULT_POSITION; // convert to signed ind
						int8_t received_sy = sy-GRID_PARAMETER_DEFAULT_POSITION; // convert to signed ind
						int8_t rotated_sx;
						int8_t rotated_sy;

						// APPLY THE 2D ROTATION MATRIX
						
						//printf("Protrot %d \r\n", portrot);

						if (portrot == 0){ // 0 deg

							rotated_sx  -= received_sx;
							rotated_sy  -= received_sy;
						}
						else if(portrot == 1){ // 90 deg

							rotated_sx  -= received_sy;
							rotated_sy  += received_sx;
						}
						else if(portrot == 2){ // 180 deg

							rotated_sx  += received_sx;
							rotated_sy  += received_sy;
						}
						else if(portrot == 3){ // 270 deg

							rotated_sx  += received_sy;
							rotated_sy  -= received_sx;
						}
						else{
							// TRAP INVALID MESSAGE
						}

						grid_sys_set_module_x(&grid_sys_state, rotated_sx);
						grid_sys_set_module_y(&grid_sys_state, rotated_sy);
						grid_sys_set_module_rot(&grid_sys_state, rot);

					}
					else if (type >127){ // editor


						if (grid_sys_get_editor_connected_state(&grid_sys_state) == 0){

							grid_sys_set_editor_connected_state(&grid_sys_state, 1);
						
							editor_connected_now = 1;
							grid_port_debug_print_text("EDITOR connect");
						}

						grid_msg_set_editor_heartbeat_lastrealtime(&grid_msg_state, grid_sys_rtc_get_time(&grid_sys_state));

						if (type == 255){
							grid_ui_state.page_change_enabled = 1;
							//printf("255\r\n");
						}
						else{
							grid_ui_state.page_change_enabled = 0;
							//printf("254\r\n");
						}

						uint8_t ui_report_valid = 0;


						if (editor_connected_now){

							uint16_t report_length = 0;
							struct grid_msg_packet response;
													
							grid_msg_packet_init(&grid_msg_state, &response, GRID_PARAMETER_GLOBAL_POSITION, GRID_PARAMETER_GLOBAL_POSITION);

							uint8_t response_payload[300] = {0};
							uint16_t len = 0;
							snprintf(&response_payload[len], 299, GRID_CLASS_EVENTPREVIEW_frame_start);
							len += strlen(&response_payload[len]);



							for(uint8_t j=0; j<grid_ui_state.element_list_length; j++){


								struct grid_ui_element* ele = &grid_ui_state.element_list[j];

								uint8_t element_num = ele->index;
								uint8_t element_value = 0;

								if (ele->type == GRID_UI_ELEMENT_POTENTIOMETER){

									element_value = ele->template_parameter_list[GRID_LUA_FNC_P_POTMETER_VALUE_index];
								}
								else if (ele->type == GRID_UI_ELEMENT_ENCODER){

									element_value = ele->template_parameter_list[GRID_LUA_FNC_E_ENCODER_VALUE_index];
								}
								
								report_length += 4;

								//printf("Element %d: %d\r\n", element_num, element_value);
								
								//grid_led_state.led_lowlevel_changed[j] = 0;

								ui_report_valid = 1;

								sprintf(&response_payload[len], "%02x%02x", element_num, element_value);
								len += strlen(&response_payload[len]);

							}


							sprintf(&response_payload[len], GRID_CLASS_EVENTPREVIEW_frame_end);
							len += strlen(&response_payload[len]);

							grid_msg_packet_body_append_text(&response, response_payload);
								

							grid_msg_packet_body_set_parameter(&response, 0, GRID_INSTR_offset, GRID_INSTR_length, GRID_INSTR_REPORT_code);													
							grid_msg_packet_body_set_parameter(&response, 0, GRID_CLASS_EVENTPREVIEW_LENGTH_offset, GRID_CLASS_EVENTPREVIEW_LENGTH_length, report_length);
							
							grid_msg_packet_close(&grid_msg_state, &response);
							grid_port_packet_send_everywhere(&response);

							//printf(response.body);
							//printf("\r\n");
						}

						// report stringnames
						if (editor_connected_now){

							uint16_t report_length = 0;
							struct grid_msg_packet response;
													
							grid_msg_packet_init(&grid_msg_state, &response, GRID_PARAMETER_GLOBAL_POSITION, GRID_PARAMETER_GLOBAL_POSITION);


							// -1 to exclude system element
							for(uint8_t j=0; j<grid_ui_state.element_list_length-1; j++){

								struct grid_ui_element* ele = &grid_ui_state.element_list[j];
								
								uint8_t number = j;
								uint8_t command[20] = {0};

								sprintf(command, "gens(%d,ele[%d]:gen())", j, j);

								// lua get element name
								grid_lua_clear_stdo(&grid_lua_state);
								grid_lua_dostring(&grid_lua_state, command);

								grid_msg_packet_body_append_text(&response, grid_lua_state.stdo);

							}

							grid_msg_packet_close(&grid_msg_state, &response);
							grid_port_packet_send_everywhere(&response);

							//printf(response.body);
							//printf("\r\n");
						}


						// Report the state of the changed leds

						if (editor_connected_now){
							// reset the changed flags to force report all leds
							grid_led_change_flag_reset(&grid_led_state);
						}


						if (grid_protocol_led_change_report_length(&grid_led_state)){


							struct grid_msg_packet response;
													
							grid_msg_packet_init(&grid_msg_state, &response, GRID_PARAMETER_GLOBAL_POSITION, GRID_PARAMETER_GLOBAL_POSITION);

							uint8_t response_payload[300] = {0};
							uint16_t len = 0;
							snprintf(response_payload, 299, GRID_CLASS_LEDPREVIEW_frame_start);
							len += strlen(&response_payload[len]);

							uint16_t report_length = grid_protocol_led_change_report_generate(&grid_led_state, -1, &response_payload[len]);

							len += strlen(&response_payload[len]);

							grid_msg_packet_body_append_text(&response, response_payload);


							grid_msg_packet_body_append_printf(&response, GRID_CLASS_LEDPREVIEW_frame_end);

							grid_msg_packet_body_set_parameter(&response, 0, GRID_INSTR_offset, GRID_INSTR_length, GRID_INSTR_REPORT_code);													
							grid_msg_packet_body_set_parameter(&response, 0, GRID_CLASS_LEDPREVIEW_LENGTH_offset, GRID_CLASS_LEDPREVIEW_LENGTH_length, report_length);
							
							grid_msg_packet_close(&grid_msg_state, &response);
							grid_port_packet_send_everywhere(&response);

						}

						

						// from editor

					}
					else{
						// unknown type
					}
							
				}
				else if(msg_class == GRID_CLASS_SERIALNUMBER_code && msg_instr == GRID_INSTR_FETCH_code && (position_is_me || position_is_global)){
					
					uint32_t uniqueid[4] = {0};
					grid_sys_get_id(&grid_sys_state, uniqueid);					
					// Generate RESPONSE
					struct grid_msg_packet response;
											
					grid_msg_packet_init(&grid_msg_state, &response, GRID_PARAMETER_GLOBAL_POSITION, GRID_PARAMETER_GLOBAL_POSITION);

					uint8_t response_payload[50] = {0};
					snprintf(response_payload, 49, GRID_CLASS_SERIALNUMBER_frame);

					grid_msg_packet_body_append_text(&response, response_payload);
						
					grid_msg_packet_body_set_parameter(&response, 0, GRID_INSTR_offset, GRID_INSTR_length, GRID_INSTR_REPORT_code);					
											
					grid_msg_packet_body_set_parameter(&response, 0, GRID_CLASS_SERIALNUMBER_WORD0_offset, GRID_CLASS_SERIALNUMBER_WORD0_length, uniqueid[0]);
					grid_msg_packet_body_set_parameter(&response, 0, GRID_CLASS_SERIALNUMBER_WORD1_offset, GRID_CLASS_SERIALNUMBER_WORD1_length, uniqueid[1]);
					grid_msg_packet_body_set_parameter(&response, 0, GRID_CLASS_SERIALNUMBER_WORD2_offset, GRID_CLASS_SERIALNUMBER_WORD2_length, uniqueid[2]);
					grid_msg_packet_body_set_parameter(&response, 0, GRID_CLASS_SERIALNUMBER_WORD3_offset, GRID_CLASS_SERIALNUMBER_WORD3_length, uniqueid[3]);


											
					grid_msg_packet_close(&grid_msg_state, &response);
					grid_port_packet_send_everywhere(&response);
						
	
				}
				else if(msg_class == GRID_CLASS_UPTIME_code && msg_instr == GRID_INSTR_FETCH_code && (position_is_me || position_is_global)){

					// Generate RESPONSE
					struct grid_msg_packet response;
				
					grid_msg_packet_init(&grid_msg_state, &response, GRID_PARAMETER_GLOBAL_POSITION, GRID_PARAMETER_GLOBAL_POSITION);

					uint8_t response_payload[50] = {0};
					snprintf(response_payload, 49, GRID_CLASS_UPTIME_frame);

					grid_msg_packet_body_append_text(&response, response_payload);
				
					grid_msg_packet_body_set_parameter(&response, 0, GRID_INSTR_offset, GRID_INSTR_length, GRID_INSTR_REPORT_code);
				
					grid_msg_packet_body_set_parameter(&response, 0, GRID_CLASS_UPTIME_UPTIME_offset, GRID_CLASS_UPTIME_UPTIME_length, grid_sys_get_uptime(&grid_sys_state));
					
					uint32_t uptime = grid_sys_get_uptime(&grid_sys_state);
					
					uint32_t milliseconds = uptime/RTC1MS%1000;
					uint32_t seconds =		uptime/RTC1MS/1000%60;
					uint32_t minutes =		uptime/RTC1MS/1000/60%60;
					uint32_t hours =		uptime/RTC1MS/1000/60/60%60;
					
				
					grid_msg_packet_close(&grid_msg_state, &response);
					grid_port_packet_send_everywhere(&response);
				
				
				}
				else if(msg_class == GRID_CLASS_RESETCAUSE_code && msg_instr == GRID_INSTR_FETCH_code && (position_is_me || position_is_global)){

					// Generate RESPONSE
					struct grid_msg_packet response;
					
					grid_msg_packet_init(&grid_msg_state, &response, GRID_PARAMETER_GLOBAL_POSITION, GRID_PARAMETER_GLOBAL_POSITION);

					uint8_t response_payload[50] = {0};
					snprintf(response_payload, 49, GRID_CLASS_RESETCAUSE_frame);

					grid_msg_packet_body_append_text(&response, response_payload);
					
					grid_msg_packet_body_set_parameter(&response, 0, GRID_INSTR_offset, GRID_INSTR_length, GRID_INSTR_REPORT_code);
					
					

					grid_msg_packet_body_set_parameter(&response, 0, GRID_CLASS_RESETCAUSE_CAUSE_offset, GRID_CLASS_RESETCAUSE_CAUSE_length, grid_platform_get_reset_cause());
			
					grid_msg_packet_close(&grid_msg_state, &response);
					grid_port_packet_send_everywhere(&response);
					
					
				}
				else if(msg_class == GRID_CLASS_RESET_code && msg_instr == GRID_INSTR_EXECUTE_code && (position_is_me)){

					NVIC_SystemReset();
				
				}				
			
				else if (msg_class == GRID_CLASS_PAGEDISCARD_code && msg_instr == GRID_INSTR_EXECUTE_code && (position_is_me || position_is_global)){
					
					grid_msg_store_lastheader(&grid_msg_state, GRID_MSG_LASTHEADER_DISCARD_INDEX, id);
					grid_ui_bulk_pageread_init(&grid_ui_state, &grid_protocol_nvm_read_succcess_callback);


				}		
				else if (msg_class == GRID_CLASS_PAGEDISCARD_code && msg_instr == GRID_INSTR_CHECK_code && (position_is_me || position_is_global)){
					
					uint8_t state = grid_msg_get_lastheader_state(&grid_msg_state, GRID_MSG_LASTHEADER_DISCARD_INDEX);
					uint8_t id = grid_msg_get_lastheader_id(&grid_msg_state, GRID_MSG_LASTHEADER_DISCARD_INDEX);


					struct grid_msg_packet response;	
					grid_msg_packet_init(&grid_msg_state, &response, GRID_PARAMETER_GLOBAL_POSITION, GRID_PARAMETER_GLOBAL_POSITION);
					grid_msg_packet_body_append_printf(&response, GRID_CLASS_PAGEDISCARD_frame);
					grid_msg_packet_body_append_parameter(&response, GRID_CLASS_PAGEDISCARD_LASTHEADER_offset, GRID_CLASS_PAGEDISCARD_LASTHEADER_length, id);		


					if (state != -1){ // ACK
						grid_msg_packet_body_append_parameter(&response, GRID_INSTR_offset, GRID_INSTR_length, GRID_INSTR_ACKNOWLEDGE_code);
					}		
					else{ // NACK
						grid_msg_packet_body_append_parameter(&response, GRID_INSTR_offset, GRID_INSTR_length, GRID_INSTR_NACKNOWLEDGE_code);
					}	

					grid_msg_packet_close(&grid_msg_state, &response);
					grid_port_packet_send_everywhere(&response);	

				}			
				else if (msg_class == GRID_CLASS_PAGESTORE_code && msg_instr == GRID_INSTR_EXECUTE_code && (position_is_me || position_is_global)){
								
					grid_msg_store_lastheader(&grid_msg_state, GRID_MSG_LASTHEADER_STORE_INDEX, id);
					
					// start animation (it will be stopped in the callback function)
					grid_led_set_alert(&grid_led_state, GRID_LED_COLOR_YELLOW_DIM, -1);		
					grid_led_set_alert_frequency(&grid_led_state, -4);	

					grid_ui_bulk_pagestore_init(&grid_ui_state, &grid_protocol_nvm_store_succcess_callback);					

				}			
				else if (msg_class == GRID_CLASS_PAGECLEAR_code && msg_instr == GRID_INSTR_EXECUTE_code && (position_is_me || position_is_global)){
				
					grid_msg_store_lastheader(&grid_msg_state, GRID_MSG_LASTHEADER_CLEAR_INDEX, id);			


					grid_ui_bulk_pageclear_init(&grid_ui_state, &grid_protocol_nvm_clear_succcess_callback);					

				}		
				else if (msg_class == GRID_CLASS_PAGESTORE_code && msg_instr == GRID_INSTR_CHECK_code && (position_is_me || position_is_global)){
					
					uint8_t state = grid_msg_get_lastheader_state(&grid_msg_state, GRID_MSG_LASTHEADER_STORE_INDEX);
					uint8_t id = grid_msg_get_lastheader_id(&grid_msg_state, GRID_MSG_LASTHEADER_STORE_INDEX);

					struct grid_msg_packet response;	
					grid_msg_packet_init(&grid_msg_state, &response, GRID_PARAMETER_GLOBAL_POSITION, GRID_PARAMETER_GLOBAL_POSITION);
					grid_msg_packet_body_append_printf(&response, GRID_CLASS_PAGESTORE_frame);
					grid_msg_packet_body_append_parameter(&response, GRID_CLASS_PAGESTORE_LASTHEADER_offset, GRID_CLASS_PAGESTORE_LASTHEADER_length, id);		
				
					if (state != -1 && 0 == grid_ui_bulk_pagestore_is_in_progress(&grid_ui_state)){ // ACK
						grid_msg_packet_body_append_parameter(&response, GRID_INSTR_offset, GRID_INSTR_length, GRID_INSTR_ACKNOWLEDGE_code);
					}		
					else{ // NACK
						grid_msg_packet_body_append_parameter(&response, GRID_INSTR_offset, GRID_INSTR_length, GRID_INSTR_NACKNOWLEDGE_code);
					}	

					grid_msg_packet_close(&grid_msg_state, &response);
					grid_port_packet_send_everywhere(&response);	

				}	
				else if (msg_class == GRID_CLASS_NVMERASE_code && msg_instr == GRID_INSTR_EXECUTE_code && (position_is_me || position_is_global)){

				
					if (current_length==5){ // erase all nvm configs

						grid_msg_store_lastheader(&grid_msg_state, GRID_MSG_LASTHEADER_ERASE_INDEX, id);
						grid_ui_bulk_nvmerase_init(&grid_ui_state, &grid_protocol_nvm_erase_succcess_callback);


						// start animation (it will be stopped in the callback function)
						grid_led_set_alert(&grid_led_state, GRID_LED_COLOR_YELLOW_DIM, -1);	
						grid_led_set_alert_frequency(&grid_led_state, -2);	

						for (uint8_t i = 0; i<grid_led_get_led_count(&grid_led_state); i++){
							grid_led_set_layer_min(&grid_led_state, i, GRID_LED_LAYER_ALERT, GRID_LED_COLOR_YELLOW_DIM);
						}

					}
					else{
						grid_port_debug_printf("Erase: Invalid params!");
					}
					

				}		
				else if (msg_class == GRID_CLASS_NVMERASE_code && msg_instr == GRID_INSTR_CHECK_code && (position_is_me || position_is_global)){
					
					uint8_t state = grid_msg_get_lastheader_state(&grid_msg_state, GRID_MSG_LASTHEADER_ERASE_INDEX);
					uint8_t id = grid_msg_get_lastheader_id(&grid_msg_state, GRID_MSG_LASTHEADER_ERASE_INDEX);


					struct grid_msg_packet response;	
					grid_msg_packet_init(&grid_msg_state, &response, GRID_PARAMETER_GLOBAL_POSITION, GRID_PARAMETER_GLOBAL_POSITION);
					grid_msg_packet_body_append_printf(&response, GRID_CLASS_NVMERASE_frame);
					grid_msg_packet_body_append_parameter(&response, GRID_CLASS_NVMERASE_LASTHEADER_offset, GRID_CLASS_NVMERASE_LASTHEADER_length, id);		
				
					if (state != -1 && 0 == grid_ui_bulk_nvmerase_is_in_progress(&grid_ui_state)){ // ACK
						grid_msg_packet_body_append_parameter(&response, GRID_INSTR_offset, GRID_INSTR_length, GRID_INSTR_ACKNOWLEDGE_code);
					}		
					else{ // NACK
						grid_msg_packet_body_append_parameter(&response, GRID_INSTR_offset, GRID_INSTR_length, GRID_INSTR_NACKNOWLEDGE_code);
					}	

					grid_msg_packet_close(&grid_msg_state, &response);
					grid_port_packet_send_everywhere(&response);	

				}	
				else if (msg_class == GRID_CLASS_NVMDEFRAG_code && msg_instr == GRID_INSTR_EXECUTE_code && (position_is_me || position_is_global)){
				
					grid_d51_nvm_toc_defragment(&grid_d51_nvm_state);

				}
				else if (msg_class == GRID_CLASS_CONFIG_code && msg_instr == GRID_INSTR_FETCH_code && (position_is_me || position_is_global)){
					
					
					uint8_t pagenumber = grid_msg_string_get_parameter(message, current_start+GRID_CLASS_CONFIG_PAGENUMBER_offset, GRID_CLASS_CONFIG_PAGENUMBER_length, NULL);
					uint8_t elementnumber = grid_msg_string_get_parameter(message, current_start+GRID_CLASS_CONFIG_ELEMENTNUMBER_offset, GRID_CLASS_CONFIG_ELEMENTNUMBER_length, NULL);
					uint8_t eventtype = grid_msg_string_get_parameter(message, current_start+GRID_CLASS_CONFIG_EVENTTYPE_offset, GRID_CLASS_CONFIG_EVENTTYPE_length, NULL);
					//uint16_t actionlength = grid_msg_string_get_parameter(message, current_start+GRID_CLASS_CONFIG_ACTIONLENGTH_offset, GRID_CLASS_CONFIG_ACTIONLENGTH_length, NULL);

					// Helper to map system element to 255
					if (elementnumber == 255){
						elementnumber = grid_ui_state.element_list_length - 1;
					}


					uint8_t temp[GRID_PARAMETER_ACTIONSTRING_maxlength]  = {0};

					grid_ui_event_recall_configuration(&grid_ui_state, pagenumber, elementnumber, eventtype, temp);
					
					if (strlen(temp) != 0){



						struct grid_msg_packet message;

						grid_msg_packet_init(&grid_msg_state, &message, GRID_PARAMETER_GLOBAL_POSITION, GRID_PARAMETER_GLOBAL_POSITION);

						grid_msg_packet_body_append_printf(&message, GRID_CLASS_CONFIG_frame_start);

						grid_msg_packet_body_append_parameter(&message, GRID_INSTR_offset, GRID_INSTR_length, GRID_INSTR_REPORT_code);
						
						grid_msg_packet_body_append_parameter(&message, GRID_CLASS_CONFIG_VERSIONMAJOR_offset, GRID_CLASS_CONFIG_VERSIONMAJOR_length, GRID_PROTOCOL_VERSION_MAJOR);
						grid_msg_packet_body_append_parameter(&message, GRID_CLASS_CONFIG_VERSIONMINOR_offset, GRID_CLASS_CONFIG_VERSIONMINOR_length, GRID_PROTOCOL_VERSION_MINOR);
						grid_msg_packet_body_append_parameter(&message, GRID_CLASS_CONFIG_VERSIONPATCH_offset, GRID_CLASS_CONFIG_VERSIONPATCH_length, GRID_PROTOCOL_VERSION_PATCH);

						// Helper to map system element to 255
						uint8_t element_helper = elementnumber;
						if (elementnumber == grid_ui_state.element_list_length - 1){
							element_helper = 255;
						}

						grid_msg_packet_body_append_parameter(&message, GRID_CLASS_CONFIG_PAGENUMBER_offset, GRID_CLASS_CONFIG_PAGENUMBER_length, pagenumber);
						grid_msg_packet_body_append_parameter(&message, GRID_CLASS_CONFIG_ELEMENTNUMBER_offset, GRID_CLASS_CONFIG_EVENTTYPE_length, element_helper);
						grid_msg_packet_body_append_parameter(&message, GRID_CLASS_CONFIG_EVENTTYPE_offset, GRID_CLASS_CONFIG_EVENTTYPE_length, eventtype);
						grid_msg_packet_body_append_parameter(&message, GRID_CLASS_CONFIG_ACTIONLENGTH_offset, GRID_CLASS_CONFIG_ACTIONLENGTH_length, 0);



						grid_msg_packet_body_append_parameter(&message, GRID_CLASS_CONFIG_ACTIONLENGTH_offset, GRID_CLASS_CONFIG_ACTIONLENGTH_length, strlen(temp));		
						grid_msg_packet_body_append_text(&message, temp);

						grid_msg_packet_body_append_printf(&message, GRID_CLASS_CONFIG_frame_end);


						//printf("CFG: %s\r\n", message.body);
						grid_msg_packet_close(&grid_msg_state, &message);
						grid_port_packet_send_everywhere(&message);
						

					}


				}
				else if (msg_class == GRID_CLASS_CONFIG_code && msg_instr == GRID_INSTR_EXECUTE_code){

                    if (position_is_local || position_is_global || position_is_me){
                        // disable hid automatically
						grid_keyboard_disable(&grid_keyboard_state);          
                        //grid_port_debug_print_text("Disabling KB");

						uint8_t vmajor = grid_msg_string_get_parameter(message, current_start+GRID_CLASS_CONFIG_VERSIONMAJOR_offset, GRID_CLASS_CONFIG_VERSIONMAJOR_length, NULL);
						uint8_t vminor = grid_msg_string_get_parameter(message, current_start+GRID_CLASS_CONFIG_VERSIONMINOR_offset, GRID_CLASS_CONFIG_VERSIONMINOR_length, NULL);
						uint8_t vpatch = grid_msg_string_get_parameter(message, current_start+GRID_CLASS_CONFIG_VERSIONPATCH_offset, GRID_CLASS_CONFIG_VERSIONPATCH_length, NULL);
									
						if (vmajor == GRID_PROTOCOL_VERSION_MAJOR && vminor == GRID_PROTOCOL_VERSION_MINOR && vpatch == GRID_PROTOCOL_VERSION_PATCH){
							// version ok	
							//printf("version ok\r\n");
						}
						else{
							//printf("error.buf.config version mismatch\r\n");
						}

						uint8_t pagenumber = grid_msg_string_get_parameter(message, current_start+GRID_CLASS_CONFIG_PAGENUMBER_offset, GRID_CLASS_CONFIG_PAGENUMBER_length, NULL);
						uint8_t elementnumber = grid_msg_string_get_parameter(message, current_start+GRID_CLASS_CONFIG_ELEMENTNUMBER_offset, GRID_CLASS_CONFIG_ELEMENTNUMBER_length, NULL);
						uint8_t eventtype = grid_msg_string_get_parameter(message, current_start+GRID_CLASS_CONFIG_EVENTTYPE_offset, GRID_CLASS_CONFIG_EVENTTYPE_length, NULL);
						uint16_t actionlength = grid_msg_string_get_parameter(message, current_start+GRID_CLASS_CONFIG_ACTIONLENGTH_offset, GRID_CLASS_CONFIG_ACTIONLENGTH_length, NULL);


						if (elementnumber == 255){
							
							elementnumber = grid_ui_state.element_list_length - 1;
						}	

						char* action = &message[current_start + GRID_CLASS_CONFIG_ACTIONSTRING_offset];

						uint8_t ack = 0; // nacknowledge by default

						if (action[actionlength] == GRID_CONST_ETX){
							
							if (actionlength < GRID_PARAMETER_ACTIONSTRING_maxlength){

							action[actionlength] = 0;
							//printf("Config: %d %d %d %d -> %s\r\n", pagenumber, elementnumber, eventtype, actionlength, action);
							
								grid_led_set_alert(&grid_led_state, GRID_LED_COLOR_WHITE, 64);
								if (pagenumber == grid_ui_state.page_activepage){

									//find event
									struct grid_ui_event* eve = grid_ui_event_find(&grid_ui_state.element_list[elementnumber], eventtype);
									
									if (eve != NULL){



										//register actionstring
										grid_ui_state.page_change_enabled = 0;
										grid_ui_event_register_actionstring(eve, action);
										//printf("Registered\r\n");
										//acknowledge
										ack = 1;

										//grid_port_debug_printf("autotrigger: %d", autotrigger);

										grid_ui_event_trigger_local(eve);	
										


									}

								}
								
								action[actionlength] = GRID_CONST_ETX;
							}
							else{
								grid_port_debug_printf("config too long");
							}

						}
						else{

							printf("Config frame invalid: %d %d %d %d end: %d %s\r\n", pagenumber, elementnumber, eventtype, actionlength, action[actionlength], message);

						}


						// Generate ACKNOWLEDGE RESPONSE
						struct grid_msg_packet response;

						grid_msg_packet_init(&grid_msg_state, &response, GRID_PARAMETER_GLOBAL_POSITION, GRID_PARAMETER_GLOBAL_POSITION);
						
						grid_msg_packet_body_append_printf(&response, GRID_CLASS_CONFIG_frame_check);
						
						grid_msg_packet_body_append_parameter(&response, GRID_CLASS_CONFIG_LASTHEADER_offset, GRID_CLASS_CONFIG_LASTHEADER_length, id);
						
						if (ack == 1){

							grid_msg_store_lastheader(&grid_msg_state, GRID_MSG_LASTHEADER_CONFIG_INDEX, id);
							grid_msg_clear_lastheader(&grid_msg_state, GRID_MSG_LASTHEADER_CONFIG_INDEX);

							grid_msg_packet_body_append_parameter(&response, GRID_INSTR_offset, GRID_INSTR_length, GRID_INSTR_ACKNOWLEDGE_code);
							grid_port_debug_printf("Config %d", id);
						}
						else{
							grid_msg_packet_body_append_parameter(&response, GRID_INSTR_offset, GRID_INSTR_length, GRID_INSTR_NACKNOWLEDGE_code);
							grid_port_debug_printf("Config Error %d", id);
						}
						
						grid_msg_packet_close(&grid_msg_state, &response);
						grid_port_packet_send_everywhere(&response);

                    }
                }		
				else if (msg_class == GRID_CLASS_CONFIG_code && msg_instr == GRID_INSTR_CHECK_code && (position_is_me || position_is_global)){
					
					uint8_t state = grid_msg_get_lastheader_state(&grid_msg_state, GRID_MSG_LASTHEADER_CONFIG_INDEX);
					uint8_t id = grid_msg_get_lastheader_id(&grid_msg_state, GRID_MSG_LASTHEADER_CONFIG_INDEX);


					struct grid_msg_packet response;	
					grid_msg_packet_init(&grid_msg_state, &response, GRID_PARAMETER_GLOBAL_POSITION, GRID_PARAMETER_GLOBAL_POSITION);
					grid_msg_packet_body_append_printf(&response, GRID_CLASS_CONFIG_frame);
					grid_msg_packet_body_append_parameter(&response, GRID_CLASS_CONFIG_LASTHEADER_offset, GRID_CLASS_CONFIG_LASTHEADER_length, id);		
				
					if (state != -1){ // ACK
						grid_msg_packet_body_append_parameter(&response, GRID_INSTR_offset, GRID_INSTR_length, GRID_INSTR_ACKNOWLEDGE_code);
					}		
					else{ // NACK
						grid_msg_packet_body_append_parameter(&response, GRID_INSTR_offset, GRID_INSTR_length, GRID_INSTR_NACKNOWLEDGE_code);
					}	

					grid_msg_packet_close(&grid_msg_state, &response);
					grid_port_packet_send_everywhere(&response);	

				}	
                else if (msg_class == GRID_CLASS_HIDKEYSTATUS_code && msg_instr == GRID_INSTR_EXECUTE_code && (position_is_me || position_is_global)){
				
                    uint8_t isenabled =	grid_msg_string_read_hex_string_value(&message[current_start+GRID_CLASS_HIDKEYSTATUS_ISENABLED_offset]		, GRID_CLASS_HIDKEYSTATUS_ISENABLED_length	, &error_flag);
					
                    if (isenabled){
						grid_keyboard_enable(&grid_keyboard_state);
					}
					else{
						grid_keyboard_disable(&grid_keyboard_state);
					}

                    
                    // Generate ACKNOWLEDGE RESPONSE
                    struct grid_msg_packet response;

                    grid_msg_packet_init(&grid_msg_state, &response, GRID_PARAMETER_GLOBAL_POSITION, GRID_PARAMETER_GLOBAL_POSITION);

                    grid_msg_packet_body_append_printf(&response, GRID_CLASS_HIDKEYSTATUS_frame);

                    grid_msg_packet_body_append_parameter(&response, GRID_CLASS_HIDKEYSTATUS_ISENABLED_offset, GRID_CLASS_HIDKEYSTATUS_ISENABLED_length, grid_keyboard_isenabled(&grid_keyboard_state));

                    grid_msg_packet_body_append_parameter(&response, GRID_INSTR_offset, GRID_INSTR_length, GRID_INSTR_ACKNOWLEDGE_code);


                    grid_msg_packet_close(&grid_msg_state, &response);
                    grid_port_packet_send_everywhere(&response);

                }
				else{
					//SORRY
				}
		
				current_start = 0;
				current_stop = 0;
			}
	
	
		}

		
	}
	
	
}



void grid_protocol_nvm_erase_succcess_callback(){

	uint8_t lastheader_id = grid_msg_get_lastheader_id(&grid_msg_state, GRID_MSG_LASTHEADER_ERASE_INDEX);
		
	grid_msg_clear_lastheader(&grid_msg_state, GRID_MSG_LASTHEADER_ERASE_INDEX);

	// Generate ACKNOWLEDGE RESPONSE
	struct grid_msg_packet response;
		
	grid_msg_packet_init(&grid_msg_state, &response, GRID_PARAMETER_GLOBAL_POSITION, GRID_PARAMETER_GLOBAL_POSITION);

	// acknowledge
	grid_msg_packet_body_append_printf(&response, GRID_CLASS_NVMERASE_frame);
	grid_msg_packet_body_append_parameter(&response, GRID_INSTR_offset, GRID_INSTR_length, GRID_INSTR_ACKNOWLEDGE_code);
	grid_msg_packet_body_append_parameter(&response, GRID_CLASS_NVMERASE_LASTHEADER_offset, GRID_CLASS_NVMERASE_LASTHEADER_length, lastheader_id);		


	// debugtext
	grid_msg_packet_body_append_printf(&response, GRID_CLASS_DEBUGTEXT_frame_start);		
	grid_msg_packet_body_append_parameter(&response, GRID_INSTR_offset, GRID_INSTR_length, GRID_INSTR_EXECUTE_code);
	grid_msg_packet_body_append_printf(&response, "xxerase complete");				
	grid_msg_packet_body_append_printf(&response, GRID_CLASS_DEBUGTEXT_frame_end);	

	grid_msg_packet_close(&grid_msg_state, &response);

	grid_port_packet_send_everywhere(&response);


		
	grid_keyboard_enable(&grid_keyboard_state);

	grid_ui_page_load(&grid_ui_state, grid_ui_page_get_activepage(&grid_ui_state));

}



void grid_protocol_nvm_clear_succcess_callback(){


	uint8_t lastheader_id = grid_msg_get_lastheader_id(&grid_msg_state, GRID_MSG_LASTHEADER_CLEAR_INDEX);
	grid_msg_clear_lastheader(&grid_msg_state, GRID_MSG_LASTHEADER_CLEAR_INDEX);

	struct grid_msg_packet response;

	grid_msg_packet_init(&grid_msg_state, &response, GRID_PARAMETER_GLOBAL_POSITION, GRID_PARAMETER_GLOBAL_POSITION);

	// acknowledge
	grid_msg_packet_body_append_printf(&response, GRID_CLASS_PAGECLEAR_frame);
	grid_msg_packet_body_append_parameter(&response, GRID_INSTR_offset, GRID_INSTR_length, GRID_INSTR_ACKNOWLEDGE_code);
	grid_msg_packet_body_append_parameter(&response, GRID_CLASS_PAGECLEAR_LASTHEADER_offset, GRID_CLASS_PAGECLEAR_LASTHEADER_length, lastheader_id);		
				
	// debugtext
	grid_msg_packet_body_append_printf(&response, GRID_CLASS_DEBUGTEXT_frame_start);		
	grid_msg_packet_body_append_parameter(&response, GRID_INSTR_offset, GRID_INSTR_length, GRID_INSTR_EXECUTE_code);
	grid_msg_packet_body_append_printf(&response, "xxclear complete");				
	grid_msg_packet_body_append_printf(&response, GRID_CLASS_DEBUGTEXT_frame_end);

	grid_msg_packet_close(&grid_msg_state, &response);
	grid_port_packet_send_everywhere(&response);



	// clear template variable after clear command
	grid_ui_page_clear_template_parameters(&grid_ui_state, grid_ui_page_get_activepage(&grid_ui_state));

	grid_ui_page_load(&grid_ui_state, grid_ui_page_get_activepage(&grid_ui_state));

}

void grid_protocol_nvm_read_succcess_callback(){

	uint8_t lastheader_id = grid_msg_get_lastheader_id(&grid_msg_state, GRID_MSG_LASTHEADER_DISCARD_INDEX);
	grid_msg_clear_lastheader(&grid_msg_state, GRID_MSG_LASTHEADER_DISCARD_INDEX);

	// Generate ACKNOWLEDGE RESPONSE
	struct grid_msg_packet response;	
	grid_msg_packet_init(&grid_msg_state, &response, GRID_PARAMETER_GLOBAL_POSITION, GRID_PARAMETER_GLOBAL_POSITION);
	grid_msg_packet_body_append_printf(&response, GRID_CLASS_PAGEDISCARD_frame);
	grid_msg_packet_body_append_parameter(&response, GRID_INSTR_offset, GRID_INSTR_length, GRID_INSTR_ACKNOWLEDGE_code);
	grid_msg_packet_body_append_parameter(&response, GRID_CLASS_PAGEDISCARD_LASTHEADER_offset, GRID_CLASS_PAGEDISCARD_LASTHEADER_length, lastheader_id);

	grid_msg_packet_body_append_printf(&response, GRID_CLASS_DEBUGTEXT_frame_start);		
	grid_msg_packet_body_append_parameter(&response, GRID_INSTR_offset, GRID_INSTR_length, GRID_INSTR_EXECUTE_code);
	grid_msg_packet_body_append_printf(&response, "xxread complete");				
	grid_msg_packet_body_append_printf(&response, GRID_CLASS_DEBUGTEXT_frame_end);

	grid_msg_packet_close(&grid_msg_state, &response);
	grid_port_packet_send_everywhere(&response);



	grid_keyboard_enable(&grid_keyboard_state);


	// phase out the animation
	grid_led_set_alert(&grid_led_state, GRID_LED_COLOR_WHITE_DIM, 100);
	grid_led_set_alert_timeout_automatic(&grid_led_state);

}

void grid_protocol_nvm_store_succcess_callback(){

	uint8_t lastheader_id = grid_msg_get_lastheader_id(&grid_msg_state, GRID_MSG_LASTHEADER_STORE_INDEX);
	grid_msg_clear_lastheader(&grid_msg_state, GRID_MSG_LASTHEADER_STORE_INDEX);


	struct grid_msg_packet response;

	grid_msg_packet_init(&grid_msg_state, &response, GRID_PARAMETER_GLOBAL_POSITION, GRID_PARAMETER_GLOBAL_POSITION);

	// acknowledge
	grid_msg_packet_body_append_printf(&response, GRID_CLASS_PAGESTORE_frame);
	grid_msg_packet_body_append_parameter(&response, GRID_INSTR_offset, GRID_INSTR_length, GRID_INSTR_ACKNOWLEDGE_code);
	grid_msg_packet_body_append_parameter(&response, GRID_CLASS_PAGESTORE_LASTHEADER_offset, GRID_CLASS_PAGESTORE_LASTHEADER_length, lastheader_id);		

	// debugtext
	grid_msg_packet_body_append_printf(&response, GRID_CLASS_DEBUGTEXT_frame_start);		
	grid_msg_packet_body_append_parameter(&response, GRID_INSTR_offset, GRID_INSTR_length, GRID_INSTR_EXECUTE_code);
	grid_msg_packet_body_append_printf(&response, "xxstore complete 0x%x", GRID_D51_NVM_LOCAL_BASE_ADDRESS + grid_plaform_get_nvm_nextwriteoffset());				
	grid_msg_packet_body_append_printf(&response, GRID_CLASS_DEBUGTEXT_frame_end);

	grid_msg_packet_close(&grid_msg_state, &response);
	grid_port_packet_send_everywhere(&response);


	//enable keyboard
	grid_keyboard_enable(&grid_keyboard_state);

	// phase out the animation
	grid_led_set_alert_timeout_automatic(&grid_led_state);

	// clear template variable after store command

	grid_ui_page_clear_template_parameters(&grid_ui_state, grid_ui_page_get_activepage(&grid_ui_state));

	// reload configuration

	grid_ui_page_load(&grid_ui_state, grid_ui_page_get_activepage(&grid_ui_state));

}


void grid_protocol_nvm_defrag_succcess_callback(){

	uint8_t lastheader_id = grid_msg_get_lastheader_id(&grid_msg_state, GRID_MSG_LASTHEADER_ERASE_INDEX);
		
	grid_msg_clear_lastheader(&grid_msg_state, GRID_MSG_LASTHEADER_ERASE_INDEX);

}

