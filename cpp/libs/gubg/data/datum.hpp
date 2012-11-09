#ifndef HEADER_gubg_data_datum_hpp_ALREADY_INCLUDED
#define HEADER_gubg_data_datum_hpp_ALREADY_INCLUDED

#include <iostream>
#include <vector>
#include <math.h>
#include <sstream>
#include <stdlib.h>

using namespace std;

namespace gubg
{
    enum FieldTypeT
    {
        eFieldTypeEmpty,
        eFieldTypeString,
        eFieldTypeFloat,
        eFieldTypeInteger,
        eFieldTypeUnknown
    };
    class Datum
    {
    public:
        static bool canCreate(string str)
            {
                if (str.size()==0)
                    return false;
                else
                    return true;
            }
        virtual string toString(){return ".";};
        static string fieldTypeToString(FieldTypeT ft);
        static int size(FieldTypeT ft);

        virtual bool convert(int &t){return false;}
        virtual bool convert(long &t){return false;}
        virtual bool convert(double &t){return false;}
        virtual bool convert(string &t){t="";return true;}

        virtual bool set(int t){return false;}
        virtual bool set(long t){return false;}
        virtual bool set(double t){return false;}
        virtual bool set(string t){return false;}
        template<typename T>
        class ConvertTo
        {
        public:
            static bool isPossible(FieldTypeT ft){return false;}
        };

    private:
    };
    template<>
    class Datum::ConvertTo<int>
    {
    public:
        static bool isPossible(FieldTypeT ft){return ft==eFieldTypeInteger;}
    };
    template<>
    class Datum::ConvertTo<long>
    {
    public:
        static bool isPossible(FieldTypeT ft){return ft==eFieldTypeInteger;}
    };
    template<>
    class Datum::ConvertTo<string>
    {
    public:
        static bool isPossible(FieldTypeT ft)
            {
                switch (ft)
                {
                case eFieldTypeEmpty:
                case eFieldTypeString:
                case eFieldTypeInteger:
                case eFieldTypeFloat:
                    return true;
                default:
                    return false;
                };
            }
    };
    template<>
    class Datum::ConvertTo<double>
    {
    public:
        static bool isPossible(FieldTypeT ft)
            {
                switch (ft)
                {
                case eFieldTypeString:
                case eFieldTypeInteger:
                case eFieldTypeFloat:
                    return true;
                default:
                    return false;
                };
            }
    };

    class StringD: public Datum
    {
    public:
        StringD():
            mString(""){};
        StringD(string str):
            mString(str){};
        static bool canCreate(string str){return true;}
        string data(){return mString;};
        string toString(){return mString;};
        virtual bool convert(int &t){return false;}
        virtual bool convert(long &t){return false;}
        virtual bool convert(double &t){return false;}
        virtual bool convert(string &t){t=mString;return true;}

        virtual bool set(int t)
            {
                ostringstream ostr;
                ostr << t;
                mString = ostr.str();
                return true;
            }
        virtual bool set(long t)
            {
                ostringstream ostr;
                ostr << t;
                mString = ostr.str();
                return true;
            }
        virtual bool set(double t)
            {
                ostringstream ostr;
                ostr << t;
                mString = ostr.str();
                return true;
            }
        virtual bool set(string t){mString=t;return true;}
    private:
        string mString;
    };

    class FloatD: public Datum
    {
    public:
        FloatD():
            mValue(NAN){};
        FloatD(double value):
            mValue(value){};
        static bool canCreate(string str)
            {
                bool can = true;
                bool dot=false;
                int eIX = 0, sIX = 0;
                char ch;
                // Check for numeric characters
                int i;
                if (str.length() == 0)
                {
                    can =false;
                } else
                {
                    ch=str[0];
                    if (ch != '+' && ch != '-' && (ch < '0' || ch > '9')) can=false;
                    for (i=1;can && i<str.length();i++)
                    {
                        ch=str[i];
                        if (ch=='.')
                            if (dot)
                                can=false;
                            else
                                dot=true;
                        else if (ch == 'e')
                        {
                            if (eIX)
                                can = false;
                            else
                                eIX = i;
                        } else if (ch == '-' || ch == '+')
                        {
                            if (eIX == i-1)
                                sIX = i;
                            else
                                can = false;
                        } else if (ch < '0' || ch > '9')
                            can=false;
                    }
                }
                return can;
            }
        double data(){return mValue;};
        string toString()
            {
                ostringstream oss;
                oss << mValue;
                return oss.str();
            };
        virtual bool convert(int &t){return false;}
        virtual bool convert(long &t){return false;}
        virtual bool convert(double &t){t=mValue;return true;}
        virtual bool convert(string &t){ostringstream oss;oss << mValue;t=oss.str();return true;}

        virtual bool set(int t){return false;}
        virtual bool set(long t){return false;}
        virtual bool set(double t){mValue = t;return true;}
        virtual bool set(string t)
            {
                if (canCreate(t))
                {
                    mValue = atof(t.c_str());
                    return true;
                } else
                    return false;
            };
    private:
        double mValue;
    };

    class IntegerD: public Datum
    {
    public:
        IntegerD():
            mValue(-12345678){};
        IntegerD(long value):
            mValue(value){};
        static bool canCreate(string str)
            {
                bool can = true;
                char ch;
                // Check for numeric characters
                int i;
                if (str.length() == 0)
                {
                    can =false;
                } else
                {
                    ch=str[0];
                    if (ch!='+' && ch!='-' && (ch < '0' || ch > '9')) can=false;
                    for (i=1;can && i<str.length();i++)
                    {
                        ch=str[i];
                        if (ch < '0' || ch > '9') can=false;
                    }
                }
                return can;
            }
        long data(){return mValue;};
        string toString()
            {
                ostringstream oss;
                oss << mValue;
                return oss.str();
            };
        virtual bool convert(int &t){t=mValue;return true;}
        virtual bool convert(long &t){t=mValue;return true;}
        virtual bool convert(double &t){t=mValue;return true;}
        virtual bool convert(string &t){ostringstream oss;oss << mValue;t=oss.str();return true;}

        virtual bool set(int t){mValue = t;return true;}
        virtual bool set(long t){mValue = t;return true;}
        virtual bool set(double t){return false;}
        virtual bool set(string t)
            {
                if (canCreate(t))
                {
                    mValue = atol(t.c_str());
                    return true;
                } else
                    return false;
            };
    private:
        long mValue;
    };
}

#endif
