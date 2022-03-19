//	Convert codepage-string to stream defined via collation table.
stock PawnUTF_StringToCollation(const string[], const string_size, output[], const output_size, const is_packed, const collation_table[], const collation_size) {
	new
		cp_character,
		i,
		max_i = (string_size > output_size) ? output_size : string_size
	;
	for(i = 0; i < max_i; ++i) {

		if( is_packed ) {
			cp_character = string{i};
		} else {
			cp_character = string[i];
		}
		if( cp_character < collation_size ) {
			output[i] = collation_table[cp_character];
		}
	}
}
//	Extract UTF characters from string-stream.
stock PawnUTF_StreamToUTF(const array[], const array_size, output[], const output_size, const is_packed) {
	new
		max_bytes = ((1 + is_packed * 3) * array_size),
		output_index = 0,
		current_byte,
		accamulated_size = 0,
		accamulated_value = 0,
		current_value,
		was_cont
	;
	for(current_byte = 0; current_byte < max_bytes; ++current_byte) {
		if( output_index >= output_size ) {
			output[output_size - 1] = 0;	// Enforce string to end.
			break;
		}
		if( is_packed ) {
			current_value = array{current_byte};
		} else {
			current_value = array[current_byte];
		}
		if( current_value & 0x80 ) {	// Ending or continuous char byte.
			if( current_value & 0x40 ) {	// Ending char.
				if( was_cont ) {
					output[output_index++] = accamulated_value;
					accamulated_value = 0;
				}
				for(new i = 5; i > 0; --i) {
					if( !((current_value >> i) & 1) ) {	// The first zero bit found.
						accamulated_size = 7 - i;
						break;
					}
				}
				was_cont = 0;
			} else {	// Continuous byte.
				was_cont = 1;
			}
			accamulated_value |= current_value << (8 * --accamulated_size);
		} else {	// Single byte (char).
			if( was_cont ) {
				output[output_index++] = accamulated_value;
				accamulated_value = 0;
			}
			output[output_index++] = current_value;
			if( !current_value ) {
				break;
			}
			was_cont = 0;
		}
	}
}
//==============================================================================
//	Decode UTF-character to Unicode symbol.
stock PawnUTF_DecodeUTF_ToUnicode(const utf_code) {
	new result = 0, i, byte_value;
	if( utf_code & 0x80 ) {
		i = 0;
		byte_value = utf_code & 0xFF;
		while( byte_value & 0x80 ) {
			if( byte_value & 0x40 ) {	// is end
				result |= ((byte_value & (0xFF >> (i + 2)) ) << (6 * i));
			} else {	// is continiuous
				result |= ((byte_value & 0x3F) << (6 * i));
			}
			i++;
			byte_value = (utf_code >> (8 * i)) & 0xFF;
		}

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
//	Converts UTF-stream to stream with codepage defined in collation tables.
stock PawnUTF_StreamUTF_ToCodePage(
	const utf_stream[], const utf_stream_size,
	output[], const output_size,
	const table_from_utf[0x10000 char], const table_from_utf_extra[][2], const extra_size, const is_packed
) {
	new
		i, j = 0, k,
		byte_value,
		utf_code, codepage_code, extra_slot
	;
	for(i = 0, j = 0; i < utf_stream_size; ++i) {
		if( j >= output_size ) {
			output[output_size - 1] = 0;
			break;
		}
		utf_code = utf_stream[i];
		if( !utf_code ) {
			output[j] = 0;
			break;
		}
		if( utf_code > 0xFFFF ) {
			extra_slot = UTF_FindExtraChar(table_from_utf_extra, extra_size, utf_code);
			if( extra_slot == -1 ) {
				codepage_code = 0;
			} else {
				codepage_code = table_from_utf_extra[extra_slot][1];
			}
		} else {
			codepage_code = table_from_utf{utf_code};
		}
		for(k = 3; k > -1; --k) {
			byte_value = (codepage_code >> (8 * k)) & 0xFF;
			if( byte_value ) {
				if( is_packed ) {
					output{j} = byte_value;
				} else {
					output[j] = byte_value;
				}
				j++;
			}
		}
	}
}
//	Convert UTF-stream to Unicode-stream.
stock PawnUTF_StreamUTF_ToUnicode(
	const utf_stream[], const utf_stream_size,
	output[], const output_size
) {
	new max_i = (utf_stream_size > output_size) ? output_size : utf_stream_size;
	for(new i = 0; i < max_i; ++i) {
		output[i] = PawnUTF_DecodeUTF_ToUnicode(utf_stream[i]);
	}
	output[max_i - 1] = EOS;
}
//	Convert Unicode-stream to UTF-stream.
stock PawnUTF_StreamUnicode_ToUTF(
	const unicode_stream[], const unicode_stream_size,
	output[], const output_size
) {
	new max_i = (unicode_stream_size > output_size) ? output_size : unicode_stream_size;
	for(new i = 0; i < max_i; ++i) {
		output[i] = PawnUTF_EncodeUnicode_ToUTF(unicode_stream[i]);
	}
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

//	Reads one UTF-symbol from opened file.
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
stock UTF_LoadChatsetMapping(const filepath[], table_to_utf[256], table_from_utf[0x10000 char], table_from_utf_extra[][2], const extra_size, table_unicode[0x10000 char], table_to_unicode[256]) {
	new
		File:f = fopen(filepath, io_read),
		string[64],
		utf_code, codepage_code, unicode_code
	;
	if( f ) {
		while( fread(f, string) ) {
			if( sscanf(string, "p<\t>hh", codepage_code, utf_code) ) {
				printf("Found invalid string in %s @ %s.", filepath, string);
			} else {
				if( !(codepage_code & 0xFFFFFF00) ) {
					if( utf_code < 0x10000 ) {
						table_from_utf{utf_code} = codepage_code;
					} else {
						UTF_AddExtraChar(table_from_utf_extra, extra_size, utf_code, codepage_code);
					}
					unicode_code = PawnUTF_DecodeUTF_ToUnicode(utf_code);
					table_to_utf[codepage_code] = utf_code;
					table_unicode{unicode_code} = codepage_code;
					table_to_unicode[codepage_code] = unicode_code;
				} else {
					printf("Local or UTF code is out of range in %s @ %s", filepath, string);
				}
			}
		}
		fclose(f);
	}
}