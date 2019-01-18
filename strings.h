// small char* library developed on a previous laboratory

unsigned int strlen(char* string) {
	if (string == nullptr) {
		return 0;
	}
	unsigned int size = 0;
	while (string[size] != '\0') size++;
	return size;
}

unsigned int strcpy(char* dest, char* source, unsigned int mem) {
	unsigned int sourceLen = strlen(source);
	if (sourceLen >= mem) return 0;
	for (unsigned int i = 0; i < sourceLen; i++) {
		dest[i] = source[i];
	}
	dest[sourceLen] = '\0';
	return 1;
}

unsigned int strcat(char* dest, char* source, unsigned int mem) {
	unsigned int sourceLen = strlen(source);
	unsigned int destLen = strlen(dest);
	if (sourceLen + destLen >= mem) return 0;
	for (unsigned int i = 0; i < sourceLen; i++) 
		dest[i + destLen] = source[i];
	dest[destLen + sourceLen] = '\0';
	return 1;
}

int strsearch(char* string, char* substring) {
	unsigned int stringLen = strlen(string);
	unsigned int substringLen = strlen(substring);
	if (substringLen >= stringLen) return -1;

	for (unsigned int i = 0; i < stringLen - substringLen + 1; i++) {
		unsigned int searchLen = 0;
		while (searchLen < substringLen && string[i + searchLen] == substring[searchLen]) 
			searchLen++;
		if (searchLen == substringLen) return i;
	}
	return -1;
}

int chrsearch(char* string, char chr) {
	unsigned int stringLen = strlen(string);
	for (unsigned int i = 0; i < stringLen; i++) 
		if (string[i] == chr) return i;
	return -1;
}

int getword(char* dest, unsigned int mem, char* source, char* sep, unsigned int index) {
	unsigned int sepLen = strlen(sep);
	unsigned int sourceLen = strlen(source);
	unsigned int currentWord = 0;
	unsigned int currentIndex = 0;

	while (currentIndex < sourceLen && chrsearch(sep, source[currentIndex]) != -1)
		currentIndex++;
	while (currentWord < index) {
		while (currentIndex < sourceLen && chrsearch(sep, source[currentIndex]) == -1)
			currentIndex++;
		currentWord++;
		while (currentIndex < sourceLen && chrsearch(sep, source[currentIndex]) != -1)
			currentIndex++;

		if (currentIndex == sourceLen)
			return -1;
	}
	unsigned int wordEnd = currentIndex;
	while (wordEnd < sourceLen && chrsearch(sep, source[wordEnd]) == -1)
		wordEnd++;
	if (wordEnd - currentIndex >= mem) return 0;
	for (unsigned int i = 0; i < wordEnd - currentIndex; i++)
		dest[i] = source[i + currentIndex];
	dest[wordEnd - currentIndex] = '\0';
	return 1;
}

void strlwr_s(char* string) {
	unsigned int stringLen = strlen(string);
	for (int i = 0; i < stringLen; i++) {
		if (string[i] >= 'A' && string[i] <= 'Z')
			string[i] -= ('A' - 'a');
	}
}

void strupr_s(char* string) {
	unsigned int stringLen = strlen(string);
	for (int i = 0; i < stringLen; i++) {
		if (string[i] >= 'a' && string[i] <= 'z')
			string[i] += ('A' - 'a');
	}
}

int atoi(char* string) {
	int number = 0;
	unsigned int powerOfTen = 1;
	unsigned int stringLen = strlen(string);
	for (int i = stringLen - 1; i > 0; i--) {
		number += powerOfTen * (string[i] - '0');
		powerOfTen *= 10;
	}
	if (string[0] == '-') return number * -1;
	return number + powerOfTen * (string[0] - '0');
}


unsigned int serialRead(char * string,unsigned int mem){ 
  String obj = Serial.readString();
  if (obj.length()>=mem)
    return 0;
  strcpy(string,(char*)obj.c_str(),mem);
  return 1;
}
