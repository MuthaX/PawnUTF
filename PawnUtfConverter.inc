//	Test general collation size (cells).
#if defined UNICODE_GENCOLLATION_SIZE
	#if (UNICODE_GENCOLLATION_SIZE - 0) == 0
		#error You forgot to set value of UNICODE_GENCOLLATION_SIZE. Type something like: #define UNICODE_GENCOLLATION_SIZE (64).
	#endif
	#if (UNICODE_GENCOLLATION_SIZE < 1)
		#error You are trying to allocate invalid number of unicode general collation table size. Set value of UNICODE_GENCOLLATION_SIZE greather than 0.
	#endif
#else
	#define UNICODE_GENCOLLATION_SIZE	(64)
#endif
//	Test max substitution size (bytes).
#if defined UNICODE_GENCOLLATION_ANALOGSIZE
	#if (UNICODE_GENCOLLATION_ANALOGSIZE - 0) == 0
		#error You forgot to set value of UNICODE_GENCOLLATION_ANALOGSIZE. Type something like: #define UNICODE_GENCOLLATION_ANALOGSIZE (5).
	#endif
	#if (UNICODE_GENCOLLATION_ANALOGSIZE < 1)
		#error You are trying to allocate invalid analog size. Set value of UNICODE_GENCOLLATION_ANALOGSIZE greather than 0.
	#endif
#else
	#define UNICODE_GENCOLLATION_ANALOGSIZE	(5)
#endif

enum e_utf_bom_mark_type {
	E_MARK_UNDEFINED,	// No BOM or unknown sequence detected.
	E_MARK_UTF8,		// UTF-8
	E_MARK_UTF16BE,		// UTF-16 Big Endian
	E_MARK_UTF16LE,		// UTF-16 Little Endian
	E_MARK_UTF32BE,		// UTF-32 Big Endian
	E_MARK_UTF32LE		// UTF-32 Little Endian
};
static stock invalidSymbol_utfcode = '?';
static stock invalidSymbol_unicode = '?';
static stock gencollation_unicode_min = cellmax;	// non include r.
static stock gencollation_unicode_max = 0;			// non include.
static stock gencollation_unicode_number = 0;
static stock gencollation_unicode_symbol[UNICODE_GENCOLLATION_SIZE]; // Unicode code.
static stock gencollation_unicode_length[UNICODE_GENCOLLATION_SIZE];
static stock gencollation_unicode_string[UNICODE_GENCOLLATION_SIZE][UNICODE_GENCOLLATION_ANALOGSIZE];

static stock internal_string_get_symbol(const array[], const pos, const bool:is_byte) {
	if( is_byte ) {
		return array{pos};
	} else {
		return array[pos];
	}
}
static stock internal_string_set_symbol(string[], const pos, const bool:is_byte, const value) {
	if( is_byte ) {
		string{pos} = value;
	} else {
		string[pos] = value;
	}
}
static stock internal_string_get_size(const bool:is_packed, const string_size) {
	return (1 + 3 * _:(is_packed != false)) * string_size;
}
static stock internal_string_lastPos(const pos) {
	return (pos + (cellbits / charbits) - 1) / (cellbits / charbits);
}
static stock internal_putCollationItem(output[], &output_index, const output_size, const collation_item[UNICODE_GENCOLLATION_ANALOGSIZE], const collation_item_size, const collation_table[], const bool:is_packed) {
	new alter_idx;
	for(alter_idx = 0; alter_idx < collation_item_size; ++alter_idx) {
		if( output_index < output_size ) {
			internal_string_set_symbol(output, output_index++, is_packed, collation_table{ collation_item[alter_idx] });
		} else {
			break;
		}
	}
}
static stock internal_tryPutGenCollationSeq(output[], &output_index, const output_size, const unicode_code, const bool:is_packed, const collation_table[]) {
	new alter_slotid, alter_len;
	if( (alter_slotid = gencollation_findSymbol(unicode_code)) != -1 ) {
		alter_len = gencollation_unicode_length[alter_slotid];
		internal_putCollationItem(output, output_index, output_size, gencollation_unicode_string[alter_slotid], alter_len, collation_table, is_packed);
	}
}

static stock gencollation_findFree() {
	if( gencollation_unicode_number < UNICODE_GENCOLLATION_SIZE ) {
		for(new slotid = 0; slotid < UNICODE_GENCOLLATION_SIZE; ++slotid) {
			if( !gencollation_unicode_symbol[slotid] ) {
				return slotid;
			}
		}
	}
	return -1;
}
stock gencollation_findSymbol(const unicode_code) {
	if(	unicode_code < gencollation_unicode_max &&
		unicode_code > gencollation_unicode_min
	) {
		for(new slotid = 0, number = gencollation_unicode_number; slotid < number; ++slotid) {
			if( gencollation_unicode_symbol[slotid] == unicode_code ) {
				return slotid;
			}
		}
	}
	return -1;
}
stock PawnUTF_gencollation_addSymbol(const unicode_code, const substitutuion[]) {
	new slotid = gencollation_findFree(),
		length = 0,
		idx
	;
	while((length < UNICODE_GENCOLLATION_ANALOGSIZE) && substitutuion[length]) {
		length++;
	}
	if( length > UNICODE_GENCOLLATION_ANALOGSIZE ) {
		length = UNICODE_GENCOLLATION_ANALOGSIZE;
	}
	if( slotid != -1 ) {
		gencollation_unicode_symbol[slotid] = unicode_code;
		gencollation_unicode_length[slotid] = length;
		gencollation_unicode_number++;
		if( gencollation_unicode_min >= unicode_code ) {
			gencollation_unicode_min = unicode_code - 1;
		}
		if( gencollation_unicode_max <= unicode_code ) {
			gencollation_unicode_max = unicode_code + 1;
		}
		for(idx = 0; idx < length; ++idx) {
			gencollation_unicode_string[slotid][idx] = substitutuion[idx];
		}
	}
}
//	Loaded file have to be UTF8 encoded.
stock PawnUTF_GenCollation_loadFile(const filepath[]) {
	new File:f = fopen(filepath, io_read),
		string[1 + 1 + UNICODE_GENCOLLATION_ANALOGSIZE + 1 + 1 + 32],
		i, value, string_len,
		unicode_code, unicode_sequence[UNICODE_GENCOLLATION_ANALOGSIZE],
		alter_slotid
	;
	if( f ) {
		while( (string_len = fread(f, string)) ) {
			// Since sscanf2 cant parse UTF/Unicode strings normally we have to do this **** by ourself.
			if( string[1] == '\t' && (string_len > 3) ) {
				i = 0;
				do {
					value = string[i];
					if( value == '\r' || value == '\n' ) {
						string[i] = 0;
						break;
					}
				} while(value && (i++ < sizeof(string)) );
				unicode_code = string[0];
				alter_slotid = gencollation_findSymbol(unicode_code);
				if( alter_slotid == -1 ) {
					alter_slotid = gencollation_findFree();
					if( alter_slotid != -1 ) {
						for(i = 0; i < UNICODE_GENCOLLATION_ANALOGSIZE; ++i) {
							unicode_sequence[i] = string[i + 2];
						}
						PawnUTF_gencollation_addSymbol(unicode_code, unicode_sequence);
					} else {
						printf("[PawnUTF_GenCollation_loadFile] Too many general collations loaded already(reading unicode_code=%x)!", unicode_code);
					}
				} else {
					printf("[PawnUTF_GenCollation_loadFile] Unicode symbol=%x already loaded!", unicode_code);
				}
			}
		}
		fclose(f);
	}
}

//	Returns true if UTF-8 code symbol is valid.
stock PawnUTF_IsValidCodeUTF8(const utf_code) {
	return (
		(0x00000000 == (utf_code & 0xFFFFFF80)) ||	// 1 bytes (since there is no unsigned values the (utf_code < 128) is incorrect in case of 4B code).
		(0x0000C080 == (utf_code & 0xFFFFE0C0)) ||	// 2 bytes.
		(0x00E08080 == (utf_code & 0xFFF0C0C0)) ||	// 3 bytes.
		(0xF0808080 == (utf_code & 0xF8C0C0C0))		// 4 bytes.
	);
}
//	Returns True if Unicode-symbol is BOM.
stock PawnUTF_IsUnicodeBOM(const unicode_code) {
	return (unicode_code == 0xFEFF);
}
//	Returns True if UTF BOM type is big-endian.
stock PawnUTF_IsTypeBOM_BigEndian(const e_utf_bom_mark_type:bom_type) {
	return (bom_type == e_utf_bom_mark_type:E_MARK_UTF16BE || bom_type == e_utf_bom_mark_type:E_MARK_UTF32BE);
}
//	Returns True if UTF BOM type is little-endian.
stock PawnUTF_IsTypeBOM_LittleEndian(const e_utf_bom_mark_type:bom_type) {
	return (bom_type == e_utf_bom_mark_type:E_MARK_UTF16LE || bom_type == e_utf_bom_mark_type:E_MARK_UTF32LE);
}
//	Trying to read UTF BOM-symbol from start of the file.
//	Places file pointer right after BOM-symbol or to the start of file if no known BOM-sequence found.
//	Returns BOM-mark type.
stock e_utf_bom_mark_type:PawnUTF_TryGetBOM(const File:handle) {
	fseek(handle, 0, seek_start);
	new
		byte_0 = fgetchar(handle, 0, false),
		byte_1 = fgetchar(handle, 0, false),
		byte_2 = fgetchar(handle, 0, false),
		byte_3 = fgetchar(handle, 0, false)
	;
	if( byte_0 == 0xEF ) {
		if( byte_1 == 0xBB && byte_2 == 0xBF ) {
			fseek(handle, 3, seek_start);
			return e_utf_bom_mark_type:E_MARK_UTF8;
		}
	} else if( byte_0 == 0xFE && byte_1 == 0xFF ) {
		fseek(handle, 2, seek_start);
		return e_utf_bom_mark_type:E_MARK_UTF16LE;
	} else if( byte_0 == 0xFF && byte_1 == 0xFE ) {
		if( !byte_2 && !byte_3 ) {
			return e_utf_bom_mark_type:E_MARK_UTF32BE;
		} else {
			fseek(handle, 2, seek_start);
			return e_utf_bom_mark_type:E_MARK_UTF16BE;
		}
	} else if( !byte_0 && !byte_1 && byte_2 == 0xFE && byte_3 == 0xFF) {
		return e_utf_bom_mark_type:E_MARK_UTF32LE;
	}
	fseek(handle, 0, seek_start);
	return e_utf_bom_mark_type:E_MARK_UNDEFINED;
}
//	Convert codepage-string to other representation via collation table.
stock PawnUTF_StringToCollation(const string[], const string_size, output[], const output_size, const bool:is_packed, const collation_table[], const collation_size) {
	new
		cp_character,
		source_index = 0, output_index = 0,
		max_i = internal_string_get_size(is_packed, string_size)
	;
	do {
		cp_character = internal_string_get_symbol(string, source_index++, is_packed);
		if( cp_character < collation_size ) {
			output[output_index++] = collation_table[cp_character];
		}
	} while( cp_character && (source_index < max_i) && (output_index < output_size) );
	output[output_size - 1] = 0;
}
//	Convert codepage-string to other representation via collation table with 1-byte cell.
stock PawnUTF_StringToCollationB(const string[], const string_size, output[], const output_size, const bool:is_packed, const collation_table[]) {
	new
		cp_character,
		collation_symbol,
		source_index = 0, output_index = 0,
		max_i = internal_string_get_size(is_packed, string_size)
	;
	do {
		cp_character = internal_string_get_symbol(string, source_index++, is_packed);
		if( cp_character < 0x10000 ) {
			collation_symbol = collation_table{cp_character};
			if( collation_symbol || (collation_symbol < 128) ) {
				output[output_index++] = collation_symbol;
			} else {
				internal_tryPutGenCollationSeq(output, output_index, output_size, cp_character, false, collation_table);
			}
		} else {
			internal_tryPutGenCollationSeq(output, output_index, output_size, cp_character, false, collation_table);
		}
	} while( cp_character && (source_index < max_i) && (output_index < output_size) );
	output[output_size - 1] = 0;
}
//	Unpack UTF-string to string-stream.
//		If not packed destination then you better use 4x output size (than input).
stock PawnUTF_UTFToStream(const utf_string[], const utf_string_size, output[], const output_size, const bool:is_packed) {
	new utf_code,
		utf_pos = 0,
		byte_index,
		byte_value,
		output_index,
		last_pos,
		output_first_pos_of_cur_symbol,
		max_output = internal_string_get_size(is_packed, output_size - 1)
	;
	if( output_size < 1 ) {
		return;
	}
	if( is_packed ) {
		max_output -= ((cellbits / charbits) - 1);
		while( (utf_pos < utf_string_size) && (utf_code = utf_string[utf_pos]) ) {
			output_first_pos_of_cur_symbol = output_index;
			for(byte_index = 3; byte_index > -1; --byte_index) {
				byte_value = (utf_code >> (8 * byte_index)) & 0xFF;
				if( byte_value ) {
					output{output_index++} = byte_value;
					if( output_index > max_output ) {
						//	Clear all cells related to partial symbol.
						while(output_first_pos_of_cur_symbol < max_output) {
							output{output_first_pos_of_cur_symbol++} = 0;
						}
						goto procedure_end;
					}
				}
			}
			utf_pos++;
		}
		const cellsize = cellbits / charbits;
		last_pos = ((output_index - 1 + cellsize) / cellsize) * cellsize;
		while(output_index < last_pos) {
			output{output_index++} = 0;
		}
	} else {
		while( (utf_pos < utf_string_size) && (utf_code = utf_string[utf_pos]) ) {
			output_first_pos_of_cur_symbol = output_index;
			for(byte_index = 3; byte_index > -1; --byte_index) {
				byte_value = (utf_code >> (8 * byte_index)) & 0xFF;
				if( byte_value ) {
					output[output_index++] = byte_value;
					if( output_index > max_output ) {
						//	Clear all cells related to partial symbol.
						while(output_first_pos_of_cur_symbol < max_output) {
							output[output_first_pos_of_cur_symbol++] = 0;
						}
						goto procedure_end;
					}
				}
			}
			utf_pos++;
		}
	}
	procedure_end: {
		output[output_size - 1] = 0;
	}
}
//	Extract UTF-string characters from string-stream.
stock PawnUTF_StreamToUTF(const array[], const array_size, output[], const output_size, const bool:is_packed, const bool:allow_invalid = true) {
	new
		max_bytes = internal_string_get_size(is_packed, array_size),
		output_index = 0,
		current_byte,
		accamulated_size = 0,
		accamulated_value = 0,
		current_value
	;
	for(current_byte = 0; current_byte < max_bytes; ++current_byte) {
		if( output_index >= output_size ) {
			output[output_size - 1] = 0;	// Enforce string to end.
			break;
		}
		current_value = internal_string_get_symbol(array, current_byte, is_packed);
		if( current_value & 0x80 ) {	// Ending or continuous char byte.
			if( current_value & 0x40 ) {	// Ending char.
				if( accamulated_value ) {
					if( output_index < output_size ) {
						if( PawnUTF_IsValidCodeUTF8(accamulated_value) ) {
							output[output_index++] = accamulated_value;
						} else if( allow_invalid ) {
							output[output_index++] = invalidSymbol_utfcode;
						}
					}
					accamulated_size = accamulated_value = 0;
				}

				for(new i = 5; i > 2; --i) {
					if( !((current_value >> i) & 1) ) {	// The first zero bit found.
						accamulated_size = 7 - i;
						break;
					}
				}
				if( accamulated_size > 4 ) {	// invalid size. UTF-8 cant have code with 5-6 bytes length.
					if( output_index < output_size && allow_invalid ) {
						output[output_index++] = invalidSymbol_utfcode;
						accamulated_size = accamulated_value = 0;
						continue;
					}
				}
			} else {	// Continuous byte.
				if( !accamulated_value ) {	// no start bytes found before
					if( output_index < output_size && allow_invalid ) {
						output[output_index++] = invalidSymbol_utfcode;
						accamulated_size = accamulated_value = 0;
						continue;
					}
				}
			}
			accamulated_value |= current_value << (8 * --accamulated_size);
			if( accamulated_size == 0 ) {
				if( output_index < output_size ) {
					output[output_index++] = accamulated_value;
					accamulated_value = 0;
				}
			}
		} else {	// Single byte (char).
			if( accamulated_value ) {
				if( output_index < output_size ) {
					if( PawnUTF_IsValidCodeUTF8(accamulated_value) ) {
						output[output_index++] = accamulated_value;
					} else if( allow_invalid ) {
						output[output_index++] = invalidSymbol_utfcode;
					}
				}
				accamulated_size = accamulated_value = 0;
			}
			if( !current_value ) {
				output[output_index] = 0;
				break;
			}
			if( output_index < output_size ) {
				output[output_index++] = current_value;
			}
		}
	}
	output[output_size - 1] = 0;	// Enforce string to end.
}
//==============================================================================
//	Decode UTF-character to Unicode symbol.
stock PawnUTF_DecodeUTF_ToUnicode(const utf_code) {
	new result, clean_utf;
	if( utf_code & 0x80 ) {
		clean_utf = utf_code & (((~(utf_code & 0xF8C0C0C0)) & 0xFEFEFEFE) >> 1);
		result =
			((clean_utf & 0x0000003F)) |
			((clean_utf & 0x00003F00) >> 2) |
			((clean_utf & 0x003F0000) >> 4) |
			((clean_utf & 0xFF000000) >> 6)
		;
	} else {
		result = utf_code;
	}
	return result;
}
//	Encode Unicode to UTF character.
stock PawnUTF_EncodeUnicode_ToUTF(const unicode_code) {
	new n, i, result;
	if( unicode_code < 0x80 ) {
		return unicode_code;
	}
	n =								// 0: 00000000-0000007F
		(unicode_code > 0x00007F) +	// 1: 00000080-000007FF
		(unicode_code > 0x0007FF) +	// 2: 00000800-0000FFFF
		(unicode_code > 0x00FFFF)	// 3: 00010000-0010FFFF
	;
	for(i = 0; i < n; ++i) {
		result |= ((0x80 | ((unicode_code >> (6 * i)) & 0x3F)) << (8 * i));
	}
	i =		(unicode_code >> (6 * n));				// last value portion.
	i |=	((1 << (n + 1)) - 1) << (8 - 1 - n);	// size appendix.
	result |= ( i << (8 * n) );
	return result;
}
//	Converts UTF-string to codepage-string via collation tables.
stock PawnUTF_StringUTF_ToCodePage(
	const utf_string[], const utf_string_size,
	output[], const output_size,
	const table_from_unicode[0x10000 char], const bool:is_packed
) {
	new
		source_index = 0, output_index = 0,
		byte_value,
		unicode_code,
		utf_code,
		max_output = internal_string_get_size(is_packed, output_size)
	;
	do {
		utf_code = utf_string[source_index++];
		unicode_code = PawnUTF_DecodeUTF_ToUnicode(utf_code);
		if( unicode_code < 0x10000 ) {
			byte_value = table_from_unicode{unicode_code};
			if( byte_value || (unicode_code < 128) ) {
				internal_string_set_symbol(output, output_index++, is_packed, byte_value);
			} else {
				internal_tryPutGenCollationSeq(output, output_index, max_output, unicode_code, is_packed, table_from_unicode);
			}
		} else {
			internal_tryPutGenCollationSeq(output, output_index, max_output, unicode_code, is_packed, table_from_unicode);
		}
	} while( unicode_code && (source_index < utf_string_size) && (output_index < max_output) );
	if( output_index < max_output ) {
		if( is_packed ) {
			output[internal_string_lastPos(output_index)] = 0;
		} else {
			output[output_index] = 0;
		}
	}
	output[output_size - 1] = 0;
}
//	Convert UTF-string to Unicode-string.
stock PawnUTF_StringUTF_ToUnicode(
	const utf_string[], const utf_string_size,
	output[], const output_size
) {
	new max_i = (utf_string_size > output_size) ? output_size : utf_string_size,
		i = 0, unicode_code
	;
	do {
		output[i] = unicode_code = PawnUTF_DecodeUTF_ToUnicode(utf_string[i++]);
	} while( unicode_code && (i < max_i) );
	output[max_i - 1] = EOS;
}
//	Convert Unicode-string to UTF-string.
stock PawnUTF_StringUnicode_ToUTF(
	const unicode_string[], const unicode_string_size,
	output[], const output_size
) {
	new max_i = (unicode_string_size > output_size) ? output_size : unicode_string_size,
		i = 0, utf_code
	;
	do {
		output[i] = utf_code = PawnUTF_EncodeUnicode_ToUTF(unicode_string[i++]);
	} while( utf_code && (i < max_i) );
	output[max_i - 1] = EOS;
}
//	Writes current CP-UTF collation (which used in fwrite).
stock PawnUTF_WriteCurrentCP(const filepath_input[], const filepath_output[]) {
	new
		File:f_input = fopen(filepath_input, io_write),
		File:f_output = fopen(filepath_output, io_write),
		string[32],
		start_from = 32,
		cval,
		counter = start_from
	;

	for(new i = start_from; i < 256; ++i) {
		if( i == '\n' ) {
			continue;
		}
		string[0] = i;
		string[1] = 0;
		fwrite(f_input, string);
		fwrite(f_input, "\n");
	}
	fclose(f_input);

	if(start_from > 0 && start_from != '\n') {
		format(string, sizeof(string), "%x\t", counter);
		fwrite(f_output, string);
	}

	fopen(filepath_input, io_read);
	while( (cval = fgetchar(f_input, 0, false)) != EOF ) {
		if( cval == '\n') {
			counter++;
			if( counter == '\n' ) {
				counter++;
			}
			if( counter < 256 ) {
				format(string, sizeof(string), "\n%x\t", counter);
				fwrite(f_output, string);
			}
		} else {
			format(string, sizeof(string), "%x", cval);
			fwrite(f_output, string);
		}
	}
	fclose(f_input);
	fclose(f_output);
}

//	Reads one UTF-symbol from opened file without decoding.
stock PawnUTF_GetFileCharUTF(const File:handle) {
	new
		single_char,
		i,
		max_byte,
		result = 0
	;
	result = single_char = fgetchar(handle, 0, false);
	if( single_char != EOF ) {
		if( single_char & (1 << 7) ) {
			for(i = 5; i > 0; --i) {
				if( !((single_char >> i) & 1) ) {
					max_byte = 7 - 1 - i;
					break;
				}
			}
		} else {
			max_byte = 0;
		}
		result = single_char;
		for(i = max_byte; i > 0; --i) {
			single_char = fgetchar(handle, 0, false);
			if( single_char != EOF ) {
				result = (result << 8) | single_char;
			} else {
				break;
			}
		}
	} else {
		result = EOF;
	}
	return result;
}
//==============================================================================
static stock UTF_FindExtraChar(const array[][2], const array_size, const utf_code) {
	for(new i = 0; i < array_size; ++i) {
		if( array[i][0] == utf_code ) {
			return i;
		}
	}
	return -1;
}
static stock UTF_AddExtraChar(array[][2], const array_size, const utf_code, const codepage_code) {
	new slot = UTF_FindExtraChar(array, array_size, 0);
	if( slot != -1 ) {
		array[slot][0] = utf_code;
		array[slot][1] = codepage_code;
	}
}
//==============================================================================
//	Load codepage collation table.
stock UTF_LoadCharsetMapping(const filepath[], table_unicode[0x10000 char], table_to_unicode[256]) {
	new
		File:f = fopen(filepath, io_read),
		string[64],
		codepage_code, unicode_code
	;
	if( f ) {
		while( fread(f, string) ) {
			if( sscanf(string, "p<\t>hh", codepage_code, unicode_code) ) {
				printf("Found invalid string in %s @ %s.", filepath, string);
			} else {
				if( !(codepage_code & 0xFFFFFF00) ) {
					if( unicode_code ) {	// if it have collation.
						table_unicode{unicode_code} = codepage_code;
						table_to_unicode[codepage_code] = unicode_code;
					}
				} else {
					printf("Local or UTF code is out of range in %s @ %s", filepath, string);
				}
			}
		}
		table_unicode{0} = 0;
		table_to_unicode[0] = 0;
		fclose(f);
	}
}
//==============================================================================
//	Section UTF-16.
//	Writes UTF-16 BOM-symbol into the file.
stock PawnUTF_FWriteUTF16BOM(const File:handle, const is_bigendian) {
	if( is_bigendian ) {
		fputchar(handle, 0xFE, false);
		fputchar(handle, 0xFF, false);
	} else {
		fputchar(handle, 0xFF, false);
		fputchar(handle, 0xFE, false);
	}
}
//	Reads single UTF-16(big endian) symbol from file.
//	Returns Unicode-symbol.
stock PawnUTF_GetFileCharUTF16_BE(const File:handle) {
	new
		word_a,
		word_b,
		result = EOF
	;
	word_a = fgetchar(handle, 0, false);
	word_b = fgetchar(handle, 0, false);
	if( word_a != EOF && word_a != EOF ) {
		result = word_b | (word_a << 8);
		if( result < 0xD800 || result > 0xDFFF ) {
			return result;
		} else if( result < 0xDC00 ) {
			word_a = fgetchar(handle, 0, false);
			word_b = fgetchar(handle, 0, false);
			if( word_a == EOF && word_b == EOF ) {
				return EOF;
			}
			word_b = word_b | (word_a << 8);
			if( (word_b < 0xDC00) || (word_b > 0xDFFF) ) {
				return 0x0000;
			} else {
				result = (result << 10) | (word_b & 0x03FFF);
				result += 0x10000;
			}
		} else {
			return 0x0000;
		}
	}
	return result;
}
//	Reads single UTF-16(little endian) symbol from file.
//	Returns Unicode-symbol.
stock PawnUTF_GetFileCharUTF16_LE(const File:handle) {
	new
		word_a,
		word_b,
		result = EOF
	;
	word_a = fgetchar(handle, 0, false);
	word_b = fgetchar(handle, 0, false);
	if( word_a != EOF && word_a != EOF ) {
		result = (word_b << 8) | word_a;
		if( result < 0xD800 || result > 0xDFFF ) {
			return result;
		} else if( result < 0xDC00 ) {
			word_a = fgetchar(handle, 0, false);
			word_b = fgetchar(handle, 0, false);
			if( word_a == EOF && word_b == EOF ) {
				return EOF;
			}
			word_b = (word_b << 8) | word_a;
			if( (word_b < 0xDC00) || (word_b > 0xDFFF) ) {
				return 0x0000;
			} else {
				result = (result << 10) | (word_b & 0x03FFF);
				result += 0x10000;
			}
		} else {
			return 0x0000;
		}
	}
	return result;
}
//	Return encoded Unicode-symbol to UTF-16.
stock PawnUTF_EncodeUnicode_ToUTF16(const unicode_code) {
	if( unicode_code < 0x10000 ) {
		return unicode_code;
	}
	new
		pre_code =	unicode_code - 0x10000,
		word_a =	0xD800 | (pre_code & 0x03FF),
		word_b =	0xDC00 | ((pre_code >> 10) & 0x03FFF)
	;
	return ((word_b << 16) | word_a);
}
//	Writes single Unicode-symbol encoded to big-endian UTF-16 into the file.
stock PawnUTF_PutFileCharUTF16_BE(const File:handle, const unicode_code) {
	new utf_code = PawnUTF_EncodeUnicode_ToUTF16(unicode_code);
	if( utf_code & 0xFFFF0000 ) {
		fputchar(handle, (utf_code >> 24) & 0xFF, false);
		fputchar(handle, (utf_code >> 16) & 0xFF, false);
	}
	fputchar(handle, (utf_code >> 8) & 0xFF, false);
	fputchar(handle, (utf_code) & 0xFF, false);
}
//	Writes single Unicode-symbol encoded to big-endian UTF-16 into the file.
stock PawnUTF_PutFileCharUTF16_LE(const File:handle, const unicode_code) {
	new utf_code = PawnUTF_EncodeUnicode_ToUTF16(unicode_code);
	if( utf_code & 0xFFFF0000 ) {
		fputchar(handle, (utf_code >> 16) & 0xFF, false);
		fputchar(handle, (utf_code >> 24) & 0xFF, false);
	}
	fputchar(handle, (utf_code) & 0xFF, false);
	fputchar(handle, (utf_code >> 8) & 0xFF, false);
}
//	Reads Unicode-string decoded from UTF-16 file.
//	Returns number of readen symbols.
stock PawnUTF_FRead_UTF16(const File:handle, unicode_string[], const size, const is_bigendian) {
	new idx = 0, unicode_code;
	if( is_bigendian ) {
		while( (idx < size) && (unicode_code = PawnUTF_GetFileCharUTF16_BE(handle)) != EOF ) {
			unicode_string[idx++] = unicode_code;
			if( unicode_code == '\n' ) {
				break;
			}
		}
	} else {
		while( (idx < size) && (unicode_code = PawnUTF_GetFileCharUTF16_BE(handle)) != EOF ) {
			unicode_string[idx++] = unicode_code;
			if( unicode_code == '\n' ) {
				break;
			}
		}
	}
	unicode_string[((idx < size) ? idx : (size -1))] = EOS;	// NULL-termine symbol at the end.
	return idx;
}
//	Writes Unicode string which encoded to UTF-16 into the file.
//	Returns number of writen symbols.
stock PawnUTF_FWrite_UTF16(const File:handle, const string[], const is_bigendian) {
	new idx = 0;
	if( is_bigendian ) {
		while(string[idx]) {
			PawnUTF_PutFileCharUTF16_BE(handle, string[idx++]);
		}
	} else {
		while(string[idx]) {
			PawnUTF_PutFileCharUTF16_LE(handle, string[idx++]);
		}
	}
	return idx;
}
//==============================================================================
//	Section UTF-32.
//	Writes UTF-32 BOM-symbol into the file.
stock PawnUTF_FWriteUTF32BOM(const File:handle, const is_bigendian) {
	if( is_bigendian ) {
		fputchar(handle, 0x00, false);
		fputchar(handle, 0x00, false);
		fputchar(handle, 0xFE, false);
		fputchar(handle, 0xFF, false);
	} else {
		fputchar(handle, 0xFF, false);
		fputchar(handle, 0xFE, false);
		fputchar(handle, 0x00, false);
		fputchar(handle, 0x00, false);
	}
}
//	Reads single UTF-32(big endian) symbol from file.
//	Returns Unicode-symbol.
stock PawnUTF_GetFileCharUTF32_BE(const File:handle) {
	new single_char = (fgetchar(handle, 0, false) << 24);
	single_char |= (fgetchar(handle, 0, false) << 16);
	single_char |= (fgetchar(handle, 0, false) << 8);
	single_char |= (fgetchar(handle, 0, false));
}
//	Reads single UTF-32(little endian) symbol from file.
//	Returns Unicode-symbol.
stock PawnUTF_GetFileCharUTF32_LE(const File:handle) {
	new single_char = fgetchar(handle, 0, false);
	single_char |= (fgetchar(handle, 0, false) << 8);
	single_char |= (fgetchar(handle, 0, false) << 16);
	single_char |= (fgetchar(handle, 0, false) << 24);
}
//	Writes single Unicode-symbol encoded to big-endian UTF-32 into the file.
stock PawnUTF_PutFileCharUTF32_BE(const File:handle, const unicode_code) {
	fputchar(handle, (unicode_code >> 24) & 0xFF, false);
	fputchar(handle, (unicode_code >> 16) & 0xFF, false);
	fputchar(handle, (unicode_code >> 8) & 0xFF, false);
	fputchar(handle, (unicode_code) & 0xFF, false);
}
//	Writes single Unicode-symbol encoded to little-endian UTF-32 into the file.
stock PawnUTF_PutFileCharUTF32_LE(const File:handle, const unicode_code) {
	fputchar(handle, (unicode_code) & 0xFF, false);
	fputchar(handle, (unicode_code >> 8) & 0xFF, false);
	fputchar(handle, (unicode_code >> 16) & 0xFF, false);
	fputchar(handle, (unicode_code >> 24) & 0xFF, false);
}
//	Reads Unicode-string decoded from UTF-32 file.
//	Returns number of readen symbols.
stock PawnUTF_FRead_UTF32(const File:handle, unicode_string[], const size, const is_bigendian) {
	new idx = 0, unicode_code;
	if( is_bigendian ) {
		while( (idx < size) && (unicode_code = PawnUTF_GetFileCharUTF32_BE(handle)) != EOF ) {
			unicode_string[idx++] = unicode_code;
			if( unicode_code == '\n' ) {
				break;
			}
		}
	} else {
		while( (idx < size) && (unicode_code = PawnUTF_GetFileCharUTF32_LE(handle)) != EOF ) {
			unicode_string[idx++] = unicode_code;
			if( unicode_code == '\n' ) {
				break;
			}
		}
	}
	unicode_string[((idx < size) ? idx : (size -1))] = EOS;	// NULL-termine symbol at the end.
	return idx;
}
//	Writes Unicode string which encoded to UTF-32 into the file.
//	Returns number of writen symbols.
stock PawnUTF_FWrite_UTF32(const File:handle, const string[], const is_bigendian) {
	new idx = 0;
	if( is_bigendian ) {
		while(string[idx]) {
			PawnUTF_PutFileCharUTF32_BE(handle, string[idx++]);
		}
	} else {
		while(string[idx]) {
			PawnUTF_PutFileCharUTF32_LE(handle, string[idx++]);
		}
	}
	return idx;
}
