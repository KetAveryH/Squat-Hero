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
    output logic msb_true, // Renamed from "true" for clarity
	output logic msb_true1
);

    LSOSC #() 
         lf_osc (.CLKLFPU(1'b1), .CLKLFEN(1'b1), .CLKLF(int_osc)); //COMMENT OUT WHEN IN MODEL SIM
            
    // Declare output variables
    logic [9:0] x_1, y_1, x_2, y_2, x_3, y_3, x_4, y_4;
    logic [3:0] r, g, b;
	
	
    // Instantiate SPI module
    spi spi_inst(
        .clk(int_osc),
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
        .true(msb_true), // Connect the MSB signal\
		.true_1(msb_true1)
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
    output logic true, // Changed to output
	output logic true_1
);
    // Internal variables
    logic [127:0] datacaptured; // Internal shift register for the 128-bit packet
    integer bit_index;          // Counter for tracking shifted bits
    logic sdodelayed;           // Delayed signal for sdo

    // Initialize variables (only affects simulation
    //initial begin
        //bit_index = 0;
        //datacaptured = 128'b0;
        //done = 1'b0;
    //end
	

    // Assign MSB of datacaptured to "true"
    
	assign true_1 = sdi;
	assign true = sdo;
	
    // SPI IN shift operation: Handle all data capture here
    always_ff @(posedge sck) begin
		if (load && bit_index == 0) begin
			// Reset logic only if no transaction is ongoing
			datacaptured <= 128'b0;
			bit_index <= 0;
			done <= 1'b0;
		end else if (bit_index < 128) begin
			// Shift in new bit from sdi
			datacaptured <= {datacaptured[126:0], sdi}; // Shift data from MSB down
			bit_index <= bit_index + 1;
	
			// Assert done when the last bit is captured
			if (bit_index == 127) begin
				done <= 1'b1;
			end
		end
	end

    // Assign captured data to output once done
    always_ff @(posedge clk) begin
        if (done) begin
            {x_1, y_1, x_2, y_2, x_3, y_3, x_4, y_4, r, g, b} <= datacaptured; // Map captured data to outputs
        end
    end

    // Handle sdo on the negative edge of sck
    //always_ff @(negedge sck) begin
        //sdodelayed <= datacaptured[127]; // Shift out MSB first
    //end

    // Assign sdo
    //assign sdo = sdodelayed;
endmodule