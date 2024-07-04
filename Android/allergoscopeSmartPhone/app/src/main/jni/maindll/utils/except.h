#ifndef __EXCEPT_TMP_
#define __EXCEPT_TMP_
#pragma pack(push) //NADA
#pragma pack(4)

class CException : public std::exception {
public:

    CException(int cause = 0) : m_cause(cause)
    {
    };

    CException(const CException &eb) : m_cause(eb.m_cause)
    {};

    CException *operator->()
    { return this; }

    virtual ~CException()
    {}

    virtual void Delete()
    {}

public:
    int m_cause;
};

#pragma pack(pop) //NADA

typedef CException CMemoryException;
////////////////////////////////////////////////////////////////
//	Excertion macroses
////////////////////////////////////////////////////////////////

#ifdef    TRY
#undef TRY
#endif
#ifdef    END_CATCH
#define END_CATCH
#endif
#ifdef    CATCH_ALL
#define	CATCH_ALL
#endif
#ifndef THROW
#define    THROW(code)    \
    {\
        throw c; \
    }
#endif
#ifndef THROW1
#define    THROW1(class, code) \
        throw class(code);
#endif
#ifndef TRY
#define TRY try
#endif
#ifndef CATCH
#define CATCH(type, e) \
            catch (type &e)
#endif
#ifndef END_CATCH
#define END_CATCH
#endif

#ifndef CATCH_ALL
#define CATCH_ALL(e) \
    catch (CException &e)\

#endif
#ifndef END_CATCH_ALL
#define END_CATCH_ALL
#endif

class CArchiveException : public CException {
public:
    enum {
        none,
        generic,
        readOnly,
        endOfFile,
        writeOnly,
        badIndex,
        badClass,
        badSchema
    };

    CArchiveException(int cause = none) : CException(cause)
    {}

    CArchiveException(const CArchiveException &c) : CException(c)
    {}

    virtual ~CArchiveException()
    {}
};

class CFileException : public CException {
public:
    enum {
        none,
        generic,
        fileNotFound,
        badPath,
        tooManyOpenFiles,
        accessDenied,
        invalidFile,
        removeCurrentDir,
        directoryFull,
        badSeek,
        hardIO,
        sharingViolation,
        lockViolation,
        diskFull,
        endOfFile
    };

    CFileException(int cause = none) : CException(cause)
    {}

    CFileException(const CFileException &c) : CException(c)
    {}

    virtual ~CFileException()
    {}
};

#endif
