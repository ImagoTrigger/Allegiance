#pragma once

////////////////////////////////////////////////////////////////////////////////
#include "HashTable.h"

////////////////////////////////////////////////////////////////////////////////
typedef void (*PACK_CREATE_CALLBACK) ( int iCurrentFileIndex, int iMaxFileIndex );


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class CPackExclusion
{
////////////////////////////////////////////////////////////////////////////////
public:
	enum EExclusionType
	{
		eET_Undefined = -1,
		eET_FileName = 0,
		eET_SubString,

		eET_NumExclusionTypes
	};

////////////////////////////////////////////////////////////////////////////////
public:
	CPackExclusion *	m_pNext;
	EExclusionType		m_eType;
	ZString				m_szExclusionFilter;

////////////////////////////////////////////////////////////////////////////////
public:
	CPackExclusion();
	CPackExclusion( const char * szFilter );
	bool IsExcluded( const char * szFileName );
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class CPackFileHashEntry : public CHashEntry
{
public:
	CPackFileHashEntry() : CHashEntry( 0, NULL )
	{
		m_dwPackOffset = 0;
	}
	ZString mszFileName;
	DWORD m_dwPackOffset;
	DWORD m_dwFileSize;
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class CDX9PackFile
{
private:
	struct SPackFileHeader
	{
		char	szID[ 8 ];				// Pack file ID - AllegPak
		DWORD	dwFileDataOffset;		// Offset to raw file data.
		DWORD	dwFileDataSize;			// Complete raw file data size.
		DWORD	dwTableSize;			// Size of original hash table.
		DWORD	dwTableOffset;			// Offset to SHashTableEntry array.
		DWORD	dwTableDataSize;		// Complete data size.
		DWORD	dwNumTableEntries;		// Number of entries in SHashTableEntry array.
		DWORD	dwTableEntrySize;		// Size of individual SHashTableEntry.
	};

	////////////////////////////////////////////////////////////////////////
	// Pack file hash table entries, as written by this class.
	struct SHashTableEntry
	{
		DWORD dwHashValue;
		DWORD dwNextIndex;
		DWORD dwDataOffset;
		DWORD dwFileSize;
	};

////////////////////////////////////////////////////////////////////////////////
private:
	CDX9PackFile *			m_pNext;						// Linked list.

	// Pack file creation vars.
	static const int		s_iWriteBufferSize = 64 * 1024 * 1024;
	ZFile *					m_pFile;
	CHashTable *			m_pHashTable;
	BYTE *					m_pWriteBuffer;
	DWORD					m_dwWritePos;
	DWORD					m_dwPackOffset;
	CPackExclusion *		m_pExclusionList;
	int						m_iTotalFiles;
	int						m_iCurrentFile;
	PACK_CREATE_CALLBACK	m_fnCallBack;
	bool					m_bUserCancelled;
	bool					m_bFinished;

	// Shared vars.
	SPackFileHeader			m_header;
	ZString					m_szDataPath;					// Root data directory.
	ZString					m_szFileName;					// File name used to create pack file.
	ZString					m_szOutputFileName;				// Complete output path.

	// Pack file retrieval vars.
	BYTE *					m_pPackFile;					// Imported pack file data.
	SHashTableEntry *		m_pHashEntryArray;
	BYTE *					m_pRawFileData;

	static CDX9PackFile *	m_pPackFileLinkedList;

////////////////////////////////////////////////////////////////////////////////
public:
	////////////////////////////////////////////////////////////////////////
	CDX9PackFile( const char * szDataPath, const char * szFileName );
	~CDX9PackFile( void );

	////////////////////////////////////////////////////////////////////////
	bool Exists();

	////////////////////////////////////////////////////////////////////////
	bool Create( PACK_CREATE_CALLBACK pFnCreateCallback );

	////////////////////////////////////////////////////////////////////////
	bool ImportPackFile( );

	////////////////////////////////////////////////////////////////////////
	void SetUserCancelled( bool bCancel )	{	m_bUserCancelled = bCancel;		}
	bool GetPackFileFinished()				{	return m_bFinished;				}

	////////////////////////////////////////////////////////////////////////
	static void AddToPackFileList( CDX9PackFile * pPackFile );

	////////////////////////////////////////////////////////////////////////
	static void * LoadFile( const char * szFileName, DWORD * pdwFileSize );

////////////////////////////////////////////////////////////////////////////////
private:
	////////////////////////////////////////////////////////////////////////
	void AddExclusionFilter( const char * szFilter );

	////////////////////////////////////////////////////////////////////////
	int GetFileCount( ZString szDir, ZString szFilter );

	////////////////////////////////////////////////////////////////////////
	bool AddFiles( ZString szDir, ZString szFilter );

	////////////////////////////////////////////////////////////////////////
	bool WriteHashTableData( );
	
	////////////////////////////////////////////////////////////////////////
	void * LoadFileInternal( const char * szFileName, DWORD * pdwFileSize );
};
