#pragma once
#ifndef REUBEN_LIB
#define REUBEN_LIB
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit
#ifndef USES_CONVERSION
	#include <atlbase.h>
#endif

#pragma once
#ifndef _GDFCOMMON_H_
#define _GDFCOMMON_H_

#include <Reuben/System/Reuben.h>
#include <Reuben/Platform/File.h>
#include <vector>

typedef Reuben::Platform::File File;
typedef Reuben::Platform::FileException FileException;

enum MPQ_FILE_LANGUAGE
{
	MPQNeutral		= 0,
	MPQChinese		= 0x404,
	MPQCzech		= 0x405,
	MPQGerman		= 0x407,
	MPQEnglish		= 0x409,
	MPQSpanish		= 0x40a,
	MPQFrench		= 0x40c,
	MPQItalian		= 0x410,
	MPQJapanese		= 0x411,
	MPQKorean		= 0x412,
	MPQDutch		= 0x413,
	MPQPolish		= 0x415,
	MPQPortuguese	= 0x416,
	MPQRusssian		= 0x419,
	MPQEnglishUK	= 0x809
};

enum MPQ_FILE_FLAG
{
	MPQ_FILE_COMPRESS_PKWARE	= 0x00000100, // File is compressed using PKWARE Data compression library 
	MPQ_FILE_COMPRESS_MULTI		= 0x00000200, // File is compressed using combination of compression methods 
	MPQ_FILE_ENCRYPTED			= 0x00010000, // The file is encrypted 
	MPQ_FILE_FIXSEED			= 0x00020000, // The decryption key for the file is altered according to the position of the file in the archive 
	MPQ_FILE_SINGLE_UNIT		= 0x01000000, // Instead of being divided to 0x1000-bytes blocks, the file is stored as single unit 
	MPQ_FILE_DUMMY_FILE			= 0x02000000, // The file has length of 0 or 1 byte and its name is a hash 
	MPQ_FILE_HAS_EXTRA			= 0x04000000, // The file has extra data appended after regular data. Must be a compressed file 
	MPQ_FILE_EXISTS				= 0x80000000  // Set if file exists, reset when the file was deleted 
};

enum MPQ_ARCHIVE_CHANGE
{
	MPQ_ARCHIVE_CHANGE_INVALID = 0,
	MPQ_ARCHIVE_CHANGE_ADD,
	MPQ_ARCHIVE_CHANGE_UPDATE,
	MPQ_ARCHIVE_CHANGE_DELETE
};

enum MPQ_ARCHIVE_CHANGE_SOURCE
{
	MPQ_ARCHIVE_CHANGE_SOURCE_INVALID = 0,
	MPQ_ARCHIVE_CHANGE_SOURCE_FILE,
	MPQ_ARCHIVE_CHANGE_SOURCE_GDF,
	MPQ_ARCHIVE_CHANGE_SOURCE_DUMMY
};

enum MPQ_FILE_FLAGS
{
	MPQ_FILE_FLAGS_CHANGED				= 1,
	MPQ_FILE_FLAGS_PROTECTED			= 2,
	MPQ_FILE_FLAGS_COMPRESSED_PK		= 0x100,
	MPQ_FILE_FLAGS_COMPRESSED_MULTI		= 0x200,
	MPQ_FILE_FLAGS_COMPRESSED			= 0xff00,
	MPQ_FILE_FLAGS_ENCRYPTED			= 0x10000,
	MPQ_FILE_FLAGS_FIXSEED				= 0x20000,
	MPQ_FILE_FLAGS_SINGLEUNIT			= 0x1000000,
	MPQ_FILE_FLAGS_UNKNOWN_02000000		= 0x02000000,
	MPQ_FILE_FLAGS_FILE_HAS_METADATA	= 0x04000000,
	MPQ_FILE_FLAGS_EXISTS				= 0x80000000
};

enum MPQ_BLOCK_FLAGS
{
	MPQ_BLOCK_FLAGS_HASH_VALIDEMPTY = 0xfffffffe,
	MPQ_BLOCK_FLAGS_HASH_EMPTY		= 0xffffffff
};

enum GDFFILE_STATE
{
	GDFFILE_STATE_GAME = 0,			// "(listfile)" is not loaded, continual connection required
	GDFFILE_STATE_MASTER_WRITE,		// "(listfile)" is loaded, write mode, backup required
	GDFFILE_STATE_PATCH_READ,		// "(listfile)" is loaded, read mode, continual connection required
};

#pragma pack (push)
#pragma pack (4)

struct GdfHeader_S
{
	char magic[4];					// Indicates that the file is a MoPaQ archive. Must be ASCII "MPQ" 1Ah.
	UInt32 headerSize;				// Size of the archive header.
	UInt32 archiveSize;				// Size of the whole archive, including the header. Does not include the strong digital signature, if present. This size is used, among other things, for determining the region to hash in computing the digital signature. This field is deprecated in the Burning Crusade MoPaQ format, and the size of the archive is calculated as the size from the beginning of the archive to the end of the hash table, block table, or extended block table (whichever is largest).
	UInt16 formatVersion;			// ** MoPaQ format version. MPQAPI will not open archives where this is negative. Known versions:	0000h: Original format. HeaderSize should be 20h, and large archives are not supported.	0001h: Burning Crusade format. Header size should be 2Ch, and large archives are supported.
	UInt8 sectorSizeShift;			// Power of two exponent specifying the number of 512-byte disk sectors in each logical sector in the archive. The size of each logical sector in the archive is 512 * 2^SectorSizeShift. Bugs in the Storm library dictate that this should always be 3 (4096 byte sectors).
	UInt32 hashTableOffset;			// Offset to the beginning of the hash table, relative to the beginning of the archive.
	UInt32 blockTableOffset;		// Offset to the beginning of the block table, relative to the beginning of the archive.
	UInt32 hashTableEntries;		// Number of entries in the hash table. Must be a power of two, and must be less than 2^16 for the original MoPaQ format, or less than 2^20 for the Burning Crusade format.
	UInt32 blockTableEntries;		// Number of entries in the block table.  Fields only present in the Burning Crusade format and later:
	UInt64 extendedBlockTableOffset;// Offset to the beginning of the extended block table, relative to the beginning of the archive.
	UInt16 hashTableOffsetHigh;		// High 16 bits of the hash table offset for large archives.
	UInt16 blockTableOffsetHigh;	//High 16 bits of the block table offset for large archives.
};

#pragma pack (pop)

struct GdfHeader : public GdfHeader_S
{
	GdfHeader() {};

	VOID SetMagic(char* input) { memcpy(&magic, input, 4 * sizeof(char)); };
	VOID SetHeaderSize(UInt32 input) { headerSize = input; };
	VOID SetArchiveSize(UInt32 input) { archiveSize = input; };
	VOID SetFormatVersion(UInt16 input) { formatVersion = input; };
	VOID SetSectorSizeShift(UInt8 input) { sectorSizeShift = input; };
	VOID SetHashTableOffset(UInt32 input) { hashTableOffset = input; };
	VOID SetBlockTableOffset(UInt32 input) { blockTableOffset = input; };
	VOID SetHashTableEntries(UInt32 input) { hashTableEntries = input; };
	VOID SetBlockTableEntries(UInt32 input) { blockTableEntries = input; };
	VOID SetExtendedBlockTableOffset(UInt32 input) { extendedBlockTableOffset = input; };
	VOID SetHashTableOffsetHigh(UInt32 input) { hashTableOffsetHigh = input; };
	VOID SetBlockTableOffsetHigh(UInt32 input) { blockTableOffsetHigh = input; };
};

struct GdfHashTable
{
	UInt32 filePathHashA;	// The hash of the file path, using method A.
	UInt32 filePathHashB;	// The hash of the file path, using method B.
	UInt16 language;		// The language of the file. This is a Windows LANGID data type, and uses the same values. 0 indicates the default language (American English), or that the file is language-neutral.
	UInt8 platform;			// The platform the file is used for. 0 indicates the default platform. No other values have been observed.
	UInt32 fileBlockIndex;	// If the hash table entry is valid, this is the index into the block table of the file. Otherwise, one of the following two values:	FFFFFFFFh: Hash table entry is empty, and has always been empty. Terminates searches for a given file.	FFFFFFFEh: Hash table entry is empty, but was valid at some point (in other words, the file was deleted). Does not terminate searches for a given file.

	GdfHashTable() {};
};

struct GdfBlockTable
{
	UInt32 blockOffset;		// Offset of the beginning of the block, relative to the beginning of the archive.
	UInt32 blockSize;		// Size of the block in the archive.
	UInt32 fileSize;		// Size of the file data stored in the block. Only valid if the block is a file; otherwise meaningless, and should be 0. If the file is compressed, this is the size of the uncompressed file data.
	UInt32 flags;			// Bit mask of the flags for the block. The following values are conclusively identified:	80000000h: Block is a file, and follows the file data format; otherwise, block is free space or unused. If the block is not a file, all other flags should be cleared, and FileSize should be 0.	01000000h: File is stored as a single unit, rather than split into sectors.	00020000h: The file's encryption key is adjusted by the block offset and file size (explained in detail in the File Data section). File must be encrypted.	00010000h: File is encrypted.	00000200h: File is compressed. File cannot be imploded.	00000100h: File is imploded. File cannot be compressed.

	GdfBlockTable() {};
};

struct GdfExtBlockTable
{
	// nothing different from GdfBlockTable
};

struct GdfSignature
{
	char magic[4];			// Indicates the presence of a digital signature. Must be "NGIS" ("SIGN" backwards).
	UInt32 signature[64];	// The digital signature, stored in little-endian format.
};

struct GdfArchiveChange
{
	String filename;
	UInt32 filePathHashA;
	UInt32 filePathHashB;
	UInt8 type; // 0: invalid, 1: add, 2: update, 3: delete
	UInt8 source; // 0: invalid, 1: file, 2: gdf-patch
	
	GdfArchiveChange()	{};
};

struct GdfListFile
{
	String filename;
	UInt32 filePathHashA;
	UInt32 filePathHashB;

	GdfListFile() {};
};

// Assistant File format for backup:
struct GdfBackupHeader
{
	UInt32 headerSize;
	UInt32 hashOffset;
	UInt32 hashSize;
	UInt32 blockOffset;
	UInt32 blockSize;
	UInt32 bFileCompleted;		// 0: incomplete backup; 1: completed backup.  For any revert use.
	UInt32 restoreHashOffset;
	UInt32 restoreBlockOffset;
};

// Assistant File format for change log, for versioning use:
struct GdfPatchStep
{
	String filename;
	UInt8 type;					// 0: invalid, 1: add, 3: delete	(2 is ignored to eliminate the ambiguous UPDATE case)

	GdfPatchStep() {};
};

//THIS CODE MAY BE INCORRECT, AND HAS NOT BEEN TESTED 

#define EPOCH_OFFSET 116444736000000000ULL	// Number of 100 ns units between 01/01/1601 and 01/01/1970

#define BLOCK_OFFSET_ADJUSTED_KEY 0x00020000L

#define MPQ_HASH_ENTRY_EMPTY 0xFFFFFFFFL
#define MPQ_HASH_ENTRY_DELETED 0xFFFFFFFEL

#define MPQ_HASH_TABLE_OFFSET	0
#define MPQ_HASH_NAME_A	1
#define MPQ_HASH_NAME_B	2
#define MPQ_HASH_FILE_KEY	3

typedef StlVector<GdfHashTable*, ManagedAllocator<GdfHashTable*> > HashTableVector;
typedef StlVector<GdfBlockTable*, ManagedAllocator<GdfBlockTable*> > BlockTableVector;
typedef StlVector<GdfArchiveChange*, ManagedAllocator<GdfArchiveChange*> > ArchiveChangeVector;
typedef StlVector<GdfListFile*, ManagedAllocator<GdfListFile*> > ListFileVector;
typedef StlVector<GdfPatchStep*, ManagedAllocator<GdfPatchStep*> > PatchStepVector;
typedef StlMap<UInt32, PatchStepVector, std::less<UInt32>, ManagedAllocator<std::pair<UInt32, PatchStepVector> > > PatchStepMap;

#define MEM_BLOCK_SIZE 65536
#define SECTOR_OFFSET	4

#endif