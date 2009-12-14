module gubg.UTF8;

void convertToUTF8(inout char[] str)
{
	foreach (inout ch; str)
	{
		switch (ch)
		{
		case 0xe0:
		case 0xe1:
		case 0xe2:
			ch = 'a';
			break;
		case 0xe8:
		case 0xe9:
		case 0xea:
		case 0xeb:
			ch = 'e';
			break;
		case 0xec:
		case 0xed:
		case 0xee:
		case 0xef:
			ch = 'i';
			break;
		case 0xf2:
		case 0xf3:
		case 0xf4:
			ch = 'o';
			break;
		case 0xfa:
		case 0xfb:
			ch = 'u';
			break;
		default:
			break;
		}
	}
}
