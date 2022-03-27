static cp1251_from_unicode[0x10000 char];
static cp1251_to_unicode[256];

//	Load collation table from file.
stock CP1251_Init(const filepath[]) {
	UTF_LoadCharsetMapping(filepath, cp1251_from_unicode, cp1251_to_unicode);
}
//	Convert UTF-string to CP1251-string.
stock CP1251_FromUTF(const utf_string[], const utf_string_size, output[], const output_size, const is_packed) {
	PawnUTF_StringUTF_ToCodePage(utf_string, utf_string_size, output, output_size, cp1251_from_unicode, is_packed);
}
//	Convert Unicode-string to CP1251-string.
stock CP1251_FromUnicode(const unicode_string[], const unicode_string_size, output[], const output_size) {
	PawnUTF_StringToCollationB(unicode_string, unicode_string_size, output, output_size, false, cp1251_from_unicode);
}
//	Convert CP1251-string to UTF-string.
stock CP1251_StringToUTF(const array[], const array_size, output[], const output_size, const is_packed) {
	CP1251_StringToUnicode(array, array_size, output, output_size, is_packed);
	PawnUTF_StringUnicode_ToUTF(output, output_size, output, output_size);
}
//	Convert CP1251-string to Unicode-stream.
stock CP1251_StringToUnicode(const array[], const array_size, output[], const output_size, const is_packed) {
	PawnUTF_StringToCollation(array, array_size, output, output_size, is_packed, cp1251_to_unicode, sizeof(cp1251_to_unicode));
}
//	Returns CP1251-code from Unicode-symbol.
stock CP1251_CharFromUnicode(const unicode_char) {
	return cp1251_from_unicode{unicode_char};
}
//	Returns Unicode-symbol code from CP1251-code symbol.
stock CP1251_CharToUnicode(const character) {
	return cp1251_to_unicode[character];
}
//	Fixes file
stock CP1251_FileFixUTF(const filepath_source[], const filepath_dest[]) {
	if( !strcmp(filepath_source, filepath_dest, false) ) {
		print("[CP1251_FileFixUTF] Impossible to write to the same file!");
		return;
	}
	CP1251_FileFixUTF_Handles(f_input, f_output);
	fclose(f_input);
	fclose(f_output);
}

stock CP1251_FileFixUTF_Handles(const File:f_input, const File:f_output) {
	new
		e_utf_bom_mark_type:bom_type,
		truechar_as_unicode,
		true_unicode,
		prev_char = 0,
		count = 0
	;
	bom_type = PawnUTF_TryGetBOM(f_input);
	if( bom_type != E_MARK_UNDEFINED ) {
		printf("Found BOM-type: %d", _:bom_type);
	}
	while( (truechar_as_unicode = fgetchar(f_input, 0, true)) != EOF ) {
		if( truechar_as_unicode > 0xFF ) {
			if( count ) {
				printf("Cant convert symbol: %x @ %d", truechar_as_unicode, count);
				truechar_as_unicode = '?';
			}
		}
		true_unicode = CP1251_CharToUnicode(truechar_as_unicode);
		if( true_unicode == '\n' && prev_char != '\r' ) {
			fputchar(f_output, '\r', true);
		}
		fputchar(f_output, true_unicode, true);
		count++;
		prev_char = true_unicode;
	}
}
