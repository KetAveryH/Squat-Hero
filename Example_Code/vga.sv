module vga(
    input logic clk, reset,
    output logic vgaclk, // 25 MHz VGA clock
    output logic hsync, vsync,
    output logic sync_b, blank_b, // to monitor & DAC
    output logic [7:0] r, g, b // to video DAC
);
    logic [9:0] x, y;

    // divide 50 MHz input clock by 2 to get 25 MHz clock
    always_ff @(posedge clk, posedge reset)
        if (reset) 
            vgaclk = 1'b0;
        else 
            vgaclk = ~vgaclk;

    // generate monitor timing signals
    vgaController vgaCont(
        vgaclk, 
        reset, 
        hsync, 
        vsync, 
        sync_b, 
        blank_b, 
        x, 
        y
    );

    // user-defined module to determine pixel color
    videoGen videoGen(
        x, 
        y, 
        r, 
        g, 
        b
    );
endmodule

module vgaController #(
    parameter HBP = 10'd48, // horizontal back porch
              HACTIVE = 10'd640, // number of pixels per line
              HFP = 10'd16, // horizontal front porch
              HSYN = 10'd96, // horizontal sync pulse
              HMAX = HBP + HACTIVE + HFP + HSYN, // total horizontal pixels (48 + 640 + 16 + 96 = 800)
              VBP = 10'd32, // vertical back porch
              VACTIVE = 10'd480, // number of lines
              VFP = 10'd11, // vertical front porch
              VSYN = 10'd2, // vertical sync pulse
              VMAX = VBP + VACTIVE + VFP + VSYN // total vertical pixels (32 + 480 + 11 + 2 = 525)
) (
    input logic vgaclk, reset,
    output logic hsync, vsync, sync_b, blank_b,
    output logic [9:0] hcnt, vcnt
);
    // counters for horizontal and vertical positions
    always @(posedge vgaclk, posedge reset) begin
        if (reset) begin
            hcnt <= 0;
            vcnt <= 0;
        end else begin
            hcnt++;
            if (hcnt == HMAX) begin
                hcnt <= 0;
                vcnt++;
                if (vcnt == VMAX)
                    vcnt <= 0;
            end
        end
    end

    // compute sync signals (active low)
    assign hsync = ~((hcnt >= (HACTIVE + HFP)) & (hcnt < (HACTIVE + HFP + HSYN)));
    assign vsync = ~((vcnt >= (VACTIVE + VFP)) & (vcnt < (VACTIVE + VFP + VSYN)));
    
    assign sync_b = 1'b0; // this should be 0 for newer monitors
    // for older monitors, use: assign sync_b = hsync & vsync;

    // force outputs to black when not writing pixels
    assign blank_b = (hcnt < HACTIVE) & (vcnt < VACTIVE);
endmodule

module videoGen(
    input logic [9:0] x, y,
    output logic [7:0] r, g, b
);
    logic pixel, inrect;

    // given y position, choose a character to display
    // then look up the pixel value from the character ROM
    // and display it in red or blue. Also draw a green rectangle.
    chargenrom chargenromb(
        y[8:3] + 8'd65, 
        x[2:0], 
        y[2:0], 
        pixel
    );
    rectgen rectgen(
        x, 
        y, 
        10'd120, 
        10'd150, 
        10'd200, 
        10'd230, 
        inrect
    );

    assign {r, b} = (y[3] == 0) ? {{8{pixel}}, 8'h00} : {8'h00, {8{pixel}}};
    assign g = inrect ? 8'hFF : 8'h00;
endmodule

module chargenrom(
    input logic [7:0] ch,
    input logic [2:0] xoff, yoff,
    output logic pixel
);
    logic [5:0] charrom[2047:0]; // character generator ROM
    logic [7:0] line; // a line read from the ROM

    // initialize ROM with characters from text file
    initial $readmemb("charrom.txt", charrom);

    // index into ROM to find line of character
    assign line = charrom[yoff + {ch - 65, 3'b000}]; // subtract 65 because 'A' is entry 0

    // reverse order of bits
    assign pixel = line[3'd7 - xoff];
endmodule

module rectgen(
    input logic [9:0] x, y, left, top, right, bot,
    output logic inrect
);
    assign inrect = (x >= left && x <= right && y >= top && y <= bot);
endmodule
