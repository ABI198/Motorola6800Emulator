#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int IP;		// Instruction Pointer  
int A = 0;		// Accumlator A
int B = 0;		// Accumlator B
int IX = 0;		// Index register
char CCR[8];	// Condition Code Register
int Memory[4096] = {0}; // Memory
int cmp = 0; //for comparing
int i;

struct Label
{
	char line[20];
};
struct Label labelArray[20];
int labelArraySize = 0;


int colonMarkQuery(char* str) //check colon mark situation(:)
{
	for (i = 0; i < strlen(str); ++i)
	{
		if (str[i] == ':')
			return 1;
	}
	return 0;
}

int immediateAddr(char* str, char* operand, char* reference) //check sharp mark situation(#)
{
	if (str == NULL) //because last loop must return NULL to quit 
		return 0;
	else
	{
		for (i = 0; i < strlen(str); ++i)
		{
			if (str[i] == '#')
			{
				char* tempToken;
				tempToken = strtok(str, "#");

				strcpy(operand, tempToken);
				strcpy(reference, "0");
				return 1;
			}
		}
		return 0;
	}
}

int indexedAddr(char* str, char* operand, char* reference) //check comma situation(,)
{
	if (str == NULL) //because last loop must return NULL to quit 
		return 0;
	else
	{
		for (i = 0; i < strlen(str); ++i)
		{
			if (str[i] == ',')
			{
				char* tempToken;
				tempToken = strtok(str, ",");

				strcpy(operand, tempToken);
				strcpy(reference, "0");

				return 1;
			}
		}
		return 0;
	}
}

int inherentAddr(char* str1, char* str2) //check inherent addressing or not
{
	if (str1 == NULL)
		return 0;
	else
	{
		char* tempToken;
		tempToken = strtok(str2, "\n");

		for (i = 0; i < strlen(str2); ++i) //Compare
		{
			if (str1[i] != str2[i])
				return 0;
		}
		return 1;	//EQUAL
	}
}

int relativeAddr(char* str, char* label)//check relative addressing or not
{
	if (str == NULL) //because last loop must return NULL to quit 
		return 0;
	else
	{
		if (!strcmp(str, label))
			return 1;
		else
			return 0;
	}
}

int directAddr(char* str, char* operand)
{
	for (i = 0; i < strlen(str); ++i)
	{
		if (str[i] == 'H')
		{
			strcpy(operand, "0");
			return 1;
		}
	}
	return 0;
}

void operationParse(char* instruction, char* label, char* opcode, char* operand, char* reference, char* addressingMode)
{
	int nonstatic_labelArraySize = 0;

	char* tempData = (char*)malloc(20);		//creat a char array dynamically
	char* tempDataCopy = (char*)malloc(20);
	char* token = (char*)malloc(20);
	char* token2 = (char*)malloc(20);
	char* token3 = (char*)malloc(20);
	char* token4 = (char*)malloc(20);
	char* token5 = (char*)malloc(20);


	strcpy(tempData, instruction);
	strcpy(tempDataCopy, instruction);
	token = strtok(tempData, " ,\n");	//begenning tokens

	if (colonMarkQuery(tempData)) //Check Label and colon mark situation(:)
	{
		char* tempToken;
		tempToken = strtok(NULL, " ");			//LOOP: (STAA)
		char* tokenData = (char*)malloc(20);	//for tokenizing part2
		tokenData = strtok(token, ":");	//purifying from ':'
		strcpy(labelArray[labelArraySize].line, tokenData);
		++labelArraySize;
		++nonstatic_labelArraySize; //for Inherent Problem!!


		strcpy(label, tokenData);
		strcpy(opcode, tempToken);
	}
	else
	{
		strcpy(label, "0");
		strcpy(opcode, token);
	}


	//ADDRESSING MODES

	strcpy(token, tempDataCopy);
	token = strtok(token, " ");
	while (token != NULL)			//IMMEDIATE ADDRESSÝNG
	{
		token = strtok(NULL, " \n"); //following tokens    
		if (immediateAddr(token, operand, reference))//check sharp mark situation(#)   
		{
			strcpy(addressingMode, "Immediate");
			return;
		}
	}


	strcpy(token2, tempDataCopy);
	token2 = strtok(token2, " ");
	while (token2 != NULL)			//INDEXED ADDRESSÝNG
	{
		token2 = strtok(NULL, " "); //following tokens    

		if (indexedAddr(token2, operand, reference))//check sharp mark situation(#)   
		{
			strcpy(addressingMode, "Indexed");
			return;
		}
	}



	strcpy(token4, tempDataCopy);
	token4 = strtok(token4, " ");
	while (token4 != NULL)			  //RELATIVE ADDRESSÝNG
	{
		token4 = strtok(NULL, " \n"); //following tokens  
		int i = 0;
		for (i = 0; i < labelArraySize; ++i)
		{
			if (relativeAddr(token4, labelArray[i].line)) //controlling the static label array!
			{
				strcpy(operand, "0");
				strcpy(reference, labelArray[i].line);
				strcpy(addressingMode, "Relative");
			}
		}
	}

	strcpy(token5, tempDataCopy);
	token5 = strtok(token5, " ");
	while (token5 != NULL)			//DIRECT ADDRESSÝNG
	{
		token5 = strtok(NULL, " \n"); //following tokens   

		if (token5 != NULL)	// For Inherent Special Case!
		{
			if (!(colonMarkQuery(token5) || indexedAddr(token5, operand, reference)))//check sharp mark situation(#)   
			{
				if (directAddr(token5, operand))
				{

					strcpy(reference, token5);
					strcpy(addressingMode, "Direct");
					return;
				}
			}
		}
	}


	if (nonstatic_labelArraySize > 0)  //nonstatic_labelArraySize is for Inherent Label Problem!!
	{
		token3 = strtok(tempDataCopy, " ");
		token3 = strtok(NULL, "");		//To destroy label!!

		strcpy(token3, tempDataCopy);
		while (token3 != NULL)			//INHERENT ADDRESSÝNG
		{
			token3 = strtok(token3, " "); //following tokens 
			if (inherentAddr(token3, tempDataCopy))
			{
				strcpy(operand, "0");
				strcpy(reference, "0");
				strcpy(addressingMode, "Inherent");
				return;
			}
			else
				token3 = NULL; // to block infinite loop!!
		}
	}
	else
	{
		strcpy(token3, tempDataCopy);
		while (token3 != NULL)			//INHERENT ADDRESSÝNG
		{
			token3 = strtok(token3, " "); //following tokens 
			if (inherentAddr(token3, tempDataCopy))
			{
				strcpy(operand, "0");
				strcpy(reference, "0");
				strcpy(addressingMode, "Inherent");

				return;
			}
			else
				token3 = NULL; // to block infinite loop!!
		}
	}
}

int find_H(char* str)
{
	for (i = 0; i < strlen(str); ++i)
	{
		if (str[i] == 'H')
			return 1;
	}
	return 0;
}



int main(void)
{
	struct txt {
		char line[20];
	};
	struct txt textbuffer[100]; //a hundred array with 20 bit

	struct st {
		char	label[5];
		int	textindex;
	};
	struct st sdt[20];		//for labels


	FILE* file;	//file pointer
	file = fopen("AssemblyCode.txt", "r"); //open input file for reading

	int i = 0;//counter for reading line by line from structure
	int j = 0; //counter for std struct
	while (!feof(file))
	{
		char* tempInstruction = (char*)malloc(20);
		fgets(tempInstruction, 20, file);

		strcpy(textbuffer[i].line, tempInstruction);
		if (colonMarkQuery(tempInstruction)) //check ':' situation in terms of having or not
		{
			char* tempLabel;	//current label
			tempLabel = strtok(tempInstruction, ":");
			strcpy(sdt[j].label, tempLabel); //copy in sdt struct
			sdt[j].textindex = i;
			++j;
		}
		++i;
	}

	IP = 0; //Instruction Pointer Initialization

	strcpy(textbuffer[i - 1].line, "END");
	int memoryVariable;
	Memory[0] = 4;
	while (strcmp(textbuffer[IP].line, "END")) //Start Emulator Mode  *strcmp() return 0 if strings are equal   //UYARI!!! i'deðil de IP olma durumuda var dikkat ETT
	{
		char* label = (char*)malloc(5);
		char* opcode = (char*)malloc(5);
		char* operand = (char*)malloc(5);
		char* reference = (char*)malloc(5);
		char* addressingMode = (char*)malloc(10);

		operationParse(textbuffer[IP].line, label, opcode, operand, reference, addressingMode);

		//to destroy 'H' and obtain integer value!

		if (find_H(operand))
		{
			char* token;

			token = strtok(operand, "H");

			strcpy(operand, token);
			memoryVariable = atoi(operand);

		}
		if (find_H(reference))
		{
			char* token;

			token = strtok(reference, "H");

			strcpy(reference, token);
			memoryVariable = atoi(operand);
		}



		//Beginning of opcode operations
		if (!strcmp(opcode, "LDAA"))
		{
			if (!strcmp(addressingMode, "Direct"))
			{
				A = Memory[atoi(reference)];
			}
			else if (!strcmp(addressingMode, "Immediate"))
			{
				A = atoi(operand);
			}
			else if (!strcmp(addressingMode, "Indexed"))
			{
				A = Memory[IX + atoi(operand)];
			}
		}
		else if (!strcmp(opcode, "LDAB"))
		{
			if (!strcmp(addressingMode, "Direct"))
			{
				B = Memory[atoi(reference)];
			}
			else if (!strcmp(addressingMode, "Immediate"))
			{
				B = atoi(operand);
			}
			else if (!strcmp(addressingMode, "Indexed"))
			{
				B = Memory[IX + atoi(operand)];
				cmp = B;
			}
		}
		else if (!strcmp(opcode, "LDX"))
		{
			if (!strcmp(addressingMode, "Direct"))
			{
				IX = Memory[atoi(reference)];
			}
			else if (!strcmp(addressingMode, "Immediate"))
			{
				IX = atoi(operand);
			}
			else if (!strcmp(addressingMode, "Indexed"))
			{
				IX = Memory[IX + atoi(operand)];
			}
		}
		else if (!strcmp(opcode, "STAA"))
		{
			if (!strcmp(addressingMode, "Direct"))
			{
				Memory[atoi(reference)] = A;
				memoryVariable = Memory[atoi(reference)];
			}
			else if (!strcmp(addressingMode, "Indexed"))
			{
				Memory[atoi(operand) + IX] = A;
				memoryVariable = Memory[atoi(reference)];
			}
		}
		else if (!strcmp(opcode, "STAB"))
		{
			if (!strcmp(addressingMode, "Direct"))
			{
				Memory[atoi(reference)] = B;
			}
			else if (!strcmp(addressingMode, "Indexed"))
			{
				Memory[atoi(operand) + IX] = B;
			}
		}
		else if (!strcmp(opcode, "ADDA"))	//CCR VAR!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		{
			if (!strcmp(addressingMode, "Direct"))
			{
				A += Memory[atoi(reference)];
			}
			else if (!strcmp(addressingMode, "Immediate"))
			{
				A += atoi(operand);
			}
			else if (!strcmp(addressingMode, "Indexed"))
			{
				A += Memory[IX + atoi(operand)];
			}
		}
		else if (!strcmp(opcode, "ADDB"))//CCR VAR!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		{
			if (!strcmp(addressingMode, "Direct"))
			{
				B += Memory[atoi(reference)];
			}
			else if (!strcmp(addressingMode, "Immediate"))
			{
				B += atoi(operand);
			}
			else if (!strcmp(addressingMode, "Indexed"))
			{
				B += Memory[IX + atoi(operand)];
			}
		}
		else if (!strcmp(opcode, "SUBA"))//CCR VAR!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		{
			if (!strcmp(addressingMode, "Direct"))
			{
				A -= Memory[atoi(reference)];
			}
			else if (!strcmp(addressingMode, "Immediate"))
			{
				A -= atoi(operand);
			}
			else if (!strcmp(addressingMode, "Indexed"))
			{
				A -= Memory[IX + atoi(operand)];
			}
		}
		else if (!strcmp(opcode, "SUBB"))//CCR VAR!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		{
			if (!strcmp(addressingMode, "Direct"))
			{
				B -= Memory[atoi(reference)];
			}
			else if (!strcmp(addressingMode, "Immediate"))
			{
				B -= atoi(operand);
			}
			else if (!strcmp(addressingMode, "Indexed"))
			{
				B -= Memory[IX + atoi(operand)];
			}
		}
		else if (!strcmp(opcode, "INCA"))//CCR VAR!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		{
			if (!strcmp(addressingMode, "Inherent"))
			{
				++A;
			}
		}
		else if (!strcmp(opcode, "INCB"))//CCR VAR!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		{
			if (!strcmp(addressingMode, "Inherent"))
			{
				++B;
			}
		}
		else if (!strcmp(opcode, "INC"))//CCR VAR!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		{
			if (!strcmp(addressingMode, "Direct"))
			{
				(Memory[atoi(reference)]) += 1;
			}
			else if (!strcmp(addressingMode, "Indexed"))
			{
				(Memory[IX + atoi(operand)]) += 1;
			}
		}
		else if (!strcmp(opcode, "INX"))
		{
			if (!strcmp(addressingMode, "Inherent"))
			{
				++IX;
			}
		}
		else if (!strcmp(opcode, "CMPA"))
		{
			if (!strcmp(addressingMode, "Direct"))
			{
				cmp = A - Memory[atoi(reference)];
			}
			else if (!strcmp(addressingMode, "Immediate"))
			{
				cmp = A - atoi(operand);
			}
			else if (!strcmp(addressingMode, "Indexed"))
			{
				cmp = A - Memory[IX + atoi(operand)];
			}
		}
		else if (!strcmp(opcode, "CMPB"))
		{
			if (!strcmp(addressingMode, "Direct"))
			{
				cmp = B - Memory[atoi(reference)];
			}
			else if (!strcmp(addressingMode, "Immediate"))
			{
				cmp = B - atoi(operand);

			}
			else if (!strcmp(addressingMode, "Indexed"))
			{
				cmp = B - Memory[IX + atoi(operand)];
			}
		}
		else if (!strcmp(opcode, "BRA"))
		{
			if (!strcmp(addressingMode, "Relative"))
			{
				for (i = 0; i < labelArraySize; ++i)
				{
					if (!strcmp(reference, sdt[i].label))
					{
						IP = sdt[i].textindex;
						break;
					}
				}
			}
		}
		else if (!strcmp(opcode, "BGT"))
		{
			if (!strcmp(addressingMode, "Relative"))
			{
				if (cmp > 0)
				{
					for (i = 0; i < labelArraySize; ++i)
					{
						if (reference == sdt[i].label)
							IP = sdt[i].textindex;
						break;
					}
				}
			}
		}
		else if (!strcmp(opcode, "BNE"))
		{
			if (!strcmp(addressingMode, "Relative"))
			{
				if (cmp != 0)
				{
					for (i = 0; i < labelArraySize; ++i)
					{
						if (!strcmp(reference, sdt[i].label))
						{
							IP = sdt[i].textindex;
							break;
						}
					}
				}
			}
		}
		++IP;
		printf("A:%d   B:%d\n", A, B);
	}
	return 0;
}
//Alkan Batu Ilgaz 
