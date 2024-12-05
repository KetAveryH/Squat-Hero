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
	    // Assign debug signals
    assign ck_debug = sck;
    assign data_debug = sdi;
    assign dc0 = x_1[0];
    assign dc1 = x_1[1];
    assign dc2 = x_1[2];
    assign dc3 = x_1[3];
    assign dc4 = x_1[4];
    assign dc5 = x_1[5];
    assign dc6 = x_1[6];
    assign dc7 = x_1[7];

	
	
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
//////////////////////////
// SPI Module
//////////////////////////
module spi(
    input  logic clk,   // System clock (if needed)
    input  logic sck,   // SPI clock from master
    input  logic sdi,   // SPI data input (MOSI)
    output logic sdo,   // SPI data output (MISO)
    input  logic load,  // Active-high chip select
    output logic done,
    output logic [9:0] x_1, y_1, x_2, y_2, x_3, y_3, x_4, y_4,
    output logic [3:0] r, g, b,
    // Debug signals (if needed)
    output logic ck_debug, data_debug, correct, dc0, dc1, dc2, dc3, dc4, dc5, dc6, dc7
);
    // Internal variables
    logic [127:0] datacaptured;   // Shift register for receiving data
    logic [127:0] data_to_send;   // Shift register for transmitting data
    integer bit_index;            // Counter for tracking received bits
    integer bit_out_index;        // Counter for tracking transmitted bits


    // SPI Receive (MOSI) - Shift in data on rising edge of sck
    always_ff @(posedge sck or negedge load) begin
        if (!load) begin
            bit_index <= 0;
            done <= 0;
            datacaptured <= 128'b0;  // Clear the data on reset
        end else begin
            if (bit_index < 128) begin
                datacaptured <= {datacaptured[126:0], sdi};  // Shift in MSB first
                bit_index <= bit_index + 1;
                done <= (bit_index == 127);  // Set done on the last bit
            end
        end
    end

    // SPI Transmit (MISO) - Shift out data on falling edge of sck
    always_ff @(negedge sck or negedge load) begin
        if (!load) begin
            // Reset transmit counter
            bit_out_index <= 0;
            sdo <= 1'b0; // Drive '0' when not transmitting
        end else begin
            if (bit_out_index < 128) begin
                sdo <= data_to_send[127]; // Send MSB first
                data_to_send <= {data_to_send[126:0], 1'b0}; // Shift left
                bit_out_index <= bit_out_index + 1;
            end else begin
                sdo <= 1'b0; // Continue driving '0' after data sent
            end
        end
    end

    // Assign captured data to output once done (optional clk domain)
    always_ff @(posedge clk or negedge load) begin
        if (!load) begin
            // Optionally reset outputs here if needed
        end else if (done) begin
            // Map captured data to outputs
            {x_1, y_1, x_2, y_2, x_3, y_3, x_4, y_4, r, g, b} <= datacaptured;
            // Prepare data to send back to master
            data_to_send <= datacaptured; // For example, echo back the received data
            // Note: Assigning 'data_to_send' here is acceptable because 'data_to_send' is only assigned in this block
        end
    end

endmodule
