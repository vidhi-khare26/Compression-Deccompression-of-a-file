#include<queue>
#include<vector>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<unordered_map>
#include<bits/stdc++.h>
using namespace std;

typedef std::bitset<8> byte;
const std::string WHITESPACE = " \n\r\t\f\v";

class BinaryTree
{
public:
	char value;
	int frequency;
	BinaryTree *left;
	BinaryTree *right;
	
	BinaryTree(char value, int frequency, BinaryTree *left = NULL, BinaryTree *right = NULL)
	{
		this->value = value;
		this->frequency = frequency;
		this->left = left;
		this->right = right;
	}
};

struct CompareFrequencies {
    bool operator()(BinaryTree *node1, BinaryTree *node2)
    {
        return node1->frequency > node2->frequency;
    }
};

class HuffmanEncodeDecode
{
public:
	std::string path;
	priority_queue<BinaryTree*, std::vector<BinaryTree*>, CompareFrequencies> heap; //Min Heap to maitain the minimum frequency element at top
	unordered_map<char, std::string> code; //Compressed code for each distinct char in the input file
	unordered_map<std::string, char> reversed_code;

	HuffmanEncodeDecode(std::string input_file_path)
	{
		this->path = input_file_path;
	}

	//COMPRESSION
	unordered_map<char, int> frequency_from_text(std::string text);
	void build_heap(unordered_map<char, int> frequency_map);
	void build_binary_tree();
	void build_tree_code_helper(BinaryTree *root, std::string current_bits);
	void build_tree_code();
	std::string build_encoded_text(std::string text);
	std::string built_padded_text(std::string encoded_text);
	std::string build_output_text(std::string padded_text);
	std::string compress();

	//DECOMPRESSION
	std::string read_compressed_file(std::string input_path);
	std::string remove_padding(std::string encoded_text);
	std::string decode_encoded_text(std::string text);
	void decompress(std::string input_path);
};

/* ============================  COMPRESSION ============================  */

//Method to calculate frequency of each distinct character in the text
unordered_map<char, int> HuffmanEncodeDecode::frequency_from_text(std::string text)
{
	std::cout << endl << "Frequency of each character in the file" << endl;
	unordered_map<char, int> frequency_map;
	
	for (auto ch : text)
	{
		if(frequency_map.find(ch) == frequency_map.end())
			frequency_map[ch] = 0;

		frequency_map[ch]++;
	}

	for(auto element : frequency_map)
	{
		std::cout << element.first << " -> " << element.second << endl;
	}

	std::cout << endl;
	
	return frequency_map;
}

//Method to build min heap to facilitate the removal of minimum frequency character
void HuffmanEncodeDecode :: build_heap(unordered_map<char, int> frequency_map)
{
	for(auto element : frequency_map)
	{
		BinaryTree *node =  new BinaryTree(element.first, element.second);
		heap.push(node);
	}

	return;
}

//Method to build binary tree from the min heap to decide the code of each character
//Construct binary tree in bottom to top manner
void HuffmanEncodeDecode::build_binary_tree()
{
	while(heap.size() > 1)
	{
		BinaryTree *node1 = heap.top();
		heap.pop();
		BinaryTree *node2 = heap.top();
		heap.pop();

		int sum_of_frequencies = node1->frequency + node2->frequency;
		
		BinaryTree *new_node = new BinaryTree('\0', sum_of_frequencies, node1, node2);
		heap.push(new_node);
	}

	return;
}

//Helper Method to build the compressed code for each character
void HuffmanEncodeDecode::build_tree_code_helper(BinaryTree *root, std::string current_bits)
{
	if(root == NULL)
		return;

	if(root->value != '\0')
	{
		code[root->value] = current_bits;
		reversed_code[current_bits] = root->value;
		std::cout << root->value << " " << current_bits << endl;

		return;
	}

	build_tree_code_helper(root->left, current_bits + "0");
	build_tree_code_helper(root->right, current_bits + "1");

	return;
}

//Method to build the compressed code for each character
void HuffmanEncodeDecode :: build_tree_code()
{
	BinaryTree *root = heap.top();
	heap.pop();

	std::cout << "Code corresponding to each character" << endl;
	build_tree_code_helper(root, "");
	std::cout << endl; 
	
	return;
}

//Method to encode the text using the compressed code for each character
std::string HuffmanEncodeDecode :: build_encoded_text(std::string text)
{
	std::string encoded_text = "";

	for(auto ch : text)
		encoded_text += code[ch];

	return encoded_text;
}

//Method to pad the encoded text
std::string HuffmanEncodeDecode :: built_padded_text(std::string encoded_text)
{
	int padding_value = 8 - (encoded_text.length() % 8);
	
	for(int i = 0; i < padding_value; i++)
		encoded_text += "0";

	std::string padded_information = bitset<8>(padding_value).to_string();
	std::string padded_text = padded_information + encoded_text;

	return padded_text;
}

//Method to convert binary string to decimal 
int binary_to_decimal(string input_str) 
{
    int result = 0;

    for (auto ch : input_str) 
    {
        result = result * 2 + ch - '0';
    }

    return result;
}

//Method to built a output text of the padded text
std::string HuffmanEncodeDecode :: build_output_text(std::string padded_text)
{
	string output_text = "";

	for(int i = 0; i < padded_text.length(); i += 8)
	{
		std::string str_byte = padded_text.substr(i, 8);
		char ch = binary_to_decimal(str_byte);
		output_text += ch;
	}

	return output_text;
}

//Method to get the extension of the file
std::string get_file_name(std::string file_path) 
{
	std::string file_name;

   	size_t i = file_path.rfind('.', file_path.length());
   	file_name = file_path.substr(0, i);

	return file_name;
}

//Method to read text from the file into string
std::string read_file_into_string(std::string file_path) 
{
    auto ss = ostringstream{};

    std::ifstream input_file(file_path);
    
    if(!input_file.is_open()) 
    {
        std::cerr << "Could not open the file - '" << file_path << "'" << endl;
        exit(EXIT_FAILURE);
    }

    ss << input_file.rdbuf();

    input_file.close();

    //Removing white spaces from the end of the file
    std::string text = ss.str();
    size_t end = text.find_last_not_of(WHITESPACE);
    text = (end == std::string::npos) ? "" : text.substr(0, end + 1);

    return text;
}

//Method to write the compressed text to the output file
int write_data_to_file(std::string output_path, std::string text) 
{
	std::cout << "Writing the data to the output file: " << output_path << endl;

	std::ofstream output_file(output_path);
    
    if(!output_file.is_open()) 
    {
        std::cerr << "Could not open the file - '" << output_path << "'" << endl;
        exit(EXIT_FAILURE);
    }
    
    output_file << text;
 
    output_file.close();

    if(!output_file.good()) 
    {
      cout << "Error occurred at writing time!" << endl;
      return 0;
   }

    return 1;
}

//Method to access the file and extract text from the file
std::string HuffmanEncodeDecode::compress()
{
	std::cout << endl << "-------------------- File Compression Started --------------------" << endl;

	//To access the file and extract the text from the file
	std::string file_name = get_file_name(path);
	std::string output_path = file_name + "_compressed.txt";
	std::string text = read_file_into_string(path);

	//Calculate frequency of each character and store iit in frequency dictionary
	unordered_map<char, int> frequency_map = frequency_from_text(text);

	//Build Min Heap
	build_heap(frequency_map);

	//Construct binary tree from heap
	build_binary_tree();

	//Construct code from binary tree and store it in the unordered_map
	build_tree_code();

	//Construct encoded text
	std::string encoded_text = build_encoded_text(text);

	//Padding of encoded text
	std::string padded_text = built_padded_text(encoded_text);

	//Return the compressed binary file-data as output
	std::string compressed_text = build_output_text(padded_text);

	//Write the comprressed data to the output file
	int status = write_data_to_file(output_path, compressed_text);
	
	if(status == 1)
		std::cout << endl << "-------------------- File Compressed Successfully --------------------" << endl;

	return output_path;
}

/* ============================  DECOMPRESSION ============================  */

//Method to convert decimal to binary string
std::string decimal_to_binary(int inNum) 
{
    std::string result = "";
    
    for (int i = 7; i >= 0; i--) 
    {
        int k = inNum >> i;
        if (k & 1)
            result += "1";
        else
            result += "0";
    }

    return result;
}

//Method to read the compressed file and return bit 
std::string HuffmanEncodeDecode :: read_compressed_file(std::string text)
{
    std::string bit_string = "";
    for(auto ch : text)
    {	
    	int int_text = (int)ch;
    	
    	if(int_text < 0)
    		int_text = 256 + int_text;
   
    	bit_string += decimal_to_binary(int_text);
    }

    return bit_string;
}

//Method to remove padding from the encoded text
std::string HuffmanEncodeDecode :: remove_padding(std::string encoded_text)
{
	std::string padded_info = encoded_text.substr(0, 8);
	int padded_value = stoi(padded_info, nullptr, 2);

	encoded_text = encoded_text.substr(8, encoded_text.length()-8);
	encoded_text = encoded_text.substr(0, encoded_text.length()-padded_value);

	return encoded_text;
}

//Method to decode the encoded text to actual text
std::string HuffmanEncodeDecode :: decode_encoded_text(std::string text)
{
	std::string current_bits = "";
	std::string decoded_text = "";

	for(auto ch : text)
	{
		current_bits += ch;

		if(reversed_code.find(current_bits) != reversed_code.end())
		{
			decoded_text += reversed_code[current_bits];
			current_bits = "";
		}
	}

	return decoded_text;
}

//Method to decompress the compressed file
void HuffmanEncodeDecode::decompress(std::string input_path)
{
	std::cout << endl << "-------------------- File Decompression Started --------------------" << endl << endl;

	//To access the file and extract the text from the file
	std::string file_name = get_file_name(input_path);
	std::string output_path = file_name + "_decompressed.txt";
	std::string text = read_file_into_string(input_path);
	std::string bit_text = read_compressed_file(text);

	//Remove padding of encoded text
	std::string text_after_removing_padding = remove_padding(bit_text);
	std::string actual_text = decode_encoded_text(text_after_removing_padding);
	
	//Write the actual data to th output file
	int status = write_data_to_file(output_path, actual_text);

	if(status == 1)
		std::cout << endl << "-------------------- File Deompressed Successfully --------------------" << endl;

	return;
}

int main()
{
	string input_file_path;
	
	std::cout << "Enter the path of your file to be compressed." << endl;
	std::cin >> input_file_path;

	HuffmanEncodeDecode hc(input_file_path);
	std::string compressed_file_path = hc.compress();
	
	hc.decompress(compressed_file_path);

	return 0;
}