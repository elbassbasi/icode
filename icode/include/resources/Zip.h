/*
 * Zip.h
 *
 *  Created on: 19 oct. 2021
 *      Author: azeddine
 */

#ifndef ICODE_INCLUDE_RESOURCES_ZIP_H_
#define ICODE_INCLUDE_RESOURCES_ZIP_H_
class ZipEntry {
public:

};
class Zip {
public:
	enum {
		/**
		 * Default zip compression level.
		 */
		ZIP_DEFAULT_COMPRESSION_LEVEL = 6,

		/**
		 * Error codes
		 */
		ZIP_ENOINIT = -1, // not initialized
		ZIP_EINVENTNAME = -2, // invalid entry name
		ZIP_ENOENT = -3, // entry not found
		ZIP_EINVMODE = -4, // invalid zip mode
		ZIP_EINVLVL = -5, // invalid compression level
		ZIP_ENOSUP64 = -6, // no zip 64 support
		ZIP_EMEMSET = -7, // memset error
		ZIP_EWRTENT = -8, // cannot write data to entry
		ZIP_ETDEFLINIT = -9, // cannot initialize tdefl compressor
		ZIP_EINVIDX = -10, // invalid index
		ZIP_ENOHDR = -11, // header not found
		ZIP_ETDEFLBUF = -12, // cannot flush tdefl buffer
		ZIP_ECRTHDR = -13, // cannot create entry header
		ZIP_EWRTHDR = -14, // cannot write entry header
		ZIP_EWRTDIR = -15, // cannot write to central dir
		ZIP_EOPNFILE = -16, // cannot open file
		ZIP_EINVENTTYPE = -17, // invalid entry type
		ZIP_EMEMNOALLOC = -18, // extracting data using no memory allocation
		ZIP_ENOFILE = -19, // file not found
		ZIP_ENOPERM = -20, // no permission
		ZIP_EOOMEM = -21, // out of memory
		ZIP_EINVZIPNAME = -22, // invalid zip archive name
		ZIP_EMKDIR = -23, // make dir error
		ZIP_ESYMLINK = -24, // symlink error
		ZIP_ECLSZIP = -25, // close archive error
		ZIP_ECAPSIZE = -26, // capacity size too small
		ZIP_EFSEEK = -27, // fseek error
		ZIP_EFREAD = -28, // fread error
		ZIP_EFWRITE = -29, // fwrite error
	};
	Zip();
	~Zip();
public:
	/**
	 * Opens zip archive with compression level using the given mode.
	 *
	 * @param zipname zip archive file name.
	 * @param level compression level (0-9 are the standard zlib-style levels).
	 * @param mode file access mode.
	 *        - 'r': opens a file for reading/extracting (the file must exists).
	 *        - 'w': creates an empty file for writing.
	 *        - 'a': appends to an existing archive.
	 *
	 * @return the zip archive handler or NULL on error
	 */
	int Open(const char *zipname, int level, char mode);
};

#endif /* ICODE_INCLUDE_RESOURCES_ZIP_H_ */
