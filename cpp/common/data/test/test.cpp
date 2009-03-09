#include <iostream>

#include "data.hpp"
#include "vector.hpp"
#include "array.hpp"

using namespace std;

class CollectRecord: public Array<Datum*>::CollectBlock<string>
  {
  public:
    bool yield(string &res,Datum*&input)
    {
      res=input->toString();
      return true;
    };
  };

class Printer: public Data::EachBlock
  {
  public:
    bool preYield()
    {
      cout << Vector::toString(fieldNames) << endl;
      array.set(record);
      return true;
    }
    bool yield()
    {
      vector<string> *pStringRecord;
      pStringRecord = array.collect(collectRecord);
      cout << Vector::toString(*pStringRecord) << endl;
      delete pStringRecord;
      return true;
    }
    bool postYield()
    {
      cout << "Printer is finished" << endl;
      return true;
    }
  private:
    Array<Datum*> array;
    CollectRecord collectRecord;
  };

int main()
{
  string str("5.60437e-05");
//     string str("5.60437");
  FloatD fl;
  cout << fl.set(str) << endl;
  cout << fl.toString() << endl;
  return 0;

  Data data("test.tsv");
  cout << Vector::toString(data.fieldNames()) << endl;

  Printer printer;
  data.eachRecord(printer);

  vector<vector<int> > extractInt;
  vector<vector<double> > extractDouble;
  vector<int> fieldIXS;
  fieldIXS.push_back(1);
  fieldIXS.push_back(1);
  fieldIXS.push_back(0);
  fieldIXS.push_back(1);
  data.extract(extractInt,fieldIXS);
  data.extract(extractDouble,fieldIXS);
  cout << Vector::toString(extractDouble) << endl;

  vector<string> fieldNames;
  vector<FieldTypeT> fieldTypes;
  Vector::set<string>(fieldNames,"XX","YY","ZZ");
  Vector::set<FieldTypeT>(fieldTypes,eFieldTypeInteger,eFieldTypeInteger,eFieldTypeInteger);
  Data data2(fieldNames,fieldTypes);
  vector<int> record;
  Vector::set(record,1,2,3);
  data2.addRecord(record);
  data2.addRecord(record);
  data2.addRecord(record);
  data2.addRecord(record);
  cout << Vector::toString(data2.fieldNames()) << endl;
  data2.eachRecord(printer);

  vector<double> newField;
  Vector::set(newField,0.1,0.2,0.3,0.4);
  data2.replaceField(eFieldTypeFloat,newField,2);
  data2.eachRecord(printer);
  return 0;
}
