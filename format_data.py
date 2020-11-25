import sys
import time

c_file = open(sys.argv[1], 'r')
c_code = c_file.read()
c_file.close()

data_file = open(sys.argv[2], 'r')

def get_function_name(line_num, col_num):
	line_count = 0
	line_list = []
	for line in c_code.split('\n'):
		line_list.append(line.strip()) 
		line_count = line_count + 1
		if line_count == line_num:
			close_count = 0
			curr_line_num = line_count-1
			char_number = col_num + 1
			while not (close_count == 0 and line_list[curr_line_num][char_number] == "("):
				if line_list[curr_line_num][char_number] == ")":
					close_count = close_count + 1
				if line_list[curr_line_num][char_number] == "(":
					close_count = close_count - 1
				if char_number == 0:
					curr_line_num = curr_line_num-1
					char_number = len(line_list[curr_line_num])-1
				else:
					char_number = char_number - 1
			if char_number == 0:
				curr_line_num = curr_line_num - 1
				char_number = len(line_list[curr_line_num])-1
			end_char_number = char_number
			start_char_number = char_number-1
			while line_list[curr_line_num][start_char_number] == " ":
				start_char_number = start_char_number - 1
			while line_list[curr_line_num][start_char_number] != " " and start_char_number != 0:
				start_char_number = start_char_number - 1
			return line_list[curr_line_num][start_char_number:end_char_number].strip()

def extract_function(info_line):
	info = info_line.strip().split('<')[1].split('>')[0].split(',')[0]
	line_num = int(info.split(":")[1])
	col_num = int(info.split(":")[2])
	return get_function_name(line_num, col_num)

def is_pointer(info_line):
	info = "\'".join(info_line.strip().split('\'')[1:])
	if ":" in info:
		info = info.split('\'')[2]
	else:
		info = info[:-1]
	return ('*' in info)

def process_lines(line_list):
	output_string = ""
	output_string = output_string + line_list[0].split(',')[0]
	if output_string == "Function":
		output_string = output_string + "," + line_list[0].strip().split(',')[1].strip()
		print(output_string)
		return
	if output_string == "Global":
		output_string = output_string + "," + line_list[0].strip().split(',')[1].strip()
	if output_string == "Local":
		output_string = output_string + "," + line_list[0].strip().split(',')[1].strip()
	if output_string == "Parameter":
		function_name = extract_function(line_list[1])
		parameter_name = line_list[0].split(',')[1].strip()
		argument_number = line_list[0].split(',')[2].strip()
		output_string = output_string + "," + function_name + "," + str(argument_number) + "," + parameter_name
	if is_pointer(line_list[1]):
		output_string = output_string + ",1"
	else:
		output_string = output_string + ",0"
	print(output_string)

line_list = []
for line in data_file:
	if len(line.strip()) == 0:
		if len(line_list) > 0:
			process_lines(line_list)
		line_list = []
	else:
		line_list.append(line.strip())
data_file.close()

