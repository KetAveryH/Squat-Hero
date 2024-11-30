module vga(input logic clk, reset,
		   output logic vgaclk, // 25 MHz VGA clock  25.175MHz
		   output logic hsync, vsync,
		   output logic sync_b, blank_b, // to monitor & DAC
		   output logic [3:0] r, g, b); // to video DAC
	
	logic [9:0] x, y; 
	
	logic int_osc;
	logic pll_locked;
    logic [24:0] counter;
  
    // Internal high-speed oscillator
    HSOSC #(.CLKHF_DIV(2'b00)) 
         hf_osc (.CLKHFPU(1'b1), .CLKHFEN(1'b1), .CLKHF(int_osc));
		 
	// Instantiate the PLL
    no_lock_pll pll_inst (
        .ref_clk_i(int_osc),             // Input clock (48 MHz)
        .rst_n_i(~reset),            // Active-low reset
        //.lock_o(pll_locked),         // PLL lock signal
        .outcore_o(vgaclk),          // Core clock output (25 MHz)
        .outglobal_o()               // Global clock output (not used)
    );
   
	// generate monitor timing signals
	vgaController vgaCont(vgaclk, reset, hsync, vsync, sync_b, blank_b, x, y);
	
	// userdefined module to determine pixel color
	//videoGen videoGen(x, y, r, g, b);
	
	// Instantiate videoGen with blank_b
	videoGen videoGen(
		.vgaclk(vgaclk),
		.reset(reset),
		.x(x),
		.y(y),
		.blank_b(blank_b),
		.r(r),
		.g(g),
		.b(b)
	);


endmodule

module vgaController #(parameter HBP     = 10'd48,  // horizontal back porch
								  HACTIVE = 10'd640, // number of pixels per line
								  HFP     = 10'd16,  // horizontal front porch
								  HSYN    = 10'd96,  // horizontal sync pulse = 60 to move
												     // electron gun back to left 
								  // number of horizontal pixels (i.e., clock cycles)
								  HMAX = HBP + HACTIVE + HFP + HSYN, //48+640+16+96=800:
								  VBP = 10'd32, // vertical back porch
								  VACTIVE = 10'd480, // number of lines
								  VFP = 10'd11, // vertical front porch
								  VSYN = 10'd2, // vertical sync pulse = 2 to move
												// electron gun back to top
								  // number of vertical pixels (i.e., clock cycles)
								  VMAX = VBP + VACTIVE + VFP + VSYN) //32+480+11+2=525:
	(input logic vgaclk, reset,
	 output logic hsync, vsync, sync_b, blank_b,
	 output logic [9:0] hcnt, vcnt);

	 // counters for horizontal and vertical positions
	 always @(posedge vgaclk, posedge reset) begin
	   if (reset) begin
		 hcnt <= 0;
	     vcnt <= 0;
	   end
       else begin
	     hcnt <= hcnt + 1;
		   if (hcnt == HMAX) begin
		     hcnt <= 0;
			     vcnt <= vcnt + 1;
                 if (vcnt == VMAX)
					vcnt <= 0;
		   end
		end
	  end
	// compute sync signals (active low)
	assign hsync = ~( (hcnt >= (HACTIVE + HFP)) & (hcnt < (HACTIVE + HFP + HSYN)) );
	assign vsync = ~( (vcnt >= (VACTIVE + VFP)) & (vcnt < (VACTIVE + VFP + VSYN)) );
	assign sync_b = 1'b0; // this should be 0 for newer monitors
						  // for older monitors, use: assign sync_b = hsync & vsync;
	// assign sync_b = hsync & vsync;
	// force outputs to black when not writing pixels
	assign blank_b = (hcnt < HACTIVE) & (vcnt < VACTIVE);
endmodule

//module videoGen(input logic [9:0] x, y, input logic blank_b, output logic [3:0] r, g, b);
  //logic pixel, inrect;
    //given y position, choose a character to display
    //then look up the pixel value from the character ROM
     //and display it in red or blue. Also draw a green rectangle.
  //always_comb begin
    //assign {g, b} = (y[3] == 0) ? {{4{pixel}}, 4'h0} : {4'h0, {4{pixel}}};
    //assign r = inrect ? 4'hF : 4'h0; // Maximum value for 4 bits is 4'hF (15 in decimal)
  //chargenrom chargenromb(y[8:3]+8'd65, x[2:0], y[2:0], pixel);
  //rectgen rectgen(x, y, 10'd120, 10'd150, 10'd200, 10'd230, inrect);
  
module moving_box #(
    parameter BOX_WIDTH = 10'd100  // Define as parameter
)(
    input  logic        vgaclk,
    output logic [9:0]  left,
    output logic [9:0]  right,
    output logic [9:0]  top,
    output logic [9:0]  bot
);
    
    logic [31:0] counter;
    
    // Fixed vertical position assignments
    assign top = 10'd150;
    assign bot = 10'd230;
    
    always_ff @(posedge vgaclk) begin  // Added reset to sensitivity list
        if (counter >= 1_000_000) begin
            counter <= 0;
            if (left <= (10'd48 + 10'd480)) begin
                left <= left + 1'd1;    
                right <= right + 1'd1;  
            end else begin
                left <= 10'd48;
                right <= 10'd48 + BOX_WIDTH;
            end
        end else begin
            counter <= counter + 1'd1;
        end
    end
    
endmodule

module videoGen(
    input  logic        vgaclk,     // Add vgaclk input
    input  logic        reset,      // Add reset input
    input  logic [9:0]  x, y,
    input  logic        blank_b,
    output logic [3:0]  r, g, b
);
    logic pixel, inrect;
    logic [9:0] box_left, box_right, box_top, box_bot;  // Add signals to connect modules

    // Instantiate the character generator ROM
    chargenrom chargenromb(
        .ch(y[8:3] + 8'd65),
        .xoff(x[2:0]),
        .yoff(y[2:0]),
        .pixel(pixel)
    );

    // Instantiate moving box with connections
    moving_box #(.BOX_WIDTH(10'd10)) moving_box_inst(
        .vgaclk(vgaclk),
        .left(box_left),
        .right(box_right),
        .top(box_top),
        .bot(box_bot)
    );

    // Use the moving box coordinates for rectgen
    rectgen rectgen_inst(
        .x(x),
        .y(y),
        .left(box_left),    // Use moving box coordinates
        .right(box_right),  // instead of fixed values
        .top(box_top),
        .bot(box_bot),
        .inrect(inrect)
    );

    

    // Use an always_comb block for conditional assignments
    always_comb begin
        if (blank_b == 0) begin
            // During blanking periods, set all pixel outputs to zero
            r = 4'h0;
            g = 4'h0;
            b = 4'h0;
        end else begin
            // When not blanking, apply your original logic
            // Assign green and blue channels based on pixel and y[3]
            if (y[3] == 0) begin
                g = {4{pixel}};
                b = 4'h0;
            end else begin
                g = 4'h0;
                b = {4{pixel}};
            end
            // Assign red channel based on inrect
            r = inrect ? 4'hF : 4'h0; // Maximum value for 4 bits is 4'hF (15 in decimal)
        end
    end
endmodule

//endmodule
//module videoGen(input logic [9:0] x, y, output logic [3:0] r, g, b);
   //Always display green
  //assign r = 4'b0000;   // Red channel is 0
  //assign g = 4'b0001;   // Green channel is max (15 for 4 bits)
  //assign b = 4'b0000;   // Blue channel is 0

//endmodule
//module videoGen(
    //input logic [9:0] x, y,
    //input logic blank_b,
    //output logic [3:0] r, g, b
//);
  // Always display green when in active area
  //always @(*) begin
    //if (blank_b) begin
      //r = 4'b1111; // Red channel is 0
      //g = 4'b1111; // Green channel is max (15 for 4 bits)
      //b = 4'b1111; // Blue channel is 0
    //end else begin
       //During porch periods, set outputs to zero
      //r = 4'b0000;
      //g = 4'b0000;
      //b = 4'b0000;
    //end
  //end
//endmodule


module chargenrom(input logic [7:0] ch,
                  input logic [2:0] xoff, yoff,
                  output logic pixel);
  
  logic [5:0] charrom[2047:0]; // character generator ROM
  logic [7:0] line; // a line read from the ROM
  // initialize ROM with characters from text file
  initial $readmemb("charrom.txt", charrom);
  
  // index into ROM to find line of character
   assign line = charrom[yoff+{ch - 65, 3'b000}]; // subtract 65 because A is entry 0

												 // is entry 0
  // reverse order of bits
  assign pixel = line[3'd7-xoff];
endmodule

module rectgen(input logic [9:0] x, y, left, top, right, bot,
               output logic inrect);
  assign inrect = (x >= left & x < right & y >= top & y < bot);
endmodule