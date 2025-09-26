#include "u2a.h"

typedef struct {
    uint32_t quot;
    uint8_t  rem;
} divmod10_t;

inline static divmod10_t divmodu10(uint32_t n)
{
    divmod10_t res;
// умножаем на 0.8
    res.quot = n >> 1;
    res.quot += res.quot >> 1;
    res.quot += res.quot >> 4;
    res.quot += res.quot >> 8;
    res.quot += res.quot >> 16;
    uint32_t qq = res.quot;
// делим на 8
    res.quot >>= 3;
// вычисляем остаток
    res.rem = (uint8_t)(n - ((res.quot << 1) + (qq & ~7ul)));
// корректируем остаток и частное
    if(res.rem > 9)
    {
        res.rem -= 10;
        res.quot++;
    }
    return res;
}

unsigned char * utoa(uint32_t value, unsigned char *buffer)
{
    buffer += 10;
    *buffer = 0;
    do
    {
        divmod10_t res = divmodu10(value);
        *--buffer = res.rem + '0';
        value = res.quot;
    }
    while (value != 0);
    return buffer;
}

unsigned char * itoa(int32_t value, unsigned char *buffer)
{
    uint_fast8_t isNegative = value < 0;
    if (isNegative) {
      value = -value;
    }

    buffer += 10;
    *buffer = 0;
    do {
        divmod10_t res = divmodu10(value);
        *--buffer = res.rem + '0';
        value = res.quot;
    }
    while (value != 0);

    if (isNegative)
      *--buffer = '-';
    return buffer;
}

unsigned char * utoaLTR(uint32_t value, unsigned char *buffer) {
    unsigned char buf[11];
    unsigned char *pos = utoa(value, buf);
    while(*pos)
        *(buffer++) = *pos++;
    return buffer;
}

unsigned char * itoaLTR(int32_t value, unsigned char *buffer) {
    unsigned char buf[11];
    unsigned char *pos = itoa(value, buf);
    while(*pos)
        *(buffer++) = *pos++;
    return buffer;
}

unsigned char utoaLTRCB(uint32_t value, void CB(char)) {
    unsigned char buf[11];
    unsigned char *pos = utoa(value, buf);
    unsigned char *p = pos;

    while(*pos)
        CB(*pos++);
    return pos - p;
}

unsigned char itoaLTRCB(int32_t value, void CB(char)) {
    unsigned char buf[11];
    unsigned char *pos = itoa(value, buf);
    unsigned char *p = pos;

    while(*pos)
        CB(*pos++);

    return pos - p;
}

//==================== Float

static inline uint32_t MantissaMul(uint32_t mantissa, uint32_t multiplier)
{
    mantissa <<= 8;
//    uint32_t t = 0;
//    do
//    {
// 	 	mantissa >>= 1;
//        if(multiplier & 0x80000000)
//        {
//            t += mantissa;
//        }
//        multiplier <<= 1;
//    }while(multiplier);
//    return t;
	return ((uint64_t)mantissa * multiplier) >> 32;
}

const uint32_t table2[] =
{
    0xF0BDC21A,
    0x3DA137D5,
    0x9DC5ADA8,
    0x2863C1F5,
    0x6765C793,
    0x1A784379,
    0x43C33C19,
    0xAD78EBC5,
    0x2C68AF0B,
    0x71AFD498,
    0x1D1A94A2,
    0x4A817C80,
    0xBEBC2000,
    0x30D40000,
    0x7D000000,
    0x20000000,
    0x51EB851E,
    0xD1B71758,
    0x35AFE535,
    0x89705F41,
    0x232F3302,
    0x5A126E1A,
    0xE69594BE,
    0x3B07929F,
    0x971DA050,
    0x26AF8533,
    0x63090312,
    0xFD87B5F2,
    0x40E75996,
    0xA6274BBD,
    0x2A890926,
    0x6CE3EE76
};

int ftoaEngine(float value, char *buffer, int presc)
{
	uint32_t uvalue = *(uint32_t*)&value;
	uint8_t exponent = (uint8_t)(uvalue >> 23);
	uint32_t fraction = (uvalue & 0x00ffffff) | 0x00800000;
	char *ptr = buffer;

	if(uvalue & 0x80000000)
        *ptr++ = '-';
    else
        *ptr++ = '+';

	if(exponent == 0) // don't care about a subnormals
	{
		ptr[0] = '0';
		ptr[1] = 0;
		return 0xff;
	}

	if(exponent == 0xff)
	{
	    if(fraction & 0x007fffff)
	    {
	    	ptr[0] = 'n';
	    	ptr[1] = 'a';
	    	ptr[2] = 'n';
			ptr[3] = 0;
	    }
	    else
		{
	    	ptr[0] = 'i';
	    	ptr[1] = 'n';
	    	ptr[2] = 'f';
			ptr[3] = 0;
	    }
		return 0xff;
	}

	*ptr++ = '0';

    int exp10 = ((((exponent>>3))*77+63)>>5) - 38;
    uint32_t t = MantissaMul(fraction, table2[exponent / 8]) + 1;
    uint_fast8_t shift = 7 - (exponent & 7);
    t >>= shift;

    uint8_t digit = t >> 24;
	digit >>= 4;
    while(digit == 0)
    {
        t &= 0x0fffffff;
		//t <<= 1;
		//t += t << 2;
		t *= 10;
        digit = (uint8_t)(t >> 28);
        exp10--;
    }

    for(uint_fast8_t i = presc+1; i > 0; i--)
    {
		digit = (uint8_t)(t >> 28);
        *ptr++ = digit + '0';
        t &= 0x0fffffff;
		t *= 10;
		//t <<= 1;
		//t += t << 2;
    }
    // roundup
	if(buffer[presc+2] >= '5')
        buffer[presc+1]++;
    ptr[-1] = 0;
    ptr-=2;
	for(uint_fast8_t i = presc + 1; i > 1; i--)
	{
		if(buffer[i] > '9')
		{
		    buffer[i]-=10;
			buffer[i-1]++;
		}
	}
	while(ptr[0] == '0')
	{
	    *ptr-- = 0;
	}
    return exp10;
}


char * ftoa(float value, char *result)
{
	uint8_t precision = 6;
	char *out_ptr = result;
    const int bufferSize = 10;
	char buffer[bufferSize+1];

	int exp10 = ftoaEngine(value, buffer, precision);
    if(exp10 == 0xff)
    {
		uint8_t digits = strlen(buffer);
		uint_fast8_t i = 0;
        if(buffer[0] == '+')
			i = 1;

		for(i = 0; i < digits; i++)
           	*out_ptr++ = buffer[i];
		*out_ptr = 0;
        return result;
    }

    if (buffer[0] == '-')
        *out_ptr++ = '-';

    char *str_begin = &buffer[2];
	if(buffer[1] != '0')
    {
        exp10++;
        str_begin--;
    }

    uint_fast8_t digits = strlen(str_begin);

	uint_fast8_t intDigits=0, leadingZeros = 0;
	if(abs(exp10) >= precision)
	{
		intDigits = 1;
	}else if(exp10 >= 0)
	{
        intDigits = exp10+1;
        exp10 = 0;
	}else
	{
		intDigits = 0;
		leadingZeros = -exp10 - 1;
		exp10 = 0;
	}
	uint_fast8_t fractDigits = digits > intDigits ? digits - intDigits : 0;
	//FieldFill(fractPartSize + intPartSize, IOS::right);
	if(intDigits)
	{
	    uint_fast8_t count = intDigits > digits ? digits : intDigits;
		while(count--)
			*out_ptr++ = *str_begin++;
        int_fast8_t tralingZeros = intDigits - digits;
        while(tralingZeros-- > 0)
            *out_ptr++ ='0';
	}
	else
		*out_ptr++ = '0';

	if(fractDigits)
	{
		*out_ptr++ = '.';
        while(leadingZeros--)
            *out_ptr++ = '0';
		while(fractDigits--)
			*out_ptr++ = *str_begin++;
	}
	if(exp10 != 0)
	{
		*out_ptr++ = 'e';
		uint_fast8_t upow10;
		if(exp10 < 0)
		{
			*out_ptr++ = '-';
			upow10 = -exp10;
		}
		else
		{
			*out_ptr++ = '+';
			upow10 = exp10;
		}
		char *powPtr = utoa(upow10, buffer);
		while(powPtr < buffer + bufferSize)
		{
			*out_ptr++ = *powPtr++;
		}
	}
	*out_ptr = 0;
	return result;
}

void ftoaCB(float value, void CB(char)) {
    char buf[15];
    char* a = ftoa(value, buf);
    while(*a) CB(*a++);
}

//==================================Parse

float parseFloat(char *ptr)
{
  unsigned char c;

  // Grab first character and increment pointer. No spaces assumed in line.
  c = *ptr++;

  // Capture initial positive/minus character
  uint8_t isnegative = 0;
  if (c == '-') {
    isnegative = 1;
    c = *ptr++;
  } else if (c == '+') {
    c = *ptr++;
  }

  // Extract number into fast integer. Track decimal in terms of exponent value.
  uint32_t intval = 0;
  int8_t exp = 0;
  uint8_t ndigit = 0;
  uint8_t isdecimal = 0;
  while(1) {
    c -= '0';
    if (c <= 9) {
      ndigit++;
      if (ndigit <= 9) {
        if (isdecimal) { exp--; }
        intval = (((intval << 2) + intval) << 1) + c; // intval*10 + c
      } else {
        if (!(isdecimal)) { exp++; }  // Drop overflow digits
      }
    } else if (c == (('.'-'0') & 0xff)  &&  !(isdecimal)) {
      isdecimal = 1;
    } else {
      break;
    }
    c = *ptr++;
  }

  // Return if no digits have been read.
  if (!ndigit)
      return 0;

  // Convert integer into floating point.
  float fval;
  fval = (float)intval;

  // Apply decimal. Should perform no more than two floating point multiplications for the
  // expected range of E0 to E-4.
  if (fval != 0) {
    while (exp <= -2) {
      fval *= 0.01;
      exp += 2;
    }
    if (exp < 0) {
      fval *= 0.1;
    } else if (exp > 0) {
      do {
        fval *= 10.0;
      } while (--exp > 0);
    }
  }

  // Assign floating point value with correct sign.
  return (isnegative) ? -fval : fval;
}
