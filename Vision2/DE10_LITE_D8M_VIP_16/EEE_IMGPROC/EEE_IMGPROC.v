module EEE_IMGPROC(
	// global clock & reset
	clk,
	reset_n,
	
	// mm slave
	s_chipselect,
	s_read,
	s_write,
	s_readdata,
	s_writedata,
	s_address,

	// stream sink
	sink_data,
	sink_valid,
	sink_ready,
	sink_sop,
	sink_eop,
	
	// streaming source
	source_data,
	source_valid,
	source_ready,
	source_sop,
	source_eop,
	
	// conduit
	mode
	
);


// global clock & reset
input	clk;
input	reset_n;

// mm slave
input							s_chipselect;
input							s_read;
input							s_write;
output	reg	[31:0]	s_readdata;
input	[31:0]				s_writedata;
input	[2:0]					s_address;


// streaming sink
input	[23:0]            	sink_data;
input								sink_valid;
output							sink_ready;
input								sink_sop;
input								sink_eop;

// streaming source
output	[23:0]			  	   source_data;
output								source_valid;
input									source_ready;
output								source_sop;
output								source_eop;

// conduit export
input                         mode;

////////////////////////////////////////////////////////////////////////
//
parameter IMAGE_W = 11'd640;
parameter IMAGE_H = 11'd480;
parameter MESSAGE_BUF_MAX = 256;
parameter MSG_INTERVAL = 6;
parameter BB_COL_DEFAULT = 24'h00ff00;


wire [7:0]   red, green, blue, grey;
wire [7:0]   red_out, green_out, blue_out;

wire         sop, eop, in_valid, out_ready;
////////////////////////////////////////////////////////////////////////

// HSV Conversion
wire [7:0] hue, saturation, value, cmax, cmin;

assign cmax = (blue > green) ? ((blue > red) ? blue[7:0] : red[7:0]) : (green > red) ? green [7:0] : red[7:0];
assign cmin = (blue < green) ? ((blue < red) ? blue[7:0] : red[7:0]) : (green < red) ? green [7:0] : red[7:0];
assign hue = (cmax == cmin) ? 0 
: (cmax == red) ? ( (green>blue) ? ((((15*((green - blue) / ((cmax - cmin)>>2)))>>1)+180)%180) : ((180-((15*((blue - green) / ((cmax - cmin)>>2)))>>1))%180) ) 
: (cmax == green) ? ( (blue>red) ? ((((15*((blue - red) / ((cmax - cmin)>>2)))>>1)+60)%180) : ((60-((15*((red - blue) / ((cmax - cmin)>>2)))>>1))%180) ) 
:               ( (red>green) ? ((((15*((red - green) / ((cmax - cmin)>>2)))>>1)+120)%180) : ((120-((15*((green - red) / ((cmax - cmin)>>2)))>>1))%180) ); // 0 to 180
assign saturation = (cmax == 0) ? 0 : ((cmax - cmin)* 100 / cmax); // 0 to 100%
assign value = ((cmax)/255)*100; // 0 to 100%

// Detect areas of interest
wire red_detect, blue_detect, yellow_detect, wall_detect;
assign red_detect = (hue >= 0 && hue  <= 10) && (saturation > 50 && saturation <= 100 && value >= 70 );
assign blue_detect = (hue >= 80 && hue <= 125) && (saturation > 60 && saturation <= 100 && value >= 70 );
assign yellow_detect = (hue >= 30 && hue <= 60) && (saturation > 60 && saturation <= 100 && value >= 40);
assign wall_detect = (hue < 1) && (saturation < 1) && (value > 99);

// Find boundary of cursor box

// Filter
reg prev_b, prev_b1, prev_b2;
reg prev_r, prev_r1, prev_r2;
reg prev_y, prev_y1, prev_y2;
reg prev_w, prev_w1, prev_w2;

initial begin 
	prev_b<=0;
	prev_b1<=0;
	prev_b2<=0;
	prev_r<=0;
	prev_r1<=0;
	prev_r2<=0;
	prev_y<=0;
	prev_y1<=0;
	prev_y2<=0;
	prev_w<=0;
	prev_w1<=0;
	prev_w2<=0;
end

always@(negedge clk) begin
	prev_b2 = prev_b1;
	prev_b1 = prev_b;
	prev_b = blue_detect;	
	
	prev_r2 = prev_r1;
	prev_r1 = prev_r;
	prev_r = red_detect;	
	
	prev_y2 = prev_y1;
	prev_y1 = prev_y;
	prev_y = yellow_detect;
	
	prev_w2 = prev_w1;
	prev_w1 = prev_w;
	prev_w = wall_detect;
end

// Highlight detected areas
wire [23:0] highlight;
assign grey = green[7:1] + red[7:2] + blue[7:2]; // Grey = green/2 + red/4 + blue/4
assign highlight = 
		  (red_detect && prev_r && prev_r1 && prev_r2) ? {8'hff, 8'h0, 8'h0}
		: (blue_detect && prev_b && prev_b1 && prev_b2) ? {8'h0,8'h0,8'hff}
		: (yellow_detect && prev_y && prev_y1 && prev_y2) ? {8'hff,8'hff,8'h0}
		: (wall_detect && prev_w && prev_w1 && prev_w2) ? {8'hff,8'hff,8'hff}
		: {grey, grey, grey};

// Show bounding box
wire [23:0] red_new_image;
wire red_bb_active;
assign red_bb_active = (x == red_left) | (x == red_right) | (y == red_top) | (y == red_bottom);
assign red_new_image = red_bb_active ? {24'hff0000} : highlight;

wire [23:0] blue_new_image;
wire blue_bb_active;
assign blue_bb_active = (x == blue_left) | (x == blue_right) | (y == blue_top) | (y == blue_bottom);
assign blue_new_image = blue_bb_active ? {24'h0000ff} : red_new_image;

wire [23:0] yellow_new_image;
wire yellow_bb_active;
assign yellow_bb_active = (x == yellow_left) | (x == yellow_right) | (y == yellow_top) | (y == yellow_bottom);
assign yellow_new_image = yellow_bb_active ? {24'hffff00} : blue_new_image;

wire [23:0] wall_new_image;
wire wall_bb_active;
assign wall_bb_active = (x == l1_wall_x) | (y == l1_wall_y) | (x == l2_wall_x) | (y == l2_wall_y) | (x == r1_wall_x) | (y == r1_wall_y) | (x == r2_wall_x) | (y == r2_wall_y) | (x == m1_wall_x) | (y == m1_wall_y) | (x == m2_wall_x) | (y == m2_wall_y);
assign wall_new_image = wall_bb_active ? {24'hffffff} : yellow_new_image;

// Switch output pixels depending on mode switch
// Don't modify the start-of-packet word - it's a packet discriptor
// Don't modify data in non-video packets
assign {red_out, green_out, blue_out} = (mode & ~sop & packet_video) ? wall_new_image : {red,green,blue};

//Count valid pixels to tget the image coordinates. Reset and detect packet type on Start of Packet.
reg [10:0] x, y;
reg packet_video;
always@(posedge clk) begin
	if (sop) begin
		x <= 11'h0;
		y <= 11'h0;
		packet_video <= (blue[3:0] == 3'h0);
	end
	else if (in_valid) begin
		if (x == IMAGE_W-1) begin
			x <= 11'h0;
			y <= y + 11'h1;
		end
		else begin
			x <= x + 11'h1;
		end
	end
end

//Find first and last pixels
reg [10:0] 	red_x_min, red_x_max, red_y_min, red_y_max;
reg [10:0]	blue_x_min, blue_x_max, blue_y_min, blue_y_max;
reg [10:0]	yellow_x_min, yellow_x_max, yellow_y_min, yellow_y_max;
reg [10:0]	l1_wall_x_min, l1_wall_y_min, l2_wall_x_min, l2_wall_y_min;
reg [10:0]	r1_wall_x_min, r1_wall_y_min, r2_wall_x_min, r2_wall_y_min;
reg [10:0]	m1_wall_x_min, m1_wall_y_min, m2_wall_x_min, m2_wall_y_min;
reg			l_wall, r_wall, m_wall;

always@(posedge clk) begin

	if ((red_detect && prev_r && prev_r1 && prev_r2)& in_valid) begin
		if (x < red_x_min) red_x_min <= x;
		if (x > red_x_max) red_x_max <= x;
		if (y < red_y_min) red_y_min <= y;
		red_y_max <= y;
	end
	
	if ((blue_detect && prev_b && prev_b1 && prev_b2)& in_valid) begin
		if (x < blue_x_min) blue_x_min <= x;
		if (x > blue_x_max) blue_x_max <= x;
		if (y < blue_y_min) blue_y_min <= y;
		blue_y_max <= y;	
	end
	
	if ((yellow_detect && prev_y && prev_y1 && prev_y2)& in_valid) begin
		if (x < yellow_x_min) yellow_x_min <= x;
		if (x > yellow_x_max) yellow_x_max <= x;
		if (y < yellow_y_min) yellow_y_min <= y;
		yellow_y_max <= y;	
	end

	if ((wall_detect && prev_w && prev_w1 && prev_w2)& in_valid) begin
	
		if (x < 120 && y > 420) begin
		l1_wall_x_min <= x; 
		l1_wall_y_min <= y;
		end
		if ((x >= 80 && x < 180) && (y > 350 && y <= 410)) begin
		l2_wall_x_min <= x; 
		l2_wall_y_min <= y;
		end
		
		if ((x >= 220 && x < 300) && (y > 400 && y <= 440)) begin
		m1_wall_x_min <= x; 
		m1_wall_y_min <= y;
		end
		if ((x >= 340 && x < 420) && (y > 400 && y <= 440)) begin
		m2_wall_x_min <= x; 
		m2_wall_y_min <= y;
		end
		
		if ((x >= 460 && x < 560) && (y > 350 && y <= 410)) begin
		r1_wall_x_min <= x; 
		r1_wall_y_min <= y;
		end
		if (x >= 520 && y > 420) begin
		r2_wall_x_min <= x; 
		r2_wall_y_min <= y;
		end
		
		// Calculate beacon distance from camera
		if (l1_wall_x_min && l2_wall_x_min)
			l_wall <= 1'b1;
		else 
			l_wall <= 0;
		
		if (r1_wall_x_min && r2_wall_x_min)
			r_wall <= 1'b1;
		else 
			r_wall <= 0;
		
		if (m1_wall_x_min && m2_wall_x_min)
			m_wall <= 1'b1;
		else 
			m_wall <= 0;
		
	end
	
	if (sop & in_valid) begin	// Reset bounds on start of packet
		red_x_min <= IMAGE_W-11'h1;
		red_x_max <= 0;
		red_y_min <= IMAGE_H-11'h1;
		red_y_max <= 0;
		
		blue_x_min <= IMAGE_W-11'h1;
		blue_x_max <= 0;
		blue_y_min <= IMAGE_H-11'h1;
		blue_y_max <= 0;

		yellow_x_min <= IMAGE_W-11'h1;
		yellow_x_max <= 0;
		yellow_y_min <= IMAGE_H-11'h1;
		yellow_y_max <= 0;
		
		l1_wall_x_min <= 0;
		l1_wall_y_min <= 0;
		l2_wall_x_min <= 0;
		l2_wall_y_min <= 0;
		
		r1_wall_x_min <= 0;
		r1_wall_y_min <= 0;
		r2_wall_x_min <= 0;
		r2_wall_y_min <= 0;
		
		m1_wall_x_min <= 0;
		m1_wall_y_min <= 0;
		m2_wall_x_min <= 0;
		m2_wall_y_min <= 0;
	end
end

//Process bounding box at the end of the frame.
reg [2:0] msg_state;
reg [7:0] frame_count;
reg [10:0] red_left, red_right, red_top, red_bottom, red_distance,
			  blue_left, blue_right, blue_top, blue_bottom, blue_distance,
			  yellow_left, yellow_right, yellow_top, yellow_bottom, yellow_distance,
			  l1_wall_x, l1_wall_y, l2_wall_x, l2_wall_y,
			  r1_wall_x, r1_wall_y, r2_wall_x, r2_wall_y,
			  m1_wall_x, m1_wall_y, m2_wall_x, m2_wall_y;
			  
always@(posedge clk) begin
	if (eop & in_valid & packet_video) begin  //Ignore non-video packets
		
		//Latch edges for display overlay on next frame
		red_left <= red_x_min;
		red_right <= red_x_max;
		red_top <= red_y_min;
		red_bottom <= red_y_max;
		red_distance <= 4800/(red_y_max - red_y_min);
		
		blue_left <= blue_x_min;
		blue_right <= blue_x_max;
		blue_top <= blue_y_min;
		blue_bottom <= blue_y_max;
		blue_distance <= 4800/(blue_y_max - blue_y_min);
		
		yellow_left <= yellow_x_min;
		yellow_right <= yellow_x_max;
		yellow_top <= yellow_y_min;
		yellow_bottom <= yellow_y_max;
		yellow_distance <= 4800/(yellow_y_max - yellow_y_min);
		
		l1_wall_x <= l1_wall_x_min;
		l1_wall_y <= l1_wall_y_min;
		l2_wall_x <= l2_wall_x_min;
		l2_wall_y <= l2_wall_y_min;
		
		r1_wall_x <= r1_wall_x_min;
		r1_wall_y <= r1_wall_y_min;
		r2_wall_x <= r2_wall_x_min;
		r2_wall_y <= r2_wall_y_min;
		
		m1_wall_x <= m1_wall_x_min;
		m1_wall_y <= m1_wall_y_min;
		m2_wall_x <= m2_wall_x_min;
		m2_wall_y <= m2_wall_y_min;
		
		
		//Start message writer FSM once every MSG_INTERVAL frames, if there is room in the FIFO
		frame_count <= frame_count - 1;
		
		if (frame_count == 0 && msg_buf_size < MESSAGE_BUF_MAX - 3) begin
			msg_state <= 3'b0001;
			frame_count <= MSG_INTERVAL-1;
		end
	end
	
	//Cycle through message writer states once started
	if (msg_state != 3'b0000) msg_state <= msg_state + 3'b0001;

end
	
//Generate output messages for CPU
reg [31:0] msg_buf_in; 
wire [31:0] msg_buf_out;
reg msg_buf_wr;
wire msg_buf_rd, msg_buf_flush;
wire [7:0] msg_buf_size;
wire msg_buf_empty;

`define RED_BOX_MSG_ID "RBB"
`define BLUE_BOX_MSG_ID "BBB"
`define YELLOW_BOX_MSG_ID "YBB"
`define WALL_BOX_MSG_ID "WBB"

always@(*) begin	// Write words to FIFO as state machine advances
	case(msg_state)
		3'b000: begin
			msg_buf_in = 32'b0;
			msg_buf_wr = 1'b0;
		end
		3'b001: begin
			msg_buf_in = {8'b01110010, 13'b0, red_distance}; // Bottom left coordinate
			msg_buf_wr = 1'b1;
		end
		
		3'b010: begin
			msg_buf_in = {8'b01100010, 13'b0, blue_distance}; // Bottom left coordinate
			msg_buf_wr = 1'b1;
		end

		3'b011: begin
			msg_buf_in = {8'b01111001, 13'b0, yellow_distance}; // Bottom left coordinate
			msg_buf_wr = 1'b1;
		end

		3'b100: begin
			msg_buf_in = {29'b0, l_wall, m_wall, r_wall}; // All walls
			msg_buf_wr = 1'b1;
		end

		default: begin
			msg_buf_in = 32'b0;
			msg_buf_wr = 1'b0;
		end
	endcase
end


//Output message FIFO
MSG_FIFO	MSG_FIFO_inst (
	.clock (clk),
	.data (msg_buf_in),
	.rdreq (msg_buf_rd),
	.sclr (~reset_n | msg_buf_flush),
	.wrreq (msg_buf_wr),
	.q (msg_buf_out),
	.usedw (msg_buf_size),
	.empty (msg_buf_empty)
	);


//Streaming registers to buffer video signal
STREAM_REG #(.DATA_WIDTH(26)) in_reg (
	.clk(clk),
	.rst_n(reset_n),
	.ready_out(sink_ready),
	.valid_out(in_valid),
	.data_out({red,green,blue,sop,eop}),
	.ready_in(out_ready),
	.valid_in(sink_valid),
	.data_in({sink_data,sink_sop,sink_eop})
);

STREAM_REG #(.DATA_WIDTH(26)) out_reg (
	.clk(clk),
	.rst_n(reset_n),
	.ready_out(out_ready),
	.valid_out(source_valid),
	.data_out({source_data,source_sop,source_eop}),
	.ready_in(source_ready),
	.valid_in(in_valid),
	.data_in({red_out, green_out, blue_out, sop, eop})
);


/////////////////////////////////
/// Memory-mapped port		 /////
/////////////////////////////////

// Addresses
`define REG_STATUS    			0
`define READ_MSG    				1
`define READ_ID    				2
`define REG_BBCOL					3

//Status register bits
// 31:16 - unimplemented
// 15:8 - number of words in message buffer (read only)
// 7:5 - unused
// 4 - flush message buffer (write only - read as 0)
// 3:0 - unused


// Process write

reg  [7:0]   reg_status;
reg	[23:0]	bb_col;

always @ (posedge clk)
begin
	if (~reset_n)
	begin
		reg_status <= 8'b0;
		bb_col <= BB_COL_DEFAULT;
	end
	else begin
		if(s_chipselect & s_write) begin
		   if      (s_address == `REG_STATUS)	reg_status <= s_writedata[7:0];
		   if      (s_address == `REG_BBCOL)	bb_col <= s_writedata[23:0];
		end
	end
end


//Flush the message buffer if 1 is written to status register bit 4
assign msg_buf_flush = (s_chipselect & s_write & (s_address == `REG_STATUS) & s_writedata[4]);


// Process reads
reg read_d; //Store the read signal for correct updating of the message buffer

// Copy the requested word to the output port when there is a read.
always @ (posedge clk)
begin
   if (~reset_n) begin
	   s_readdata <= {32'b0};
		read_d <= 1'b0;
	end
	
	else if (s_chipselect & s_read) begin
		if   (s_address == `REG_STATUS) s_readdata <= {16'b0,msg_buf_size,reg_status};
		if   (s_address == `READ_MSG) s_readdata <= {msg_buf_out};
		if   (s_address == `READ_ID) s_readdata <= 32'h1234EEE2;
		if   (s_address == `REG_BBCOL) s_readdata <= {8'h0, bb_col};
	end
	
	read_d <= s_read;
end

//Fetch next word from message buffer after read from READ_MSG
assign msg_buf_rd = s_chipselect & s_read & ~read_d & ~msg_buf_empty & (s_address == `READ_MSG);
						
endmodule
