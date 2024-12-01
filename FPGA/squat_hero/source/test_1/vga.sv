module vga(input logic reset,
		   input logic frame_switch,
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
        //.lock_o(pll_locked),         // PLL lock signal  // TODO Experiment with turning this o
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
		.frame_switch(frame_switch),  
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

typedef struct packed {
    logic [9:0] left;
    logic [9:0] right;
    logic [9:0] top;
    logic [9:0] bot;
} rect_t;

module image_frame0(
    output rect_t [13:0] frame0_rects
);
    // Use assign statements instead of procedural blocks
    assign frame0_rects[0] = '{left: 10'd20, right: 10'd340, top: 10'd100, bot: 10'd140};  // Head
    assign frame0_rects[1] = '{left: 10'd310, right: 10'd330, top: 10'd140, bot: 10'd220};  // Body
    assign frame0_rects[2] = '{left: 10'd270, right: 10'd310, top: 10'd150, bot: 10'd170};  // Left Arm
    assign frame0_rects[3] = '{left: 10'd330, right: 10'd370, top: 10'd150, bot: 10'd170};  // Right Arm
    assign frame0_rects[4] = '{left: 10'd310, right: 10'd320, top: 10'd220, bot: 10'd300};  // Left Leg
    assign frame0_rects[5] = '{left: 10'd320, right: 10'd330, top: 10'd220, bot: 10'd300};  // Right Leg
    
    // Initialize remaining rectangles to 0
    genvar i;
    generate
        for (i = 6; i < 14; i = i + 1) begin : init_rects
            assign frame0_rects[i] = '{left: 10'd0, right: 10'd0, top: 10'd0, bot: 10'd0};
        end
    endgenerate
endmodule

module image_frame1(
    output rect_t [13:0] frame1_rects
);

    assign frame1_rects[0] = '{left: 10'd0, right: 10'd340, top: 10'd100, bot: 10'd140};  // Head
    assign frame1_rects[1] = '{left: 10'd310, right: 10'd330, top: 10'd140, bot: 10'd220};  // Body
    assign frame1_rects[2] = '{left: 10'd270, right: 10'd310, top: 10'd150, bot: 10'd170};  // Left Arm
    assign frame1_rects[3] = '{left: 10'd330, right: 10'd370, top: 10'd150, bot: 10'd170};  // Right Arm
    assign frame1_rects[4] = '{left: 10'd310, right: 10'd320, top: 10'd220, bot: 10'd300};  // Left Leg
    assign frame1_rects[5] = '{left: 10'd270, right: 10'd330, top: 10'd220, bot: 10'd300};  // Right Leg

    genvar i;
    generate
        for (i = 6; i < 14; i = i + 1) begin : init_rects
            assign frame1_rects[i] = '{left: 10'd0, right: 10'd0, top: 10'd0, bot: 10'd0};
        end
    endgenerate
endmodule


module frame_mux(
    input  logic         frame_switch,
    input  rect_t [13:0] frame0_rects,
    input  rect_t [13:0] frame1_rects,
    output rect_t [13:0] selected_rects
);
    always_comb begin
        selected_rects = frame_switch ? frame1_rects : frame0_rects;
    end
endmodule

module videoGen(
    input  logic        vgaclk,
    input  logic        reset,
    input  logic        frame_switch,
    input  logic [9:0]  x, y,
    input  logic        blank_b,
    output logic [3:0]  r, g, b
);
    rect_t [13:0] frame0_rects;
    rect_t [13:0] frame1_rects;
    rect_t [13:0] selected_rects;
	logic inrect_all;
	logic inline;

    // Frame 0 instantiation
    image_frame0 frame0_inst(
        .frame0_rects(frame0_rects)
    );

    image_frame1 frame1_inst(
        .frame1_rects(frame1_rects)  
    );

    // Instantiate the multiplexer
    frame_mux frame_mux_inst(
        .frame_switch(frame_switch),
        .frame0_rects(frame0_rects),
        .frame1_rects(frame1_rects),
        .selected_rects(selected_rects)
    );

    logic inrect_all;
    
    rect_struct_gen rect_struct_gen_inst(
        .x(x),
        .y(y),
        .selected_rects(selected_rects),
        .inrect_all(inrect_all)
    );

    line_gen line_gen_inst(
        .x(x),
        .y(y),
        .x_1(10'd0),
        .y_1(10'd0),
        .x_2(10'd200),
        .y_2(10'd200),
        .inline(inline)
    );

    // Set the color based on whether the pixel is in the rectangle
    // assign {r, b} = inrect_all ? 4'b1111 : 4'b0000;
    // assign g = 4'b0000;

    

    assign {r, b} = inline ? 4'b1111 : 4'b0000;
    assign g = 4'b0000;

endmodule


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

// NOTES: we wil assume that x_1 is always less than x_2
module line_gen(
    input  logic [9:0] x, y,    // Current pixel coordinates
    input  logic [9:0] x_1, y_1, // Line start point
    input  logic [9:0] x_2, y_2, // Line end point
    output logic inline          // True if pixel is on the line
);

    // Declare intermediate signals with sufficient bit widths
    logic signed [10:0] delta_x, delta_y;
    logic signed [21:0] cross_product;
    logic [21:0] abs_cross_product;
    logic signed [10:0] x_diff, y_diff;
    parameter logic [21:0] TOLERANCE = 22'd100; // Adjust tolerance as needed

    always_comb begin
        delta_x = signed'(x_2) - signed'(x_1);
        delta_y = signed'(y_2) - signed'(y_1);

        x_diff = signed'(x) - signed'(x_1);
        y_diff = signed'(y) - signed'(y_1);

        // Compute the cross product
        cross_product = x_diff * delta_y - y_diff * delta_x;

        // Take absolute value
        abs_cross_product = (cross_product < 0) ? -cross_product : cross_product;

        // Determine if the point is close enough to the line
        inline = (abs_cross_product <= TOLERANCE);
    end
endmodule


module rect_struct_gen(
    input logic [9:0] x, y,     // Add missing x,y inputs
    input rect_t [13:0] selected_rects,
    output logic inrect_all
);
    logic [13:0] inrect;

    genvar i;
    generate
        for (i = 0; i < 14; i = i + 1) begin : rect_gen
            assign inrect[i] = (x >= selected_rects[i].left & x < selected_rects[i].right & 
                              y >= selected_rects[i].top & y < selected_rects[i].bot);
        end
    endgenerate
    
    assign inrect_all = |inrect; //Logical OR of all inrect signals
endmodule
