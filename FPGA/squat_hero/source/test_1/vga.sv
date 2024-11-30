module vga(input logic clk, reset, reset1,
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
		.reset1(reset1),
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

// module moving_box #(
//     parameter BOX_WIDTH = 10'd100,
//     parameter INITIAL_LEFT = 10'd48,
//     parameter INITIAL_RIGHT = INITIAL_LEFT + BOX_WIDTH,
//     parameter VERTICAL_TOP = 10'd150,
//     parameter VERTICAL_BOT = 10'd230
// )(
//     input  logic        vgaclk,
//     input  logic        reset,
//     output logic [9:0]  left,
//     output logic [9:0]  right,
//     output logic [9:0]  top,
//     output logic [9:0]  bot
// );
    
//     logic [31:0] counter;
    
//     // Fixed vertical position assignments
//     assign top = VERTICAL_TOP;
//     assign bot = VERTICAL_BOT;
    
//     always_ff @(posedge vgaclk or posedge reset) begin
//         if (reset) begin
//             counter <= 32'd0;
//             left <= INITIAL_LEFT;
//             right <= INITIAL_RIGHT;
//         end else if (counter >= 1_000_000) begin
//             counter <= 0;
//             if (left <= (10'd48 + 10'd480)) begin // If not at the end of the screen, move the box
//                 left <= left + 1'd1;    
//                 right <= right + 1'd1;  
//             end else begin
//                 left <= INITIAL_LEFT;
//                 right <= INITIAL_RIGHT;
//             end
//         end else begin // Increment the counter
//             counter <= counter + 1'd1;
//         end
//     end
    
// endmodule

typedef struct packed {
    logic [9:0] left;
    logic [9:0] right;
    logic [9:0] top;
    logic [9:0] bot;
} rect_t;

module image_frame(
    output rect_t [13:0] rects
);
    // Use assign statements instead of procedural blocks
    assign rects[0] = '{left: 10'd300, right: 10'd340, top: 10'd100, bot: 10'd140};  // Head
    assign rects[1] = '{left: 10'd310, right: 10'd330, top: 10'd140, bot: 10'd220};  // Body
    assign rects[2] = '{left: 10'd270, right: 10'd310, top: 10'd150, bot: 10'd170};  // Left Arm
    assign rects[3] = '{left: 10'd330, right: 10'd370, top: 10'd150, bot: 10'd170};  // Right Arm
    assign rects[4] = '{left: 10'd310, right: 10'd320, top: 10'd220, bot: 10'd300};  // Left Leg
    assign rects[5] = '{left: 10'd320, right: 10'd330, top: 10'd220, bot: 10'd300};  // Right Leg
    
    // Initialize remaining rectangles to 0
    genvar i;
    generate
        for (i = 6; i < 14; i = i + 1) begin : init_rects
            assign rects[i] = '{default: '0};
        end
    endgenerate
endmodule

module videoGen(
    input  logic        vgaclk,     // Add vgaclk input
    input  logic        reset,      // Add reset input
	    input  logic        reset1,
    input  logic [9:0]  x, y,
    input  logic        blank_b,
    output logic [3:0]  r, g, b
);
    logic pixel, inrect[14];
    logic [9:0] box_left, box_right, box_top, box_bot;  // Add signals to connect modules

	  rect_t [13:0] rects;  // Only need one array

    // Instantiate image_frame with only output
    image_frame image_frame_inst(
        .rects(rects)    // Connect to the output-only port
    );

  

     //Instantiate the character generator ROM
    //chargenrom chargenromb(
        //.ch(y[8:3] + 8'd65),
        //.xoff(x[2:0]),
        //.yoff(y[2:0]),
        //.pixel(pixel)
    //);


    logic inrect_all;
    
    rect_struct_gen rect_struct_gen_inst(
        .x(x),
        .y(y),
        .rects(rects),
        .inrect_all(inrect_all)
    );

    

    // Instantiate moving box with connections
    // moving_box #(
    //     .BOX_WIDTH(10'd10),
    //     .INITIAL_LEFT(10'd48),
    //     .INITIAL_RIGHT(10'd58),  // or let it calculate from INITIAL_LEFT + BOX_WIDTH
    //     .VERTICAL_TOP(10'd150),
    //     .VERTICAL_BOT(10'd230)
    // ) moving_box_inst(
    //     .vgaclk(vgaclk),
    //     .reset(reset),
    //     .left(box_left),
    //     .right(box_right),
    //     .top(box_top),
    //     .bot(box_bot)
    // );

    // Use the moving box coordinates for rectgen
    // rectgen rectgen_inst(
    //     .x(x),
    //     .y(y),
    //     .left(box_left),    // Use moving box coordinates
    //     .right(box_right),  // instead of fixed values
    //     .top(box_top),
    //     .bot(box_bot),
    //     .inrect(inrect)
    // );

    

    

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
            r = inrect_all ? 4'hF : 4'h0; // Maximum value for 4 bits is 4'hF (15 in decimal)
        end
    end
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

module rect_struct_gen(
    input logic [9:0] x, y,     // Add missing x,y inputs
    input rect_t [13:0] rects,
    output logic inrect_all
);
    logic [13:0] inrect;

    genvar i;
    generate
        for (i = 0; i < 14; i = i + 1) begin : rect_gen
            assign inrect[i] = (x >= rects[i].left & x < rects[i].right & 
                              y >= rects[i].top & y < rects[i].bot);
        end
    endgenerate
    
    assign inrect_all = |inrect; //Logical OR of all inrect signals
endmodule
