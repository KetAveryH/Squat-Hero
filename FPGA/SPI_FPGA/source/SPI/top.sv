//////////////////////////
// top module
// SPI Interface to retrieve data from MCU
//////////////////////////

module top(
    input  logic clk, //COMMENT BACK IN WHEN USING MODEL SIM
    input  logic sck, 
    input  logic sdi,
    output logic sdo,
    input  logic load,
    output logic done
);

    LSOSC #() 
         lf_osc (.CLKLFPU(1'b1), .CLKLFEN(1'b1), .CLKLF(int_osc)); //COMMENT OUT WHEN IN MODEL SIM
			
    // We will receive the following data:
    // 4 10-bit x and y coordinates
    // 3 4-bit color values

    // Declare output variables
    logic [9:0] x_1, y_1, x_2, y_2, x_3, y_3, x_4, y_4;
    logic [3:0] r, g, b;

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
        .b(b)
    );
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
    output logic [3:0] r, g, b
);
    // Internal variables
    logic [127:0] datacaptured; // Internal shift register for the 128-bit packet
    integer bit_index;          // Counter for tracking shifted bits
    logic sdodelayed;           // Delayed signal for sdo

    // Initialize variables
    initial begin
        bit_index = 0;
        datacaptured = 128'b0;
        done = 1'b0;
    end

    // SPI shift operation: Handle all data capture here
    always_ff @(posedge sck or posedge load) begin
        if (load) begin
            // Reset logic when load is asserted
            datacaptured <= 128'b0;
            bit_index <= 0;
            done <= 1'b0;
        end else if (bit_index < 128) begin
            // Shift in new bit from sdi
            datacaptured <= {datacaptured[126:0], sdi};
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
            {x_1, y_1, x_2, y_2, x_3, y_3, x_4, y_4, r, g, b} <= datacaptured;
        end
    end

    // Handle sdo on the negative edge of sck
    always_ff @(negedge sck) begin
        sdodelayed <= datacaptured[127]; // Shift out MSB first
    end

    // Assign sdo
    assign sdo = sdodelayed;
endmodule
