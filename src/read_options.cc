#include "read_options.h"

#ifdef READ_OPTIONS_H_

#include <glog/logging.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

ReadOptions::ReadOptions(const char *fileName)
{
  // if(!google::IsGoogleLoggingInitialized())
    // google::InitGoogleLogging("./log");
  m_pFile = NULL;
  m_pFileName = NULL;
  m_pString = NULL;
  m_pOption = NULL;
  m_pVal = NULL;
  m_Length = 256;
  m_Line = 0;
  m_SepIndex = 0;
  m_bPermitted = false;
  
  FILE *pFile = fopen(fileName, "rb");
  m_pFile = (void*)pFile;
  m_pFileName = const_cast<char*>(fileName);
  if(NULL == m_pFile)
  {
    m_bPermitted = false;
    return;
  }
  else
    m_bPermitted = true;
  
  fclose(pFile);
  
  m_pString = new char[m_Length];
  m_pOption = new char[m_Length];
  m_pVal = new char[m_Length];
  if((NULL == m_pString || NULL == m_pOption) || (NULL == m_pVal))
  {
    LOG(ERROR) << "Error: no enough memory to allocate for params\n";
    m_bPermitted = false;
    return;
  }
  else
    m_bPermitted = true;
  
  memset(m_pString, 0, m_Length*sizeof(char));
  memset(m_pOption, 0, m_Length*sizeof(char));
  memset(m_pVal, 0, m_Length*sizeof(char));
  
}

ReadOptions::~ReadOptions(void)
{
  if(NULL != m_pString)
  {
    delete m_pString;
    m_pString = NULL;
  }
  if(NULL != m_pOption)
  {
    delete m_pOption;
    m_pOption = NULL;
  }
  if(NULL != m_pVal)
  {
    delete m_pVal;
    m_pVal = NULL;
  }
  // google::ShutdownGoogleLogging();
}

bool ReadOptions::readPermitted(void)
{
  if(!m_bPermitted)
    LOG(ERROR) << "Error: open file <" << m_pFileName << "> failed, permission denied or file not exsits\n";
  return m_bPermitted;
}

char *ReadOptions::read(const char *option)
{
  FILE *pFile = fopen(m_pFileName, "rb");
  if(NULL == m_pFile)
  {
    m_bPermitted = false;
    return m_pVal;
  }
  m_pFile = (void*)pFile;
  while(!feof(pFile))
  {
    getNext();
    if(!readOption(option))
      continue;
    readVal();
    break;
  }
  fclose(pFile);
  return m_pVal;
}

void ReadOptions::getNext(void)
{
  fgets(m_pString, m_Length-1, (FILE*)m_pFile);
}

bool ReadOptions::readOption(const char *option)
{
  for(int i=0;i<strlen(m_pString);++i)
  {
    // skip if space in the front so that get option
    if(m_pString[i] == ' ')
      continue;
    memset(m_pOption, 0, m_Length*sizeof(char));
    memcpy(m_pOption, &m_pString[i], strlen(option)*sizeof(char));
    // LOG(INFO) << "m_pOption-> " << m_pOption;
    if(strcmp(m_pOption, option))
    {
      // LOG(INFO) << "Not match";
      return false;
    }
    // skip if space in the front so that get value
    m_SepIndex = i+1+strlen(option);
    return true;
  }
  return false;
}

void ReadOptions::readVal(void)
{
  for(int i=m_SepIndex;i<strlen(m_pString);++i)
  {
    if(m_pString[i] == ' ' || m_pString[i] == '=')
      continue;
    memset(m_pVal, 0, m_Length*sizeof(char));
    memcpy(m_pVal, &m_pString[i], m_Length);
    char *p = strchr(m_pVal, '\n');
    if(p != NULL)
      *p = '\0';
    break;
  }
}

bool ReadOptions::matchSuffix(const char *fileName, const char *suffix)
{
  if(std::string(suffixOf(fileName)) == (std::string)(suffix))
    return true;
  return false;
}

char *ReadOptions::suffixOf(const char *val)
{
  int len = strlen(val);
  char *cast_val = const_cast<char*>(val);
  char *p = strchr(cast_val, '\n');
    if(p != NULL)
      *p = '\0';
  for(int i=len-1;i>=0;--i)
  {
    if(val[i] == '.')
    {
      p = &cast_val[i+1];
      break;
    }
  }
  return p;
}

#endif // READ_OPTIONS_H_