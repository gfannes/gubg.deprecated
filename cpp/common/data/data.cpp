#include <iostream>
#include <fstream>
#include <sstream>

#include "data.hpp"

using namespace std;
using namespace gubg;

Data::Data(string fileName, char delimiter)
{
  mpDataSource = new DataSource(fileName,delimiter);
  vector<vector<string> > stringFields;
  bool ok=true;

  ok &= loadAsStrings(mFieldNames,stringFields);
  if (!ok)
    cerr << "Could not load the file " << fileName << " as strings.\n" << endl;

  ok &= convertStringsToDatums(stringFields);
  if (!ok)
    cerr << "Could not convert the file " << fileName << " to datum.\n" << endl;

  if (!ok)
    {
      delete mpDataSource;
      mpDataSource = NULL;
    }
};

Data::Data(vector<string> fieldNames, vector<FieldTypeT> fieldTypes):
    mpDataSource(NULL),
    nrRecords(0)
{
  mFieldNames = fieldNames;
  mFieldTypes = fieldTypes;
  if (mFieldNames.size() != mFieldTypes.size())
    cerr << "The number of fieldnames does not match the number of fieldtypes." << endl;
  mFields.resize(fieldNames.size(),NULL);
  for (long i = 0; i<fieldNames.size(); i++)
    {
      switch (mFieldTypes[i])
        {
        case eFieldTypeEmpty:
          mFields[i] = new vector<Datum>;
          break;
        case eFieldTypeString:
          mFields[i] = reinterpret_cast<vector<Datum>*>(new vector<StringD>);
          break;
        case eFieldTypeFloat:
          mFields[i] = reinterpret_cast<vector<Datum>*>(new vector<FloatD>);
          break;
        case eFieldTypeInteger:
          mFields[i] = reinterpret_cast<vector<Datum>*>(new vector<IntegerD>);
          break;
        default:
          cerr << "Unknown field type " << mFieldTypes[i] << endl;
          break;
        }
    }
}

Data::~Data()
{
  if (mpDataSource)
    delete mpDataSource;
}

bool Data::loadAsStrings(vector<string> &fieldNames, vector<vector<string> > &fields)
{
  if (!mpDataSource)
    {
      cerr << "No data source found.\n" << endl;
      return false;
    }

  cout << "Loading data as strings from \"" << mpDataSource->fileName << "\" ..." << endl;

  ifstream fi(mpDataSource->fileName.c_str());
  string line,token;
  int recNr,fieldIX,nrFields;
  char delimiter = mpDataSource->delimiter;
  if (!fi.is_open())
    {
      cerr << "Could not open file \"" << mpDataSource->fileName << "\"" << endl;
      return false;
    }

  if (!fi.eof())
    {
      getline(fi,line);
      istringstream iss(line);
      while ( getline(iss,token,delimiter) )
        fieldNames.push_back(token);
    }
  nrFields = fieldNames.size();
  fields.resize(nrFields);
  recNr = 0;
  while (!fi.eof())
    {
      getline(fi,line);
      istringstream iss(line);
      fieldIX=0;
      while ( getline(iss,token,delimiter) )
        fields[fieldIX++].push_back(token);
      if (fieldIX == nrFields)
        {
          recNr++;
        }
      else if (fieldIX == 0)
        {
          break;
        }
      else
        {
          cerr << "I expected " << nrFields << " fields but I read " << fieldIX << " for record " << recNr << endl;
          break;
        }
    }
  fi.close();
  cout << "Finished (" << nrFields << " fields and " << recNr << " records read)" << endl;
  return true;
};

bool Data::convertStringsToDatums(vector<vector<string> > &stringFields)
{
  bool ok = true;
  FieldTypeT fieldType;
  vector<StringD> *pStringField;
  vector<FloatD> *pFloatField;
  vector<IntegerD> *pIntegerField;

  mFields.resize(stringFields.size());
  mFieldTypes.resize(stringFields.size());

  int i;
  for (i=0;ok && i<stringFields.size();i++)
    {
      mFieldTypes[i] = fieldType = determineFieldType(stringFields[i]);
//         cout << "Field type for field " << i << " == " << Datum::fieldTypeToString(fieldType) << endl;
      switch (fieldType)
        {
        case eFieldTypeString:
          ok &= convertStringsField(pStringField,stringFields[i]);
          if (ok)
            mFields[i] = reinterpret_cast<GenericFieldPtr>(pStringField);
          else
            cerr << "Failed to convert string the StringD" << endl;
          break;
        case eFieldTypeFloat:
          ok &= convertStringsField(pFloatField,stringFields[i]);
          if (ok)
            mFields[i] = reinterpret_cast<GenericFieldPtr>(pFloatField);
          else
            cerr << "Failed to convert string the FloatD" << endl;
          break;
        case eFieldTypeInteger:
          ok &= convertStringsField(pIntegerField,stringFields[i]);
          if (ok)
            mFields[i] = reinterpret_cast<GenericFieldPtr>(pIntegerField);
          else
            cerr << "Failed to convert string the IntegerD" << endl;
          break;
        default:
          ok=false;
          cerr << "Unknown fieldType " << fieldType << endl;
          break;
        }
    }
  nrRecords = stringFields[0].size();
  return ok;
};

FieldTypeT Data::determineFieldType(vector<string> &field)
{
  bool emptyD = false;
  bool stringD = false;
  bool floatD = false;
  bool integerD = false;

  int i;
  vector<string>::iterator it;
  for (it=field.begin();it!=field.end();it++)
    {
      if (IntegerD::canCreate(*it))
        integerD = true;
      else if (FloatD::canCreate(*it))
        floatD = true;
      else if (Datum::canCreate(*it))
        emptyD = true;
      else
        stringD = true;
    }
  if (stringD)
    return eFieldTypeString;
  else if (floatD)
    return eFieldTypeFloat;
  else if (integerD)
    return eFieldTypeInteger;
  else
    return eFieldTypeEmpty;
};
