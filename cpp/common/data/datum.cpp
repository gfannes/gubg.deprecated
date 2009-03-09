#include "datum.hpp"

string Datum::fieldTypeToString(FieldTypeT ft)
{
  switch (ft)
    {
    case eFieldTypeEmpty:
      return "eFieldTypeEmpty";
    case eFieldTypeString:
      return "eFieldTypeString";
    case eFieldTypeFloat:
      return "eFieldTypeFloat";
    case eFieldTypeInteger:
      return "eFieldTypeInteger";
    default:
      return "Unknown fieldType";
    }
}

int Datum::size(FieldTypeT ft)
{
  switch (ft)
    {
    case eFieldTypeEmpty:
      return sizeof(Datum);
    case eFieldTypeString:
      return sizeof(StringD);
    case eFieldTypeFloat:
      return sizeof(FloatD);
    case eFieldTypeInteger:
      return sizeof(IntegerD);
    default:
      cerr << "Unknown field type " << ft << endl;
      return -1;
    }
}
