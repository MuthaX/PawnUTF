# PawnUTF
UTF to codepages and vise verse convertion stuff for Pawn. Supports only BMP (Basic Multilingual Plane).  

## Glossary
__N-String__ - arrays of symbols of type N with variable lenght in bytes (1 symbol - 1 cell) which can not be packed.
__N-Stream__ - arrays of symbols of type N with fixed lenght in bytes which also can be packed.
__CP-N__ - Code Page N.

## General procedures
__PawnUTF_StringToCollation(string[], string_size, output[], output_size, is_packed, collation_table[], collation_size)__
> Convert codepage-string to other representation via collation table.
> * string[] - source string
> * string_size - size of string.
> * output[] - result string of collation table on string.
> * output_size - size of output.
> * is_packed - flag if source string is packed.
> * collation_table[] - collation table where index - is source character and value in cell - collated symbol.
> * collation_size - size of collation_table.

**PawnUTF_StreamToUTF(array[], array_size, output[], output_size, is_packed)**
> Extract UTF-string characters from string-stream.
> * array[] - source CP-N string.
> * array_size - size of array.
> * output[] - destination for result UTF-string.
> * output_size - size of output.
> * is_packed
>

**stock PawnUTF_DecodeUTF_ToUnicode(utf_code)**
> Decode UTF-character to Unicode symbol.
> * utf_code - value representing UTF symbol.
>
**PawnUTF_EncodeUnicode_ToUTF(unicode_code)**
> Encode Unicode to UTF character.
> * unicode_code - value representing Unicode symbol.
>
**PawnUTF_StringUTF_ToCodePage(utf_stream[], utf_stream_size, output[], output_size, table_from_utf[0x10000 char], table_from_utf_extra[][2], extra_size, is_packed)**
> Converts UTF-string to codepage-string via collation tables.
> * utf_string[] - source UTF-string.
> * utf_string_size - size of utf_string.
> * output[] - result CP-N string.
> * output_size - size of output.
> * table_from_utf[0x10000 char] - main collation table (CP-N -> UTF).
> * table_from_utf_extra[][2] - collation table for large UTF-code values.
> * extra_size - size of table_from_utf_extra.
> * is_packed - true if output have to be packed.
>
**PawnUTF_StringUTF_ToUnicode(utf_stream[], utf_stream_size, output[], output_size)**
> Convert UTF-string to Unicode-string.
> * utf_string[] - source UTF-string.
> * utf_string_size - size of utf_string.
> * output[] - destination for Unicode-string.
> * output_size - size of output.
>
**PawnUTF_StringUnicode_ToUTF(unicode_stream[], unicode_stream_size, output[], output_size)**
> Convert Unicode-string to UTF-string.
> * unicode_string[] - source Unicode-string.
> * unicode_string_size - size of unicode_stream.
> * output[] - destination for UTF-string.
> * output_size - size of output.


**PawnUTF_WriteCurrentCP(filepath_input[], filepath_output[])**
> Writes current CP-UTF collation (which used in fwrite).
> * filepath_input[]
> * filepath_output[]

**PawnUTF_GetFileCharUTF(File:handle)**
> Reads one UTF-symbol from opened file.
> * File:handle
>

**UTF_LoadChatsetMapping(filepath[], table_to_utf[256], table_from_utf[0x10000 char], table_from_utf_extra[][2], extra_size, table_unicode[0x10000 char], table_to_unicode[256])**
> Load codepage collation table.
> * filepath[] - path of file with collation table to load.
> * table_to_utf[256] - destination collation table[CP-N code] = utf_code.
> * table_from_utf[0x10000 char] - destination collation table[utf_code] = CP-N code.
> * table_from_utf_extra[][2] - destination collation table[i][2] = {utf_code, CP-N code}.
> * extra_size - size of table_From_utf_extra.
> * table_unicode[0x10000 char] - destination collation table[unicode_symbol] = CP-N code.
> * table_to_unicode[256] - destination collation table[CP-N] = unicode_symbol.
>


## CP1251 example procedures
**CP1251_Init(filepath[])**
> Load collation table from file.
> * filepath - path of the file to load collation table (CP-N -> UTF-8).
>

**CP1251_FromUTF(utf_stream[], utf_stream_size, output[], output_size, is_packed)**
> Convert UTF-string to CP1251-string.
> * utf_string[] - source UTF-8 string.
> * utf_string_size - size of source UTF-8 string.
> * output[] - destination of CP1251-string.
> * output_size - size of destination output.
> * is_packed - true if output have to be packed.
>

**CP1251_FromUnicode(unicode_string[], unicode_string_size, output[], output_size)**
> Convert Unicode-string to CP1251-string.
> * unicode_string[] - source Unicode-string.
> * unicode_string_size - size of unicode_string.
> * output[] - destination of result CP1251-string.
> * output_size - size of output.


**CP1251_StringToUTF(array[], array_size, output[], output_size, is_packed)**
> Convert CP1251-string to UTF-stream.
> * array[] - source CP1251-string.
> * array_size - size of source string.
> * output[] - destination of UTF result stream.
> * output_size - size of output.
> * is_packed - true if array is packed.
>

**CP1251_StringToUnicode(array[], array_size, output[], output_size, is_packed)**
> Convert CP1251-string to Unicode-string.
> * array[] - source CP1251-string.
> * array_size - size of source string.
> * output[] - destination of Unicode result string.
> * output_size - size of output.
> * is_packed - true if array is packed.
> 

**CP1251_CharFromUnicode(unicode_char)**  
> Returns CP1251-code from Unicode-symbol.
> * unicode_char - Unicode symbol.  
> 

**CP1251_CharToUnicode(character)**  
> Returns Unicode-symbol code from CP1251-code symbol.  
> * character - CP1251 code.
> 

**CP1251_FileFixUTF(filepath_source[], filepath_dest[])**  
> Fixes classic encoding problem in CP1251-based file.  
> * filepath_source[] - source file.
> * filepath_dest[] - destination file.
> 
