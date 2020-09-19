// File name: xlr8_hdmi.v
// Author:    Paul Clark
// Based on:  xlr8_wrap_template.v
// By:        Steve Phillips

module xlr8_hdmi  // NOTE: Change the module name to match your design
  #(
    // Parameter definitions. The ADDR parameters will be defined when
    // this module is instantiated.
    parameter VOLUME_ADDR = 0,      // Address of the register that will hold the volume attenuation value
	 parameter RAM_ADDRESS_LO = 0,   // Address of the register that will hold the RAM address (lo byte)
	 parameter RAM_ADDRESS_HI = 0,   // Address of the register that will hold the RAM address (hi byte)
	 parameter RAM_CHAR_DATA = 0,		// Address of the register that will hold the RAM character data
	 parameter RAM_ATTR_DATA = 0,		// Address of the register that will hold the RAM attribute data
	 parameter RAM_ROW_OFFSET = 0,	// Address of the register that will hold the RAM row offset (for fast display updates)
    parameter WIDTH = 8
    )
   (
    // Input/Ouput definitions for the module. These are standard and
    // while other ports could be added, these are required.
    //  
    // Clock and Reset
    input        clk_pixel, // 252MHz Pixel Clock
	 input        clk_audio, // 48kHz Audio Clock
	 input        clk_core,  // 16MHz core clock
    input        rstn, //      Reset
    input        clken, //     Clock Enable
	 
    // I/O 
    input [7:0]  dbus_in, //   Data Bus Input
    output [7:0] dbus_out, //  Data Bus Output
    output       io_out_en, // IO Output Enable
	 
    output [2:0] datap,			// HDMI RGB Data Pins (+ve)
    output [2:0] datan,			// HDMI RGB Data Pins (-ve)
    output clkp,					// HDMI Clock (+ve)
    output clkn,					// HDMI Clock (-ve)
	 output heartbeat,			// Heartbeat
	 
    // DM
    input [7:0]  ramadr, //    RAM Address
    input        ramre, //     RAM Read Enable
    input        ramwe, //     RAM Write Enable
    input        dm_sel //    DM Select
    );
   
   //======================================================================
   // Interfaces to the user module:
   
	logic volume_sel;
	logic volume_we;
	logic volume_re;
	logic [WIDTH-1:0] volume_reg; // The register that will hold the red component of pixels that are on

	logic ram_address_lo_sel;
	logic ram_address_lo_we;
	logic ram_address_lo_re;
	logic [WIDTH-1:0] ram_address_lo_reg; // The register that will hold the ram address (lo byte)

	logic ram_address_hi_sel;
	logic ram_address_hi_we;
	logic ram_address_hi_re;
	logic [WIDTH-1:0] ram_address_hi_reg; // The register that will hold the ram address (hi byte)

	logic ram_char_data_sel;
	logic ram_char_data_we;
	logic ram_char_data_re;
	logic [7:0] ram_char_q_reg;
	
	logic ram_char_attr_sel;
	logic ram_char_attr_we;
	logic ram_char_attr_re;
	logic [7:0] ram_attr_q_reg;
	
	logic ram_row_offset_sel;
	logic ram_row_offfset_we;
	logic ram_row_offset_re;
	logic [WIDTH-1:0] ram_row_offset_reg; // The register that will hold the ram row offset

	logic [7:0] hdmi_ram_char_data;
	logic [7:0] hdmi_ram_attr_data;
	logic [12:0] hdmi_ram_address;
	logic hdmi_ram_re;
	
   // End, interfaces
   //----------------------------------------------------------------------

   
   //======================================================================
   //  Control select
   //
   // For each register interface, do control select based on address
   assign volume_sel = dm_sel && (ramadr == VOLUME_ADDR);
   assign volume_we  = volume_sel && ramwe;
   assign volume_re  = volume_sel && ramre;
   
   assign ram_address_lo_sel = dm_sel && (ramadr == RAM_ADDRESS_LO);
   assign ram_address_lo_we  = ram_address_lo_sel && ramwe;
   assign ram_address_lo_re  = ram_address_lo_sel && ramre;
   
   assign ram_address_hi_sel = dm_sel && (ramadr == RAM_ADDRESS_HI);
   assign ram_address_hi_we  = ram_address_hi_sel && ramwe;
   assign ram_address_hi_re  = ram_address_hi_sel && ramre;
   
   assign ram_char_data_sel = dm_sel && (ramadr == RAM_CHAR_DATA);
   assign ram_char_data_we  = ram_char_data_sel && ramwe;
   assign ram_char_data_re  = ram_char_data_sel && ramre;
	
   assign ram_attr_data_sel = dm_sel && (ramadr == RAM_ATTR_DATA);
   assign ram_attr_data_we  = ram_attr_data_sel && ramwe;
   assign ram_attr_data_re  = ram_attr_data_sel && ramre;
	
   assign ram_row_offset_sel = dm_sel && (ramadr == RAM_ROW_OFFSET);
   assign ram_row_offset_we  = ram_row_offset_sel && ramwe;
   assign ram_row_offset_re  = ram_row_offset_sel && ramre;
	
	// Mux the data and enable outputs
   assign dbus_out =  ({8{  volume_sel }} &  volume_reg ) |
							 ({8{  ram_address_lo_sel }} &  ram_address_lo_reg ) |
							 ({8{  ram_address_hi_sel }} &  ram_address_hi_reg ) |
							 ({8{  ram_char_data_sel }} &  ram_char_q_reg ) |
							 ({8{  ram_attr_data_sel }} &  ram_attr_q_reg ) |
							 ({8{  ram_row_offset_sel }} &  ram_row_offset_reg );

   assign io_out_en = volume_re ||
							 ram_address_lo_re ||
							 ram_address_hi_re ||
							 ram_char_data_re ||
							 ram_attr_data_re ||
							 ram_row_offset_re;

   // End, Control Select
   //----------------------------------------------------------------------
   

   //======================================================================
   // Load write data from AVR core into registers
   //
   // For data written from the AVR core to the user module, you may
   // want to register the value here so that it is held for reference
   // until the net update in value

   always @(posedge clk_core) begin
		if (clken && volume_we) begin
        volume_reg <= dbus_in[WIDTH-1:0];
      end
   end
   
   always @(posedge clk_core) begin
		if (clken && ram_address_lo_we) begin
        ram_address_lo_reg <= dbus_in[WIDTH-1:0];
      end
   end
   
   always @(posedge clk_core) begin
		if (clken && ram_address_hi_we) begin
        ram_address_hi_reg <= dbus_in[WIDTH-1:0];
      end
   end
   
   always @(posedge clk_core) begin
		if (clken && ram_row_offset_we) begin
        ram_row_offset_reg <= dbus_in[WIDTH-1:0];
      end
   end
   
   // End, Load write data
   //----------------------------------------------------------------------
   
   
   //======================================================================
   // Instantiate RAM 2-PORT for the character data
	// We need 640x480 at 8x16 pixels per character = 80x30 chars = 2400 bytes
	// The cx and cy counters are both 10 bit
	// But We don't care about bits cx[2:0] and cy[3:0]
	// So to keep things easy, we use 13 bit addressing = 8K of dual port RAM
   
   ram char_ram_inst (
                    .clock		   (clk_pixel),
						  .data_a		(dbus_in),
						  .q_a			(ram_char_q_reg),
						  .address_a	({ram_address_hi_reg[4:0], ram_address_lo_reg}),
						  .wren_a		(ram_char_data_we), // This needs more work. I'm not sure if the we needs to be delayed or offset?
						  .rden_a		(ram_char_data_re), // This needs more work. I'm not sure if the re needs to be delayed or offset?
						  .data_b		(8'b0),
						  .q_b			(hdmi_ram_char_data),
						  .address_b	(hdmi_ram_address),
						  .wren_b		(1'b0),
						  .rden_b		(hdmi_ram_char_re) // This needs more work. re is currently bodged as (clock / 8)
                    );

   // Instantiate RAM 2-PORT for the attribute data
   
   ram attr_ram_inst (
                    .clock		   (clk_pixel),
						  .data_a		(dbus_in),
						  .q_a			(ram_attr_q_reg),
						  .address_a	({ram_address_hi_reg[4:0], ram_address_lo_reg}),
						  .wren_a		(ram_attr_data_we), // This needs more work. I'm not sure if the we needs to be delayed or offset?
						  .rden_a		(ram_attr_data_re), // This needs more work. I'm not sure if the re needs to be delayed or offset?
						  .data_b		(8'b0),
						  .q_b			(hdmi_ram_attr_data),
						  .address_b	(hdmi_ram_address),
						  .wren_b		(1'b0),
						  .rden_b		(hdmi_ram_attr_re) // This needs more work. re is currently bodged as (clock / 8)
                    );
   
   // End, Instantiate user module
   //----------------------------------------------------------------------
   
   //======================================================================
   // Instantiate user module
   //
   // Below is an example instantiation of a simple user module. It
   // provides just enough I/O to demonstrate how the above logic is
   // connected.
   
   hdmi_demo hdmi_inst (
                    .CLOCK_PIXEL   	(clk_pixel),
						  .CLOCK_AUDIO		(clk_audio),
						  .CLOCK_CORE     (clk_core),
						  .HDMI_TX     	(datap),
	                 .HDMI_TX_N		(datan),
						  .HDMI_CLK			(clkp),
						  .HDMI_CLK_N		(clkn),
						  .VOLUME			(volume_reg),
						  .heartbeat		(heartbeat),
						  .RAM_ADDRESS		(hdmi_ram_address),
						  .RAM_CHAR_DATA	(hdmi_ram_char_data),
						  .RAM_ATTR_DATA	(hdmi_ram_attr_data),
						  .RAM_CHAR_RE		(hdmi_ram_char_re),
						  .RAM_ATTR_RE		(hdmi_ram_attr_re),
						  .RAM_ROW_OFFSET (ram_row_offset_reg)
                    );
   
   // End, Instantiate user module
   //----------------------------------------------------------------------
   
endmodule

