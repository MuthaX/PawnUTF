static cp1251_to_utf[256];
static cp1251_from_utf[0x10000 char];
static cp1251_from_utf_extra[18][2];
static cp1251_from_unicode[0x10000 char];
static cp1251_to_unicode[256];

//	Load collation table from file.
stock CP1251_Init(const filepath[]) {
	UTF_LoadChatsetMapping(filepath, cp1251_to_utf, cp1251_from_utf, cp1251_from_utf_extra, sizeof(cp1251_from_utf_extra), cp1251_from_unicode, cp1251_to_unicode);
}

// Convert UTF-stream to CP1251.
stock CP1251_FromUTF(const utf_stream[], const utf_stream_size, output[], const output_size, const is_packed) {
	PawnUTF_StreamUTF_ToCodePage(
		utf_stream, utf_stream_size,
		output, output_size,
		cp1251_from_utf,
		cp1251_from_utf_extra, sizeof(cp1251_from_utf_extra),
		is_packed
	);
}
//	Convert CP1251-stream to UTF-stream.
stock CP1251_StreamToUTF(const array[], const array_size, output[], const output_size, const is_packed) {
	PawnUTF_StreamToUTF(array, array_size, output, output_size, is_packed);
}
//	Convert CP1251-string to UTF-stream.
stock CP1251_StringToUTF(const array[], const array_size, output[], const output_size, const is_packed) {
	PawnUTF_StringToCollation(array, array_size, output, output_size, is_packed, cp1251_to_utf, sizeof(cp1251_to_utf));
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
	new
		File:f_input =	fopen(filepath_source, io_read),
		File:f_output =	fopen(filepath_dest, io_write),
		utf_source_symbol,
		truechar_as_unicode,
		true_unicode
	;
	while( (utf_source_symbol = PawnUTF_GetFileCharUTF(f_input)) != EOF ) {
		truechar_as_unicode = PawnUTF_DecodeUTF_ToUnicode(utf_source_symbol);
		true_unicode = CP1251_CharToUnicode(truechar_as_unicode);
		fputchar(f_output, true_unicode, true);
	}
	fclose(f_input);
	fclose(f_output);
}
