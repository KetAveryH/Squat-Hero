module vga(input logic reset,
		   input logic [2:0] frame_switch,
		   output logic vgaclk, // 25 MHz VGA clock  25.175MHz
		   output logic hsync, vsync,
		   output logic sync_b, blank_b, // to monitor & DAC
		   output logic test,
		   output logic test1,
		   output logic [3:0] r, g, b); // to video DAC
	
	logic [9:0] x, y; 
	assign test = 1'b0;
	assign test1 = 1'b1;
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

//typedef struct packed {
    //logic [9:0] left;
    //logic [9:0] right;
    //logic [9:0] top;
    //logic [9:0] bot;
//} rect_t;


//module image_frame0(
    //output rect_t [13:0] frame0_rects
//);
     //Use assign statements instead of procedural blocks
    //assign frame0_rects[0] = '{left: 10'd20, right: 10'd340, top: 10'd100, bot: 10'd140};  // Head
    //assign frame0_rects[1] = '{left: 10'd310, right: 10'd330, top: 10'd140, bot: 10'd220};  // Body
    //assign frame0_rects[2] = '{left: 10'd270, right: 10'd310, top: 10'd150, bot: 10'd170};  // Left Arm
    //assign frame0_rects[3] = '{left: 10'd330, right: 10'd370, top: 10'd150, bot: 10'd170};  // Right Arm
    //assign frame0_rects[4] = '{left: 10'd310, right: 10'd320, top: 10'd220, bot: 10'd300};  // Left Leg
    //assign frame0_rects[5] = '{left: 10'd320, right: 10'd330, top: 10'd220, bot: 10'd300};  // Right Leg
    
     //Initialize remaining rectangles to 0
    //genvar i;
    //generate
        //for (i = 6; i < 14; i = i + 1) begin : init_rects
            //assign frame0_rects[i] = '{left: 10'd0, right: 10'd0, top: 10'd0, bot: 10'd0};
        //end
    //endgenerate
//endmodule

//module image_frame1(
    //output rect_t [13:0] frame1_rects
//);

    //assign frame1_rects[0] = '{left: 10'd0, right: 10'd340, top: 10'd100, bot: 10'd140};  // Head
    //assign frame1_rects[1] = '{left: 10'd310, right: 10'd330, top: 10'd140, bot: 10'd220};  // Body
    //assign frame1_rects[2] = '{left: 10'd270, right: 10'd310, top: 10'd150, bot: 10'd170};  // Left Arm
    //assign frame1_rects[3] = '{left: 10'd330, right: 10'd370, top: 10'd150, bot: 10'd170};  // Right Arm
    //assign frame1_rects[4] = '{left: 10'd310, right: 10'd320, top: 10'd220, bot: 10'd300};  // Left Leg
    //assign frame1_rects[5] = '{left: 10'd270, right: 10'd330, top: 10'd220, bot: 10'd300};  // Right Leg

    //genvar i;
    //generate
        //for (i = 6; i < 14; i = i + 1) begin : init_rects
            //assign frame1_rects[i] = '{left: 10'd0, right: 10'd0, top: 10'd0, bot: 10'd0};
        //end
    //endgenerate
//endmodule

//module frame_mux(
    //input  logic  [2:0]  frame_switch,
    //input  rect_t [13:0] frame0_rects,
    //input  rect_t [13:0] frame1_rects,
    //output rect_t [13:0] selected_rects
//);
    //always_comb begin
        //selected_rects = frame_switch ? frame1_rects : frame0_rects;
    //end
//endmodule

// Add this struct definition after the rect_t struct
typedef struct packed {
    logic [9:0] x_1;
    logic [9:0] y_1;
    logic [9:0] x_2;
    logic [9:0] y_2;
} line_t;



// module image_line_frame0(
// 	output line_t [4:0] frame0_lines
// );
// 	    // Head (circle approximation with a line)
//     assign frame0_lines[0] = '{x_1: 10'd150, y_1: 10'd50, x_2: 10'd150, y_2: 10'd70}; // Vertical line for head

//     assign frame0_lines[1] = '{x_1: 10'd150, y_1: 10'd70, x_2: 10'd150, y_2: 10'd150}; // Vertical line for body

//     // Left Arm
//     assign frame0_lines[2] = '{x_1: 10'd150, y_1: 10'd90, x_2: 10'd130, y_2: 10'd110}; // Diagonal line for left arm

//     // Right Arm
//     assign frame0_lines[3] = '{x_1: 10'd150, y_1: 10'd90, x_2: 10'd170, y_2: 10'd110}; // Diagonal line for right arm

//     // Left Leg
//     assign frame0_lines[4] = '{x_1: 10'd150, y_1: 10'd150, x_2: 10'd130, y_2: 10'd180}; // Diagonal line for left leg

// endmodule

module image_line_frame0( //Standing up Straight
	output line_t [3:0] frame0_lines
);
	assign frame0_lines[0] = '{x_1: 10'd285, y_1: 10'd474, x_2: 10'd320, y_2: 10'd474}; // Horizontal line for foot
	assign frame0_lines[1] = '{x_1: 10'd320, y_1: 10'd474, x_2: 10'd320, y_2: 10'd424}; // Line for Shin
	assign frame0_lines[2] = '{x_1: 10'd320, y_1: 10'd424, x_2: 10'd320, y_2: 10'd375}; // Line for Thigh
	assign frame0_lines[3] = '{x_1: 10'd320, y_1: 10'd375, x_2: 10'd320, y_2: 10'd275};   // Line for Torso
endmodule

module image_line_frame1(
	output line_t [3:0] frame1_lines
);
	assign frame1_lines[0] = '{x_1: 10'd285, y_1: 10'd474, x_2: 10'd320, y_2: 10'd474}; // Horizontal line for foot
	assign frame1_lines[1] = '{x_1: 10'd320, y_1: 10'd474, x_2: 10'd310, y_2: 10'd430}; // Line for Shin
	assign frame1_lines[2] = '{x_1: 10'd310, y_1: 10'd430, x_2: 10'd320, y_2: 10'd400}; // Line for Thigh
	assign frame1_lines[3] = '{x_1: 10'd320, y_1: 10'd400, x_2: 10'd315, y_2: 10'd340};   // Line for Torso
endmodule

module image_line_frame2(
	output line_t [3:0] frame2_lines
);
	assign frame2_lines[0] = '{x_1: 10'd285, y_1: 10'd474, x_2: 10'd320, y_2: 10'd474}; // Horizontal line for foot
	assign frame2_lines[1] = '{x_1: 10'd320, y_1: 10'd474, x_2: 10'd302, y_2: 10'd470}; // Line for Shin
	assign frame2_lines[2] = '{x_1: 10'd302, y_1: 10'd470, x_2: 10'd353, y_2: 10'd229}; // Line for Thigh
	assign frame2_lines[3] = '{x_1: 10'd353, y_1: 10'd229, x_2: 10'd342, y_2: 10'd60};  // Line for Torso
endmodule

module image_line_frame3(
	output line_t [3:0] frame3_lines
);
	assign frame3_lines[0] = '{x_1: 10'd285, y_1: 10'd474, x_2: 10'd320, y_2: 10'd474}; // Horizontal line for foot
	assign frame3_lines[1] = '{x_1: 10'd320, y_1: 10'd474, x_2: 10'd296, y_2: 10'd342}; // Line for Shin
	assign frame3_lines[2] = '{x_1: 10'd296, y_1: 10'd342, x_2: 10'd368, y_2: 10'd248}; // Line for Thigh
	assign frame3_lines[3] = '{x_1: 10'd368, y_1: 10'd248, x_2: 10'd346, y_2: 10'd60};  // Line for Torso
endmodule

module image_line_frame4(
	output line_t [3:0] frame4_lines
);
	assign frame4_lines[0] = '{x_1: 10'd285, y_1: 10'd474, x_2: 10'd320, y_2: 10'd474}; // Horizontal line for foot
	assign frame4_lines[1] = '{x_1: 10'd320, y_1: 10'd474, x_2: 10'd285, y_2: 10'd345}; // Line for Shin
	assign frame4_lines[2] = '{x_1: 10'd285, y_1: 10'd345, x_2: 10'd381, y_2: 10'd270}; // Line for Thigh
	assign frame4_lines[3] = '{x_1: 10'd381, y_1: 10'd270, x_2: 10'd343, y_2: 10'd54};  // Line for Torso
endmodule

module image_line_frame5(
	output line_t [3:0] frame5_lines
);
	assign frame5_lines[0] = '{x_1: 10'd285, y_1: 10'd474, x_2: 10'd320, y_2: 10'd474}; // Horizontal line for foot
	assign frame5_lines[1] = '{x_1: 10'd320, y_1: 10'd474, x_2: 10'd280, y_2: 10'd347}; // Line for Shin
	assign frame5_lines[2] = '{x_1: 10'd280, y_1: 10'd347, x_2: 10'd389, y_2: 10'd292}; // Line for Thigh
	assign frame5_lines[3] = '{x_1: 10'd389, y_1: 10'd292, x_2: 10'd348, y_2: 10'd377}; // Line for Torso
endmodule

module image_line_frame6(
	output line_t [3:0] frame6_lines
);
	assign frame6_lines[0] = '{x_1: 10'd285, y_1: 10'd474, x_2: 10'd320, y_2: 10'd474}; // Horizontal line for foot
	assign frame6_lines[1] = '{x_1: 10'd320, y_1: 10'd474, x_2: 10'd280, y_2: 10'd347}; // Line for Shin
	assign frame6_lines[2] = '{x_1: 10'd280, y_1: 10'd347, x_2: 10'd398, y_2: 10'd314}; // Line for Thigh
	assign frame6_lines[3] = '{x_1: 10'd398, y_1: 10'd314, x_2: 10'd334, y_2: 10'd104}; // Line for Torso
endmodule

module image_line_frame7(
	output line_t [3:0] frame7_lines
);
	assign frame7_lines[0] = '{x_1: 10'd285, y_1: 10'd474, x_2: 10'd320, y_2: 10'd474}; // Horizontal line for foot
	assign frame7_lines[1] = '{x_1: 10'd320, y_1: 10'd474, x_2: 10'd275, y_2: 10'd350}; // Line for Shin
	assign frame7_lines[2] = '{x_1: 10'd275, y_1: 10'd350, x_2: 10'd397, y_2: 10'd342}; // Line for Thigh
	assign frame7_lines[3] = '{x_1: 10'd397, y_1: 10'd342, x_2: 10'd348, y_2: 10'd128}; // Line for Torso
endmodule




module image_line_mux(
	input logic  [2:0] frame_switch,
	input line_t [3:0] frame0_lines,
	input line_t [3:0] frame1_lines,
    input line_t [3:0] frame2_lines,
    input line_t [3:0] frame3_lines,
    //input line_t [3:0] frame4_lines,
    //input line_t [3:0] frame5_lines,
    //input line_t [3:0] frame6_lines,
    //input line_t [3:0] frame7_lines,
	output line_t [3:0] selected_lines
);
	always_comb begin
		case(frame_switch)
			3'b000: selected_lines = frame0_lines;
			3'b001: selected_lines = frame1_lines;
			3'b010: selected_lines = frame2_lines;
			3'b011: selected_lines = frame3_lines;
			//3'b100: selected_lines = frame4_lines;
			//3'b101: selected_lines = frame5_lines;
			//3'b110: selected_lines = frame6_lines;
			//3'b111: selected_lines = frame7_lines;
			default: selected_lines = frame0_lines;
		endcase
	end

	//assign selected_lines = frame_switch ? frame1_lines : frame0_lines;
endmodule

/////////////
// Video Generator
/////////////
module videoGen(
    input  logic        vgaclk,
    input  logic        reset,
    input  logic [2:0]   frame_switch,
    input  logic [9:0]  x, y,
    input  logic        blank_b,
    output logic [3:0]  r, g, b
);
    //rect_t [13:0] frame0_rects;
    //rect_t [13:0] frame1_rects;
	line_t [3:0]  frame0_lines;
	line_t [3:0]  frame1_lines;
	line_t [3:0]  frame2_lines; 
	line_t [3:0]  frame3_lines;
	//line_t [3:0]  frame4_lines;
	//line_t [3:0]  frame5_lines;
	//line_t [3:0]  frame6_lines;
	//line_t [3:0]  frame7_lines;
    // rect_t [13:0] selected_rects;
	line_t [3:0]  selected_lines;
	logic inline_all;
	logic inline;

    // // Frame 0 instantiation
    // image_frame0 frame0_inst(
    //     .frame0_rects(frame0_rects)
    // );

    // image_frame1 frame1_inst(
    //     .frame1_rects(frame1_rects)  
    // );

    // Instantiate the multiplexer
    // frame_mux frame_mux_inst(
    //     .frame_switch(frame_switch),
    //     .frame0_rects(frame0_rects),
    //     .frame1_rects(frame1_rects),
    //     .selected_rects(selected_rects)
    // );

	image_line_frame0 frame0_lines_inst(
		.frame0_lines(frame0_lines)
	);

	image_line_frame1 frame1_lines_inst(
		.frame1_lines(frame1_lines)
	);

    image_line_frame2 frame2_lines_inst(
		.frame2_lines(frame2_lines)
	);

    image_line_frame3 frame3_lines_inst(
		.frame3_lines(frame3_lines)
	);

	//image_line_frame4 frame4_lines_inst(
		//.frame4_lines(frame4_lines)
	//); 

	//image_line_frame5 frame5_lines_inst(
		//.frame5_lines(frame5_lines)
	//);

	//image_line_frame6 frame6_lines_inst(
		//.frame6_lines(frame6_lines)
	//);

	//image_line_frame7 frame7_lines_inst(
		//.frame7_lines(frame7_lines)
	//);

	image_line_mux image_line_mux_inst(
		.frame_switch(frame_switch),
		.frame0_lines(frame0_lines),
		.frame1_lines(frame1_lines),
		.frame2_lines(frame2_lines),
		.frame3_lines(frame3_lines),
		//.frame4_lines(frame4_lines),
		//.frame5_lines(frame5_lines),
		//.frame6_lines(frame6_lines),
		//.frame7_lines(frame7_lines),
		.selected_lines(selected_lines)
	);

    // logic inrect_all;
    
    // rect_struct_gen rect_struct_gen_inst(
    //     .x(x),
    //     .y(y),
    //     .selected_rects(selected_rects),
    //     .inrect_all(inrect_all)
    // );

    // line_gen line_gen_inst(
    //     .x(x),
    //     .y(y),
    //     .x_1(10'd50),
    //     .y_1(10'd100),
    //     .x_2(10'd100),
    //     .y_2(10'd200),
    //     .inline(inline)
    // );

    line_struct_gen line_struct_gen_inst(
        .x(x),
        .y(y),
        .selected_lines(selected_lines),
        .inline_all(inline_all)
    );

    // Set the color based on whether the pixel is in the rectangle
    // assign {r, b} = inrect_all ? 4'b1111 : 4'b0000;
    // assign g = 4'b0000;

    

    // assign {r, b} = inline_all ? 4'b1111 : 4'b0000;
    assign r = inline_all ? 4'b1111 : 4'b0000;
    assign b = inline_all ? 4'b1111 : 4'b0000;
    assign g = inline_all ? 4'b1111 : 4'b0000;
    // assign g = 4'b0000;

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

//module rectgen(input logic [9:0] x, y, left, top, right, bot,
               //output logic inrect);
  //assign inrect = (x >= left & x < right & y >= top & y < bot);
//endmodule

module line_gen(
    input  logic [9:0] x, y,    // Current pixel coordinates
    input  logic [9:0] x_1, y_1, // Line start point
    input  logic [9:0] x_2, y_2, // Line end point
    output logic inline          // True if pixel is on the line
);

    // Declare intermediate signals with sufficient bit widths
    logic signed [10:0] delta_x, delta_y;
    logic signed [10:0] x_diff, y_diff;
    logic signed [31:0] cross_product;
    logic [42:0] error_squared;
    logic signed [31:0] length_squared;
    logic [42:0] threshold;

    parameter int TOLERANCE = 2; // Adjust tolerance as needed

    always_comb begin
        // Ensure deltas are calculated accurately
        delta_x = signed'(x_2) - signed'(x_1);
        delta_y = signed'(y_2) - signed'(y_1);

        x_diff = signed'(x) - signed'(x_1);
        y_diff = signed'(y) - signed'(y_1);

        // Cross product with extended precision
        cross_product = (x_diff * delta_y) - (y_diff * delta_x);

        // Squared error and length
        error_squared = cross_product * cross_product;
        length_squared = delta_x * delta_x + delta_y * delta_y;

        // Scaled threshold with extra precision
        threshold = (TOLERANCE * TOLERANCE) * length_squared;

        // Determine if pixel is close enough to the line
        inline = (error_squared <= threshold);

        // Restrict to endpoints: ensure pixel lies within bounding box
        if (inline) begin
            inline = (x >= (x_1 < x_2 ? x_1 : x_2)) &&
                     (x <= (x_1 > x_2 ? x_1 : x_2)) &&
                     (y >= (y_1 < y_2 ? y_1 : y_2)) &&
                     (y <= (y_1 > y_2 ? y_1 : y_2));
        end
    end

endmodule



//module rect_struct_gen(
    //input logic [9:0] x, y,     // Add missing x,y inputs
    //input rect_t [13:0] selected_rects,
    //output logic inrect_all
//);
    //logic [13:0] inrect;

    //genvar i;
    //generate
        //for (i = 0; i < 14; i = i + 1) begin : rect_gen
            //assign inrect[i] = (x >= selected_rects[i].left & x < selected_rects[i].right & 
                              //y >= selected_rects[i].top & y < selected_rects[i].bot);
        //end
    //endgenerate
    
    //assign inrect_all = |inrect; //Logical OR of all inrect signals
//endmodule

module line_struct_gen(
    input logic [9:0] x, y,
    input line_t [3:0] selected_lines,  // Changed from [13:0] to [4:0] for 5 lines
    output logic inline_all
);
    logic [3:0] inline;  // Changed from [13:0] to [4:0]
    
    genvar i;
    generate
        for (i = 0; i < 4; i = i + 1) begin : line_gen  // Changed from 14 to 5
            line_gen line_inst (
                .x(x),
                .y(y),
                .x_1(selected_lines[i].x_1),
                .y_1(selected_lines[i].y_1),
                .x_2(selected_lines[i].x_2),
                .y_2(selected_lines[i].y_2),
                .inline(inline[i])
            );
        end
    endgenerate
    
    assign inline_all = |inline;
endmodule

