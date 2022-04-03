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

**UTF_LoadCharsetMapping(filepath[], table_unicode[0x10000 char], table_to_unicode[256])**
> Load codepage collation table.
> * filepath[] - path of file with collation table to load.
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

## UTF BOM-related procedures

**PawnUTF_IsUnicodeBOM(unicode_code)**
> Returns True if Unicode-symbol is BOM (Byte Order Mark).

**e_utf_bom_mark_type:PawnUTF_TryGetBOM(File:handle)**
> Trying to read UTF BOM-mark from start of the file. Places file pointer right after BOM-symbol or to the start of file if no known BOM-sequence found.
> Returns BOM-mark type.
>

**PawnUTF_IsTypeBOM_BigEndian(e_utf_bom_mark_type:bom_type)**
> Returns True if UTF BOM type is big-endian.
>

**PawnUTF_IsTypeBOM_LittleEndian(e_utf_bom_mark_type:bom_type)**
> Returns True if UTF BOM type is little-endian.
>

## UTF-16

**PawnUTF_FWriteUTF16BOM(File:handle, is_bigendian)**
> Writes UTF-16 BOM-symbol into the file.

**PawnUTF_GetFileCharUTF16_BE(File:handle)**
> Reads single UTF-16(big endian) symbol from file.
> Returns Unicode-symbol.

**PawnUTF_GetFileCharUTF16_LE(File:handle)**
> Reads single UTF-16(little endian) symbol from file.
> Returns Unicode-symbol.

**PawnUTF_EncodeUnicode_ToUTF16(unicode_code)**
> Return encoded Unicode-symbol to UTF-16.

**PawnUTF_PutFileCharUTF16_BE(File:handle, unicode_code)**
> Writes single Unicode-symbol encoded to big-endian UTF-16 into the file.

**PawnUTF_PutFileCharUTF16_LE(File:handle, unicode_code)**
> Writes single Unicode-symbol encoded to big-endian UTF-16 into the file.

**PawnUTF_FRead_UTF16(File:handle, unicode_string[], size, is_bigendian)**
> Reads Unicode-string decoded from UTF-16 file.
> Returns number of readen symbols.
>
**PawnUTF_FWrite_UTF16(File:handle, string[], is_bigendian)**
> Writes Unicode string which encoded to UTF-16 into the file.
> Returns number of writen symbols.
>

## UTF-32.
**PawnUTF_FWriteUTF32BOM(File:handle, is_bigendian)**
> Writes UTF-32 BOM-symbol into the file.
>

**PawnUTF_GetFileCharUTF32_BE(File:handle)**
> Reads single UTF-32(big endian) symbol from file.
> Returns Unicode-symbol.
>

**PawnUTF_GetFileCharUTF32_LE(File:handle)**
> Reads single UTF-32(little endian) symbol from file.
> Returns Unicode-symbol.
>

**PawnUTF_PutFileCharUTF32_BE(File:handle, unicode_code)**
> Writes single Unicode-symbol encoded to big-endian UTF-32 into the file.
>

**PawnUTF_PutFileCharUTF32_LE(File:handle, unicode_code)**
>	Writes single Unicode-symbol encoded to little-endian UTF-32 into the file.
>

**PawnUTF_FRead_UTF32(File:handle, unicode_string[], size, is_bigendian)**
> Reads Unicode-string decoded from UTF-32 file.
> Returns number of readen symbols.
>

**PawnUTF_FWrite_UTF32(File:handle, string[], is_bigendian)**
> Writes Unicode string which encoded to UTF-32 into the file.
> Returns number of writen symbols.

# Unicode string procedures
Here is represented string-procedures especially for Unicode strings. The reason to make it was that some native/original have complex logic based on aggregate packed strings.
Every analogue procedure have same arguments as their original native procedures.

It does not need a special analogue:
strdel(), memcpy(), strval(), valstr(), strcmp(), strmid()

It is incompatible with Unicode strings:
strpack(), strunpack(), ispacked()

**Unicode_strlen(unicode_string[])**
> Returns length of Unicode-string.

**Unicode_strcat(unicode_string_destination[], unicode_string_source[], maxlength)**
> Concatenates Unicode source string to destination.

**Unicode_strins(unicode_string_destination[], unicode_string_source[], pos, maxlength)**
> Inserts substring into a string.

**Unicode_strfind(unicode_string[], unicode_search_string[], ignorecase, pos)**
> Find substring inside string.
> Returns position of substring in string if found, -1 otherwise.
