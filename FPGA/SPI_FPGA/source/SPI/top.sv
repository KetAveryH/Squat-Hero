//////////////////////////
// top module
// SPI Interface to retrieve data from MCU
//////////////////////////

module top(
    input  logic clk, // System clock
    input  logic sck, 
    input  logic sdi,
    output logic sdo,
    input  logic load, // Initialize 128-bit frame
    output logic done,
	output logic ck_debug, data_debug,
	output logic correct, dc0, dc1, dc2, dc3, dc4, dc5, dc6, dc7
    //output logic data_out, transmitting  // Renamed from "true" for clarity
	//output logic msb_true1
);

  // HSOSC instantiation
HSOSC #() 
      hf_osc (.CLKHFPU(1'b1), .CLKHFEN(1'b1), .CLKHF(int_osc)); // COMMENT OUT WHEN IN MODEL SIM

            
    // Declare output variables
    logic [9:0] x_1, y_1, x_2, y_2, x_3, y_3, x_4, y_4;
    logic [3:0] r, g, b;
	
	
    // Instantiate SPI module
    spi spi_inst(
        .clk(clk),
        .sck(sck),
        .sdi(sdi),
        .sdo(sdo),
        .load(load),
        .done(done),
        .x_1(x_1),
        .y_1(y_1),
        .x_2(x_2),
        .y_2(y_2),
        .x_3(x_3),
        .y_3(y_3),
        .x_4(x_4),
        .y_4(y_4),
        .r(r),
        .g(g),
        .b(b),
		.ck_debug(ck_debug),
		.data_debug(data_debug),
		.correct(correct),
		.dc0(dc0),
		.dc1(dc1),
		.dc2(dc2),
		.dc3(dc3),
		.dc4(dc4),
		.dc5(dc5),
		.dc6(dc6),
		.dc7(dc7)
        //.data_out(data_out), // Connect the MSB signal\
		//.transmitting(transmitting)
		//.true_1(msb_true1)
    );
	//assign msb_true = x_1[9];
endmodule



//////////////////////////
// SPI Module
//////////////////////////
module spi(
    input  logic clk,
    input  logic sck, 
    input  logic sdi,
    output logic sdo,
    input  logic load,
    output logic done,
    output logic [9:0] x_1, y_1, x_2, y_2, x_3, y_3, x_4, y_4,
    output logic [3:0] r, g, b,
    //output logic data_out, transmitting // Changed to output
	output logic ck_debug, data_debug, correct, dc0, dc1, dc2, dc3, dc4, dc5, dc6, dc7
);
    // Internal variables
    logic [127:0] datacaptured; // Internal shift register for the 128-bit packet
    integer bit_index;          // Counter for tracking shifted bits
    logic sdodelayed;           // Delayed signal for sdo

    // Initialize variables (only affects simulation)
    //initial begin
        //bit_index = 0;
        //datacaptured = 128'b0;
        //done = 1'b0;
    //end
	

    // Assign MSB of datacaptured to "true"
    
	//assign true_1 = sdi;
	//logic true;

	assign ck_debug = sck;
	assign data_debug = sdi;
	assign dc0 = 0;
	assign dc1 = 1;
	assign dc2 = 0;
	assign dc3 = 1;
	assign dc4 = 0;
	assign dc5 = 0;
	assign dc6 = 0;
	assign dc7 = 0;

	
    // SPI IN shift operation: Handle all data capture here
	always_ff @(posedge sck) begin
		if (!load) begin
			bit_index = 0;
			done = 0;
		end else if (bit_index < 128) begin
			bit_index = bit_index + 1;
			datacaptured[bit_index-1] = sdi;
			done = 0;
		end else if (bit_index >= 128) begin
			done = 1;
		end
	end
	
	
	
	
	
       //always_ff @(posedge sck or negedge load) begin
        //if (!load) begin
             //Reset logic only if no transaction is ongoing
            //bit_index <= 0;
            //done <= 1'b0; // Explicitly reset `done`
        //end else if (bit_index < 128) begin
             //Shift in new bit from sdi
            //datacaptured <= {datacaptured[126:0], sdi};
            //bit_index <= bit_index + 1;

            //if (bit_index == 127) begin
                //done <= 1'b1; // Assert `done` at the last bit
            //end
			//end
		//end

	
	
	//    logic [3:0] clk_count; // Counter for counting clk edges
    //logic [6:0] bit_out_index; // Index to track which bit is being sent out
  
 
    // Assign captured data to output once done
    always_ff @(posedge clk) begin
        if (done) begin
            {x_1, y_1, x_2, y_2, x_3, y_3, x_4, y_4, r, g, b} = datacaptured; // Map captured data to outputs
			datacaptured = 128'b0;
        end
    end
	
	
	
    // Handle sdo on the negative edge of sck
    //always_ff @(negedge sck) begin
        //sdodelayed <= datacaptured[127]; // Shift out MSB first
    //end

    // Assign sdo
    //assign sdo = sdodelayed;
endmodule