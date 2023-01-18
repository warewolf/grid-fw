#include "grid_ui_back.h"






void grid_port_process_ui_local(struct grid_ui_model* ui){


		
	// Prepare packet header LOCAL
	struct grid_msg_packet message_local;
	grid_msg_packet_init(&grid_msg_state, &message_local, GRID_PARAMETER_DEFAULT_POSITION, GRID_PARAMETER_DEFAULT_POSITION);
	uint8_t payload_local[GRID_PARAMETER_PACKET_maxlength] = {0};				
	uint32_t offset_local=0;		

	// Prepare packet header GLOBAL
	struct grid_msg_packet message_global;
	grid_msg_packet_init(&grid_msg_state, &message_global, GRID_PARAMETER_DEFAULT_POSITION, GRID_PARAMETER_DEFAULT_POSITION);
	uint8_t payload_global[GRID_PARAMETER_PACKET_maxlength] = {0};				
	uint32_t offset_global=0;
	
	
	
	// UI STATE

	for (uint8_t j=0; j<ui->element_list_length; j++){
		
		for (uint8_t k=0; k<ui->element_list[j].event_list_length; k++){
		
			if (offset_local>GRID_PARAMETER_PACKET_marign || offset_global>GRID_PARAMETER_PACKET_marign){
				continue;
			}		
			else{
				
				CRITICAL_SECTION_ENTER()
				if (grid_ui_event_istriggered_local(&ui->element_list[j].event_list[k])){
					
					offset_local += grid_ui_event_render_action(&ui->element_list[j].event_list[k], &payload_local[offset_local]);
					grid_ui_event_reset(&ui->element_list[j].event_list[k]);


					// automatically report elementname after config
					if (j<ui->element_list_length-1){
						

							uint8_t number = j;
							uint8_t command[20] = {0};

							sprintf(command, "gens(%d,ele[%d]:gen())", j, j);

							// lua get element name
							grid_lua_clear_stdo(&grid_lua_state);
							grid_lua_dostring(&grid_lua_state, command);
							strcat(payload_global, grid_lua_get_output_string(&grid_lua_state));
							grid_lua_clear_stdo(&grid_lua_state);

					}
					

				
				}
				CRITICAL_SECTION_LEAVE()
				
			}
		
		}
		
		

		
	}

	if (strlen(payload_global)>0){

		grid_msg_packet_body_append_text(&message_global, payload_global);
		grid_msg_packet_close(&grid_msg_state, &message_global);
		grid_port_packet_send_everywhere(&message_global);
	}

	
	grid_msg_packet_body_append_text(&message_local, payload_local);


	grid_msg_packet_close(&grid_msg_state, &message_local);
		
	uint32_t message_length = grid_msg_packet_get_length(&message_local);
		
	// Put the packet into the UI_TX buffer
	if (grid_buffer_write_init(&ui->port->tx_buffer, message_length)){
			
		for(uint32_t i = 0; i<message_length; i++){
				
			grid_buffer_write_character(&ui->port->tx_buffer, grid_msg_packet_send_char_by_char(&message_local, i));
		}
			
		grid_buffer_write_acknowledge(&ui->port->tx_buffer);
		
// 			uint8_t debug_string[200] = {0};
// 			sprintf(debug_string, "Space: RX: %d/%d  TX: %d/%d", grid_buffer_get_space(&ui->port->rx_buffer), GRID_BUFFER_SIZE, grid_buffer_get_space(&ui->port->tx_buffer), GRID_BUFFER_SIZE);
// 			grid_port_debug_print_text(debug_string);


	}
	else{
		// LOG UNABLE TO WRITE EVENT
	}
		





}


void grid_port_process_ui(struct grid_ui_model* ui){
	


	struct grid_msg_packet message;
	grid_msg_packet_init(&grid_msg_state, &message, GRID_PARAMETER_GLOBAL_POSITION, GRID_PARAMETER_GLOBAL_POSITION);
	

	for (uint8_t j=0; j<ui->element_list_length; j++){
	
		for (uint8_t k=0; k<ui->element_list[j].event_list_length; k++){ //j=1 because init is local
		
			if (grid_msg_packet_get_length(&message)>GRID_PARAMETER_PACKET_marign){
				continue;
			}		
			else{
							
				if (grid_ui_event_istriggered(&ui->element_list[j].event_list[k])){

					uint32_t offset = grid_msg_packet_body_get_length(&message); 

					message.body_length += grid_ui_event_render_event(&ui->element_list[j].event_list[k], &message.body[offset]);
				
					offset = grid_msg_packet_body_get_length(&message); 

					CRITICAL_SECTION_ENTER()
					message.body_length += grid_ui_event_render_action(&ui->element_list[j].event_list[k], &message.body[offset]);
					grid_ui_event_reset(&ui->element_list[j].event_list[k]);
					CRITICAL_SECTION_LEAVE()
					

				}
				
			}
		
		}
	}
	

	

	grid_msg_packet_close(&grid_msg_state, &message);
	uint32_t length = grid_msg_packet_get_length(&message);
	

	// Put the packet into the UI_RX buffer
	if (grid_buffer_write_init(&ui->port->rx_buffer, length)){

		for(uint16_t i = 0; i<length; i++){
			
			grid_buffer_write_character(&ui->port->rx_buffer, grid_msg_packet_send_char_by_char(&message, i));
		}
		
		grid_buffer_write_acknowledge(&ui->port->rx_buffer);

		
	}
	else{
		// LOG UNABLE TO WRITE EVENT
	}

	

	// LEDREPORT
	if (grid_protocol_led_change_report_length(&grid_led_state) && grid_sys_get_editor_connected_state(&grid_sys_state)){

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







	
}



uint8_t grid_ui_page_load(struct grid_ui_model* ui, uint8_t page){

	/*
	
		Reset encoder mode
		Reset button mode
		Reset potmeter mode/reso
		Reset led animations

	*/

	// reset all of the state parameters of all leds
	grid_led_reset(&grid_led_state);

	uint8_t oldpage = ui->page_activepage;
	ui->page_activepage = page;
	// Call the page_change callback

	//grid_platform_printf("LOAD PAGE: %d\r\n", page);

	for (uint8_t i = 0; i < ui->element_list_length; i++)
	{	

		struct grid_ui_element* ele = &ui->element_list[i];

		ele->timer_event_helper = 0; // stop the event's timer

		// clear all of the pending events for the element
		for (uint8_t j = 0; j < ele->event_list_length; j++){
			struct grid_ui_event* eve = &ele->event_list[j];

			grid_ui_event_reset(eve);

		}	

		// if (ele->template_initializer!=NULL){
		// 	ele->template_initializer(ele->template_buffer_list_head);
		// }


		uint8_t template_buffer_length = grid_ui_template_buffer_list_length(ele);

		//if (i==0) grid_platform_printf("TB LEN: %d\r\n", template_buffer_length);
		while (template_buffer_length < page+1){

			grid_platform_printf("$"); // emergency allocation
			grid_ui_template_buffer_create(ele);

			template_buffer_length = grid_ui_template_buffer_list_length(ele);

			//if (i==0) grid_platform_printf("CREATE NEW, LEN: %d\r\n", template_buffer_length);
		}



		//struct grid_ui_template_buffer* buf =  grid_ui_template_buffer_find(ele, page==0?1:page);
		struct grid_ui_template_buffer* buf =  grid_ui_template_buffer_find(ele, page);
		
		if (buf == NULL){

			grid_platform_printf("error.template buffer is invalid\r\n");
			grid_port_debug_print_text("error.template buffer is invalid");

		}
		else{

			// load the template parameter list
			ele->template_parameter_list = buf->template_parameter_list;

			if (buf->template_parameter_list == NULL){
				grid_port_debug_print_text("NULL");
			}


			if (i<4 && 0){


				for (uint8_t j = 0; j<13; j++){
					grid_platform_printf("%d:%d ", j, ele->template_parameter_list[j]);

				}
				grid_platform_printf("\r\n");

				grid_port_debug_printf("Val[%d]: %d", i, ele->template_parameter_list[GRID_LUA_FNC_E_ENCODER_VALUE_index]);

			}



		}


		if (ele->page_change_cb != NULL){

			ele->page_change_cb(ele, oldpage, page);
		}

	}


	grid_platform_printf("STOP\r\n");
	grid_lua_stop_vm(&grid_lua_state);
	grid_platform_printf("START\r\n");
	grid_lua_start_vm(&grid_lua_state);

	grid_ui_bulk_pageread_init(ui, &grid_ui_page_load_success_callback);

	
}

void grid_ui_page_load_success_callback(void){

	grid_keyboard_enable(&grid_keyboard_state);


	// phase out the animation
	grid_led_set_alert(&grid_led_state, GRID_LED_COLOR_WHITE_DIM, 100);
	grid_led_set_alert_timeout_automatic(&grid_led_state);
	
}








// ==================== BULK OPERATIONS ======================== //


void grid_ui_bulk_pageread_init(struct grid_ui_model* ui, void (*success_cb)()){


	ui->read_success_callback = success_cb;


	ui->read_bulk_last_element = 0;
	ui->read_bulk_last_event = -1;

	ui->read_bulk_status = 1;
			
}



void grid_ui_bulk_pageread_next(struct grid_ui_model* ui){
	
	if (!grid_ui_bulk_pageread_is_in_progress(ui)){
		return;
	}

	if (!grid_platform_get_nvm_state()){
		return;
	}

	grid_lua_gc_collect(&grid_lua_state);


	uint8_t last_element_helper = ui->read_bulk_last_element;
	uint8_t last_event_helper = ui->read_bulk_last_event + 1;

	uint8_t firstrun = 1;
	


	//grid_platform_printf("BULK READ PAGE LOAD %d %d\r\n", last_element_helper, last_event_helper);

	for (uint8_t i=last_element_helper; i<ui->element_list_length; i++){

		struct grid_ui_element* ele = &ui->element_list[i];



		for (uint8_t j=0; j<ele->event_list_length; j++){

			if (firstrun){

				j = last_event_helper;
				firstrun = 0;
							
				if (j>=ele->event_list_length){ // to check last_event_helper in the first iteration
					break;
				}
			}			

			struct grid_ui_event* eve = &ele->event_list[j];

			// file pointer
			void* entry = NULL;
			
			entry = grid_platform_find_actionstring_file(ui->page_activepage, ele->index, eve->type);

			if (entry != NULL){
				
				//grid_platform_printf("Page Load: FOUND %d %d %d 0x%x (+%d)!\r\n", entry->page_id, entry->element_id, entry->event_type, entry->config_string_offset, entry->config_string_length);

				uint16_t length = grid_platform_get_actionstring_file_size(entry);

				if (length){
					uint8_t temp[GRID_PARAMETER_ACTIONSTRING_maxlength + 100] = {0};

					grid_platform_read_actionstring_file_contents(entry, temp);
					grid_ui_event_register_actionstring(eve, temp);
					
					// came from default so no need to store it in eve->action_string
					grid_ui_event_free_actionstring(eve);
					eve->cfg_changed_flag = 0; // clear changed flag
				}
				else{
					//grid_platform_printf("Page Load: NULL length\r\n");
				}
				
			}
			else{
				
				//grid_platform_printf("Page Load: NOT FOUND, Set default!\r\n");


				uint8_t temp[GRID_PARAMETER_ACTIONSTRING_maxlength + 100] = {0};



				grid_ui_event_generate_actionstring(eve, temp);
				grid_ui_event_register_actionstring(eve, temp);


				// came from TOC so no need to store it in eve->action_string
				grid_ui_event_free_actionstring(eve);
				eve->cfg_changed_flag = 0; // clear changed flag

			}


			if (eve->type == GRID_UI_EVENT_INIT){

				grid_ui_event_trigger_local(eve);
			}

			ui->read_bulk_last_element = i;
			ui->read_bulk_last_event = j;

			return;

		}

	}
	


	if (ui->read_success_callback != NULL){
		ui->read_success_callback();
		ui->read_success_callback = NULL;
	}

	ui->read_bulk_status = 0;


}


void grid_ui_bulk_pagestore_init(struct grid_ui_model* ui, void (*success_cb)()){

	ui->store_success_callback = success_cb;

	ui->store_bulk_status = 1;



}

uint8_t grid_ui_bulk_pagestore_is_in_progress(struct grid_ui_model* ui){

	return ui->store_bulk_status;
	
}

// DO THIS!!
void grid_ui_bulk_pagestore_next(struct grid_ui_model* ui){

 	if (!grid_ui_bulk_pagestore_is_in_progress(ui)){
		return;
	}

	if (!grid_platform_get_nvm_state()){
		return;
	}



	for (uint8_t i=0; i<ui->element_list_length; i++){

		struct grid_ui_element* ele = &ui->element_list[i];

		for (uint8_t j=0; j<ele->event_list_length; j++){

			struct grid_ui_event* eve = &ele->event_list[j];

			if (eve->cfg_changed_flag){
				
				if (eve->cfg_default_flag){

					void* entry = grid_platform_find_actionstring_file(ele->parent->page_activepage, ele->index, eve->type);
					
					if (entry != NULL){
						//grid_platform_printf("DEFAULT, FOUND - SO DESTROY! %d %d\r\n", ele->index, eve->index);

						
						grid_platform_delete_actionstring_file(entry);

					}
					else{

						//grid_platform_printf("DEFAULT BUT NOT FOUND! %d %d\r\n", ele->index, eve->index);
					}

					// its reverted to default, so no need to keep it in eve->action_string
					grid_ui_event_free_actionstring(eve);
	
				}
				else{
					
					//grid_platform_printf("NOT DEFAULT! %d %d\r\n", ele->index, eve->index);

					grid_platform_write_actionstring_file(ele->parent->page_activepage, ele->index, eve->type, eve->action_string, strlen(eve->action_string));

					// now its stored in TOC so no need to keep it in eve->action_string
					grid_ui_event_free_actionstring(eve);

				}

				eve->cfg_changed_flag = 0; // clear changed flag

				// after the first successful store quit from this function
				return;
			}

		}

	}

	// if every change was previously stored then execution will continue from here

	if (ui->store_success_callback != NULL){
		ui->store_success_callback();
		ui->store_success_callback = NULL;
	}

	ui->store_bulk_status = 0;
}




void grid_ui_bulk_pageclear_init(struct grid_ui_model* ui, void (*success_cb)()){

	ui->clear_success_callback = success_cb;


	ui->clear_bulk_status = 1;

}

uint8_t grid_ui_bulk_pageclear_is_in_progress(struct grid_ui_model* ui){

	return ui->clear_bulk_status;
	
}

// DO THIS!!
void grid_ui_bulk_pageclear_next(struct grid_ui_model* ui){

 	if (!grid_ui_bulk_pageclear_is_in_progress(ui)){
		return;
	}

	if (!grid_platform_get_nvm_state()){
		return;
	}

	grid_platform_clear_actionstring_files_from_page(ui->page_activepage);

	
	if (ui->clear_success_callback != NULL){
		ui->clear_success_callback();
		ui->clear_success_callback = NULL;
	}

	ui->clear_bulk_status = 0;



		
}


void grid_ui_bulk_nvmerase_init(struct grid_ui_model* ui, void (*success_cb)()){


	ui->erase_success_callback = success_cb;



	grid_platform_delete_actionstring_files_all();



	ui->erase_bulk_status = 1;

}

uint8_t grid_ui_bulk_nvmerase_is_in_progress(struct grid_ui_model* ui){

	return ui->erase_bulk_status;
	
}


void grid_ui_bulk_nvmerase_next(struct grid_ui_model* ui){

	if (!grid_ui_bulk_nvmerase_is_in_progress(ui)){
		return;
	}

	if (!grid_platform_get_nvm_state()){
		return;
	}

	// flash_erase takes 32 ms so no need to implement timeout here, only erase one block at a time!

	if(grid_platform_erase_nvm_next()){

		// there is still stuff to be erased
	}
	else{ // done with the erase

		// call success callback
		if (ui->erase_success_callback != NULL){

			ui->erase_success_callback();
			ui->erase_success_callback = NULL;
		}



		ui->erase_bulk_status = 0;


	}

	
}


uint8_t grid_ui_bluk_anything_is_in_progress(struct grid_ui_model* ui){

	if (grid_ui_bulk_nvmerase_is_in_progress(ui)){
		return 1;
	}
	
	if (grid_ui_bulk_pageclear_is_in_progress(ui)){
		return 1;
	}

	if (grid_ui_bulk_pageread_is_in_progress(ui)){
		return 1;
	}
	
	if (grid_ui_bulk_pagestore_is_in_progress(ui)){
		return 1;
	}

	// if (grid_d51_nvm_ui_bulk_nvmdefrag_is_in_progress(nvm, ui)){
	// 	return 1;
	// }

	// Return 0 if nothing is in progress
	return 0;


}



