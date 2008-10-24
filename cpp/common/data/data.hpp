#ifndef data_h
#define data_h

#include <string>
#include <vector>
#include <typeinfo>
#include <fstream>

#include "datum.hpp"

using namespace std;

class DataSource
{
public:
    DataSource(string fn, char del):
        fileName(fn),
        delimiter(del){};
    string fileName;
    char delimiter;
};

class Data
{
public:
    typedef vector<Datum>* GenericFieldPtr;

    Data(string fileName,char delimiter = '\t');
    Data(vector<string> fieldNames, vector<FieldTypeT> fieldTypes);
    ~Data();

    string toString()
        {
            ostringstream ostr;
            ostr << "Number of fields = " << nrFields() << endl;
            ostr << "Number of records = " << nrRecords << endl;
            return ostr.str();
        }

    bool save(string fileName, char delimiter = '\t')
        {
            ofstream fo(fileName.c_str(),ios_base::out);
            if (!fo.is_open())
            {
                cerr << "Could not open file " << fileName << " for saving data set." << endl;
                return false;
            }

            // Save the fieldnames line
            int nrF = nrFields();
            for (int i=0;i<nrF;i++)
            {
                if (i>0)
                    fo << delimiter;
                fo << mFieldNames[i];
            }

            // Save the records
            for (int i=0;i<nrRecords;i++)
            {
                fo << endl;
                for (int j=0;j<nrF;j++)
                {
                    if (j>0)
                        fo << delimiter;
                    switch (mFieldTypes[j])
                    {
                    case eFieldTypeString:
                        fo << (*reinterpret_cast<vector<StringD>*>(mFields[j]))[i].toString();
                        break;
                    case eFieldTypeFloat:
                        fo << (*reinterpret_cast<vector<FloatD>*>(mFields[j]))[i].toString();
                        break;
                    case eFieldTypeInteger:
                        fo << (*reinterpret_cast<vector<IntegerD>*>(mFields[j]))[i].toString();
                        break;
                    default:
                        cerr << "Unknown field type " << mFieldTypes[j] << "" << endl;
                        fo.close();
                        return false;
                    }
                }
            }
            fo.close();
            return true;
        }

    template <typename T>
    bool addRecord(vector<T> &record)
        {
            // Check if the length of the record is ok
            long nrF = nrFields();
            bool ok = (record.size() == nrF);
            if (!ok)
                cerr << "The number of elements in the passed record does not correspond with the number of fields." << endl;

            // Check if we can convert T to all field types we have
            for (long i = 0; ok && i<nrF; i++)
            {
                // Not finished yet
            }
            
            // Add this record to the fields
            Datum emptyD;
            StringD stringD;
            FloatD floatD;
            IntegerD intD;
            for (long i = 0; ok && i<nrF; i++)
            {
                switch (mFieldTypes[i])
                {
                case eFieldTypeEmpty:
                    reinterpret_cast<vector<Datum>*>(mFields[i])->push_back(emptyD);
                    break;
                case eFieldTypeString:
                    stringD.set(record[i]);
                    reinterpret_cast<vector<StringD>*>(mFields[i])->push_back(stringD);
                    break;
                case eFieldTypeFloat:
                    floatD.set(record[i]);
                    reinterpret_cast<vector<FloatD>*>(mFields[i])->push_back(floatD);
                    break;
                case eFieldTypeInteger:
                    intD.set(record[i]);
                    reinterpret_cast<vector<IntegerD>*>(mFields[i])->push_back(intD);
                    break;
                default:
                    cerr << "Unknown field type " << mFieldTypes[i] << endl;
                    break;
                }
            }
            if (ok) nrRecords++;
            return ok;
        }

    template<typename T, typename IndexT>
    bool extractField(vector<T> &res, IndexT ix)
        {
            bool ok=true;
            long i,j,nrF=nrFields();
            int size;
            Datum* pCase;

            // Check the provided IX and prepare size
            if (ix<0 || ix>=nrF)
            {
                cerr << "Wanted field ix is out of range: " << ix << " and nrF = " << nrF << endl;
                ok=false;
            }
            if (!Datum::ConvertTo<T>::isPossible(mFieldTypes[ix]))
            {
                cerr << "I cannot convert field at ix " << ix << " from type " << Datum::fieldTypeToString(mFieldTypes[ix]) << " to " << typeid(T).name() << endl;
                ok=false;
            }
            if (ok)
            {
                pCase = &((*mFields[ix])[0]);
                size=Datum::size(mFieldTypes[ix]);
            }
            
            if (ok)
            {
                res.resize(nrRecords);
                for (j=0;ok && j<nrRecords;j++)
                {
                    ok &= pCase->convert(res[j]);
                    // Forward to the next one
                    pCase = (Datum*)((char*)pCase + size);
                }
            }
            return ok;
        };

    template<typename T, typename IndexT>
    bool replaceField(FieldTypeT newType, vector<T> &newField, IndexT ix)
        {
            bool ok=true;
            long nrF=nrFields();

            // Check the provided IX
            if (ix<0 || ix>=nrF)
            {
                cerr << "Wanted field ix is out of range: " << ix << " and nrF = " << nrF << endl;
                ok=false;
            }
            // Check the provided size
            if (newField.size() != nrRecords)
            {
                cerr << "You provided " << newField.size() << " new values, but the data set contains " << nrRecords << " records." << endl;
                ok = false;
            }

            vector<StringD> *pSF = NULL;
            vector<FloatD> *pFF = NULL;
            vector<IntegerD> *pIF = NULL;
            if (ok)
            {
                switch (newType)
                {
                case eFieldTypeString:
                    pSF = new vector<StringD>;
                    pSF->resize(nrRecords);
                    for (int i=0;ok && i<nrRecords;i++)
                        ok = (*pSF)[i].set(newField[i]);
                    if (ok)
                    {
                        freeField(ix);
                        mFields[ix] = reinterpret_cast<GenericFieldPtr>(pSF);
                        pSF = NULL;
                        mFieldTypes[ix] = newType;
                    }
                    break;
                case eFieldTypeFloat:
                    pFF = new vector<FloatD>;
                    pFF->resize(nrRecords);
                    for (int i=0;ok && i<nrRecords;i++)
                        ok = (*pFF)[i].set(newField[i]);
                    if (ok)
                    {
                        freeField(ix);
                        mFields[ix] = reinterpret_cast<GenericFieldPtr>(pFF);
                        pFF = NULL;
                        mFieldTypes[ix] = newType;
                    }
                    break;
                case eFieldTypeInteger:
                    pIF = new vector<IntegerD>;
                    pIF->resize(nrRecords);
                    for (int i=0;ok && i<nrRecords;i++)
                        ok = (*pIF)[i].set(newField[i]);
                    if (ok)
                    {
                        freeField(ix);
                        mFields[ix] = reinterpret_cast<GenericFieldPtr>(pIF);
                        pIF = NULL;
                        mFieldTypes[ix] = newType;
                    }
                    break;
                default:
                    cerr << "Unknown field type " << mFieldTypes[ix] << "" << endl;
                    ok = false;
                }
            }

            if (!ok)
                cerr << "Could not convert field " << ix << "" << endl;
            return ok;
        }

    template<typename T, typename IndexT>
    bool extract(vector<vector<T> > &res, vector<IndexT> &wantedFieldsIXS)
        {
            bool ok=true;
            long i,j,nrWantedFields=wantedFieldsIXS.size(),nrF=nrFields();
            IndexT ix;
            vector<int> sizes;
            vector<T> *pRecord;
            vector<Datum*> record;

            // Check the provided IXS and prepare sizes
            sizes.resize(nrWantedFields);
            record.resize(nrWantedFields);
            for (i=0;ok && i<nrWantedFields;i++)
            {
                ix=wantedFieldsIXS[i];
                if (ix<0 || ix>=nrF)
                {
                    cerr << "Wanted field ix[" << i << "] is out of range: " << ix << " and nrF = " << nrF << endl;
                    ok=false;
                }
//                 cout << "Convertion from " << Datum::fieldTypeToString(mFieldTypes[ix]) << " is " << Datum::ConvertTo<T>::isPossible(mFieldTypes[ix]) << endl;
                if (!Datum::ConvertTo<T>::isPossible(mFieldTypes[ix]))
                {
                    cerr << "I cannot convert field at ix " << ix << " from type " << Datum::fieldTypeToString(mFieldTypes[ix]) << " to " << typeid(T).name() << endl;
                    ok=false;
                }
                if (ok)
                {
                    record[i] = &((*mFields[ix])[0]);
                    switch (mFieldTypes[ix])
                    {
                    case eFieldTypeEmpty:
                        sizes[i] = sizeof(Datum);
                        break;
                    case eFieldTypeString:
                        sizes[i] = sizeof(StringD);
                        break;
                    case eFieldTypeFloat:
                        sizes[i] = sizeof(FloatD);
                        break;
                    case eFieldTypeInteger:
                        sizes[i] = sizeof(IntegerD);
                        break;
                    default:
                        cerr << "Unknown field type " << mFieldTypes[ix] << endl;
                        break;
                    }
                }
            }

            if (ok)
            {
                res.resize(nrRecords);
                for (j=0;ok && j<nrRecords;j++)
                {
                    pRecord = &res[j];
                    pRecord->resize(nrWantedFields);
                  
                    for (i=0;ok && i<nrWantedFields;i++)
                    {
                        ok &= record[i]->convert((*pRecord)[i]);
                        // Forward to the next one
                        record[i] = (Datum*)((char*)record[i] + sizes[i]);
                    }        
                }
            }
            return ok;
        };

    class EachBlock
    {
    public:
        bool preYield(){return true;};
        bool yield(){return true;};
        bool postYield(){return true;};

        vector<string> fieldNames;
        vector<FieldTypeT> fieldTypes;
        vector<Datum*> record;
        long index;
    };
    template<typename T>
    bool eachRecord(T &t)
        {
            bool ok=true;
            t.fieldNames = mFieldNames;
            t.fieldTypes = mFieldTypes;

            long nrF = nrFields();
            long i,j;

            // Initialize record to point to the first cases of the first record
            // and initialize sizes to contain the sizeof's for each field
            vector<int> sizes;
            vector<Datum*> &record = t.record;
            record.resize(nrF);
            sizes.resize(nrF);
            for (i=0;i<nrF;i++)
            {
                record[i] = &((*mFields[i])[0]);
                switch (mFieldTypes[i])
                {
                case eFieldTypeEmpty:
                    sizes[i] = sizeof(Datum);
                    break;
                case eFieldTypeString:
                    sizes[i] = sizeof(StringD);
                    break;
                case eFieldTypeFloat:
                    sizes[i] = sizeof(FloatD);
                    break;
                case eFieldTypeInteger:
                    sizes[i] = sizeof(IntegerD);
                    break;
                default:
                    cerr << "Unknown field type " << mFieldTypes[i] << endl;
                    break;
                }
            }

            // Pre
            t.index = j;
            ok &= t.preYield();

            // Iterate over the records
            for (j=0;ok && j<nrRecords;j++)
            {
                t.index = j;

                ok &= t.yield();

                // Forward record to the next one
                for (i=0;i<nrF;i++)
                    record[i] = (Datum*)((char*)record[i] + sizes[i]);
            }
            if (ok)
            {
                // Don't leave these pointers to something out of bound
                for (i=0;i<nrF;i++)
                    record[i] = NULL;
            }

            // Post
            t.index = nrRecords;
            ok &= t.postYield();
            return ok;
        }

    vector<string> &fieldNames(){return mFieldNames;};
    long nrFields(){return mFieldNames.size();};
private:
    bool loadAsStrings(vector<string> &fieldNames, vector<vector<string> > &fields);
    bool convertStringsToDatums(vector<vector<string> > &stringFields);
    static FieldTypeT determineFieldType(vector<string> &field);
    template<typename T>
    bool convertStringsField(vector<T> *&pField, vector<string> &stringField)
        {
            bool ok=true;
            if (pField = new vector<T>)
                pField->resize(stringField.size());
            else
            {
                ok=false;
                cerr << "Could not allocate the new field." << endl;
            }

            int i;
            for (i=0;ok && i<stringField.size();i++)
            {
                ok &= (*pField)[i].set(stringField[i]);
                if (!ok)
                    cerr << "Could not convert " << i << " (" << stringField[i] << ") to something." << endl;
            }
            return ok;
        }
    template <typename IndexT>
    bool freeField(IndexT ix)
        {
            bool ok;
            // Check the provided IX and prepare size
            if (ix<0 || ix>=nrFields())
            {
                cerr << "Wanted field ix is out of range: " << ix << " and nrFields = " << nrFields() << endl;
                ok=false;
            }
            switch (mFieldTypes[ix])
            {
            case eFieldTypeString:
                delete reinterpret_cast<vector<StringD>*>(mFields[ix]);
                mFields[ix] = NULL;
                break;
            case eFieldTypeFloat:
                delete reinterpret_cast<vector<FloatD>*>(mFields[ix]);
                mFields[ix] = NULL;
                break;
            case eFieldTypeInteger:
                delete reinterpret_cast<vector<IntegerD>*>(mFields[ix]);
                mFields[ix] = NULL;
                break;
            default:
                cerr << "Unknown fieldtype " << mFieldTypes[ix] << "" << endl;
            }
            mFieldTypes[ix] = eFieldTypeUnknown;
            return ok;
        }
    
    DataSource *mpDataSource;
    vector<string> mFieldNames;
    vector<FieldTypeT> mFieldTypes;
    vector<GenericFieldPtr> mFields;
    long nrRecords;
};
#endif
