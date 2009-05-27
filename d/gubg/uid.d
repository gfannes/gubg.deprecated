module gubg.uid;

// This will never return UID == 0, so that value can safely be used to indicate "invalid UID"
uint generateUID(char[] namespace = "")
{
    static uint[char[]] sUIDs;

    uint *pUID = (namespace in sUIDs);
    if (pUID == null)
    {
        sUIDs[namespace] = 0;
        pUID = (namespace in sUIDs);
    }
    ++(*pUID);
    
    if (0 == *pUID)
        throw new Exception("All UIDs for namespace " ~ namespace ~ " are consumed.");

    return *pUID;
}

version (UnitTest)
{
    import gubg.puts;

    void main()
    {
        puts("uid = {}", generateUID);
        puts("uid = {}", generateUID);
        puts("uid = {}", generateUID("test"));
        puts("uid = {}", generateUID("test"));
        puts("uid = {}", generateUID);
    }
}
