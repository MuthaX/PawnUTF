/*******************************************************************************
Name: Unicode-string stuff.

It does not need a special analogue:
	strdel(), memcpy(), strval(), valstr(), strcmp(), strmid()

It is incompatible with Unicode strings:
	strpack(), strunpack(), ispacked()


native strmid(dest[], const source[], start, end, maxlength=sizeof dest);
native bool: strins(string[], const substr[], pos, maxlength=sizeof string);
native strcmp(const string1[], const string2[], bool:ignorecase=false, length=cellmax);

TODO:
	tolower(), toupper(), format()
*******************************************************************************/
//	Returns length of Unicode-string.
stock Unicode_strlen(const unicode_string[]) {
	new symbol_pos = 0;
	while(unicode_string[symbol_pos]) {
		symbol_pos++;
	}
	return symbol_pos;
}
//	Concatenates Unicode source string to destination.
stock Unicode_strcat(
	unicode_string_destination[],
	const unicode_string_source[],
	const maxlength
) {
	new
		symbol_pos = Unicode_strlen(unicode_string_destination),
		source_pos = 0
	;
	while((symbol_pos < maxlength) && (unicode_string_source[source_pos])) {
		unicode_string_destination[symbol_pos++] = unicode_string_source[source_pos++];
	}
	unicode_string_destination[symbol_pos - 1] = EOS;
}
//	Inserts substring into a string.
stock Unicode_strins(
	unicode_string_destination[],
	const unicode_string_source[],
	const pos,
	const maxlength
) {
	new
		destination_len =	Unicode_strlen(unicode_string_destination),
		source_len,
		destination_pos, source_pos
	;
	if( pos <= destination_len ) {
		source_len = Unicode_strlen(unicode_string_source);
		// Step 1: move right origin to the end starting from last character.
		destination_pos = destination_len + source_len;
		if( destination_pos > maxlength ) {
			destination_pos = maxlength;
		}
		source_pos = destination_pos - source_len;
		while(source_pos > pos) {
			unicode_string_destination[--destination_pos] = unicode_string_destination[--source_pos];
		}
		// Step: 2: insert substring.
		destination_pos = pos;
		if( pos + source_len > maxlength ) {
			source_len = maxlength - pos;
		}
		for(source_pos = 0; source_pos < source_len; ++source_pos) {
			unicode_string_destination[destination_pos++] = unicode_string_source[source_pos];
		}
		//	Insert EOS.
		destination_pos = destination_len + source_len;
		if( destination_pos >= maxlength ) {
			destination_pos = maxlength;
		}
		unicode_string_destination[destination_pos] = EOS;
	}
}
//	Find substring inside string.
stock Unicode_strfind(
	const unicode_string[],
	const unicode_search_string[],
	const bool:ignorecase,
	const pos
) {
	if( unicode_string_source[0] < 0x01000000 && unicode_string[0] < 0x01000000 ) {
		return strfind(unicode_string, unicode_search_string, ignorecase, pos);
	}
	new
		symbol_pos = pos,
		unicode_symbol,
		source_pos
	;
	while(unicode_string[symbol_pos]) {
		unicode_symbol = unicode_string[symbol_pos];
		source_pos = 0;
		while(unicode_search_string[source_pos] && (unicode_search_string[source_pos] == unicode_symbol) ) {
			source_pos++;
		}
		if( !unicode_search_string[source_pos] ) {
			return symbol_pos;
		}
	}
	return -1;
}
