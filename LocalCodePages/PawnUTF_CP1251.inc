static cp1251_to_utf[256];
static cp1251_from_utf[0x10000 char];
static cp1251_from_utf_extra[18][2];

//	Load collation table from file.
stock CP1251_Init(const filepath[]) {
	UTF_LoadChatsetMapping(filepath, cp1251_to_utf, cp1251_from_utf, cp1251_from_utf_extra, sizeof(cp1251_from_utf_extra));
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
	PawnUTF_StreamToUTF(array, array_size, output[], output_size, is_packed);
}
//	Convert CP1251-string to UTF-stream.
stock CP1251_StringToUTF(const array[], const array_size, output[], const output_size, const is_packed) {
	PawnUTF_StringToUTF(string, array_size, output, output_size, is_packed, cp1251_to_utf);
}
