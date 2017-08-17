#ifndef READ_OPTIONS_H_
#define READ_OPTIONS_H_

class ReadOptions
{
public:
	ReadOptions(const char *);
	~ReadOptions(void);
	bool readPermitted(void);
	char *read(const char *);
private:
	void getNext(void);
	bool readOption(const char *);
	void readVal(void);
	
	void *m_pFile;
	char *m_pFileName;
	char *m_pString;
	char *m_pOption;
	char *m_pVal;
	int m_Length;
	int m_Line;
	int m_SepIndex;
	bool m_bPermitted;
};

#endif // READ_OPTIONS_H_